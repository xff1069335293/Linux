#include "common.h"
#include "sysutil.h"
#include "session.h"

//02.28.43
int main(int argc,char* argv[])
{
	//提升权限root运行
	if (getuid() != 0)
	{
		printf("miniftp : must be start as root.\n");
		exit(EXIT_FAILURE);
	}

	session_t sess = 
		{
		/*控制连接*/
			-1,-1,"","","",
		/*数据连接*/
			NULL,-1,-1,
		/*协议状态*/
			0,
		/*父子进程通道*/
			-1,-1
		};
	int listenfd = tcp_server("192.168.140.136",9001);
	
	struct sockaddr_in addrCli;
	socklen_t addrlen;
	
	int sockConn;
	while (1)
	{
		//获取新连接
		if ((sockConn =  accept(listenfd,(struct sockaddr*)&addrCli,&addrlen)) < 0)
			ERR_EXIT("accept");
		pid_t pid = fork();
		if (pid<0)
			ERR_EXIT("fork");
		if (pid == 0)
		{
			//子进程
			close(listenfd);

			//会话
			sess.ctrl_fd = sockConn;
			begin_session(&sess);
			exit(EXIT_SUCCESS);
		}
		else
		{
			//父进程
			close(sockConn);
		}
	}
	close(listenfd);
	return 0;
}
