#include <string.h>
#include <math.h>
#include "global_setting.h"
#include "cmdparser.h"
#include "am335x_setting.h"
#include "alpha_motion_control.h"
#include "high_level_control.h"


// 发送错误消息
// 格式: $E报警代码，其他故障码\r\n
// 其他故障:使用寿命预警、RS485故障、OT信号
int send_error_msg()
{
    //// Debug语句
    //printf("come in: send_error_msg()\n");

    int ret=0;
    char sendbuf[64];
    memset(sendbuf,0,64);

    // 获取故障码
    int alm0 = get_out_status(ALRM_CODE0_ad);
    if(-1==alm0) return -1;
    int alm1 = get_out_status(ALRM_CODE1_ad);
    if(-1==alm1) return -1;
    int alm2 = get_out_status(ALRM_CODE2_ad);
    if(-1==alm2) return -1;
    int alm3 =  get_out_status(ALRM_CODE3_ad);
    if(-1==alm3) return -1;
    int alm4 =  get_out_status(ALRM_CODE4_ad);
    if(-1==alm4) return -1;
    // 其他故障
    int life_alarm = get_out_status(LIFE_WRNING_ad);
    if(-1==life_alarm) return -1;
    int rs485_alarm =  get_out_status(DATA_ERROR_ad);
    if(-1==rs485_alarm) return -1;
    int ot_alarm =  get_out_status(OT_DETC_ad);
    if(-1==ot_alarm) return -1;

    sprintf(sendbuf, "$E%1d%1d%1d%1d%1d,%1d%1d%1d\r\n",alm4,alm3,alm2,alm1,alm0,life_alarm,rs485_alarm,ot_alarm);
    ret = m_socket_write(sendbuf,strlen(sendbuf));
    
    //// Debug语句
    //printf("send %d bytes error codes\n", ret);
    //printf("error message: %s\n", sendbuf);

    if(ret > 0) return 0;
    else return -1;
}

// 报警重置，ON边缘复位
int alarm_reset()
{
    //// Debug 获取RST当前状态
    //printf("in function: alarm reset.\n ");
    //printf("current RST CONT status: %d\n", get_cont_status(RST_ad));
    int ret=0;

    ret = set_cont_status(RST_ad,1);
    if(-1==ret) return -1;
    ret = set_cont_status(RST_ad,0);

    //printf("alarm reset success.\n");
    return ret;
}

// 位置预置，在ON边缘上将当前位置及反馈位置设置为PA2_19寄存器内保存的设定值
// 偏差清除，PA3_36设置有效形态，默认为ON边缘上有效
// 注意，在伺服电机停止的情况下预置
int position_reset()
{
    int ret=0;
    
    ret = task_cancel(); //取消当前运动
    if(-1==ret) return -1;

    // 等待停止
    int waittime = 30; // 30* 200ms
    for(int i=0; i < waittime; ++i)
    {
        if(!is_INP())
	{
            if(i == waittime-1){
		return -1;
            }
            usleep(200000); // 200ms
            continue;
	}
	else
	{
	    break;
	}
    }

    // 位置预置
    ret = set_cont_status(PST_PRESET_ad,1);
    if(-1==ret) return -1;
    ret = set_cont_status(PST_PRESET_ad,0);
    if(-1==ret) return -1;
    // 偏差清除
    ret = set_cont_status(DEVIATION_CLR_ad,1);
    if(-1==ret) return -1;
    ret = set_cont_status(DEVIATION_CLR_ad,0);

    return ret;
}


// 取消当前运动
int task_cancel()
{
	int ret;
	
    ret = positioning_cancel_on();
    if(-1 == ret) return -1;
    ret = positioning_cancel_off();
    if(-1 == ret) return -1;

    return 0;
}

// 急停
int force_stop()
{
    int ret;

    ret = forced_stop_on();
    if(-1 == ret) return -1;
    ret = forced_stop_off();
    if(-1 == ret) return -1;

    return 0;
}

// 转到指定角度
int goto_point(double angle)
{
    //// debug语句
    //double test_angle = get_encoder_angle();
    //printf("get encoder angle %f\n", test_angle);
    //double test_start = get_g_start_angle();
    //printf("zero start angle %f\n", test_start);

    if(angle < get_g_left_angle()) angle = get_g_left_angle();
    if(angle > get_g_right_angle()) angle = get_g_right_angle();
    //更新目标角度
    update_destination_angle(angle);
	//2018.3.19 add:wmh
	double curangle = get_encoder_angle();
	double diff = fabs(curangle - angle);
	update_g_sum_angle(diff / ANGLE_CONF);
	write_g_sum_angle();
	//
    int ret;
    double actual_angle = angle - get_g_start_angle();
    actual_angle *= TRANSMISSION_RATIO;

    //// Debug
    //printf("goto_point %f\n", angle);
    //printf("actual_angle %f\n", actual_angle);

    ret = run_to_angle(actual_angle);

    if(1==ret) return 0;
    else return -1;
}

