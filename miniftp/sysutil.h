#ifndef _SYSUTIL_H_
#define _SYSUTIL_H_

#include"common.h"



//获取监听套接字
int tcp_server(const char* host,unsigned short port);

int tcp_client(int port);

//获取权限
const char *statbuf_get_perms(struct stat *sbuf);

//获取时间
const char *statbuf_get_date(struct stat *sbuf);

//发送描述符
void send_fd(int sock_fd, int fd);

//接收描述符
int recv_fd(const int sock_fd);

long get_time_sec();
long get_time_usec();
#endif /*_SYSUTIL_H_*/