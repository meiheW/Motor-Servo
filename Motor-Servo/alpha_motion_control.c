#include <unistd.h>
#include <stdio.h>
#include <pthread.h>

#include "elog.h"
#include "alpha_motion_control.h"


#define MAX_MOTOR_SPEED (200000) // 0.01r/min
#define MIN_MOTOR_SPEED (1)
#define MAX_ACCE_DECE_TIME (99999) // 0.1ms
#define MIN_ACCE_DECE_TIME (1)
uint16_t cruise_speed[2]; // 运行时的速度
uint16_t imme_acceleration_time[2]; // 运行时的加速时间
uint16_t imme_deceleration_time[2]; // 运行时的减速时间
uint16_t check_speed[2]; // 自检操作时的运行速度
uint16_t check_acce_time[2]; // 自检操作时的加速时间
uint16_t check_dece_time[2]; // 自检操作时的减速时间


// 伺服电机的运动
volatile MovementStatus motor_movement = NOTMOVE;
// 用于变量赋值时的互斥
pthread_mutex_t mutex_motion_ctrl = PTHREAD_MUTEX_INITIALIZER;


// 更新伺服电机的运动状态
void update_motor_movement(double angle)
{
	pthread_mutex_lock(&mutex_motion_ctrl);
	if(angle > 0)
		motor_movement = RIGHTMOVE;
	else if(angle < 0)
		motor_movement = LEFTMOVE;
	else motor_movement = NOTMOVE;
	pthread_mutex_unlock(&mutex_motion_ctrl);
}
// 获取伺服电机的运动状态
MovementStatus get_motor_movement()
{
	pthread_mutex_lock(&mutex_motion_ctrl);
	MovementStatus direct = motor_movement;
	pthread_mutex_unlock(&mutex_motion_ctrl);
	return direct;
}
// 更新伺服电机的运动为静止，主要为is_INP调用
void set_motor_movement_notmove()
{
	pthread_mutex_lock(&mutex_motion_ctrl);
	motor_movement = NOTMOVE;
	pthread_mutex_unlock(&mutex_motion_ctrl);
}


// 获取OUT线圈状态
// 返回-1表示通信失败，未读取到数据
int get_out_status(int out_addr)
{
	for(int i = 0; i < OPLOOPS && 1 != modbus_read_bits(ctx, out_addr, 1, tab_rp_bits); i++){
		if(OPLOOPS-1==i)
		{
			log_e("get_out_status: communication failed.");
			return -1;
		}
	}
	return tab_rp_bits[0];
}
// 设置CONT线圈状态
// 返回-1表示通信失败，未设置成功
int set_cont_status(int cont_addr, int status)
{
	for(int i = 0; i < OPLOOPS && 1 != modbus_write_bit(ctx, cont_addr, status); i++){
		if(i==OPLOOPS-1)
		{
			log_e("set_cont_status: communication failed.");
			return -1;
		}
	}
	return 0;
}
// Debug函数，获取当前CONT线圈状态
int get_cont_status(int cont_addr)
{
	for(int i = 0; i < OPLOOPS && 1 != modbus_read_bits(ctx, cont_addr, 1, tab_rp_bits); i++){
		if(OPLOOPS-1==i)
		{
			log_e("get_cont_status: communication failed.");
			return -1;
		}
	}
	return tab_rp_bits[0];
}

// 转换角度为位置增量
void convert_angle_to_inc_position(double angle, uint16_t *inc_position)
{
	int32_t position = (int32_t)(angle*M_PULSE_PER_CIRCLE/360);
	inc_position[1] = position & 0xFFFF;
	inc_position[0] = (position >> 16) & 0xFFFF;

	//// debug语句
	//int32_t test_position = inc_position[0] * (1<<16) + inc_position[1];
	//printf("convert_angle_to_inc_position %f to %d\n", angle, test_position);
}

