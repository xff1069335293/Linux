#include"ftpproto.h"
#include"ftpcodes.h"//��
void ftp_reply(session_t *sess, int code, const char *text)
{
	char buf[MAX_BUFFER_SIZE] = {0};
	sprintf(buf,"%d %s\r\n",code,text);
	send(sess->ctrl_fd,buf,strlen(buf),0);
}
///////////////////////////////////////
//����ӳ�����
static void do_user(session_t *sess);
static void do_pass(session_t *sess);
static void do_syst(session_t *sess);

typedef struct ftpcmd
{
	const char* cmd;
	void(*cmd_handler)(session_t *sess);
}ftpcmd_t;

//����ӳ���
static ftpcmd_t ctrl_cmds[]=
{
	/*{"USER",do_user},
	{"PASS",do_pass},
	{"SYST",do_syst}*/
};


//ftp �������
void handle_child(session_t *sess)
{
	ftp_reply(sess,FTP_GREET,"(vsFTPd 3.0.2)");
	while (1)
	{
		//��ͣ�ȴ��ͻ��˵�������д���
	}
}