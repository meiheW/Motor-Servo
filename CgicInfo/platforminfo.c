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


void  word_dist(  )
{
    tableid++;
    
    fprintf(cgiOut,"<tr><td>%d</td><td>运行路程</td><td>10000</td></tr>",tableid);
}

void check_sd(  )
{
    int file=-1;
    file = open( "/dev/mmcblk0p1",O_RDONLY ); 
    tableid++;
    if( file > 0 )
    {

        fprintf(cgiOut,"<tr><td>%d</td><td>SD卡</td><td>安装OK</td></tr>",tableid);
    }
    else
    {
        fprintf(cgiOut,"<tr><td>%d</td><td>SD卡</td><td>未安装</td></tr>",tableid);
    }

    close(file);
  
}

void diskinfo(  )
{

     
    struct statfs diskInfo;
    statfs("/",&diskInfo);
    unsigned long long totalBlocks = diskInfo.f_bsize;
    unsigned long long totalSize = totalBlocks * diskInfo.f_blocks;
    unsigned long long freeDisk = diskInfo.f_bfree*totalBlocks;
    double freesize = freeDisk/1024.0f/1024.0f;
    double allsize = totalSize/1024.0f/1024.0f;
    tableid++;
    fprintf(cgiOut,"<tr><td>%d</td><td>主盘剩余空间</td><td>%.1fM</td></tr>",tableid,freesize);

    tableid++;
    fprintf(cgiOut,"<tr><td>%d</td><td>主盘空间</td><td>%.1fM</td></tr>",tableid,allsize);

}
void cpumem_info()
{

    CPU_OCCUPY ocpu,ncpu;
    MEM_OCCUPY mem;

    //获取cpu核数
    cpu_num = sysconf(_SC_NPROCESSORS_ONLN); 
    
    tableid++;
    fprintf(cgiOut,"<tr><td>%d</td><td>cpu核</td><td>%d</td></tr>",tableid,cpu_num);

    //获取cpu使用率
    get_occupy(&ocpu);                     
    sleep(1);                                  
    get_occupy(&ncpu);                      
    cal_occupy(&ocpu, &ncpu); 
    //printf("cpu used:%4.2f \n", g_cpu_used);  

    tableid++;
    fprintf(cgiOut,"<tr><td>%d</td><td>cpu使用率</td><td>%4.2f</td></tr>",tableid,g_cpu_used);

    //获取内存使用率
    get_mem_occupy(&mem);

    double using = ((double)(mem.total - mem.free)/mem.total)*100;
    //printf("mem used:%4.2f\n",using);

    tableid++;
    fprintf(cgiOut,"<tr><td>%d</td><td>内存使用率</td><td>%4.2f</td></tr>",tableid,using);
    //获取io使用率
    //printf("io used:%4.2f\n",get_io_occupy());

    tableid++;
    fprintf(cgiOut,"<tr><td>%d</td><td>io使用率</td><td>%4.2f</td></tr>",tableid,get_io_occupy());



    //获取当前磁盘的使用率
    //char t[20]="";
    //char *used = t;
    //get_disk_occupy(&used);

    //char used[20]=" " ;
    //get_disk_occupy((char **)&used);
    //printf("disk used:%s\n",used);

    //tableid++;
    //fprintf(cgiOut,"<tr><td>%d</td><td>磁盘使用率</td><td>%s</td></tr>",tableid,used);

    //网络延迟
    /*long int start_download_rates;  //保存开始时的流量计数  
    long int end_download_rates;    //保存结果时的流量计数  
    getCurrentDownloadRates(&start_download_rates);//获取当前流量，并保存在start_download_rates里  
    sleep(WAIT_SECOND); //休眠多少秒，这个值根据宏定义中的WAIT_SECOND的值来确定 
    getCurrentDownloadRates(&end_download_rates);//获取当前流量，并保存在end_download_rates里  

    //printf("download is : %4.2f byte/s \n",( (float)(end_download_rates-start_download_rates))/WAIT_SECOND );

    tableid++;
    fprintf(cgiOut,"<tr><td>%d</td><td>网络延迟</td><td>%4.2f</td></tr>",tableid,
                ((float)(end_download_rates-start_download_rates))/WAIT_SECOND);
    */
}

