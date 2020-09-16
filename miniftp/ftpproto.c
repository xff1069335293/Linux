#include"ftpproto.h"
#include"ftpcodes.h"//״̬��
#include"str.h"



void ftp_reply(session_t *sess, int code, const char *text)
{
	char buf[MAX_BUFFER_SIZE] = {0};
	sprintf(buf,"%d %s\r\n",code,text);
	send(sess->ctrl_fd,buf,strlen(buf),0);
}

/////////////////////////////////////////////////////////
//����ӳ�����
static void do_user(session_t *sess);
static void do_pass(session_t *sess);
static void do_syst(session_t *sess);
static void do_feat(session_t *sess);
static void do_pwd(session_t *sess);
static void do_type(session_t *sess);
static void do_port(session_t *sess);
static void do_list(session_t *sess);
static void do_pasv(session_t *sess);
/*static void do_pwd(session_t *sess);
static void do_pwd(session_t *sess);
static void do_pwd(session_t *sess);
static void do_pwd(session_t *sess);
static void do_pwd(session_t *sess);
static void do_pwd(session_t *sess);
static void do_pwd(session_t *sess);
static void do_pwd(session_t *sess);
static void do_pwd(session_t *sess);
static void do_pwd(session_t *sess);
static void do_pwd(session_t *sess);
static void do_pwd(session_t *sess);
*/










typedef struct ftpcmd
{
	const char* cmd;//��������������
	void(*cmd_handler)(session_t *sess);//�����Ӧ�Ĵ�����
}ftpcmd_t;

//����ӳ���
static ftpcmd_t ctrl_cmds[]=
{
	{"USER",do_user},
	{"PASS",do_pass},
	{"SYST",do_syst},
	{"FEAT",do_feat},
	{"PWD" ,do_pwd },
	{"TYPE",do_type},
	{"PORT",do_port},
	{"PASV",do_pasv},
	{"LIST",do_list}
};

