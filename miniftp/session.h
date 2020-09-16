#ifndef _SESSION_H_
#define _SESSION_H_

#include"common.h"
typedef struct session
{
	/*控制连接*/
	uid_t uid;
	int ctrl_fd;//控制连接套接字
	char cmdline[MAX_COMMAND_LINE];
	char cmd[MAX_COMMAND];
	char arg[MAX_ARG]; 

	/*数据连接*/

	struct sockaddr_in *port_addr;
	int data_fd;//主动连接监听套接字
	
	int pasv_listen_fd;//被动连接套接字

	/*协议状态*/
	int is_ascci;//
	
	/*父子进程通道*/
	int parent_fd;
	int child_fd;

}session_t; 

void begin_session(session_t *sess);

#endif /*_SESSION_H_*/