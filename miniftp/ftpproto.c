#include"ftpproto.h"
#include"ftpcodes.h"//״̬��
#include"str.h"
#include"sysutil.h"
#include"privsock.h"
#include"tunable.h"

session_t *p_sess;
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
static void do_cwd(session_t *sess);
static void do_mkd(session_t *sess);
static void do_rmd (session_t *sess);
static void do_rnfr(session_t *sess);
static void do_rnto(session_t *sess);
static void do_size(session_t *sess);
static void do_dele(session_t *sess);
static void do_stor(session_t *sess);
static void do_retr(session_t *sess);
static void do_rest(session_t *sess);
static void do_quit(session_t *sess);










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
	{"LIST",do_list},
	{"CWD" ,do_cwd },
	{"MKD", do_mkd },
	{"RMD", do_rmd },
	{"RNFR",do_rnfr},
	{"RNTO",do_rnto},
	{"SIZE",do_size},
	{"DELE",do_dele},
	{"STOR",do_stor},
	{"RETR",do_retr},
	{"REST",do_rest},
	{"QUIT",do_quit}
		
};

//���жϿ�
void start_data_alarm();

void handle_ctrl_timeout(int sig)
{
	shutdown(p_sess->ctrl_fd, SHUT_RD);
	ftp_reply(p_sess, FTP_IDLE_TIMEOUT, "Timeout.");
	shutdown(p_sess->ctrl_fd, SHUT_WR);
	exit(EXIT_SUCCESS);
	//close(p_sess->ctrl_fd);
}
void start_cmdio_alarm()
{
	if(tunable_idle_session_timeout > 0)
	{
		signal(SIGALRM, handle_ctrl_timeout);
		alarm(tunable_idle_session_timeout); //��������
	}
}

void handle_data_timeout(int sig)
{
	if(!p_sess->data_process)
	{
		ftp_reply(p_sess, FTP_DATA_TIMEOUT, "Data timeout. Reconnect Sorry.");
		exit(EXIT_FAILURE);
	}
	p_sess->data_process = 0;
	start_data_alarm();
}
void start_data_alarm()
{
	if(tunable_data_connection_timeout > 0)
	{
		signal(SIGALRM, handle_data_timeout);
		alarm(tunable_data_connection_timeout);  //
	}
	else if(tunable_idle_session_timeout > 0)
		alarm(0);
}


