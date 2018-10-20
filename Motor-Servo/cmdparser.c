#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <math.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "elog.h"
#include "modbus.h"
#include "global_setting.h"
#include "alpha_setting.h"
#include "high_level_control.h"
#include "am335x_setting.h"


// 记录执行的命令
//volatile GFLAGS last_flags = 0;
// 用于赋值时的互斥
pthread_mutex_t mutex_cmd = PTHREAD_MUTEX_INITIALIZER;
// Socket
int server_port, queue_size;

static int sock = -1;
static int conneted = 0;
struct sockaddr_in clientAddr;

// Socket收发
int m_socket_read(void *buf, size_t count)
{
    pthread_mutex_lock(&mutex_cmd);
    //struct sockaddr_in  XAddr    
    int len = sizeof(clientAddr);    
    int bytes = recvfrom(sock, buf, count, 0, (struct sockaddr*)&clientAddr, (socklen_t*)&len);
    
    if(bytes>0) 
    {
        
        conneted = 1;
    }
    
    
    pthread_mutex_unlock(&mutex_cmd);    
    return bytes;
}
int m_socket_write(const void *buf, size_t count)
{
    int bytes;
    pthread_mutex_lock(&mutex_cmd);
    
    if(conneted){

	    bytes = sendto(sock, buf, count, 0, (struct sockaddr *)&clientAddr, sizeof(clientAddr));

    }else{

	    bytes = 0;
    }
    pthread_mutex_unlock(&mutex_cmd);    
    return bytes;
}
// 信息发送
int message_send(const char* msg)
{
    return m_socket_write(msg,strlen(msg));
}


static void run_cmd(param *temp_x,  CtrlStatus  curstatus )
{
    int ret = 5;
    char buf[64];
    
    switch( temp_x->cmd ){
    
        case GPST_CANCEL:
            update_g_ctrl_status(STOPING);	
            ret = task_cancel();
            break;                    
        case GEMG :
            update_g_ctrl_status(STOPING);
            ret = force_stop();			   
	    ret = serve_off(); //释放伺服
            set_stop(true); // 退出程序
            break;
        case GSPEED:
            if(  FREE != curstatus ){						
                message_send("$D\r\n"); // 未执行
            }
            else{
		      ret = set_speed_value(temp_x->v[0]);
	        }
            break;
        case GACCE_TIME :
            if(  FREE != curstatus ){						
                message_send("$D\r\n"); // 未执行
            }
            else{
		       ret = set_acce_value(temp_x->v[0]);
	        }
            break;
        case GDECE_TIME  :
            if(  FREE != curstatus ){						
                message_send("$D\r\n"); // 未执行
            }
            else{
		    ret = set_dece_value(temp_x->v[0]);
	        }
            break;                    
       case GMAX_POINT:               		
            // degreea			        
            //??????????? same value
            if(  FREE != curstatus ){						
                message_send("$D\r\n"); // 未执行
            }
            else{
		        double left_angle = temp_x->v[0];
		        double right_angle = temp_x->v[1];
		        set_g_left_angle(left_angle);
		        set_g_right_angle(right_angle);
		        ret = 0;
	        }
            break;
        case   GCHECK:			
            if(  FREE != curstatus ){						
                message_send("$D\r\n"); // 未执行
            }
            else{
		        update_g_ctrl_status(LEFTORRIGHT);
		        ret = check_motion();
	        }
            break;
        case   GERROR_MSG:			
            if(  FREE != curstatus ){						
                message_send("$D\r\n"); // 未执行
            }
            else{

		        ret = send_error_msg();
	        }
            break;
        case   GALARM_RST:
            if(  FREE != curstatus ){						
                message_send("$D\r\n"); // 未执行
            }
            else{
		        ret = alarm_reset();
	        }
            break;
        case  GPOINT:			
            //printf("signal_handler: point %f\n", temp.v[0]);
            if(  FREE != curstatus ){						
                message_send("$D\r\n"); // 未执行
            }
            else{
                
		        update_g_ctrl_status(LOCATING);
		        ret = goto_point(temp_x->v[0]);
                //message_send("$C\r\n"); // 指令被执行

		//double cur_angle = get_encoder_angle();
		//if( fabs(cur_angle - temp_x->v[0] ) > 0.005) {
		//    update_g_ctrl_status(LOCATING);
		//    ret = goto_point(temp_x->v[0]);
		//    //message_send("$C\r\n");	
		//}else{
		//    message_send("$D\r\n"); // 未执行
		//}		                
                
            }
            break;
        case GLEFT:			
            //printf("signal_handler: left\n");
            if(  FREE != curstatus ){						
                message_send("$D\r\n"); // 未执行
            }
            else{
                update_g_ctrl_status(LEFTORRIGHT);
                ret = goto_left();
                //message_send("$C\r\n");	
            }
            break;
        case  GRIGHT: 
	    //printf("signal_handler: right\n");
	    if(  FREE != curstatus ){						
                message_send("$D\r\n"); // 未执行
            }
            else{
                update_g_ctrl_status(LEFTORRIGHT);
                ret = goto_right();
                //message_send("$C\r\n");	
            }
            break;
		case GSUM:		//2018.3.19 wmh
			if(FREE != curstatus){
				message_send("$D\r\n");
			}
			else{
				//send the g_sum_angle;

				char databuf[64]={0};
				get_data_buf(databuf);
				
				int j=0;
				j = sprintf(buf,   "$H%.4f,", get_g_sum_angle());
				j = sprintf(buf+j, "%s\r\n", databuf);
				m_socket_write(buf, strlen(buf));
			}

			break;
	case GSPEEDMSG:
	    // 角度信息格式: $G速度，加速时间，减速时间\r\n
	    sprintf(buf,"$G%.3f,%05d,%05d\r\n",get_speed_value(),get_acce_value(),get_dece_value());
	    int bytes = m_socket_write(buf,strlen(buf));
	    break;
        default:
            break;
    
    }

}


