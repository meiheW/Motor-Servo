#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <math.h>
#include <stddef.h>
////////////////////////
#include <sys/shm.h>
///////////////////////
#include "elog.h"
#include "iniparser.h"

#include "global_setting.h"
#include "alpha_setting.h"
#include "high_level_control.h"
#include "am335x_setting.h"
#include "cmdparser.h"

/////////////////////////////////////////////
#define   SHMKEY        (5432)
int                     g_shmid   =-1;
void *                  g_shmaddr = NULL;
const int               g_shsize  = 64;
////////////////////////////////////////////
// 保存Socket参数的对象
am335x_socket_t g_am335x_socket;
// 保存modbus参数的对象
rtu_master_t g_rtu_master;
// 保存串口参数的对象
uart_t g_am335x_uart;
// 配置表创建与解析函数
void create_example_ini_file(void);
int  parse_ini_file(char* ini_name);


int main(int argc, char** argv)
{
    int  ret;
//////////////////////
    char shmbuf[32];
////////////////////////
    // EasyLogger Log配置,创建log文件的语句在 elog_port.c
    elog_init();
    elog_set_fmt(ELOG_LVL_ASSERT, ELOG_FMT_ALL);
    elog_set_fmt(ELOG_LVL_ERROR, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_WARN, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_INFO, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_DEBUG, ELOG_FMT_ALL & ~ELOG_FMT_FUNC);
    elog_set_fmt(ELOG_LVL_VERBOSE, ELOG_FMT_ALL & ~ELOG_FMT_FUNC);
    // Start EasyLogger
    elog_start();

    // 读配置表
    ret = parse_ini_file("/home/root/configure.ini");
    if(-1 == ret){
    
        create_example_ini_file();
        ret = parse_ini_file("/home/root/configure.ini");
        if(-1 == ret){
            log_e("Init from configure.ini failed.");
            return -1;
        }
    }
    //log_i("Init from configure.ini success.");
	
	ret = read_g_sum_angle();	
	if(-1 == ret){
	
		log_e("read sum angle error.");
		return -1;
	}

    // 初始化modbus通信使用的内存缓冲区
    ret = init_buffers_for_modbus();
    if(-1 == ret){
    
        log_e("Init buffers for modbus communication failed.");
        return -1;
    }
    // 打开modbus控制终端("/dev/ttyO1", 38400, 'E', 8, 1, 1);
    ret = open_modbus_rtu_master(g_rtu_master.device, g_rtu_master.baud, \
            g_rtu_master.parity, g_rtu_master.data_bit, g_rtu_master.stop_bit, g_rtu_master.slave);
    if(-1 == ret){
    	log_e("Open modbus_rtu_master failed.");
    	free_buffers_for_modbus();
    	return -1;
    }

    // 初始化伺服电机控制器的寄存器配置，请慎重！(Warn: parameter setting.)
    if(1 == g_rtu_master.reset_parameter){
        init_parameters();
        log_w("Warn: parameter setting. Dangerous!!!");
    }
    // 检验核实伺服电机控制器的寄存器配置
    if(-1 == check_parameters()){
        log_e("check parameters failed.");
        close_modbus_rtu_master();
        free_buffers_for_modbus();
        return -1;
    }
    

    // 电机进入伺服
    ret = serve_on();
    if(-1 == ret){
        log_e("servo on failed.");
    	close_modbus_rtu_master();
    	free_buffers_for_modbus();
    	return -1;
    }
    // 确认进入就绪态
    ret = is_ready();
    if(ret != 1){
    	log_e("servo_on is ON, but is_ready OFF.");
    	serve_off();
    	close_modbus_rtu_master();
    	free_buffers_for_modbus();
        close_uart();
    	return -1;
    }
    /// 发送运行速度、加速时间、减速时间到控制器
    ret = send_cruise_speed();
    if(-1 == ret) set_stop(true);
    ret = send_imme_acceleration_time();
    if(-1 == ret) set_stop(true);
    ret = send_imme_deceleration_time();
    if(-1 == ret) set_stop(true);
    
    
    // 串口监听，监听编码器信息("/dev/ttyO2", 9600, 'N', 8, 1);
    ret = listening_uart(g_am335x_uart.device, g_am335x_uart.baud, \
            g_am335x_uart.parity, g_am335x_uart.data_bit, g_am335x_uart.stop_bit);
    if(-1 == ret){
    	log_e("Open am335x uart failed.");
    	close_modbus_rtu_master();
    	free_buffers_for_modbus();
    	return -1;
    }
    //// TODO(wangzhiheng): 为了获得启动时的实际角度，用于进行零点矫正
    /// 等待编码器响应状态为enable,即编码器有数据
    int waittime = 180; // 3分钟
    for(int i=0; i < waittime; ++i)
    {
        if(!is_encoder_enable()){
            if(i == waittime-1){
                log_e("encoder is not enable.");
                set_stop(true);
                break;
            }
            usleep(1000000); // 1s
            continue;
        }else
            break;
    }

    /// TODO 原点复位 self_chek?
    if(!get_stop())
    {
        ret = position_reset();
        if(-1==ret) set_stop(true);
        update_g_ctrl_status(INITLIZING);
    }

    // 初始化完成！
          
     
    // 监听套接字，解析终端的控制命令
    ret = listening_socket(g_am335x_socket.server_port, g_am335x_socket.queue_size);
    if(-1 == ret){
        log_e("Open am335x ethernet port failed.");
        close_modbus_rtu_master();
        free_buffers_for_modbus();
        close_uart();
        return -1;
    }


    //////////////////////shm create////////////////////////////////////
    g_shmid = shmget(SHMKEY, 0, 0666);
    if (g_shmid < 0)
    {
        g_shmid = shmget(SHMKEY, g_shsize, IPC_CREAT | IPC_EXCL | 0666);
        if (g_shmid < 0)
        {
            log_e("shmget err!\r\n");
        }
    }

    if( g_shmid >= 0 )
    {
        g_shmaddr = shmat(g_shmid, 0, 0);
        if (g_shmaddr == (void *)-1)
        {
            log_e("shmget memory err!\r\n");
            g_shmaddr = NULL;
        }
        
    }
    /////////////////////////////////////////////////////////////////////

    double curangle=0;
    CtrlStatus  ctrl_status;
    char buf[64];
    //char bufpre[64];
    //memcpy(buf,"buf",strlen("buf")+1);
    //memcpy(bufpre,"bufpre",strlen("bufpre")+1);

    sleep(1);    
    while(!get_stop()) {
        
       ctrl_status = get_g_ctrl_status();
       curangle = get_encoder_angle();
        
        // 获取角度信息
        if(get_anticlockwise()) curangle *= -1;     
        sprintf(buf,"$A%.4f,%1d\r\n",curangle,ctrl_status);

        //if(strcmp(buf,bufpre)){
        //    memcpy(bufpre,buf,strlen(buf)+1);
        //    m_socket_write(buf,strlen(buf));
        //}

        m_socket_write(buf,strlen(buf));
        ////////////////shm copy//////////////

        if( g_shmaddr > 0 )
        {
            memcpy(shmbuf,&curangle,sizeof(curangle));
            memcpy(shmbuf+8,&ctrl_status,sizeof(ctrl_status));
            memcpy(g_shmaddr, shmbuf, 32 );  
            
        }
        ////////////////////////////////////
        
        switch (ctrl_status) {
        
           case INITLIZING:
               break;
           default:
               break;
        
        } 

       usleep(5000); // 5ms, 200Hz发送频率

    }

    // 释放伺服，并退出程序
    serve_off();
    close_modbus_rtu_master();
    free_buffers_for_modbus();
    close_uart();
    //log_i("Exit Success.");
    elog_close();
    return 0;
}



