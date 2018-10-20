#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include <string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/vfs.h> /* or <sys/statfs.h> */
#include<sys/utsname.h>
#include<fcntl.h>
#include<time.h>
#include "cgic.h"

#define MAXBUFSIZE 1024
#define WAIT_SECOND 3   //暂停时间，单位为“秒”  
typedef  struct occupy        
{  
    char name[20];      
    unsigned int user;  
    unsigned int nice;  
    unsigned int system;
    unsigned int idle;  
} CPU_OCCUPY ;  

typedef struct PACKED         
{
    char name[20]; 
    long total; 
    char name2[20];
    long free;            
}MEM_OCCUPY;

float g_cpu_used;           
int cpu_num;                //定义一个全局的int类型cup_num  
void cal_occupy(CPU_OCCUPY *, CPU_OCCUPY *); 
void get_occupy(CPU_OCCUPY *); 
void get_mem_occupy(MEM_OCCUPY *) ;
float get_io_occupy();
void get_disk_occupy(char ** reused);
void getCurrentDownloadRates(long int * save_rate);

int tableid=0;


char*  work_dist( char* buf)
{
    
    tableid++;
    sprintf( buf, "{\"id\":\"%d\",\"name\":\"运行路程\",\"value\":\"1000\"}",tableid);
    return buf;
}

char* check_sd( char* buf )
{
   
    int file=-1;
    file = open( "/dev/mmcblk0",O_RDONLY ); 
    tableid++;
    if( file > 0 )
    {
        sprintf( buf, "{\"id\":\"%d\",\"name\":\"SD卡\",\"value\":\"安装OK\"}",tableid);
    }
    else
    {
        sprintf( buf, "{\"id\":\"%d\",\"name\":\"SD卡\",\"value\":\"未安装\"}",tableid);
    }

    close(file);

    return buf;
  
}

char* diskinfo(char* buf )
{

    
    char bufa[64];
    char bufb[64];
    struct statfs diskInfo;
    statfs("/",&diskInfo);
    unsigned long long totalBlocks = diskInfo.f_bsize;
    unsigned long long totalSize = totalBlocks * diskInfo.f_blocks;
    unsigned long long freeDisk = diskInfo.f_bfree*totalBlocks;
    double freesize = freeDisk/1024.0f/1024.0f;
    double allsize = totalSize/1024.0f/1024.0f;

    tableid++;
    sprintf( bufa, "{\"id\":\"%d\",\"name\":\"主盘剩余空间\",\"value\":\"%.1fM\"}",tableid,freesize);

    tableid++;
    sprintf( bufb, "{\"id\":\"%d\",\"name\":\"主盘空间\",\"value\":\"%.1fM\"}",tableid,allsize);

    sprintf( buf, "%s,%s", bufa,bufb);
    return buf;

}
char* cpumem_info(char* buf)
{
     
    char bufa[64];
    char bufb[64];
    char bufc[64];
   // char bufd[64];

    CPU_OCCUPY ocpu,ncpu;
    MEM_OCCUPY mem;

    //获取cpu核数
    cpu_num = sysconf(_SC_NPROCESSORS_ONLN);     
    tableid++;
    sprintf( bufa, "{\"id\":\"%d\",\"name\":\"cpu核\",\"value\":\"%d\"}",tableid,cpu_num);

    //获取cpu使用率
    get_occupy(&ocpu);                     
    sleep(1);                                  
    get_occupy(&ncpu);                      
    cal_occupy(&ocpu, &ncpu); 
   
    tableid++;
    sprintf( bufb, "{\"id\":\"%d\",\"name\":\"cpu使用率\",\"value\":\"%4.2f\"}",tableid,g_cpu_used);

    //获取内存使用率
    get_mem_occupy(&mem);
    double using = ((double)(mem.total - mem.free)/mem.total)*100;

    tableid++;
    sprintf( bufc, "{\"id\":\"%d\",\"name\":\"内存使用率\",\"value\":\"%4.2f\"}",tableid,using);
    //获取io使用率
    //printf("io used:%4.2f\n",get_io_occupy());

    //tableid++;
    //sprintf( bufd, "{\"id\":\"%d\",\"name\":\"io使用率\",\"value\":\"%4.2f\"}",tableid,get_io_occupy());
    //printf("%s\n%s\n%s\n", bufa,bufb,bufc);
    
    sprintf( buf, "%s,%s,%s", bufa,bufb,bufc);

    return buf;
}