// 左转
int goto_left()
{
    int ret;
	

    //更新目标角度
    double angle = get_g_left_angle();
    update_destination_angle(angle);
	//2018.3.19 add:wmh
	double curangle = get_encoder_angle();
	double diff = fabs(curangle-angle);
	update_g_sum_angle(diff / ANGLE_CONF);
	write_g_sum_angle();
	//
    double actual_angle = angle - get_g_start_angle();
    actual_angle *= TRANSMISSION_RATIO;
    ret = run_to_angle(actual_angle);

    if(1==ret) return 0;
    else return -1;
}

// 右转
int goto_right()
{
    int ret;

    //更新目标角度
    double angle = get_g_right_angle();
    update_destination_angle(angle);
    //2018.3.19 add:wmh
	double curangle = get_encoder_angle();
	double diff = fabs(curangle - angle);
	update_g_sum_angle(diff / ANGLE_CONF);
	write_g_sum_angle();
	//
    double actual_angle = angle - get_g_start_angle();
    actual_angle *= TRANSMISSION_RATIO;
    ret = run_to_angle(actual_angle);

    if(1==ret) return 0;
    else return -1;
}

// 速度设定
// 输入: 度/秒
int set_speed_value(double speed)
{
	int ret;

	//uint32_t actual_speed = speed*60*100*TRANSMISSION_RATIO/360; //寄存器单位0.01r/min
	
	//TODO: dlt_threshold = 100 约束条件，am335_setting.c
	// double dlt = (speed/360) * (65535) * (16/1000)
	double dlt = (double)speed*E_PULSE_PER_CIRCLE/22500.0;
	if(dlt>100) speed = 34.0;
	
	uint32_t actual_speed = (int)(speed*100.0*TRANSMISSION_RATIO/6.0); //寄存器单位0.01r/min
	
	set_cruise_speed(actual_speed);
	ret = task_cancel(); //取消当前运动
	if(-1 == ret) return -1;
	ret = send_cruise_speed();
	if(-1==ret) return -1;

	return 0;
}
// 获取当前速度
// 返回: 度/秒
double get_speed_value()
{
	uint32_t cruise_speed = get_cruise_speed(); // 0.01r/min
	double speed = (double)cruise_speed*360/(60*100)/TRANSMISSION_RATIO; // degree/s
	return speed;
    
}

// 加速时间设定
// 输入: 0.1ms
int set_acce_value(double acce_time)
{
	int ret;

	set_imme_acceleration_time(acce_time);
	ret = task_cancel(); //取消当前运动
	if(-1 == ret) return -1;
	ret = send_imme_acceleration_time();
	if(-1==ret) return -1;

	return 0;
}
// 获取加速时间
// 返回: 0.1ms
int get_acce_value()
{
	int acce_time = get_imme_acceleration_time();
	return acce_time;
}

// 减速时间设定
// 输入: 0.1ms
int set_dece_value(double dece_time)
{
	int ret;

	set_imme_deceleration_time(dece_time);
	ret = task_cancel(); //取消当前运动
	if(-1 == ret) return -1;
	ret = send_imme_deceleration_time();
	if(-1==ret) return -1;

	return 0;
}
// 获取减速时间
// 输入: 0.1ms
int get_dece_value()
{
	int dece_time = get_imme_deceleration_time();
	return dece_time;
}

//// 自检操作
int check_motion()
{
    int ret;

    ret = task_cancel(); //取消当前运动
    if(-1 == ret) return -1;
    // 速度设定
    ret = send_check_speed();
    if(-1 == ret) return -1;
    ret = send_check_acce_time();
    if(-1 == ret) return -1;
    ret = send_check_dece_time();
    if(-1 == ret) return -1;

    ret = goto_point(0);
    if(-1 == ret) return -1;
    while(!is_INP()){
	    usleep(10000); // 10ms
    }

    ret = goto_left();
    if(-1 == ret) return -1;
    while(!is_INP()){
	    usleep(10000); // 10ms
    }

    ret = goto_right();
    if(-1 == ret) return -1;
        while(!is_INP()){
	    usleep(10000); // 10ms
    }

    ret = goto_point(0);
    if(-1 == ret) return -1;

    //速度复原
    ret = send_cruise_speed();
    if(-1 == ret) return -1;
    ret = send_imme_acceleration_time();
    if(-1 == ret) return -1;
    ret = send_imme_deceleration_time();
    if(-1 == ret) return -1;

    return ret;
}

