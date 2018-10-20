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

int counter=1110;

int cgiMain() {
	//cgiWriteEnvironment("/CHANGE/THIS/PATH/capcgi.dat");
    int temp=10;
	cgiHeaderContentType("text/html");

    cgiFormInteger( "count", &temp, 90);
    temp=temp+10;
	fprintf(cgiOut, "%d\n", temp);
	
	return 0;
}











