#ifndef AM335X_SETTING_H
#define AM335X_SETTING_H

#include    "global_setting.h"

#ifdef __cplusplus
extern "C" {
#endif

// 编码器零点
#define ENCODER_ZERO_POSITION   (0)
// 编码器的一周编码131071
#define E_PULSE_PER_CIRCLE      (131071)
// 先验条件，每次编码器的更新值不会超过总程的一半
// 超过的情况则是在零点摆动
#define MAX_STRIDE	(65535)


// 串口参数对象
struct uart_t;
typedef struct uart_t uart_t;

struct uart_t {
    char device[32];
    int baud;
    char parity;
    int data_bit;
    int stop_bit;
};


// 判断编码器是否已经工作
int is_encoder_enable();
// 获取当前编码器的角度
double get_encoder_angle();
// 获取当前编码器的速度
double get_encoder_speed();
// 获取当前编码器的运动
MovementStatus get_encoder_movement();

int  listening_uart(const char* device, int baud, char parity, int data_bit, int stop_bit);
void close_uart();

#ifdef __cplusplus
}
#endif

#endif	//AM335X_SETTING_H
