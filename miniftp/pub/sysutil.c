#include "sysutil.h"

#include<netinet/in.h>//��ַ�ṹ��Ϣ
#include<arpa/inet.h> //�ֽ���ת���ӿ�
#include<sys/socket.h>//�׽��ֽӿ�

int tcp_server(const char* host, unsigned short port)
{
	//�����׽��֣�socket(��ַ��,�׽�������,��־λ);
	int listenfd;
	if ((listenfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
		ERR_EXIT("tcp_server");

	struct sockaddr_in addrSer;
	addrSer.sin_family = AF_INET;
	addrSer.sin_port = htons(port);
	addrSer.sin_addr.s_addr = inet_addr(host);
	
	int	on = 1;
	//�����׽���
	if (setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on)) < 0)
		ERR_EXIT("setsockopt");

	//bind(����������ַ��Ϣ����ַ��С)
	if (bind(listenfd,(struct sockaddr*)&addrSer,sizeof(addrSer)) < 0)
		ERR_EXIT("bind");

	//���׽�������Ϊ����״̬
	if (listen(listenfd,SOMAXCONN) < 0)
		ERR_EXIT("listen");
	
	return listenfd;
}