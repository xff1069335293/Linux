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
    //�����׽���
    CHECK_RET(srv_sock.Socket());
    //�󶨵�ַ��Ϣ
    CHECK_RET(srv_sock.Bind(ip,port));

    while (1)
    {		
        string buf;
        string pip;
        uint16_t pport;
        //��������
        CHECK_RET(srv_sock.Recv(&buf,&pip,&pport));
        cout<<"Client "<<ip<<"say: "<<buf<<endl;
        //��������
        buf.clear();
        cout<<"serv say: ";
        cin>>buf;
        CHECK_RET(srv_sock.Send(buf,pip,pport));
    }
    srv_sock.Close();
    //�ر��׽���
    return 0;
}