void os_info()
{

 
    struct utsname buf;

    if(uname(&buf))
    {
        return;
    }

    tableid++;
    fprintf(cgiOut,"<tr><td>%d</td><td>OS</td><td>%s%s%s%s</td></tr>",

            tableid,buf.nodename,buf.release,buf.version,buf.machine);

    //printf("sysname:%s\n",buf.sysname);
    //printf("nodename:%s\n",buf.nodename);
    //printf("release:%s\n",buf.release);
    //printf("version:%s\n",buf.version);
    //printf("machine:%s\n",buf.machine);


}

int cgiMain() 
{

    time_t timep;
    time (&timep);
   
	//cgiWriteEnvironment("/CHANGE/THIS/PATH/capcgi.dat");
	cgiHeaderContentType("text/html");
	fprintf(cgiOut, "<html>"
                      "<head>"
	                    "<meta charset=\"utf-8\">"
                        "<title>platform</title>"
                        "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
                        "<link rel=\"stylesheet\" href=\"../css/bootstrap.min.css\">"
                      "</head>");
	fprintf(cgiOut, " <body>"
                        "<div class=\"container\">"
                          "<h3 align=\"center\">云台主要参数</h2>"
                           "<p align=\"right\">系统时间:%s</p>"
                           "<div class=\"table-responsive\">"          
                            "<table class=\"table table-striped table-bordered\">"
                              "<thead>",ctime(&timep));
	fprintf(cgiOut, "<div class=\"table-responsive\">"          
                        "<table class=\"table table-striped table-bordered\">"
                          "<thead>"
                            "<tr>"
                              "<th>序号</th>"
                              "<th>名称</th>"
                              "<th>值</th>"
                            "</tr>"
                          "</thead>"
                          "<tbody>");

    word_dist();    
    check_sd();
    diskinfo();
    cpumem_info();
    os_info();
    fprintf(cgiOut, " </tbody></table></div></div></body></html>");
	return 0;
}

void  cal_occupy (CPU_OCCUPY *o, CPU_OCCUPY *n)
{  
        double od, nd;  
        double id, sd;  
        //double scale;    
        od = (double) (o->user + o->nice + o->system +o->idle);//第一次(用户+优先级+系统+空闲)的时间再赋给od  
        nd = (double) (n->user + n->nice + n->system +n->idle);//第二次(用户+优先级+系统+空闲)的时间再赋给od  
        //scale = 100.0 / (float)(nd-od);       //100除强制转换(nd-od)之差为float类型再赋给scale这个变量  
        id = (double) (n->user - o->user);    //用户第一次和第二次的时间之差再赋给id  
        sd = (double) (n->system - o->system);//系统第一次和第二次的时间之差再赋给sd  
        g_cpu_used = ((sd+id)*100.0)/(nd-od); //((用户+系统)乖100)除(第一次和第二次的时间差)再赋给g_cpu_used  
}  
void  get_occupy (CPU_OCCUPY *o) 
{  
    FILE *fd;         
   // int n;            
    char buff[MAXBUFSIZE];                                                                                               
    fd = fopen ("/proc/stat", "r"); //这里只读取stat文件的第一行及cpu总信息，如需获取每核cpu的使用情况，请分析stat文件的接下来几行。
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
                //printf("%d   %s\n",i,buffer);
                sscanf(buffer,"%s %ld",buffer,save_rate);
                break;
            }
        }
    }
    if(i==20) *save_rate=0.01;
    fclose(net_dev_file); //关闭文件  
    return ;  
}









