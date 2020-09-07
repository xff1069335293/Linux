#include<cstdio>
#include<string>
#include<iostream>
#include<unistd.h>


#include<sys/socket.h>//套接字接口信息
#include<netinet/in.h>//地址结构信息
#include<arpa/inet.h>//	字节序转化接口	

using namespace std;

class UdpSocket
{
    public:
        UdpSocket()
            :_sockfd(-1)
        {}
        bool Socket()//创建套接字
        {
	    //socket(IP类型,套接字类型，协议)
            _sockfd = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
            if (_sockfd < 0)
            {
                perror("socket error");
                return false;
            }
            return true;
        }
        bool Bind(const string& ip, uint16_t port)//为套接字绑定地址信息
        {
            struct sockaddr_in addr;
            addr.sin_family = AF_INET;
            addr.sin_port = htons(port);//主机字节序转化为网络字节序
            addr.sin_addr.s_addr = inet_addr(ip.c_str());//将字符串ip转化为网络字节序

            //bind (描述符，地址信息，地址长度)
	    socklen_t len = sizeof(struct sockaddr_in);
            int ret = bind(_sockfd,(struct sockaddr*)&addr,len);
            if (ret < 0)
            {
                perror("bind error");
                return false;
            }

            return true;
        }
        bool Recv(string* buf, string *ip, uint16_t *port)//接收数据，获取发送端地址信息
        {
            //recvfrom(套接字，接收缓冲区，数据长度，标志(0阻塞标志)，源端地址(输入输出型数据)，地址长度)
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
            buf->assign(recvbuf,ret);//从指定字符串中截取指定长度放到buf中

            return true;
        }
        bool Send(const string &data, const string &ip, const uint16_t port)//发送数据,获取接收端地址信息
        {
            //sendto(套接字，发送缓冲区，数据长度，标志，目的端口地址，地址长度)
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
        bool Close()//关闭套接字
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
