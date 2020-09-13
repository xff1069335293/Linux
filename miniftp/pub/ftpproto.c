#include"ftpproto.h"
#include"ftpcodes.h"//状态码
#include"str.h"



void ftp_reply(session_t *sess, int code, const char *text)
{
	char buf[MAX_BUFFER_SIZE] = {0};
	sprintf(buf,"%d %s\r\n",code,text);
	send(sess->ctrl_fd,buf,strlen(buf),0);
}

/////////////////////////////////////////////////////////
//命令映射机制
static void do_user(session_t *sess);
static void do_pass(session_t *sess);
static void do_syst(session_t *sess);











typedef struct ftpcmd
{
	const char* cmd;//解析出来的命令
	void(*cmd_handler)(session_t *sess);//命令对应的处理函数
}ftpcmd_t;

//命令映射表
static ftpcmd_t ctrl_cmds[]=
{
	{"USER",do_user},
	{"PASS",do_pass},
	{"SYST",do_syst}
};

////////////////////////////////////////////////////////
//ftp 服务进程
void handle_child(session_t *sess)
{
	ftp_reply(sess,FTP_GREET,"(vsFTPd 3.0.2)");
	int ret;
	while (1)
	{
		//不停等待客户端的命令进行处理
		//初始化
		memset(sess->cmdline, 0, MAX_COMMAND_LINE);
		memset(sess->cmd, 0, MAX_COMMAND);
		memset(sess->arg, 0, MAX_ARG);

		ret = recv(sess->ctrl_fd, sess->cmdline,MAX_COMMAND_LINE,0);
		if (ret == -1)
			ERR_EXIT("readline");
		else if (ret == 0)
			exit(EXIT_SUCCESS);
		
		str_trim_crlf(sess->cmdline);
		printf("cmdline = %s\n",sess->cmdline);

		str_split(sess->cmdline,sess->cmd,sess->arg,' ');
		printf("cmd = %s\n",sess->cmd);
		printf("arg = %s\n",sess->arg);

		int table_size = sizeof(ctrl_cmds)/sizeof(ftpcmd_t);
		//遍历命令映射表
		int i;
		for (i=0; i<table_size; ++i)
		{
			if (strcmp(sess->cmd,ctrl_cmds[i].cmd) == 0)
			{
				if (ctrl_cmds[i].cmd_handler)
					ctrl_cmds[i].cmd_handler(sess);
				else
					ftp_reply(sess,FTP_COMMANDNOTIMPL,"Unimplement command.");
				break;
			}
		}
		//未找到命令
		if (i >= table_size)
			ftp_reply(sess,FTP_BADCMD,"Unknown command.");
	}
}

//USER xff
static void do_user(session_t *sess)
{ 
	struct passwd *pwd = getpwnam(sess->arg);
	if (pwd != NULL)
		sess->uid = pwd->pw_uid;

	ftp_reply(sess, FTP_GIVEPWORD, "Please specify the password.");
}
//PASS xxx
static void do_pass(session_t *sess)
{
	//鉴权判断
	struct passwd *pwd = getpwuid(sess->uid);
	if(pwd == NULL)
	{
		ftp_reply(sess,FTP_LOGINERR,"Login incorrect.");
		return ;
	}
	
	struct spwd *spd = getspnam(pwd->pw_name);
	if (spd == NULL)
	{
		ftp_reply(sess,FTP_LOGINERR,"Login incorrect.");
		return ;
	}

	//和加密的密码对比
	char *encry_pwd = crypt(sess->arg,spd->sp_pwdp);
	printf("encry_pwd = %s\n",encry_pwd);
	/*if (strcmp(encry_pwd,spd->sp_pwdp) != 0)
	{
		ftp_reply(sess,FTP_LOGINERR,"Login incorrect.");
		return ;
	}*/
	
	setegid(pwd->pw_gid);
	seteuid(pwd->pw_uid);
	chdir(pwd->pw_dir);

	ftp_reply(sess,FTP_LOGINOK,"Login successful.");
}
static void do_syst(session_t *sess)
{
	ftp_reply(sess,FTP_SYSTOK,"UNIX Type: L8");
}