char* os_info(char* retbuf)
{

     
    char bufa[128];
    struct utsname buf;

    if(uname(&buf))
    {
        sprintf( retbuf, "{\"id\":\"%d\",\"name\":\"OS\",\"value\":\"NONE\"}",tableid);
        return retbuf;
    }

    tableid++;
    sprintf(bufa, "%s%s%s%s", buf.nodename,buf.release,buf.version,buf.machine);
    sprintf( retbuf, "{\"id\":\"%d\",\"name\":\"OS\",\"value\":\"%s\"}",tableid,bufa);

    return retbuf;

}

char* angle_info(char* retbuf){

	FILE* stream = NULL;
	stream = fopen("/home/root/sumangle.txt", "r+");//path

    tableid++;
    if(stream == NULL)
	{
		sprintf( retbuf, "{\"id\":\"%d\",\"name\":\"运行总角度(度/万)\",\"value\":\"%s\"}",tableid,"读取失败");
		return retbuf;
	}

	double sumangle;
	fscanf(stream, "%lf", &sumangle);
	fclose(stream);
    
    sprintf( retbuf, "{\"id\":\"%d\",\"name\":\"运行总角度(度/万)\",\"value\":\"%lf\"}",tableid,sumangle);
    return retbuf;
}

char* encoder_info(char* retbuf){
    
    char buf[256];    
    
	FILE* stream = NULL;
	stream = fopen("/home/root/info.txt", "r");
	if(stream == NULL)
	{
		sprintf( retbuf, "{\"id\":\"%d\",\"name\":\"编码器信息\",\"value\":\"%s\"}",tableid,"读取失败");
        return retbuf;
	}
	
	while(fgets(buf, 1024, stream)!=NULL)
	{
		int len = strlen(buf);
		buf[len-1] = '\0';
	}

	fclose(stream);

    tableid++;
    sprintf( retbuf, "{\"id\":\"%d\",\"name\":\"编码器信息\",\"value\":\"%s\"}",tableid,buf);
    
    return retbuf;
}



int cgiMain() 
{   
    char retbuf[256];
    char jsonbuf[1024];
    time_t timep;
    time (&timep);
   
	//cgiWriteEnvironment("/CHANGE/THIS/PATH/capcgi.dat");
	cgiHeaderContentType("text/html"); 
     
    //sprintf(jsonbuf,"[%s", work_dist(retbuf) );
    sprintf(jsonbuf,"[%s", check_sd(retbuf) );
    sprintf(jsonbuf,"%s,%s", jsonbuf, diskinfo(retbuf) );
    sprintf(jsonbuf,"%s,%s", jsonbuf, cpumem_info(retbuf) );
    //sprintf(jsonbuf,"%s,%s]", jsonbuf, os_info(retbuf) );
    sprintf(jsonbuf,"%s,%s", jsonbuf, encoder_info(retbuf) );
    sprintf(jsonbuf,"%s,%s]", jsonbuf, angle_info(retbuf) );

    fprintf(cgiOut, "%s", jsonbuf);
    //printf("%s\n", jsonbuf);
	return 0;
}



