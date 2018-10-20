#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/in.h>
#include <string.h>

unsigned short portnum = 10000;
char serverip[] = "192.168.0.16";

int main(int arg, char** argv)
{
	if(arg < 2)
	{
		char* mstr = argv[0];
		printf("%s\n", mstr);
		printf("please choose mood: on, off, check\n");     
		return;
	}

	char bufon[8]    = {0xFE, 0x05, 0x00, 0x00, 0xFF, 0x00, 0x98, 0x35}; 
	char bufoff[8]   = {0xFE, 0x05, 0x00, 0x00, 0x00, 0x00, 0xD9, 0xC5}; 
	char bufcheck[8] = {0xFE, 0x01, 0x00, 0x00, 0x00, 0x04, 0x29, 0xC6}; 
	
	int cfd; 
	char buffer[10] = {0};    
	struct sockaddr_in s_add,c_add; 

	cfd = socket(AF_INET, SOCK_STREAM, 0);
	if(-1 == cfd)
	{
    	printf("socket fail ! \r\n");
    	return -1;
	}
	
	bzero(&s_add,sizeof(struct sockaddr_in));
	s_add.sin_family=AF_INET;
	s_add.sin_addr.s_addr= inet_addr(serverip); 
	s_add.sin_port=htons(portnum);
	
	printf("s_addr = %#x ,port : %#x\r\n",s_add.sin_addr.s_addr,s_add.sin_port); 

	if(-1 == connect(cfd,(struct sockaddr *)(&s_add), sizeof(struct sockaddr)))
	{
    	printf("connect fail !\r\n");
	    return -1;
	}
	
	printf("Hello,welcome to client !\r\n");	
	//**********socket complete.******************//

	char* para1 = argv[1];
	printf("para1: %s\n", para1);
	
	int cmd = -1;
	if(!strcmp(para1, "on")    || !strcmp(para1, "ON"))
		cmd = 1;
	if(!strcmp(para1, "off")   || !strcmp(para1, "OFF") )
		cmd = 2;
	if(!strcmp(para1, "check") || !strcmp(para1, "CHECK"))
		cmd = 3;
    printf("cmd = %d\n", cmd);
	
	if(-1 != cmd)
	{
		switch(cmd)
		{
			case 1:
				if(-1 == write(cfd, bufon, 8))
				//if(-1 == write(cfd, &buf_on, sizeof(buf_on)))
				{
					printf("write on fail !\n");
					return -1;
				}
				break;
			case 2:
				if(-1 == write(cfd, bufoff, 8))
				{
					printf("write off fail !\n");
					return -1;
				}
				break;		
			case 3:
				if(-1 == write(cfd, bufcheck, 8))
				{
					printf("write check failed !\n");
					return -1;
				}
			default:
				break;
		}
	}
	
	usleep(10000);
	int recbytes;

	if(-1 == (recbytes = read(cfd,&buffer,sizeof(buffer) ) ))
	{
    	printf("read data fail !\r\n");
	    return -1;
	}
	printf("%d\n", recbytes);
	char m = 0x41;
	char n = 'A';
	printf("m=%c n=%X\n", m, n);
	
	int i;
	printf("buffer[0] = %.2X\n", buffer[0]);
	printf("buffer[1] = %.2X\n", buffer[1]);
	for(i=0;i<recbytes;i++)
	{
		//if(buffer[i]!='\0')
			printf("%.2X ", buffer[i]);
	
	}
	printf("%s\n", buffer);
	printf("read ok !\n");
	
	close(cfd); 
	return 0;

}
