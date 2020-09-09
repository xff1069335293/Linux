#include "common.h"
#include "sysutil.h"
#include "session.h"

//02.28.43
int main(int argc,char* argv[])
{
	session_t sess = 
		{
		/*��������*/
			-1
		};
	int listenfd = tcp_server("192.168.140.135",9001);
	
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