#include     <stdio.h> 
#include     <stdlib.h>
#include     <string.h>
#include     <unistd.h>
#include     <sys/time.h>
#include     <sys/types.h>
#include     <sys/stat.h>  
#include     <fcntl.h>  
#include     <termios.h>
#include     <errno.h> 
#include     <pthread.h> 
#include     <sys/ioctl.h> 
#include     <math.h>

#include    "elog.h"
#include    "am335x_setting.h"
#include    "alpha_motion_control.h"


// 用于判定是否收到编码器的响应
volatile int encoder_is_enable_ = 0;

// 编码器的速度，脉冲/毫秒
volatile double encoder_speed=0;
// 编码器上一时刻的位置,用于计算运动
volatile int prev_encoder_position;

// 编码器的当前位置
volatile int encoder_position;
// 用于编码器返回值保存
int  cur_v;

static char g_recvbuf[128];
static int  g_recvlen = 0;
// 用于变量赋值的互斥
pthread_mutex_t mutex_encoder = PTHREAD_MUTEX_INITIALIZER;

unsigned char uart_cmd[4] ={0XBC, 0XAA, 0XB1,0x00};
int fd=-1;
int speed_arr[] = {B115200, B57600, B38400, B19200, B9600, B4800, B2400, B1200};
int name_arr[] = {115200, 57600, 38400,  19200,  9600,  4800,  2400, 1200};

// 更新编码器的位置
void update_encoder_position(int position)
{
    //pthread_mutex_lock(&mutex_encoder);
    encoder_position = position;
    //pthread_mutex_unlock(&mutex_encoder);
}
// 更新编码器的速度
void update_encoder_speed(double speed)
{
    //pthread_mutex_lock(&mutex_encoder);
    encoder_speed = speed;
    //pthread_mutex_unlock(&mutex_encoder);
}
// 更新编码器的响应标志
void encoder_is_enable()
{
    //pthread_mutex_lock(&mutex_encoder);
    encoder_is_enable_ = 1;
    //pthread_mutex_unlock(&mutex_encoder);
}

// 判断编码器是否已经工作
int is_encoder_enable()
{
    return encoder_is_enable_;
}
// 获取当前编码器的角度
double get_encoder_angle()
{
    //pthread_mutex_lock(&mutex_encoder);
    int position = encoder_position;
    //pthread_mutex_unlock(&mutex_encoder);
    double angle = (double)position * 360.0 / (double)get_pulse_per_circle();
    return angle;
}
// 获取当前编码器的速度
double get_encoder_speed()
{
    //pthread_mutex_lock(&mutex_encoder);
    double speed = encoder_speed;
    //pthread_mutex_unlock(&mutex_encoder);
    return speed;
}
// 获取当前编码器的运动
MovementStatus get_encoder_movement()
{
    //pthread_mutex_lock(&mutex_encoder);
    double speed = encoder_speed;
    //pthread_mutex_unlock(&mutex_encoder);
    MovementStatus direct;
    if(speed > 0) direct = RIGHTMOVE;
    else if(speed < 0) direct = LEFTMOVE;
    else direct = NOTMOVE;
    return direct;
}


unsigned char CRC_check(unsigned char *buf, int n)
{
    unsigned char sum = 0x00;
    for(int i = 0; i < n; i++){
	    sum += buf[i];
    }
    
    return sum;
    
}

