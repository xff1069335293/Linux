#include<cstdio>
#include<string>
#include<iostream>
#include<unistd.h>


#include<sys/socket.h>//�׽��ֽӿ���Ϣ
#include<netinet/in.h>//��ַ�ṹ��Ϣ
#include<arpa/inet.h>//	�ֽ���ת���ӿ�	

using namespace std;

class UdpSocket
{
    public:
        UdpSocket()
            :_sockfd(-1)
        {}
        bool Socket()//�����׽���
        {
	    //socket(IP����,�׽������ͣ�Э��)
            _sockfd = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
            if (_sockfd < 0)
            {
                perror("socket error");
                return false;
            }
            return true;
        }
        bool Bind(const string& ip, uint16_t port)//Ϊ�׽��ְ󶨵�ַ��Ϣ
        {
            struct sockaddr_in addr;
            addr.sin_family = AF_INET;
            addr.sin_port = htons(port);//�����ֽ���ת��Ϊ�����ֽ���
            addr.sin_addr.s_addr = inet_addr(ip.c_str());//���ַ���ipת��Ϊ�����ֽ���

            //bind (����������ַ��Ϣ����ַ����)
	    socklen_t len = sizeof(struct sockaddr_in);
            int ret = bind(_sockfd,(struct sockaddr*)&addr,len);
            if (ret < 0)
            {
                perror("bind error");
                return false;
            }

            return true;
        }
        bool Recv(string* buf, string *ip, uint16_t *port)//�������ݣ���ȡ���Ͷ˵�ַ��Ϣ
        {
            //recvfrom(�׽��֣����ջ����������ݳ��ȣ���־(0������־)��Դ�˵�ַ(�������������)����ַ����)
            struct sockaddr_in source_addr;
            if (!port)
                *port = ntohs(source_addr.sin_port);
            if (!ip)
                *ip = inet_ntoa(source_addr.sin_addr);

	    char recvbuf[4096]={0};
	    socklen_t len = sizeof(struct sockaddr_in);
            int ret = recvfrom(_sockfd, recvbuf,sizeof(recvbuf),0,(struct sockaddr*)&source_addr,&len);
            if (ret < 0 )
            {
                perror("recvfrom error");
                return false;
            }
            buf->assign(recvbuf,ret);//��ָ���ַ����н�ȡָ�����ȷŵ�buf��

            return true;
        }
        bool Send(const string &data, const string &ip, const uint16_t port)//��������,��ȡ���ն˵�ַ��Ϣ
        {
            //sendto(�׽��֣����ͻ����������ݳ��ȣ���־��Ŀ�Ķ˿ڵ�ַ����ַ����)
            struct sockaddr_in destination_addr;
            destination_addr.sin_family = AF_INET;
            destination_addr.sin_port = htons(port);
            destination_addr.sin_addr.s_addr = inet_addr(ip.c_str());
		
	    socklen_t len = sizeof(struct sockaddr_in);
            int ret = sendto(_sockfd, data.c_str(), data.size(),0,(struct sockaddr*)&destination_addr,len);
            if (ret < 0 )
            {
                perror("sendto error");
                return false;
            }
            return true;

        }
        bool Close()//�ر��׽���
        {
            if (_sockfd)
	    {
		close(_sockfd);
		 _sockfd = -1;
	    }
            return true;
        }
    private:
        int _sockfd;
};