////////////////////////////////////////////////////////
//ftp �������
void handle_child(session_t *sess)
{
	ftp_reply(sess,FTP_GREET,"(vsFTPd 3.0.2)");
	int ret;
	while (1)
	{
		//��ͣ�ȴ��ͻ��˵�������д���
		//��ʼ��
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
		//��������ӳ���
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
		//δ�ҵ�����
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
	//��Ȩ�ж�
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

	//�ͼ��ܵ�����Ա�
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


//ϵͳ�汾
static void do_syst(session_t *sess)
{
	ftp_reply(sess,FTP_SYSTOK,"UNIX Type: L8");
}

//�����б�
static void do_feat(session_t *sess)
{
	send(sess->ctrl_fd,"211-Features:\r\n",strlen("211-Features:\r\n"),0);
	send(sess->ctrl_fd, " EPRT\r\n", strlen(" EPRT\r\n"), 0);
	send(sess->ctrl_fd, " EPSV\r\n", strlen(" EPSV\r\n"), 0);
	send(sess->ctrl_fd, " MDTM\r\n", strlen(" MDTM\r\n"), 0);
	send(sess->ctrl_fd, " PASV\r\n", strlen(" PASV\r\n"), 0);
	send(sess->ctrl_fd, " REST STREAM\r\n", strlen(" REST STREAM\r\n"), 0);
	send(sess->ctrl_fd, " SIZE\r\n", strlen(" SIZE\r\n"), 0);
	send(sess->ctrl_fd, " TVFS\r\n", strlen(" TVFS\r\n"), 0);
	send(sess->ctrl_fd, " UTF8\r\n", strlen(" UTF8\r\n"), 0);
	send(sess->ctrl_fd, "211 End\r\n", strlen("211 End\r\n"), 0);
}

//����Ŀ¼
static void do_pwd(session_t *sess)
{
	char buffer[MAX_BUFFER_SIZE] = {0};
	getcwd(buffer, MAX_BUFFER_SIZE);//��ȡĿ¼
	char msg[MAX_BUFFER_SIZE] = {0};
	sprintf(msg, "\"%s\"", buffer);//
	ftp_reply(sess,FTP_PWDOK,msg);
}

//TYPE A/I
static void do_type(session_t *sess)
{
	if (strcmp(sess->arg,"A") == 0)
	{
		//ASCII
		sess->is_ascci = 1;
		ftp_reply(sess, FTP_TYPEOK,"Switching to ASCII mode.");
	}
	else if (strcmp(sess->arg,"I") == 0)
	{
		ftp_reply(sess, FTP_TYPEOK,"Switching to Binary mode.");
	}
	else
		ftp_reply(sess, FTP_BADCMD,"Unrecognised TYPE command.");
}

static void do_port(session_t *sess)
{
	unsigned int v[6] = {0};
	sscanf(sess->arg, "%u,%u,%u,%u,%u,%u",&v[0],&v[1],&v[2],&v[3],&v[4],&v[5]);
	//�˿���Ϣ
	sess->port_addr = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
	unsigned char *p = (unsigned char*)&sess->port_addr->sin_port;
	p[0] = v[4];
	p[1] = v[5];
	//��ַ��Ϣ
	p = (unsigned char*)&sess->port_addr->sin_addr.s_addr;
	p[0] = v[0];
	p[1] = v[1];
	p[2] = v[2];
	p[3] = v[3];
	//��ַ��
	sess->port_addr->sin_family = AF_INET;

	ftp_reply(sess,FTP_PROTOK,"PORT command successful. Consider using PASV.");
}
//////////////////////////////////////////////////
int port_active(session_t *sess)
{
	if (sess->port_addr)
		return 1;
	return 0;
}
int pasv_active(session_t *sess)
{
	if (sess->pasv_listen_fd>0)
		return 1;
	return 0;
}

//�Ƿ񼤻�����һ��ģʽ
int get_transfer_fd(session_t *sess)
{
	if (!port_active(sess) && !pasv_active(sess))
	{
		ftp_reply(sess,FTP_BADSENDCONN,"Use PORT or PASV first.");
		return 0;
	}
	int ret = 1;
	if (port_active(sess))
	{
		//PORT����ģʽ
		int sock = tcp_client();
		if (connect(sock,(struct sockaddr*)sess->port_addr,sizeof(struct sockaddr)) < 0)
			return 0;
		else
		{
			sess->data_fd = sock;
		}
	}
	if (pasv_active(sess))
	{
		//PASV����ģʽ
		int sock = accept(sess->pasv_listen_fd,NULL,NULL);
		if (sock < 0)
			return 0;
		else
		{
			close(sess->pasv_listen_fd);
			sess->pasv_listen_fd = -1;
			sess->data_fd = sock;
		}
	}
	//���ӽ������ͷŵ�ַ��Ϣ
	if (sess->port_addr)
	{
		free(sess->port_addr);
		sess->port_addr = NULL;
	}
	return ret;
}

static void list_common(session_t *sess)
{
	DIR *dir = opendir(".");//�򿪵�ǰĿ¼
		if (dir == NULL)
			return ;
	char buf[MAX_BUFFER_SIZE] = {0};

	struct stat sbuf;//���ڱ����ļ�������
	struct dirent *dt;
	while ((dt = readdir(dir)) != NULL)
	{
		if (stat(dt->d_name,&sbuf) < 0 )
			continue;
		if (dt->d_name[0] == '.')//���������ļ�
			continue;

		memset(buf,MAX_BUFFER_SIZE,0);
		//�����Ȩ��
		const char *perms = statbuf_get_perms(&sbuf);  //drwxr-xr-x
		int offset = 0;
		offset += sprintf(buf, "%s", perms);
		offset += sprintf(buf+offset, "%3d %-8d %-8d %8u ", sbuf.st_nlink, sbuf.st_uid, sbuf.st_gid, sbuf.st_size);
		
		//�����ʱ������
		const char *pdate = statbuf_get_date(&sbuf);   //Sep 09  2019 
		offset += sprintf(buf+offset, "%s ", pdate);
		sprintf(buf+offset, "%s\r\n", dt->d_name);

		//��������
		send(sess->data_fd, buf, strlen(buf), 0);
	}
}

//��ʾ�б�
static void do_list(session_t *sess)
{
	//1.������������

	if (get_transfer_fd(sess) == 0)
		return ;
	//2.�ظ�150

	ftp_reply(sess,FTP_DATACONN,"Here comes the directory listing.");
	//3.��ʾ�б�
	list_common(sess);

	//4.�ر�����

	close(sess->data_fd);
	sess->data_fd = -1;
	//5.�ظ�226

	ftp_reply(sess,FTP_TRANSFEROK,"Directory send OK.");
}

//����ģʽ
static void do_pasv(session_t *sess)
{
	//227 Entering Passive Mode (192,168,140,136,199,4).
	char ip[16]="192.168.140.136";//������ip
	sess->pasv_listen_fd = tcp_server(ip,0);//portΪ0������ʱ���ɶ˿ں�
	
	struct sockaddr_in address;
	socklen_t addrlen = sizeof(struct sockaddr);
	if (getsockname(sess->pasv_listen_fd,(struct sockaddr*)&address,&addrlen) < 0)//��ȡ�׽�����Ϣ
		ERR_EXIT("getsockname");

	unsigned short port = ntohs(address.sin_port);
	int v[4] ={0};
	sscanf(ip ,"%u.%u.%u.%u",&v[0],&v[1],&v[2],&v[3]);
	char msg[MAX_BUFFER_SIZE] ={0};
	sprintf(msg,"Entering Passive Mode (%u.%u.%u.%u.%u.%u).",v[0],v[1],v[2],v[3],port>>8,port&0x00ff);

	ftp_reply(sess,FTP_PASVOK,msg);
}