// 用于对编码器的返回字符串解析，返回编码器的值
// Check and parse the encoder data serial
// 返回值 -1 表示没有解析到编码器的值
int recv_pst_data(char *str,int len)
{
    //printf("rec len: %d\n", len);
    //fprintf(stderr, "len: %d\n", len);
    static int sz = 0;
    static char buf[32];
    if(len>0){
        memcpy(buf+sz, str, len);
        sz += len;
    }
    
    //put buffer info
    //if (sz > 0){
	//	fprintf(stderr, "[BUFFER LEN] is %d, content is:",sz);
	//	for(int i = 0; i < sz; i++){
	//		fprintf(stderr,"%.2x ",buf[i]);
	//	}
    //    fprintf(stderr, "\n");
	//}

    int startpos=-1;
    for(int i=0;i<sz-2;i++ ){
    
            if( *(buf+i) == 0xff &&
                *(buf+i+1) == 0x81 ) {
                
                startpos = i;
                break;
            }    
    }
    //fprintf(stderr, "startpos : %d\n", startpos);
    if(-1 == startpos) return -1;

    if( startpos + 6 <= sz ){
            // Get number 
            /*unsigned short ret;
            char sorted_buff[2];
            memcpy(&sorted_buff[1],buf+startpos+3,1);
            memcpy(&sorted_buff[0],buf+startpos+4,1);
            memcpy(&ret,sorted_buff,2);*/
            int ret;
            unsigned char sorted_buff[4];
            sorted_buff[3] = 0x00;
            memcpy(&sorted_buff[2],buf+startpos+2,1);
            memcpy(&sorted_buff[1],buf+startpos+3,1);
            memcpy(&sorted_buff[0],buf+startpos+4,1);
            memcpy(&ret, sorted_buff, 4);            
            
            // Get check code
            unsigned char act_chk;
            memcpy(&act_chk,(unsigned char*)buf+startpos+5,1);
            //printf("ori crc: %0x,", act_chk);
            unsigned char chk = CRC_check((unsigned char*)buf+startpos,5);
            //printf("check crc: %0x,", chk);
            // Buffer update
            char temp[32];
            sz = sz - (startpos+6);
            memcpy(temp, buf+startpos+6, sz);
            memcpy(buf,temp,sz);

            if(chk == act_chk)
                return ret;
    }

    return -1;
}

//*************************************************************************************************************************************


#define debugnum(data,len,prefix)  \
{ \
        unsigned int i;   \
        for (i = 0;i < len;i++) { \
                if(prefix)  \
                        printf("0x%02x ",data[i]); \
                else  \
                        printf("%02x ",data[i]); \
        } \
}

void set_speed(int fd, int speed)
{
    int   i;
    int   status;
    struct termios   Opt;
    tcgetattr(fd, &Opt);
    for ( i= 0;  i < sizeof(speed_arr) / sizeof(int);  i++){
    
       	if (speed == name_arr[i]){
       	
           	    tcflush(fd, TCIOFLUSH);
	            cfsetispeed(&Opt, speed_arr[i]);
	            cfsetospeed(&Opt, speed_arr[i]);
	            status = tcsetattr(fd, TCSANOW, &Opt);
	            
	            if (status != 0)
		            perror("tcsetattr fd1");
		            
	            return;
         	}
         	
	    tcflush(fd,TCIOFLUSH);
    }
}
int set_Parity(int fd,int databits,int stopbits,int parity)
{
    struct termios options;
    if  ( tcgetattr( fd,&options)  !=  0)
    {
  	perror("SetupSerial 1");
  	return(FALSE);
    }
    options.c_cflag &= ~CSIZE;
    switch (databits)
    {
    case 7:
	options.c_cflag |= CS7;
	break;
    case 8:
	options.c_cflag |= CS8;
	break;
    default:
	fprintf(stderr,"Unsupported data size\n");
	return (FALSE);
    }
    switch (parity)
    {
    case 'n':
    case 'N':
	options.c_cflag &= ~PARENB;   
	options.c_iflag &= ~INPCK;   
	break;
    case 'o':
    case 'O':
	options.c_cflag |= (PARODD | PARENB); 
	options.c_iflag |= INPCK;           
	break;
    case 'e':
    case 'E':
	options.c_cflag |= PARENB;     
	options.c_cflag &= ~PARODD;
	options.c_iflag |= INPCK;     
	break;
    case 'S':
    case 's':  
	options.c_cflag &= ~PARENB;
	options.c_cflag &= ~CSTOPB;
	break;
    default:
	fprintf(stderr,"Unsupported parity\n");
	return (FALSE);
    }
    switch (stopbits)
    {
    case 1:
	options.c_cflag &= ~CSTOPB;
	break;
    case 2:
	options.c_cflag |= CSTOPB;
	break;
    default:
	fprintf(stderr,"Unsupported stop bits\n");
	return (FALSE);
    }


	options.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL|IXON);
	options.c_oflag &= ~OPOST;
	options.c_lflag &= ~(ECHO|ECHONL|ICANON|ISIG|IEXTEN);

    /* Set input parity option */
    
    if (parity != 'n')
    options.c_iflag |= INPCK;
    options.c_cc[VTIME] = 150; // 15 seconds
    options.c_cc[VMIN] = 0;
    
    
    tcflush(fd,TCIFLUSH); /* Update the options and do it NOW */
    if (tcsetattr(fd,TCSANOW,&options) != 0)
    {
	perror("SetupSerial 3");
	return (FALSE);
    }
    return (TRUE);
}