// 增量旋转
int run_inc_angle(double angle)
{
	//// debug语句
	//printf("run_inc_angle %f\n", angle);

	if(is_INP())
	{
		int ret;
		uint16_t inc_position[2];
		convert_angle_to_inc_position(angle, inc_position);

		ret = set_inc_control_mode(); //增量
		if(-1 == ret) {
			log_e("run_inc_position: set inc control mode failed.");
			return -1;
		}
		
		for(int i = 0; i < OPLOOPS && 2 != modbus_write_registers(ctx, IMME_VLU_POSITION_ad, 2, inc_position); i++){
			if(OPLOOPS-1==i)
			{
				log_e("run_inc_position: communication failed.");
				return -1;
			}
		}

		ret = immediate_value_operation_run();
		if(0 == ret) { 
			// 设定运动状态
			update_motor_movement(angle);
			return 1;
		} else return -1;
	}
	return 0;
}


// 运行到指定角度 (绝对模式)
int run_to_angle(double angle)
{
	//// debug语句
	//printf("run_inc_angle %f\n", angle);
	
	if(is_INP())
	{
		int ret = 0;
		uint16_t inc_position[2];
		convert_angle_to_inc_position(angle, inc_position);

		// // Debug语句
		// printf("run_to_angle: %lf\n",angle);
		// printf("run_to_angle: val0: %4x, val1: %4x\n", inc_position[0], inc_position[1]);

		ret = set_abs_control_mode(); //绝对坐标
		if(-1 == ret) {
			log_e("run_to_angle: set abs control mode failed.");
			return -1;
		}

		for(int i = 0; i < OPLOOPS && 2 != modbus_write_registers(ctx, IMME_VLU_POSITION_ad, 2, inc_position); i++){
			if(OPLOOPS-1==i)
			{
				log_e("run_to_angle: communication failed.");
				return -1;
			}
		}

		ret = immediate_value_operation_run();
		if(0 == ret) return 1;
		else return -1;
	}
	return 0;
}

//***********************************************************************************************************************
// Immediate value data setting: 
//		The immediate value status of immediate data is configured as follows:
//		Data||4 bytes|| Immediate value status			1byte	(ntoe: bit0 configure the command method,0:ABS 1:INC)
//					 || Immediate value value M code	1byte
//					 || Not used						2bytes
//***********************************************************************************************************************
// 示例:
// tab_rq_registers[0] = INC_POSITION_MODE << 8;
// tab_rq_registers[1] = 0x0000;
// for(int i = 0; i < OPLOOPS && 2 != modbus_write_registers(ctx, IMME_VLU_STATUS_ad, 2, tab_rq_registers); i++){
// 	if(i==OPLOOPS-1) fprintf(stderr,"ERR:connection is not stable.\n");
// }
//***********************************************************************************************************************
int set_abs_control_mode()
{
	tab_rq_registers[0] = ABS_POSITION_MODE << 8;
	tab_rq_registers[1] = 0x0000;
	for(int i = 0; i < OPLOOPS && 2 != modbus_write_registers(ctx, IMME_VLU_STATUS_ad, 2, tab_rq_registers); i++){
		if(OPLOOPS-1==i)
		{
			log_e("set_abs_control_mode: communication failed.");
			return -1;
		}
	}		
	return 0;
}
int set_inc_control_mode()
{
	tab_rq_registers[0] = INC_POSITION_MODE << 8;
	tab_rq_registers[1] = 0x0000;
	for(int i = 0; i < OPLOOPS && 2 != modbus_write_registers(ctx, IMME_VLU_STATUS_ad, 2, tab_rq_registers); i++){
		if(OPLOOPS-1==i)
		{
			log_e("set_inc_control_mode: communication failed.");
			return -1;
		}
	}
	return 0;
}

