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