int recv_proc_data_2(char *str,int *plen )
{

    int startpos=-1;
    int len=*plen;
    for(int i=0;i<len;i++ ){
    
            if( *(str+i) == 0xff &&
                *(str+i+1) == 0x81 ) {
                
                startpos = i;
                break;
            }    
    }
    //fprintf(stderr, "startpos : %d\n", startpos);
    if(-1 == startpos) return -1;

    if( startpos + 5 <= len ){
            // Get number 
            unsigned short ret;
            char sorted_buff[2];
            memcpy(&sorted_buff[1],str+startpos+2,1);
            memcpy(&sorted_buff[0],str+startpos+3,1);
            memcpy(&ret,sorted_buff,2);
            
            /*int ret;
            unsigned char sorted_buff[4];
            sorted_buff[3] = 0x00;
            memcpy(&sorted_buff[2],str+startpos+2,1);
            memcpy(&sorted_buff[1],str+startpos+3,1);
            memcpy(&sorted_buff[0],str+startpos+4,1);
            memcpy(&ret, sorted_buff, 4);            */
            
            // Get check code
            unsigned char act_chk;
            memcpy(&act_chk,(unsigned char*)str+startpos+4,1);
            //printf("ori crc: %0x,", act_chk);
            unsigned char chk = CRC_check((unsigned char*)str+startpos,4);
            //printf("check crc: %0x,", chk);
            // Buffer update
            //char temp[32];
            //sz = sz - (startpos+6);
            //memcpy(temp, buf+startpos+6, sz);
            //memcpy(buf,temp,sz);
             memmove(str, str+startpos+5, len - startpos- 5);
             *plen = len - (startpos+5);
           
            
            if(chk == act_chk)
                return ret;
    }

    return -1;
}


int recv_proc_data_1(char *str,int *plen )
{

    int startpos=-1;
    int len=*plen;
    for(int i=0;i<len;i++ ){
    
            if( *(str+i) == 0xff &&
                *(str+i+1) == 0x81 ) {
                
                startpos = i;
                break;
            }    
    }
    //fprintf(stderr, "startpos : %d\n", startpos);
    if(-1 == startpos) return -1;

    if( startpos + 6 <= len ){
            // Get number 
            /*unsigned short ret;
            char sorted_buff[2];
            memcpy(&sorted_buff[1],buf+startpos+3,1);
            memcpy(&sorted_buff[0],buf+startpos+4,1);
            memcpy(&ret,sorted_buff,2);*/
            int ret;
            unsigned char sorted_buff[4];
            sorted_buff[3] = 0x00;
            memcpy(&sorted_buff[2],str+startpos+2,1);
            memcpy(&sorted_buff[1],str+startpos+3,1);
            memcpy(&sorted_buff[0],str+startpos+4,1);
            memcpy(&ret, sorted_buff, 4);            
            
            // Get check code
            unsigned char act_chk;
            memcpy(&act_chk,(unsigned char*)str+startpos+5,1);
            //printf("ori crc: %0x,", act_chk);
            unsigned char chk = CRC_check((unsigned char*)str+startpos,5);
            //printf("check crc: %0x,", chk);
            // Buffer update
            //char temp[32];
            //sz = sz - (startpos+6);
            //memcpy(temp, buf+startpos+6, sz);
            //memcpy(buf,temp,sz);
             memmove(str, str+startpos+6, len - startpos- 6);
             *plen = len - (startpos+6);
           
            
            if(chk == act_chk)
                return ret;
    }

    return -1;
}