// 运行，run, 发出脉冲表示运行信号
int immediate_value_operation_run()
{
	for(int i = 0; i < OPLOOPS && 1 != modbus_write_bit(ctx, START_ad, 1); i++){
		if(OPLOOPS-1==i)
		{
			log_e("set start_on: communication failed.");
			return -1;
		}
	}
	for(int i = 0; i < OPLOOPS && 1 != modbus_write_bit(ctx, START_ad, 0); i++){
		if(OPLOOPS-1==i)
		{
			log_e("set start_off: communication failed.");
			return -1;
		}
	}
	return 0;
}

// 运行速度变量赋值
// speed: 0.01r/min
int set_cruise_speed(uint32_t speed)
{
	pthread_mutex_lock(&mutex_motion_ctrl);
	if(speed <= MIN_MOTOR_SPEED) speed = MIN_MOTOR_SPEED;
	if(speed >= MAX_MOTOR_SPEED) speed = MAX_MOTOR_SPEED;	// 0.01r/min
	cruise_speed[1] = speed & 0xFFFF;
	cruise_speed[0] = (speed >> 16) & 0xFFFF;
	pthread_mutex_unlock(&mutex_motion_ctrl);
	return 0;
}
// 运行速度变量获取
uint32_t get_cruise_speed()
{
	pthread_mutex_lock(&mutex_motion_ctrl);
	uint32_t speed = (cruise_speed[0] << 16) + cruise_speed[1];
	pthread_mutex_unlock(&mutex_motion_ctrl);
	//printf("get cruise speed: %d 0.01r/min\n",speed);
	return speed;
}
// 将目标运行速度值发送至控制器
int send_cruise_speed()
{
	for(int i = 0; i < OPLOOPS && 2 != modbus_write_registers(ctx, IMME_VLU_SPEED_ad, 2, cruise_speed); i++){
		if(OPLOOPS-1==i)
		{
			log_e("send_cruise_speed: communication failed.");
			return -1;
		}
	}		
	return 0;
}

// 运行时加速时间变量赋值
// acce time: 0.1ms
int set_imme_acceleration_time(uint32_t time)
{
	pthread_mutex_lock(&mutex_motion_ctrl);
	if(time <= MIN_ACCE_DECE_TIME) time = MIN_ACCE_DECE_TIME;
	if(time >= MAX_ACCE_DECE_TIME) time = MAX_ACCE_DECE_TIME;	// 0.1ms
	imme_acceleration_time[1] = time & 0xFFFF;
	imme_acceleration_time[0] = (time >> 16) & 0xFFFF;
	pthread_mutex_unlock(&mutex_motion_ctrl);
	return 0;
}
// 运行时加速时间读取
uint32_t get_imme_acceleration_time()
{
	pthread_mutex_lock(&mutex_motion_ctrl);
	uint32_t acce_time = (imme_acceleration_time[0] << 16) + imme_acceleration_time[1];
	pthread_mutex_unlock(&mutex_motion_ctrl);
	//printf("get imme acceleration time: %d 0.1ms\n",acce_time);
	return acce_time;

}
// 将目标加速时间发送至控制器
int send_imme_acceleration_time()
{
	for(int i = 0; i < OPLOOPS && 2 != modbus_write_registers(ctx, IMME_VLU_ACC_TIM_ad, 2, imme_acceleration_time); i++){
		if(OPLOOPS-1==i)
		{
			log_e("send_imme_acceleration_time: communication failed.");
			return -1;
		}
	}		
	return 0;
}
// 减速时间变量赋值
// dece time: 0.1ms
int set_imme_deceleration_time(uint32_t time)
{
	pthread_mutex_lock(&mutex_motion_ctrl);
	if(time <= MIN_ACCE_DECE_TIME) time = MIN_ACCE_DECE_TIME;
	if(time >= MAX_ACCE_DECE_TIME) time = MAX_ACCE_DECE_TIME;	// 0.1ms
	imme_deceleration_time[1] = time & 0xFFFF;
	imme_deceleration_time[0] = (time >> 16) & 0xFFFF;
	pthread_mutex_unlock(&mutex_motion_ctrl);
	return 0;
}
// 减速时间读取
uint32_t get_imme_deceleration_time()
{
	pthread_mutex_lock(&mutex_motion_ctrl);
	uint32_t dece_time = (imme_deceleration_time[0] << 16) + imme_deceleration_time[1];
	pthread_mutex_unlock(&mutex_motion_ctrl);
	//printf("get imme deceleration time: %d 0.1ms\n",dece_time);
	return dece_time;
}
// 发送减速时间到控制器
int send_imme_deceleration_time()
{
	for(int i = 0; i < OPLOOPS && 2 != modbus_write_registers(ctx, IMME_VLU_DEC_TIM_ad, 2, imme_deceleration_time); i++){
		if(OPLOOPS-1==i)
		{
			log_e("send_imme_deceleration_time: communication failed.");
			return -1;
		}
	}		
	return 0;
}

