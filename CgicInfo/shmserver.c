#include <stddef.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define   SHMKEY        (5432)
int                     g_shmid=-1;
void *                  g_shmaddr=NULL;
int                     g_shsize=64;


static  float   xangle=0.1;
static  int     ystatus=1;

int main()
{
    char buf[32];
    g_shmid = shmget(SHMKEY, 0, 0666);
    if (g_shmid < 0)
    {
        g_shmid = shmget(SHMKEY, g_shsize, IPC_CREAT | IPC_EXCL | 0666);
        if (g_shmid < 0)
        {
            fprintf(stderr,"shmget err!\r\n");
        }
    }

    if( g_shmid >= 0 )
    {
        g_shmaddr = shmat(g_shmid, 0, 0);
        if (g_shmaddr == (void *)-1)
        {
            fprintf(stderr,"shmget memory err!\r\n");
            g_shmaddr = NULL;
        }
        
    }
    
    
    printf( "g_shid=%d\r\n", g_shmid );
    printf( "g_shmaddr=%d\r\n", g_shmaddr );    
    
    while(1)
    {
        if( g_shmaddr > 0 )
        {
            ystatus++;
            xangle +=0.1;
            
           //sprintf( "")
            memcpy(buf,&xangle,sizeof(xangle));
            memcpy(buf+8,&ystatus,sizeof(ystatus));

            memcpy(g_shmaddr, buf, 32 );  
            
        }
        usleep(200000);
    }

    return 0;
}