void create_example_ini_file(void)
{
    FILE* ini;
    if ((ini=fopen("/home/root/configure.ini", "w"))==NULL) {
        log_e("Create *.ini failed.");
        return ;
    }

    // Write configure into *.ini
    fprintf(ini,

    "[IP]"                                  "\n"
    "RemoteIP = 192.168.0.15"               "\n"
    "RemoteName = root"                     "\n\n"

    "[Motion Control]"                      "\n"
    "anticlockwise = 0"                     "\n"
    "max_left_position = -80"               "\n"    // degree
    "max_right_position = 80"               "\n"    // degree
    "speed = 2.4"                           "\n"    // degree/s
    "imme_acceleration_time = 100"          "\n"    // 0.1ms    : 10000 means 1s
    "imme_deceleration_time = 100"          "\n"    // 0.1ms
    "check_speed = 5"                       "\n"    // degree/s 
    "check_acce_time = 100"                 "\n"    // 0.1ms    : 20000 means 2s
    "check_dece_time = 100"                 "\n\n"  // 0.1ms

    "[RTU MASTER]"                          "\n"
    "device = /dev/ttyO3"                   "\n"
    "baud = 38400"                          "\n"
    "parity = E"                            "\n"
    "data_bit = 8"                          "\n"
    "stop_bit = 1"                          "\n"
    "slave = 1"                             "\n"
    "RESET = 0"                             "\n\n"

    "[UART]"                                "\n"
    "device = /dev/ttyO2"                   "\n"
    "baud = 57600"                          "\n"
    "parity = N"                            "\n"
    "data_bit = 8"                          "\n"
    "stop_bit = 1"                          "\n"
    "encode_index = 3"                      "\n\n"

    "[SOCKET]"                              "\n"
    "server_port = 12345"                   "\n"
    "queue_size = 1"                        "\n"
    "mode = UDP"                            "\n\n"
    
    );

    fclose(ini);
    log_i("Create configure.ini success.");
}

