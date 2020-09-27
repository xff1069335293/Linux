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
#include<netinet/in.h>//地址结构
#include<arpa/inet.h>//字节序转换
#include<signal.h>
#include<pwd.h>//密码文件的头文件
#include<shadow.h>
#include<crypt.h>//加密算法
#include<netdb.h>//获取主机名和ip


#include<dirent.h>//显示目录
#include<sys/stat.h>//显示文件属性
#include<time.h>//显示时间

#include<linux/capability.h>//提升权限
#include<sys/syscall.h>//系统调用

#define ERR_EXIT(m)\
do{\
	perror(m);\
	exit(EXIT_FAILURE);\
}while(0)

#define MAX_BUFFER_SIZE  1024	//
#define MAX_COMMAND_LINE 1024	//
#define MAX_COMMAND      32		//最大命令
#define MAX_ARG          1024	//最大的参数值

#define MAX_SETTING_LINE 1024 //最大配置行
#define MAX_KEY_VALUE_SIZE 128

#define MAX_HOST_NAME_SIZE 128//主机名字

#define MAX_BUCKET_SIZE    256
#endif /*_COMMON_H_*/
