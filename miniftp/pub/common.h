#ifndef _COMMON_H_
#define _COMMON_H_

#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include<assert.h>

#include<sys/types.h>
#include<sys/socket.h>

#include<netinet/in.h>//��ַ�ṹ
#include<arpa/inet.h>//�ֽ���ת��

#include<pwd.h>//�����ļ���ͷ�ļ�
#include<shadow.h>
#include<crypt.h>

#define ERR_EXIT(m)\
do{\
	perror(m);\
	exit(EXIT_FAILURE);\
}while(0)

#define MAX_BUFFER_SIZE  1024	//
#define MAX_COMMAND_LINE 1024	//
#define MAX_COMMAND      32		//�������
#define MAX_ARG          1024	//���Ĳ���ֵ

#endif /*_COMMON_H_*/