// 自检运动速度
int set_check_speed(uint32_t speed)
{
	if(speed <= MIN_MOTOR_SPEED) speed = MIN_MOTOR_SPEED;
	if(speed >= MAX_MOTOR_SPEED) speed = MAX_MOTOR_SPEED;	// 0.01r/min
	check_speed[1] = speed & 0xFFFF;
	check_speed[0] = (speed >> 16) & 0xFFFF;
	return 0;
}
int send_check_speed()
{
	for(int i = 0; i < OPLOOPS && 2 != modbus_write_registers(ctx, IMME_VLU_SPEED_ad, 2, check_speed); i++){
		if(OPLOOPS-1==i)
		{
			log_e("send_check_speed: communication failed.");
			return -1;
		}
	}		
	return 0;
}
// 自检运动加速时间
int set_check_acce_time(uint32_t time)
{
	if(time <= MIN_ACCE_DECE_TIME) time = MIN_ACCE_DECE_TIME;
	if(time >= MAX_ACCE_DECE_TIME) time = MAX_ACCE_DECE_TIME;	// 0.1ms
	check_acce_time[1] = time & 0xFFFF;
	check_acce_time[0] = (time >> 16) & 0xFFFF;
	return 0;
}
int send_check_acce_time()
{
	for(int i = 0; i < OPLOOPS && 2 != modbus_write_registers(ctx, IMME_VLU_ACC_TIM_ad, 2, check_acce_time); i++){
		if(OPLOOPS-1==i)
		{
			log_e("send_check_acce_time: communication failed.");
			return -1;
		}
	}		
	return 0;
}
// 自检运动减速时间
int set_check_dece_time(uint32_t time)
{
	if(time <= MIN_ACCE_DECE_TIME) time = MIN_ACCE_DECE_TIME;
	if(time >= MAX_ACCE_DECE_TIME) time = MAX_ACCE_DECE_TIME;	// 0.1ms
	check_dece_time[1] = time & 0xFFFF;
	check_dece_time[0] = (time >> 16) & 0xFFFF;
	return 0;
}
int send_check_dece_time()
{
	for(int i = 0; i < OPLOOPS && 2 != modbus_write_registers(ctx, IMME_VLU_DEC_TIM_ad, 2, check_dece_time); i++){
		if(OPLOOPS-1==i)
		{
			log_e("send_check_dece_time: communication failed.");
			return -1;
		}
	}		
	return 0;
}

// 立即数运行到目标位置判定函数
int is_INP()
{
	for(int i = 0; i < OPLOOPS && 1 != modbus_read_bits(ctx, INP_ad,1,tab_rp_bits); i++){
		if(OPLOOPS-1==i)
		{
			log_e("is_INP: communication failed.");
			return 0;
		} 
	}
	if(1==tab_rp_bits[0]){
		//设定运行状态为NOTMOVE
		set_motor_movement_notmove();
		return 1;
	}
	return 0;
}

