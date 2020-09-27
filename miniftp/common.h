#ifndef _COMMON_H_
#define _COMMON_H_

#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include<assert.h>
#include<malloc.h>

#include<sys/types.h>
#include<sys/socket.h>
#include<fcntl.h>
#include<errno.h>
#include<netinet/in.h>//��ַ�ṹ
#include<arpa/inet.h>//�ֽ���ת��
#include<signal.h>
#include<pwd.h>//�����ļ���ͷ�ļ�
#include<shadow.h>
#include<crypt.h>//�����㷨
#include<netdb.h>//��ȡ��������ip


#include<dirent.h>//��ʾĿ¼
#include<sys/stat.h>//��ʾ�ļ�����
#include<time.h>//��ʾʱ��

#include<linux/capability.h>//����Ȩ��
#include<sys/syscall.h>//ϵͳ����

#define ERR_EXIT(m)\
do{\
	perror(m);\
	exit(EXIT_FAILURE);\
}while(0)

#define MAX_BUFFER_SIZE  1024	//
#define MAX_COMMAND_LINE 1024	//
#define MAX_COMMAND      32		//�������
#define MAX_ARG          1024	//���Ĳ���ֵ

#define MAX_SETTING_LINE 1024 //���������
#define MAX_KEY_VALUE_SIZE 128

#define MAX_HOST_NAME_SIZE 128//��������

#define MAX_BUCKET_SIZE    256
#endif /*_COMMON_H_*/