// 套接字监听
//***************************************************************
void parsesocket(void)
{

    if( -1 == sock ){
    
	    // udp addr
	    struct sockaddr_in addr;    // holds UDP IP address
	    memset(&addr, 0, sizeof(addr));
	    addr.sin_family = AF_INET;
	    addr.sin_addr.s_addr = htonl(INADDR_ANY);
	    addr.sin_port = htons(server_port);

	    // socket
	    sock = socket(AF_INET, SOCK_DGRAM, 0);
	    if(sock < 0){
	        log_e("socket");
	        exit(-1);
	    }

	    int flags = fcntl(sock, F_GETFL, 0); // 获取当前状态
	    fcntl(sock, F_SETFL, flags|O_NONBLOCK); // 非阻塞设置

	    // bind
	    if(bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0){
	        perror("bind");
	        exit(-1);
	    }
    }

	



	// Init Done
	int bytes = 0;
	char buf[128];

	CtrlStatus  curstatus; // 运行状态
	param temp_x; //控制参数
	// 监听: 信号解析与执行
	while(1)
	{
	    bytes = m_socket_read(buf,127);	
	    if(bytes > 0) {  
	    
	        //important
	        buf[bytes]=0;
            
            
            
	        //// TODO 指令解析 ////	注意，指令之间严禁有包含关系
	        temp_x.cmd = 0;
	        if( strstr(buf,"cancel") ){
	            //printf("cmdparse: cancel");
	            temp_x.cmd = GPST_CANCEL;
	        }
	        else if( strstr(buf,"stop") ){
	            //printf("cmdparse: stop");
	            temp_x.cmd = GEMG;
	        }
		    else if(buf == strstr(buf, "point")){

	            double position;
	            sscanf(buf,"point %lf",&position);
		    //printf("point %lf\n", position);

	            if(get_anticlockwise()) 
	                 temp_x.v[0] = (-position);
	            else temp_x.v[0] = position;
	            temp_x.cmd = GPOINT;
	        }
	        else if( strstr(buf,"runleft") ){
	            if(get_anticlockwise()) temp_x.cmd = GRIGHT;
	            else temp_x.cmd = GLEFT;
	        }
	        else if( strstr(buf,"runright") ){
	            if(get_anticlockwise()) temp_x.cmd = GLEFT;
	            else temp_x.cmd = GRIGHT;
	        }
	        else if(buf == strstr(buf,"speed")){

	            double speed;
	            sscanf(buf,"speed %lf",&speed);
		    //printf("speed %lf\n", speed);
	            temp_x.v[0] = speed;
	            temp_x.cmd = GSPEED;
	        }
	        else if(buf == strstr(buf,"acce")){
	
	            double acce;
	            sscanf(buf, "acce %lf",&acce);
		    //printf("acce %lf\n", acce);
	            temp_x.v[0] = acce;
	            temp_x.cmd = GACCE_TIME;
	        }
	        else if(buf == strstr(buf,"dece")){
	
	            double dece;
	            sscanf(buf, "dece %lf",&dece);
		    //printf("dece %lf\n", dece);
	            temp_x.v[0] = dece;
	            temp_x.cmd = GDECE_TIME;
	        }
	        else if(buf == strstr(buf,"maxpoint")){
	
	            double max_left, max_right;
	            sscanf(buf, "maxpoint %lf %lf",&max_left, &max_right);
		    //printf("maxpoint %lf %lf\n", max_left, max_right);
	            
	            if(get_anticlockwise()) max_left *= -1, max_right *= -1;
	            
	            if(max_left > max_right){
	            
		            double temp = max_right;
		            max_right = max_left;
		            max_left = temp;
	            }
	            temp_x.v[0] = max_left;
	            temp_x.v[1] = max_right;
	            temp_x.cmd = GMAX_POINT;
	        }else if(strstr(buf,"sum") ){	//2018.3.19 wmh
				temp_x.cmd = GSUM;
			}
	        else if( strstr(buf,"check") ){
	            temp_x.cmd = GCHECK;
	        }
	        else if( strstr(buf,"errormsg") ){
	            temp_x.cmd = GERROR_MSG;
	        }
	        else if( strstr(buf,"alarmrst") ){
	            temp_x.cmd = GALARM_RST;
	        }
		    else if( strstr(buf,"spdmsg") ){
		        temp_x.cmd = GSPEEDMSG;
		    }
	        else{
		        temp_x.cmd = 0;
	        }

	        curstatus = get_g_ctrl_status();
	        //if initiliazing then return;
	        if(  INITLIZING == curstatus || NONE == curstatus ){
	
	            message_send("$D\r\n"); // 未执行
	            //continue;		
	        }else{
		    // 指令执行
		        run_cmd(&temp_x,curstatus);	
		    }

	    } //END IF(BYTES>0)
		
		////delay for cmd running
		usleep(40000);	    // 40ms
		

		curstatus = get_g_ctrl_status();
		if( is_INP() ){
			
		    // Debug语句
		    //printf("is inp\n");
		    if( LOCATING == curstatus ){							
		        message_send("$B\r\n"); // position到位			    
		    }else if ( INITLIZING == curstatus ){	
		    	    
		        log_i("Init Success.");		    
		    }
		    // 更新为空闲状态 
		    update_g_ctrl_status(FREE);
		}
		
    		
		// // 到位强制停止
		// if( LOCATING == curstatus ){
		//      //printf("ccc...\n");
		//      double curangle = get_encoder_angle();
		//      double diff_angle = curangle - get_destination_angle(); 
		//      if (fabs(diff_angle) <= 0.005){                          
		// 	 int r = task_cancel();
		// 	 //if(-1 == r) update_g_ctrl_status(ERROOR);
		//      }
		//  }  

		// TODO(wangzhiheng): 报警检测
		if( 0 == get_out_status(ALRM_DETC_B_ad) ){			
		    update_g_ctrl_status(ERROOR);
		}
		// TODO(wangzhiheng): 用于校对电机与编码器是否同步一致
		// 电机50r/min,零速度信号就响应, 对应编码器为0.27个脉冲/ms,线性减速则平均速度为0.13脉冲/毫秒
		// 注意:由于减速宽慢的问题，要求编码器测速的时间间隔不能太大
		double speed = get_encoder_speed();
		if(fabs(speed) < 0.1){
		    
		    if(1 != get_motor_zero_speed()){ // 零速度信号判断

			update_g_ctrl_status(ERROOR);
			message_send("$F\r\n"); // 编码器故障信息
		        //set_stop(true);
		    }
		}
		// TODO(wangzhiheng): 超程判断
		// 超程时，将+OT或-OT置ON(1)，则电机的OT输出信号将有响应，errormsg可查看
		// 超程时，超程时，以PA2_60设定的方式减速停止，仅能反方向运动，或者手动进给
		double curangle = get_encoder_angle();
		if( curangle < (get_g_left_angle()-0.1) ){

		    set_cont_status(OT_MINUS_ad,1);
		}
		else if( curangle > (get_g_right_angle()+0.1) )	{

		    set_cont_status(OT_PLUS_ad,1);

		} else {
		    set_cont_status(OT_MINUS_ad,0);
		    set_cont_status(OT_PLUS_ad,0);
		}
		// TODO(wangzhiheng): RS485数据，寿命预警
		// printf("RS485 data error: %d\n", get_out_status(DATA_ERROR_ad));
		// printf("Life Time Warn: %d\n", get_out_status(LIFE_WRNING_ad));
		if(1 == get_out_status(DATA_ERROR_ad)){
		    update_g_ctrl_status(ERROOR);
		}
        if(1 == get_out_status(LIFE_WRNING_ad)){
		    update_g_ctrl_status(ERROOR);
		}

	} // END WHILE(1)
}

int listening_socket(int _server_port, int _queue_size)
{
	server_port = _server_port;
	queue_size = _queue_size;
	pthread_t parsesocketid;

	if(0 != pthread_create(&parsesocketid,NULL,(void*)parsesocket,NULL)) return -1;
	if(0 != pthread_detach(parsesocketid)) return -1;
	return 0;
}
