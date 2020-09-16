#include "common.h"
#include "sysutil.h"
#include "session.h"

//02.28.43
int main(int argc,char* argv[])
{
	//����Ȩ��root����
	if (getuid() != 0)
	{
		printf("miniftp : must be start as root.\n");
		exit(EXIT_FAILURE);
	}

	session_t sess = 
		{
		/*��������*/
			-1,-1,"","","",
		/*��������*/
			NULL,-1,-1,
		/*Э��״̬*/
			0,
		/*���ӽ���ͨ��*/
			-1,-1
		};
	int listenfd = tcp_server("192.168.140.136",9001);
	
	struct sockaddr_in addrCli;
	socklen_t addrlen;
	
	int sockConn;
	while (1)
	{
		//��ȡ������
		if ((sockConn =  accept(listenfd,(struct sockaddr*)&addrCli,&addrlen)) < 0)
			ERR_EXIT("accept");
		pid_t pid = fork();
		if (pid<0)
			ERR_EXIT("fork");
		if (pid == 0)
		{
			//�ӽ���
			close(listenfd);

			//�Ự
			sess.ctrl_fd = sockConn;
			begin_session(&sess);
			exit(EXIT_SUCCESS);
		}
		else
		{
			//������
			close(sockConn);
		}
	}
	close(listenfd);
	return 0;
}