// 伺服
int serve_on()
{
	for(int i = 0; i < OPLOOPS && 1 != modbus_read_bits(ctx,S_RDY_ad,1,tab_rp_bits); i++){
		if(OPLOOPS-1==i)
		{
			log_e("serve_on: require S_RDY, communication failed.");
			return -1; 
		}
	}
	if(1 != tab_rp_bits[0]) { 
		log_e("serve_on: S_RDY is not ON.");
		return -1;
	}

	for(int i = 0; i < OPLOOPS && 1 != modbus_write_bit(ctx,SERVO_ON_ad,1); i++){
		if(OPLOOPS-1==i)
		{
			log_e("serve_on: communication failed.");
			return -1;
		}
	}

	return 0;
}
// 释放伺服
int serve_off()
{
	for(int i = 0; i < OPLOOPS && 1 != modbus_write_bit(ctx,SERVO_ON_ad,0); i++){
		if(OPLOOPS-1==i)
		{
			log_e("serve_off: communication failed.");
			return -1;
		}
	}

	return 0;
}
// 准备状态判定
int is_ready()
{
	for(int i = 0; i < OPLOOPS && 1 != modbus_read_bits(ctx,RDY_ad,1,tab_rp_bits); i++){
		if(OPLOOPS-1==i)
		{
			log_e("is_ready: communication failed.");
			return 0;
		}
	}
	if(1==tab_rp_bits[0]) return 1;

	return 0;
}

// Interrupting/Stopping Operation
// [EMG]: forced stop. 紧急停止
int forced_stop_on()
{
	for(int i = 0; i < OPLOOPS && 1 != modbus_write_bit(ctx, EMG_ad, 1); i++){
		if(i==OPLOOPS-1)
		{
			log_e("forced_stop_on: communication failed.");
			return -1;
		}
	}
	return 0;
}
int forced_stop_off()
{
	for(int i = 0; i < OPLOOPS && 1 != modbus_write_bit(ctx, EMG_ad, 0); i++){
		if(i==OPLOOPS-1)
		{
			log_e("forced_stop_off: communication failed.");
			return -1;
		}
	}
	return 0;
}
// 暂停
int pause_on()
{
	for(int i = 0; i < OPLOOPS && 1 != modbus_write_bit(ctx, PAUSE_ad, 1); i++){
		if(i==OPLOOPS-1)
		{
			log_e("pause_on: communication failed.");
			return -1;
		}
	}
	return 0;
}
int pause_off()
{
	for(int i = 0; i < OPLOOPS && 1 != modbus_write_bit(ctx, PAUSE_ad, 0); i++){
		if(i==OPLOOPS-1)
		{
			log_e("pause_off: communication failed.");
			return -1;
		}
	}
	return 0;
}
// 取消当前任务，运行停止
int positioning_cancel_on()
{
	for(int i = 0; i < OPLOOPS && 1 != modbus_write_bit(ctx, PST_CANCEL_ad, 1); i++){
		if(i==OPLOOPS-1)
		{
			log_e("positioning_cancel_on: communication failed.");
			return -1;
		}
	}
	return 0;
}
int positioning_cancel_off()
{
	for(int i = 0; i < OPLOOPS && 1 != modbus_write_bit(ctx, PST_CANCEL_ad, 0); i++){
		if(i==OPLOOPS-1)
		{
			log_e("positioning_cancel_off: communication failed.");
			return -1;
		}
	}
	return 0;
}

// 停止，并按照惯性自动停下。请慎重!
// free run
// warn: if free-run is turned on, operation is stoped and the motor keeps rotating due to the inertia of the load.
int free_run_on()
{
	for(int i = 0; i < OPLOOPS && 1 != modbus_write_bit(ctx, FREE_RUN_ad, 1); i++){
		if(i==OPLOOPS-1)
		{
			log_e("free_run_on: communication failed.");
			return -1;
		}
	}
	return 0;
}
int free_run_off()
{
	for(int i = 0; i < OPLOOPS && 1 != modbus_write_bit(ctx, FREE_RUN_ad, 0); i++){
		if(i==OPLOOPS-1)
		{
			log_e("free_run_off: communication failed.");
			return -1;
		}
	}
	return 0;
}

