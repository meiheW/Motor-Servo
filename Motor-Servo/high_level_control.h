#ifndef HIGH_LEVEL_CONTROL_H
#define HIGH_LEVEL_CONTROL_H

#include "alpha_motion_control.h"

// 角度conf
#define ANGLE_CONF  10000


#ifdef __cplusplus
extern "C" {
#endif
// 发送错误消息
// 格式: $E报警代码，其他故障码\r\n
// 其他故障:使用寿命预警、RS485故障、OT信号
int send_error_msg();
// 报警重置，ON边缘复位
int alarm_reset();
// 位置预置，在ON边缘上将当前位置及反馈位置设置为PA2_19寄存器内保存的设定值
// 注意，在伺服电机停止的情况下预置
int position_reset();

// 取消当前任务
int task_cancel();
// 急停
int force_stop();
// 转到指定角度
int goto_point(double angle);
// 左转
int goto_left();
// 右转
int goto_right();
// 速度设定，输入: 度/秒
int set_speed_value(double speed);
// 获取当前速度，返回: 度/秒
double get_speed_value();
// 加速时间设定，输入: 0.1ms
int set_acce_value(double acce_time);
// 获取加速时间，返回: 0.1ms
int get_acce_value();
// 减速时间设定，输入: 0.1ms
int set_dece_value(double dece_time);
// 获取减速时间，输入: 0.1ms
int get_dece_value();
//// 自检操作
int check_motion();


#ifdef __cplusplus
}
#endif
#endif // HIGH_LEVEL_CONTROL_H
