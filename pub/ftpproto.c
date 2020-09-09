#include"ftpproto.h"
#include"ftpcodes.h"//宏
void ftp_reply(session_t *sess, int code, const char *text)
{
	char buf[MAX_BUFFER_SIZE] = {0};
	sprintf(buf,"%d %s\r\n",code,text);
	send(sess->ctrl_fd,buf,strlen(buf),0);
}
///////////////////////////////////////
//命令映射机制
static void do_user(session_t *sess);
static void do_pass(session_t *sess);
static void do_syst(session_t *sess);

typedef struct ftpcmd
{
	const char* cmd;
	void(*cmd_handler)(session_t *sess);
}ftpcmd_t;

//命令映射表
static ftpcmd_t ctrl_cmds[]=
{
	/*{"USER",do_user},
	{"PASS",do_pass},
	{"SYST",do_syst}*/
};


//ftp 服务进程
void handle_child(session_t *sess)
{
	ftp_reply(sess,FTP_GREET,"(vsFTPd 3.0.2)");
	while (1)
	{
		//不停等待客户端的命令进行处理
	}
}