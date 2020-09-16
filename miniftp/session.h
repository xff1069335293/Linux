#ifndef _SESSION_H_
#define _SESSION_H_

#include"common.h"
typedef struct session
{
	/*��������*/
	uid_t uid;
	int ctrl_fd;//���������׽���
	char cmdline[MAX_COMMAND_LINE];
	char cmd[MAX_COMMAND];
	char arg[MAX_ARG]; 

	/*��������*/

	struct sockaddr_in *port_addr;
	int data_fd;//�������Ӽ����׽���
	
	int pasv_listen_fd;//���������׽���

	/*Э��״̬*/
	int is_ascci;//
	
	/*���ӽ���ͨ��*/
	int parent_fd;
	int child_fd;

}session_t; 

void begin_session(session_t *sess);

#endif /*_SESSION_H_*/