# motor-servo #


## module ##

- alpha parameter configure
    * alpha_setting.h
    * alpha_setting.c
- alpha motion control
    * alpha_motion_control.h
    * alpha_motion_control.c
    * alpha_motion_test.c
- encoder listening
    * am335x_setting.h
    * am335x_setting.c
- console listening
    * cmdparser.h
    * cmdparser.c


## API ##
|           API         |         note       |
|-----------------------|--------------------|
| open_dev()            | 读取参数，确认配置 |
| close_dev()           |                    |
| set_acce_time()       |    (0.1ms)         |
| set_dece_time()       |    (0.1ms)         |
| set_speed()           |    (degree/s)      |
| set_max_left_point()  |    (degree)        |
| set_max_right_point() |    (degree)        |
| run_to_angle()        |    (degree)        |
| run_to_direction()    |    left/right      |
| cancel_task()         |                    |
| stop_run()            |                    |
| check()               |    开机检验        |
| get_status()          |    [角度][状态]    |

note:  
    1. 电机 - positions/circle 40000   
    2. 编码器 - positions/circle 65535  
    3. 转速比 - motor/encoder 252.5  
    4. 角度: %.3f(三位小数)
    5. 状态: INP(or MAXL or MAXR) EOPE(operation error) LIMI(over limit)   
    6. 角度帧头: $  帧尾 '\r'   
    7. 状态帧头: #  帧尾 '\r'   


## C99 Error ##
The timespec comes from POSIX, so you have to 'enable' POSIX definitions:  

    #if __STDC_VERSION__ >= 199901L  
    #define _XOPEN_SOURCE 600  
    #else  
    #define _XOPEN_SOURCE 500  
    #endif /* __STDC_VERSION__ */  
    
    #include <time.h>
    int main()
    {
        struct timespec asdf;
        return 0;
    }

For the warning: implicit declaration of function 'usleep':

    gcc -std=gnu99


## motor-client ##
the client for control the server


## References ##
<https://github.com/stephane/libmodbus>  
<https://github.com/armink/EasyLogger>  
<https://github.com/ndevilla/iniparser>  