// 编码器监听线程
//*************************************************************************************************************************************
void receivethread(void)
{
    int nread;
    char buff[128];
    
    struct timeval t_last, t_now;
    gettimeofday(&t_now,NULL);
    t_last = t_now;
    //printf("aa....\n");

    // 监听
    while(1) 
    {
        nread = read(fd,buff,120);
        if(nread<=0)    continue;
	    
        //if (nread > 0){
		//	printf("[RECEIVE LEN] is %d, content is:\n",nread);
		//	for(int i = 0; i < nread; i++){
		//		fprintf(stderr,"%.2x ",buff[i]);
		//	}
		//}

        if( nread + g_recvlen < 128 )
        {
            memcpy(g_recvbuf+g_recvlen,buff,nread);
            g_recvlen += nread;
        }
        else
        {
            memset(g_recvbuf,0,128);
            g_recvlen = 0;
            continue;
        }
	    // 一次读取多次解析
	    /*for(int i=0; i<3; ++i){
		    int val = -1;
		    if(0==i){
		        val = recv_pst_data(buff,nread);
		    }else {
		        val = recv_pst_data(NULL,0);
	        }*/

        //fprintf(stderr, "encode_index = [%d]\n", get_encode_index());
        //fprintf(stderr, "pulse_per_circle = [%d]\n", get_pulse_per_circle());

        while(g_recvlen>2)
        {
		// 逐帧解析
		int val = -1;
		//val = recv_pst_data(buff,nread);
        //fprintf(stderr, "val = [%d]\n", val);

        //vary....
        int encode_index = get_encode_index();
        
        switch(encode_index){
            case 1:
                val = recv_proc_data_1(g_recvbuf,&g_recvlen);
                break;
            case 2:
                val = recv_proc_data_1(g_recvbuf,&g_recvlen);
                break;
            case 3:
                val = recv_proc_data_2(g_recvbuf,&g_recvlen);
                break;
          
        }
		if(-1 == val) break;
		//printf("delta v: %4d\n", val - cur_v);
		cur_v = val;
        //printf("cur_v:%d\n", cur_v);
		//printf("cc...\n");
        //printf("..................................");
		//printf("val %d:%d\n", i, val);


        

		// Calculate position
		int position;
		int temp_stride = cur_v - ENCODER_ZERO_POSITION;
		if(abs(temp_stride) > (get_pulse_per_circle()/2) && temp_stride > 0)
		    position = cur_v - get_pulse_per_circle() - ENCODER_ZERO_POSITION;
		else if (abs(temp_stride) > (get_pulse_per_circle()/2) && temp_stride < 0)
		    position = cur_v + get_pulse_per_circle() - ENCODER_ZERO_POSITION;
		else
		    position = temp_stride;
		// 使其和电机的运动方向一致
        switch(encode_index){
            case 1:
                position *= -1;
                break;
            case 2:
                break;
            case 3:
            //unknown for now
                break;
        }		
        
        //printf("positon:%d\n", position);

		// 判断编码器是否已经工作
		if(!is_encoder_enable()){
		
			update_encoder_position(position);
			gettimeofday(&t_last,NULL); 
			prev_encoder_position = position;
			// 设定启动点角度
			double angle = get_encoder_angle();
			//printf("start angle %f\n", angle);
			set_g_start_angle(angle);
			encoder_is_enable();
		}

		// Update position
		//int dlt_threshold = get_speed_value()*2*10*65535/1000/360;
		//int dlt_threshold = 100; // 对应27度／秒
		int dlt_threshold = 100; // 对应27度／秒
		if(abs(encoder_position - position) < dlt_threshold){
		    update_encoder_position(position);
		}
		
        double cur_angle = get_encoder_angle();


		// 测速，60ms测一次速
		gettimeofday(&t_now,NULL);
		long t_slice = 1000000*(t_now.tv_sec - t_last.tv_sec)+(t_now.tv_usec - t_last.tv_usec);
		//printf("t_slice: %f\n",t_slice/1000.0);
		if(t_slice > 60000){
			double speed = (position - prev_encoder_position)/(t_slice/1000.0);
			update_encoder_speed(speed);
			t_last = t_now;
			prev_encoder_position = position;
		}

		// 帧间延迟
		//usleep(9573);
		usleep(500);

	    } //end of for(int i=0; i<4; ++i)

        usleep(8000);

    } 
    return;
}

// Listening UART signal
int listening_uart(const char* device, int baud, char parity, int data_bit, int stop_bit)
{
    pthread_t receiveid;
    //fprintf(stderr, "[%s]\n", "listen");
    fd = open(device, O_RDWR|O_NDELAY); // 非阻塞
    if (fd < 0){
    
	    log_e("listening_uart: open device failed.");
	    return -1;
    }
    fcntl(fd,F_SETFL,FNDELAY); // 非阻塞
    set_speed(fd,baud);
    set_Parity(fd,data_bit,stop_bit,parity);

    if(0 != pthread_create(&receiveid,NULL,(void*)receivethread,NULL)) return -1;
    if(0 != pthread_detach(receiveid)) return -1;
    return 0;
}
void close_uart()
{
    close(fd);
}