////////////////////////////////////////////////////////
//ftp �������
void handle_child(session_t *sess)
{
	ftp_reply(sess,FTP_GREET,"(xff miniftp 1.0)");
	int ret;
	while (1)
	{
		//��ͣ�ȴ��ͻ��˵�������д���
		//��ʼ��
		memset(sess->cmdline, 0, MAX_COMMAND_LINE);
		memset(sess->cmd, 0, MAX_COMMAND);
		memset(sess->arg, 0, MAX_ARG);

		start_cmdio_alarm();

		ret = recv(sess->ctrl_fd, sess->cmdline,MAX_COMMAND_LINE,0);
		if (ret == -1)
			ERR_EXIT("readline");
		else if (ret == 0)
			exit(EXIT_SUCCESS);
		
		str_trim_crlf(sess->cmdline);
		//printf("cmdline = %s\n",sess->cmdline);

		str_split(sess->cmdline,sess->cmd,sess->arg,' ');
		//printf("cmd = %s\n",sess->cmd);
		//printf("arg = %s\n",sess->arg);

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
		sess->is_ascii = 1;
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
//����ģʽ�Ƿ񱻼���
int port_active(session_t *sess)
{
	if(sess->port_addr)
	{
		if(pasv_active(sess))
		{
			fprintf(stderr, "both port and pasv are active.");
			exit(EXIT_FAILURE);
		}
		return 1;
	}
	return 0;
}
//����ģʽ�Ƿ񱻼���
int pasv_active(session_t *sess)
{
	priv_sock_send_cmd(sess->child_fd, PRIV_SOCK_PASV_ACTIVE);
	if(priv_sock_get_int(sess->child_fd))
	{
		if(port_active(sess))
		{
			fprintf(stderr, "both port and pasv are active.");
			exit(EXIT_FAILURE);
		}
		return 1;
	}
	return 0; 
}
//��ȡ�˿���Ϣ
int get_port_fd(session_t *sess)
{
	int ret = 1;
	//ftp �� nobody ����ͨѶ
	priv_sock_send_cmd(sess->child_fd, PRIV_SOCK_GET_DATA_SOCK);
	unsigned short port = ntohs(sess->port_addr->sin_port);
	char *ip = inet_ntoa(sess->port_addr->sin_addr);

	//����port
	priv_sock_send_int(sess->child_fd, (int)port);
	priv_sock_send_buf(sess->child_fd, ip, strlen(ip));

	char res = priv_sock_get_result(sess->child_fd);
	if(res == PRIV_SOCK_RESULT_BAD)
		ret = 0;
	else if(res == PRIV_SOCK_RESULT_OK)
		sess->data_fd = priv_sock_recv_fd(sess->child_fd);
	return ret;
}
//��ȡ��������
int get_pasv_fd(session_t *sess)
{
	int ret = 1;
	priv_sock_send_cmd(sess->child_fd, PRIV_SOCK_PASV_ACCEPT);
	char res = priv_sock_get_result(sess->child_fd);
	if(res == PRIV_SOCK_RESULT_BAD)
		ret = 0;
	else if(res == PRIV_SOCK_RESULT_OK)
		sess->data_fd = priv_sock_recv_fd(sess->child_fd);
	
	return ret;
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
	//PORT����ģʽ
	if (port_active(sess))
	{
		if(!get_port_fd(sess))
			ret = 0;
		//ftp��nobody����ͨѶ
	}
	//PASV����ģʽ
	if (pasv_active(sess))
	{	
		if(!get_pasv_fd(sess))
			ret = 0;
	}
	//���ӽ������ͷŵ�ַ��Ϣ
	if (sess->port_addr)
	{
		free(sess->port_addr);
		sess->port_addr = NULL;
	}
	if (ret)
		start_data_alarm();
	return ret;
}

//��ʾ�б�
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
		offset += sprintf(buf+offset, "%3d %-8d %-8d %8lld ", sbuf.st_nlink, sbuf.st_uid, sbuf.st_gid, (long long)sbuf.st_size);
		
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
	char ip[16]; //��������IP
	getlocalip(ip);

	priv_sock_send_cmd(sess->child_fd, PRIV_SOCK_PASV_LISTEN);
	unsigned short port = (unsigned short)priv_sock_get_int(sess->child_fd);

	int v[4] = {0};
	sscanf(ip, "%u.%u.%u.%u", &v[0], &v[1], &v[2], &v[3]);
	char msg[MAX_BUFFER_SIZE] = {0};
	sprintf(msg, "Entering Passive Mode (%u,%u,%u,%u,%u,%u).", v[0],v[1],v[2],v[3], port>>8, port&0x00ff);
	ftp_reply(sess, FTP_PASVOK, msg);
}

//����Ŀ¼
static void do_cwd(session_t *sess)
{
	if(chdir(sess->arg) < 0)
	{
		//550 Failed to change directory.
		ftp_reply(sess, FTP_NOPERM, "Failed to change directory.");
		return;
	}
	//250 Directory successfully changed.
	ftp_reply(sess, FTP_CWDOK, "Directory successfully changed.");
}
//����Ŀ¼
static void do_mkd(session_t *sess)
{
	if(mkdir(sess->arg, 0777) < 0)
	{
		//550 Create directory operation failed.
		ftp_reply(sess, FTP_NOPERM, "Create directory operation failed.");
		return;
	}
	// 257 "/home/xff/test1/test2/test5/test6/t1"
	char buf[MAX_BUFFER_SIZE] = {0};
	sprintf(buf, "\"%s\" created", sess->arg);
	ftp_reply(sess, FTP_MKDIROK, buf);
}

//ɾ��Ŀ¼
static void do_rmd (session_t *sess)
{
	if(rmdir(sess->arg) < 0)
	{
		// 550 Remove directory operation failed.
		ftp_reply(sess, FTP_NOPERM, "Remove directory operation failed.");
		return;
	}
	// 250 Remove directory operation successful.
	ftp_reply(sess, FTP_RMDIROK, "Remove directory operation successful.");
}
//ɾ���ļ�
static void do_dele(session_t *sess)
{
	if(unlink(sess->arg) < 0)
	{
		ftp_reply(sess, FTP_NOPERM, "Delete operation failed.");
		return;
	}
	//250 Delete operation successful.
	ftp_reply(sess, FTP_DELEOK, "Delete operation successful.");
}
//��ȡԭʼ�ļ�����
static void do_rnfr (session_t *sess)
{
	//��ʼ����ע���ͷ�
	sess->rnfr_name = (char*)malloc(strlen(sess->arg)+1);
	memset(sess->rnfr_name, 0, strlen(sess->arg)+1);
	strcpy(sess->rnfr_name, sess->arg);
	ftp_reply(sess, FTP_RNFROK, "Ready for RNTO.");
}
//�޸��ļ���
static void do_rnto (session_t *sess)
{
	if(sess->rnfr_name == NULL)//û��ԭʼ�ļ�
	{
		//503 RNFR required first.
		ftp_reply(sess, FTP_NEEDRNFR, "RNFR required first.");
		return;
	}

	if(rename(sess->rnfr_name, sess->arg) < 0)
	{
		ftp_reply(sess, FTP_NOPERM, "Rename failed.");
		return;
	}
	
	free(sess->rnfr_name);
	sess->rnfr_name = NULL;
	
	ftp_reply(sess, FTP_RENAMEOK, "Rename successful.");
}
//�ļ���С
static void do_size(session_t *sess)
{
	struct stat sbuf;
	if(stat(sess->arg, &sbuf) < 0)//�ļ�������
	{
		//550 Could not get file size.
		ftp_reply(sess, FTP_FILEFAIL, "Could not get file size.");
		return;
	}

	if(!S_ISREG(sbuf.st_mode))//�ж��Ƿ�����ͨ�ļ�
	{
		ftp_reply(sess, FTP_FILEFAIL, "Could not get file size.");
		return;
	}

	char buf[MAX_BUFFER_SIZE] = {0};
	sprintf(buf, "%d", sbuf.st_size);
	ftp_reply(sess, FTP_SIZEOK, buf);
}

static void limit_rate(session_t *sess, int bytes_transfered, int isupload)
{
	long cur_sec = get_time_sec();
	long cur_usec = get_time_usec();

	double pass_time = (double)(cur_sec - sess->transfer_start_sec);
	pass_time += (double)((cur_usec - sess->transfer_start_usec) / (double)1000000);

	//��ǰ�Ĵ����ٶ�
	unsigned int cur_rate = (unsigned int)((double)bytes_transfered / pass_time);

	double rate_ratio; //����
	if(isupload)
	{
		if(cur_rate <= sess->upload_max_rate)
		{
			sess->transfer_start_sec = cur_sec;
			sess->transfer_start_usec = cur_usec;
			return;
		}
		rate_ratio = cur_rate / sess->upload_max_rate;
	}
	else
	{
		if(cur_rate <= sess->download_max_rate)
		{
			sess->transfer_start_sec = cur_sec;
			sess->transfer_start_usec = cur_usec;
			return;
		}
		rate_ratio = cur_rate / sess->download_max_rate;
	}

	double sleep_time = (rate_ratio - 1) * pass_time;
	nano_sleep(sleep_time);

	sess->transfer_start_sec = get_time_sec();
	sess->transfer_start_usec = get_time_usec();
}

//�ļ��ϴ�
static void do_stor(session_t *sess)
{
	//������������
	if(get_transfer_fd(sess) == 0)
		return;

	int fd = open(sess->arg,  O_CREAT|O_WRONLY, 0755);//����/���ļ�
	if(fd == -1)
	{
		ftp_reply(sess, FTP_FILEFAIL, "Failed to open file.");
		return;
	}
	
	ftp_reply(sess, FTP_DATACONN,"Ok to send data.");
	
	//�ϵ�����
	long long offset = sess->restart_pos;
	sess->restart_pos = 0;
	if(lseek(fd, offset, SEEK_SET) < 0)//ƫ�Ƴ��ִ���
	{
		ftp_reply(sess, FTP_UPLOADFAIL, "Could not create file.");
		return;
	}

	char buf[MAX_BUFFER_SIZE] = {0};
	int ret;

	//�Ǽ�ʱ��
	sess->transfer_start_sec = get_time_sec(); 
	sess->transfer_start_usec = get_time_usec();

	while(1)
	{
		//���տͻ�������
		ret = recv(sess->data_fd, buf, MAX_BUFFER_SIZE, 0);
		if(ret == -1)//����ʧ��
		{
			ftp_reply(sess, FTP_BADSENDFILE, "Failure reading from local file.");
			break;
		}
		if(ret == 0)//�Զ˹رգ��������
		{
			//226 Transfer complete.
			ftp_reply(sess, FTP_TRANSFEROK, "Transfer complete.");
			break;
		}
		
		//����
		if(sess->upload_max_rate != 0)
			limit_rate(sess, ret, 1);

		if(write(fd, buf, ret) != ret)//д�����ݵ��ļ�
		{
			ftp_reply(sess, FTP_BADSENDFILE, "Failure writting to network stream.");
			break;
		}
	}
	//������ɣ��ر���������
	close(fd);
	close(sess->data_fd);
	sess->data_fd = -1;

	//���������������ӶϿ�
	start_cmdio_alarm();
}

//�����ļ�
static void do_retr(session_t *sess)
{
	//������������
	if(get_transfer_fd(sess) == 0)
		return;
	//���ļ���ȡ�������ļ�����
	int fd = open(sess->arg, O_RDONLY);
	if(fd == -1)
	{
		ftp_reply(sess, FTP_FILEFAIL, "Failed to open file.");
		return;
	}

	struct stat sbuf;
	fstat(fd, &sbuf);

	//�ϵ�����
	long long offset = sess->restart_pos;
	sess->restart_pos = 0;
	if(offset >= sbuf.st_size)
	{
		ftp_reply(sess, FTP_TRANSFEROK, "Transfer complete.");
	}
	else
	{
	char msg[MAX_BUFFER_SIZE] = {0};
	if(sess->is_ascii)
		sprintf(msg, "Opening ASCII mode data connection for %s (%d bytes).", sess->arg, sbuf.st_size);
	else
		sprintf(msg, "Opening BINARY mode data connection for %s (%d bytes).", sess->arg, sbuf.st_size);
	// 150 Opening ASCII mode data connection for /home/bss/mytt/abc/test.cpp (70 bytes).
	ftp_reply(sess, FTP_DATACONN, msg);
		
	
	char buf[MAX_BUFFER_SIZE] = {0};
	long long read_total_bytes = sbuf.st_size;
	int read_count = 0;
	int ret;

	//�Ǽ�ʱ��
	sess->transfer_start_sec = get_time_sec(); 
	sess->transfer_start_usec = get_time_usec();

	while(1)
	{
		read_count = read_total_bytes > MAX_BUFFER_SIZE ? MAX_BUFFER_SIZE : read_total_bytes;
		ret = read(fd, buf, read_count);
		if(ret==-1 || ret!=read_count)
		{
			ftp_reply(sess, FTP_BADSENDFILE, "Failure reading from local file.");
			break;
		}
		if(ret == 0)
		{
			// 226 Transfer complete.
			ftp_reply(sess, FTP_TRANSFEROK, "Transfer complete.");
			break;
		}
		
		//����
		if(sess->download_max_rate != 0)
			limit_rate(sess, read_count, 0);

		if(send(sess->data_fd, buf, ret, 0) != ret)
		{
			ftp_reply(sess, FTP_BADSENDFILE, "Failure writting to network stream.");
			break;
		}
		read_total_bytes -= read_count;
	}
	}
	
	close(fd);
	close(sess->data_fd);
	sess->data_fd = -1;

	//���������������ӶϿ�
	start_cmdio_alarm();
}
//�ϵ�����
static void do_rest(session_t *sess)
{
	sess->restart_pos = (long long)atoll(sess->arg);//��¼ƫ����

	//350 Restart position accepted (1612906496).
	char msg[MAX_BUFFER_SIZE] = {0};
	sprintf(msg, "Restart position accepted (%lld).", sess->restart_pos);
	ftp_reply(sess, FTP_RESTOK, msg);
}

static void do_quit(session_t *sess)
{
	ftp_reply(sess, FTP_GOODBYE, "Goodbye.");
}