int parse_ini_file(char * ini_name)
{
    dictionary  *   ini ;

    // Temporary variables
    double max_left_position;
    double max_right_position;
    uint32_t speed;
    uint32_t imme_acceleration_time;
    uint32_t imme_deceleration_time;
    uint32_t check_speed;
    uint32_t check_acce_time;
    uint32_t check_dece_time;

    ini = iniparser_load(ini_name);
    if (ini==NULL) {
        log_e("Load configure.ini failed.");
        return -1 ;
    }
    iniparser_dump(ini, stderr);

    // 坐标系反转标记
    int anticlock = iniparser_getint(ini, "Motion Control:anticlockwise", 0);
    set_anticlockwise(anticlock);
    // 极限角度
    double temp_angle = iniparser_getdouble(ini, "Motion Control:max_left_position", -80);
    max_left_position = temp_angle;
    temp_angle = iniparser_getdouble(ini, "Motion Control:max_right_position", 80);
    max_right_position = temp_angle;
    // 速度
    double temp_speed = iniparser_getdouble(ini, "Motion Control:speed", 2.4);
    speed = temp_speed*60*100*TRANSMISSION_RATIO/360;
    temp_speed = iniparser_getdouble(ini, "Motion Control:check_speed", 2.4);
    check_speed = temp_speed*60*100*TRANSMISSION_RATIO/360;
    // 加减速时间，默认为100×0.1ms
    imme_acceleration_time = iniparser_getint(ini, "Motion Control:imme_acceleration_time", 100);
    imme_deceleration_time = iniparser_getint(ini, "Motion Control:imme_deceleration_time", 100);
    check_acce_time = iniparser_getint(ini, "Motion Control:check_acce_time", 100);
    check_dece_time = iniparser_getint(ini, "Motion Control:check_dece_time", 100);

    // Modbus 
    const char* ptr;
    ptr = iniparser_getstring(ini, "RTU MASTER:device", "/dev/ttyO3");
    memcpy(g_rtu_master.device, ptr, strlen(ptr));
    g_rtu_master.device[strlen(ptr)] = '\0';

    ptr = iniparser_getstring(ini, "RTU MASTER:parity", "E");
    memcpy(&g_rtu_master.parity, ptr, 1);
    g_rtu_master.baud = iniparser_getint(ini, "RTU MASTER:baud", 38400);
    g_rtu_master.data_bit = iniparser_getint(ini, "RTU MASTER:data_bit", 8);
    g_rtu_master.stop_bit = iniparser_getint(ini, "RTU MASTER:stop_bit", 1);
    g_rtu_master.slave = iniparser_getint(ini, "RTU MASTER:slave", 1);
    g_rtu_master.reset_parameter = iniparser_getint(ini, "RTU MASTER:RESET", 0);

    // UART
    ptr = iniparser_getstring(ini, "UART:device", "/dev/ttyO2");
    memcpy(g_am335x_uart.device, ptr, strlen(ptr));
    g_am335x_uart.device[strlen(ptr)] = '\0';

    ptr = iniparser_getstring(ini, "UART:parity", "N");
    memcpy(&g_am335x_uart.parity, ptr, 1);
    g_am335x_uart.baud = iniparser_getint(ini, "UART:baud", 57600);
    g_am335x_uart.data_bit = iniparser_getint(ini, "UART:data_bit", 8);
    g_am335x_uart.stop_bit = iniparser_getint(ini, "UART:stop_bit", 1);

    //2019/9/8-----encoder_index
    int encode_index = iniparser_getint(ini, "UART:encode_index", 3);
    set_encode_index(encode_index);
    
    // Socket
    g_am335x_socket.server_port = iniparser_getint(ini, "SOCKET:server_port", 12345);
    g_am335x_socket.queue_size = iniparser_getint(ini, "SOCKET:queue_size", 1);

    // 设置最大偏转角
    set_g_left_angle(max_left_position);
    set_g_right_angle(max_right_position);
    // 速度，注意设置之后需要发送到寄存器才有效，其对应发送在主函数开始部分
    set_cruise_speed(speed);
    set_imme_acceleration_time(imme_acceleration_time);
    set_imme_deceleration_time(imme_deceleration_time);
    set_check_speed(check_speed);
    set_check_acce_time(check_acce_time);
    set_check_dece_time(check_dece_time);
   
    iniparser_freedict(ini);
    return 0 ;
}
