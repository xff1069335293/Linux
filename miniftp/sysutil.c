#include "sysutil.h"

#include<netinet/in.h>//地址结构信息
#include<arpa/inet.h> //字节序转化接口
#include<sys/socket.h>//套接字接口

int tcp_server(const char* host, unsigned short port)
{
	//创建套接字，socket(地址域,套接字类型,标志位);
	int listenfd;
	if ((listenfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
		ERR_EXIT("tcp_server");

	struct sockaddr_in addrSer;
	addrSer.sin_family = AF_INET;
	addrSer.sin_port = htons(port);
	addrSer.sin_addr.s_addr = inet_addr(host);
	
	int	on = 1;
	//复用套接字
	if (setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on)) < 0)
		ERR_EXIT("setsockopt");

	//bind(描述符，地址信息，地址大小)
	if (bind(listenfd,(struct sockaddr*)&addrSer,sizeof(addrSer)) < 0)
		ERR_EXIT("bind");

	//将套接字设置为监听状态
	if (listen(listenfd,SOMAXCONN) < 0)
		ERR_EXIT("listen");
	
	return listenfd;
}

//获取数据连接套接字
int tcp_client()
{
	int sock;
	if ((sock=socket(AF_INET,SOCK_STREAM,0)) <0)
		ERR_EXIT("tcp_client");
	return sock;
}


//获取权限
const char *statbuf_get_perms(struct stat *sbuf)
{
	//- --- --- ---
	static char perms[] = "----------";
	mode_t mode = sbuf->st_mode;
	switch(mode & S_IFMT)
	{
	case S_IFSOCK:
		perms[0] = 's';
		break;
	case S_IFLNK:
		perms[0] = 'l';
		break;
	case S_IFREG:
		perms[0] = '-';
		break;
	case S_IFBLK:
		perms[0] = 'b';
		break;
	case S_IFDIR:
		perms[0] = 'd';
		break;
	case S_IFCHR:
		perms[0] = 'c';
		break;
	case S_IFIFO:
		perms[0] = 'p';
		break;
	}

	if(mode & S_IRUSR)
		perms[1] = 'r';
	if(mode & S_IWUSR)
		perms[2] = 'w';
	if(mode & S_IXUSR)
		perms[3] = 'x';

	if(mode & S_IRGRP)
		perms[4] = 'r';
	if(mode & S_IWGRP)
		perms[5] = 'w';
	if(mode & S_IXGRP)
		perms[6] = 'x';

	if(mode & S_IROTH)
		perms[7] = 'r';
	if(mode & S_IWOTH)
		perms[8] = 'w';
	if(mode & S_IXOTH)
		perms[9] = 'x';

	return perms;
}

//获取时间
const char *statbuf_get_date(struct stat *sbuf)
{
	static char datebuf[64] = {0};
	time_t file_time = sbuf->st_mtime;
	struct tm *ptm = localtime(&file_time);
	strftime(datebuf, 64, "%b %e %H:%M",  ptm);
	return datebuf;
}

//发送描述符
void send_fd(int sock_fd, int fd)
{
	int ret;
	struct msghdr msg;
	struct cmsghdr *p_cmsg;
	struct iovec vec;
	char cmsgbuf[CMSG_SPACE(sizeof(fd))];
	int *p_fds;
	char sendchar = 0;
	msg.msg_control = cmsgbuf;
	msg.msg_controllen = sizeof(cmsgbuf);
	p_cmsg = CMSG_FIRSTHDR(&msg);
	p_cmsg->cmsg_level = SOL_SOCKET;
	p_cmsg->cmsg_type = SCM_RIGHTS;
	p_cmsg->cmsg_len = CMSG_LEN(sizeof(fd));
	p_fds = (int*)CMSG_DATA(p_cmsg);
	*p_fds = fd;

	msg.msg_name = NULL;
	msg.msg_namelen = 0;
	msg.msg_iov = &vec;
	msg.msg_iovlen = 1;
	msg.msg_flags = 0;

	vec.iov_base = &sendchar;
	vec.iov_len = sizeof(sendchar);
	ret = sendmsg(sock_fd, &msg, 0);
	if (ret != 1)
		ERR_EXIT("sendmsg");
}

//接收描述符
int recv_fd(const int sock_fd)
{
	int ret;
	struct msghdr msg;
	char recvchar;
	struct iovec vec;
	int recv_fd;
	char cmsgbuf[CMSG_SPACE(sizeof(recv_fd))];
	struct cmsghdr *p_cmsg;
	int *p_fd;
	vec.iov_base = &recvchar;
	vec.iov_len = sizeof(recvchar);
	msg.msg_name = NULL;
	msg.msg_namelen = 0;
	msg.msg_iov = &vec;
	msg.msg_iovlen = 1;
	msg.msg_control = cmsgbuf;
	msg.msg_controllen = sizeof(cmsgbuf);
	msg.msg_flags = 0;

	p_fd = (int*)CMSG_DATA(CMSG_FIRSTHDR(&msg));
	*p_fd = -1;  
	ret = recvmsg(sock_fd, &msg, 0);
	if (ret != 1)
		ERR_EXIT("recvmsg");

	p_cmsg = CMSG_FIRSTHDR(&msg);
	if (p_cmsg == NULL)
		ERR_EXIT("no passed fd");


	p_fd = (int*)CMSG_DATA(p_cmsg);
	recv_fd = *p_fd;
	if (recv_fd == -1)
		ERR_EXIT("no passed fd");

	return recv_fd;
}