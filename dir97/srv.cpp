#include"udpsocket.hpp"
#include<unistd.h>
#include<string.h>
#include<string>
#include<iostream>
using namespace std;

#define CHECK_RET(q) if((q)==false){return false;}

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        cout<<"Usage: ./udp_srv ip  port"<<endl;
        return -1;
    }
    uint16_t port = stoi(argv[2]);
    string ip = argv[1];

    UdpSocket srv_sock;
    //创建套接字
    CHECK_RET(srv_sock.Socket());
    //绑定地址信息
    CHECK_RET(srv_sock.Bind(ip,port));

    while (1)
    {		
        string buf;
        string pip;
        uint16_t pport;
        //发送数据
        CHECK_RET(srv_sock.Recv(&buf,&pip,&pport));
        cout<<"Client "<<ip<<"say: "<<buf<<endl;
        //接收数据
        buf.clear();
        cout<<"serv say: ";
        cin>>buf;
        CHECK_RET(srv_sock.Send(buf,pip,pport));
    }
    srv_sock.Close();
    //关闭套接字
    return 0;
}