void  cal_occupy (CPU_OCCUPY *o, CPU_OCCUPY *n)
{  
    double od, nd;  
    double id, sd;  
    //double scale;  //第一次(用户+优先级+系统+空闲)的时间再赋给od    
    od = (double) (o->user + o->nice + o->system +o->idle);
    //第二次(用户+优先级+系统+空闲)的时间再赋给od  
    nd = (double) (n->user + n->nice + n->system +n->idle);
    //scale = 100.0 / (float)(nd-od);       
    //100除强制转换(nd-od)之差为float类型再赋给scale这个变量 
    //用户第一次和第二次的时间之差再赋给id  
    id = (double) (n->user - o->user);  
    //系统第一次和第二次的时间之差再赋给sd    
    sd = (double) (n->system - o->system); 
     //((用户+系统)乖100)除(第一次和第二次的时间差)再赋给g_cpu_used 
    g_cpu_used = ((sd+id)*100.0)/(nd-od); 
}  
void  get_occupy (CPU_OCCUPY *o) 
{  
    FILE *fd;         
   // int n; 
    //这里只读取stat文件的第一行及cpu总信息，如需获取每核cpu的使用情况，请分析stat文件的接下来几行。           
    char buff[MAXBUFSIZE];  
                                                                                                 
    fd = fopen ("/proc/stat", "r"); 
    fgets (buff, sizeof(buff), fd); 
    sscanf (buff, "%s %u %u %u %u", o->name, &o->user, &o->nice,&o->system, &o->idle);  
    fclose(fd);     
}  
void get_mem_occupy(MEM_OCCUPY * mem)
{
    FILE * fd;
    char buff[MAXBUFSIZE];
    fd = fopen("/proc/meminfo","r");
    fgets (buff, sizeof(buff), fd); 
    sscanf (buff, "%s %ld", mem->name,&mem->total);  
    fgets (buff, sizeof(buff), fd); 
    sscanf (buff, "%s %ld", mem->name2,&mem->free); 
}
float get_io_occupy()
{
    char cmd[] ="iostat -d -x";
    char buffer[MAXBUFSIZE];  
    char a[20];   
    float arr[20];
    FILE* pipe = popen(cmd, "r");    
    if (!pipe)  return -1;    
    fgets(buffer, sizeof(buffer), pipe);
    fgets(buffer, sizeof(buffer), pipe);
    fgets(buffer, sizeof(buffer), pipe);
    fgets(buffer, sizeof(buffer), pipe);
    sscanf(buffer,"%s %f %f %f %f %f %f %f %f %f %f %f %f %f ",
                    a,&arr[0],&arr[1],&arr[2],&arr[3],&arr[4],&arr[5],
                    &arr[6],&arr[7],&arr[8],&arr[9],&arr[10],&arr[11],&arr[12]);
    //printf("%f\n",arr[12]);
    
    pclose(pipe);
    return arr[12];  
}
void get_disk_occupy(char ** reused)
{
    char currentDirectoryPath[ MAXBUFSIZE ];
    getcwd(currentDirectoryPath, MAXBUFSIZE);
    //printf("当前目录：%s\n",currentDirectoryPath);
    char cmd[50]="df ";
    strcat(cmd, currentDirectoryPath);
    //printf("%s\n",cmd);

    char buffer[MAXBUFSIZE];
    FILE* pipe = popen(cmd, "r");    
    char fileSys[20];
    char blocks[20];
    char used[20];
    char free[20];
    char percent[10];
    char moment[20];

    if (!pipe)  return ;  
    if(fgets(buffer, sizeof(buffer), pipe)!=NULL){
        sscanf(buffer,"%s %s %s %s %s %s",fileSys,blocks,used,free,percent,moment);
    }
    if(fgets(buffer, sizeof(buffer), pipe)!=NULL){
        sscanf(buffer,"%s %s %s %s %s %s",fileSys,blocks,used,free,percent,moment);
    }
    //printf("desk used:%s\n",percent);
    strcpy(*reused,percent);
    return ;
}
    
void getCurrentDownloadRates(long int * save_rate)  
{  
    char intface[] = "eth0:";  //这是网络接口名，根据主机配置
    FILE * net_dev_file;   
    char buffer[1024]; 
    //size_t bytes_read; 
    //char * match;  
    if ( (net_dev_file=fopen("/proc/net/dev", "r")) == NULL )
    {  
        //printf("open file /proc/net/dev/ error!\n");  
        return;  
    }  

    int i = 0;
    while(i++<20)
    {
        if(fgets(buffer, sizeof(buffer), net_dev_file)!=NULL)
        {
            if(strstr(buffer,intface)!=NULL)
            {
                sscanf(buffer,"%s %ld",buffer,save_rate);
                break;
            }
        }
    }
    if(i==20) *save_rate=0.01;
    fclose(net_dev_file); //关闭文件  
    return ;  
}









