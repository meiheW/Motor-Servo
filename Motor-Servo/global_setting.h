#ifndef GLOBAL_SETTING_H
#define GLOBAL_SETTING_H

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>


#ifdef __cplusplus
extern "C" {
#endif
// 用于控制的信号编码
typedef enum{
    GNONE=0,
    GRIGHT,
    GLEFT,
    GPOINT,
    GPST_CANCEL,
    GEMG,
    GSPEED,
    GACCE_TIME,
    GDECE_TIME,
    GMAX_POINT,
    GCHECK,
    GERROR_MSG,
    GALARM_RST,
    GSPEEDMSG,
	GSUM
} GFLAGS;
// 控制信号的数据对象格式
typedef struct {
    GFLAGS cmd;
    double v[2];
}param;

// 运动方向
typedef enum{
    NOTMOVE=1,
    LEFTMOVE,
    RIGHTMOVE
}MovementStatus;

// 控制器状态
typedef enum{
    NONE=0,
    FREE,
    INITLIZING,
    STOPING,
    LOCATING,
    LEFTORRIGHT,
    FINISH,
    ERROOR,
}CtrlStatus;


// 更新/读取控制变量
void  update_g_x(param x);
param get_g_x();

// 更新/读取控制状态
void       update_g_ctrl_status(CtrlStatus status);
CtrlStatus get_g_ctrl_status();
  

// 设置退出主程序，释放伺服开关
void set_stop(int mode);
int  get_stop();

//2018.3.19 add:wmh
void update_g_sum_angle(double angle);
double get_g_sum_angle();

int read_g_sum_angle();
int write_g_sum_angle();

void get_data_buf(char* buf);


//2018/9/8
void set_encode_index(int encode_index);
int get_encode_index();
int get_pulse_per_circle();


// 启动角度、极限角度设定
void   set_g_start_angle(double angle);
double get_g_start_angle();
void   set_g_left_angle(double angle); 
double get_g_left_angle();
void   set_g_right_angle(double angle);
double get_g_right_angle();
// 更新/获取目标角度
void   update_destination_angle(double angle);
double get_destination_angle();
// 伺服电机零速度更新/获取
void set_motor_zero_speed();
int  get_motor_zero_speed();
// 用于运动方面的反转，安装时用于调整默认方向
void set_anticlockwise(int mode);
int  get_anticlockwise();

#ifdef __cplusplus
}
#endif
#endif // GLOBAL_SETTING_H
