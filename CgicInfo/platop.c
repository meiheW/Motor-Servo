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
#include <sys/shm.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "cgic.h"
#define   SHMKEY        (5432)
int counter=1110;

static  float testxx=0.1;

static  double   xangle=0.1;
static  int      ystatus=1;



void send_cmd(char* sendbuf)
{


    int sock;
    if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
    {
        return;
    }

    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(12345);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    sendto(sock, sendbuf, strlen(sendbuf), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));

    close(sock);
    
    
}
 



void readmem()
{
        int        _shmid = 0;
        void  *    _shmaddr = NULL;
        char buf[32];    
        _shmid = shmget(SHMKEY, 0, 0666|IPC_CREAT);
        if (_shmid < 0)
        {
            fprintf(stderr,"No shmget");
            return;
        }        
      
        
        _shmaddr = shmat(_shmid, (void *)0, 0);   
  
        //printf( "g_shid=%d\r\n", _shmid );
        //printf( "g_shmaddr=%d\r\n", _shmaddr ); 
      
        if(_shmaddr !=NULL && _shmaddr != (void*)-1 )
        {
            memcpy(buf, _shmaddr,32);
            memcpy(&xangle, buf,sizeof(xangle)); 
            memcpy(&ystatus, buf+8,sizeof(ystatus));        
            fprintf(stderr, "ser%f,%d\n",xangle, ystatus);  
        }
        
        if (shmdt(_shmaddr) == -1)
        {
            fprintf(stderr, "shmdt failed\n");            
        }

}

int cgiMain() {
	//cgiWriteEnvironment("/CHANGE/THIS/PATH/capcgi.dat");

        

    int temp=-99;
    double dtemp=-999.9;

        
     
    time_t rawtime;
    
    struct tm * timeinfo;
    char buffer [128];
    char cmdbuffer [64];
    //readmem();
    //return 0;

    time (&rawtime);
    printf("%ld\n", rawtime);

    timeinfo = localtime (&rawtime);
    strftime (buffer,sizeof(buffer),"%Y/%m/%d-%H:%M:%S",timeinfo);


	cgiHeaderContentType("text/html");

    cgiFormInteger( "run", &temp, -99);
    if( temp != -99  )
    {
        fprintf(cgiOut, "{\"value\":\"%d\"}",temp);
        if( temp == 1 )        send_cmd("runleft");
        else if( temp == -1 )  send_cmd("runright");
        else if( temp == 0 )   send_cmd("cancel");
        else if( temp == 88 )  send_cmd("check");
        return 0;
    }
    cgiFormDouble( "angle", &dtemp, -999.9);
    if( dtemp != -999.9  )
    {
        fprintf(cgiOut, "{\"value\":\"%.1f\"}", dtemp);
        sprintf(cmdbuffer,"point %.1f",dtemp);
        send_cmd(cmdbuffer);
        return 0;
    }
    cgiFormInteger( "myinfo", &temp, -99);
    if( temp != 99  )
    {
        //srand((unsigned int )time(&rawtime));//设置时间的随机数种子
        readmem();
        fprintf(cgiOut, "{\"angle\":\"%.1f\",\"status\":\"%d\",\"time\":\"%s\"}", 
                                 xangle, ystatus,buffer);
        return 0;
    }

    //temp=temp+10;
	//fprintf(cgiOut, "%d\n", temp);
	
	return 0;
}











