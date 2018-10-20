//*************************************************************************************************************************************
//Notes:    Priority among Input Signals(请参照以下的输入信号优先级)
//Section            Description                                    Application signal(信号功能码)
//  01               (Operation) Highest Priority                   54,  1
//  02               (Operation) Priority                           10, 34
//  03               Controlling the torque                         19, 20 
//  04               Stopping the motor                              7,  8, 26, 31, 32, 50
//  05               turning the motor                               2,  3,  4,  5
//  06               determing the home position                     6,  7,  8,  49,16
//  07               Signal irrelativant to motor operation          11,55
//*************************************************************************************************************************************
#ifndef ALPHA_MOTION_CONTROL_H
#define ALPHA_MOTION_CONTROL_H

#include "modbus.h"
#include "global_setting.h"
#include "alpha_setting.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ABS_POSITION_MODE   (0)
#define INC_POSITION_MODE   (1)


// 获取伺服电机的运动状态
MovementStatus get_motor_movement();

// 获取OUT线圈状态
// 返回-1表示通信失败，未读取到数据
int get_out_status(int out_addr);
// 设置CONT线圈状态
// 返回-1表示通信失败，未设置成功
int set_cont_status(int cont_addr, int status);
// Debug函数，获取当前CONT线圈状态
int get_cont_status(int cont_addr);

// 增量旋转
int run_inc_angle(double angle);
// 运行到指定角度 (绝对模式)
int run_to_angle(double angle);

// 运动模式
int set_abs_control_mode(); //绝对位置模式
int set_inc_control_mode();
int immediate_value_operation_run(); //运行，run
// 运行速度
int set_cruise_speed(uint32_t speed);
uint32_t get_cruise_speed();
int send_cruise_speed();
// 加速时间
int set_imme_acceleration_time(uint32_t time);
uint32_t get_imme_acceleration_time();
int send_imme_acceleration_time();
// 减速时间
int set_imme_deceleration_time(uint32_t time);
uint32_t get_imme_deceleration_time();
int send_imme_deceleration_time();
// 自检运动速度设置
int set_check_speed(uint32_t speed);
int send_check_speed();
int set_check_acce_time(uint32_t time);
int send_check_acce_time();
int set_check_dece_time(uint32_t time);
int send_check_dece_time();

// 立即数位置到达
int is_INP();
// 伺服
int serve_on();
int serve_off();
int is_ready();

// 暂停/停止
int forced_stop_on();
int forced_stop_off();
int pause_on();
int pause_off();
int positioning_cancel_on();
int positioning_cancel_off();
int free_run_on();
int free_run_off();

#ifdef __cplusplus
}
#endif
#endif	//ALPHA_MOTION_CONTROL_H
