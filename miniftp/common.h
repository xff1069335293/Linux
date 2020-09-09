#ifndef _COMMON_H_
#define _COMMON_H_

#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>

#include<sys/types.h>
#include<sys/socket.h>

#include<netinet/in.h>//地址结构
#include<arpa/inet.h>//字节序转换

#define ERR_EXIT(m)\
do{\
	perror(m);\
	exit(EXIT_FAILURE);\
}while(0)

#define MAX_BUFFER_SIZE 1024

#endif /*_COMMON_H_*/
