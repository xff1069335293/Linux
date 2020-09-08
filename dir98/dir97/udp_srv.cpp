#include"udpsocket.hpp"
#include<string.h>

#define CHECK_RET(q) if((q) == false){return false;}

int main(int argc, char* argv[])
{
	//运行参数 ./srv 192.168.140.133 9000
    if (argc != 3)
    {
        cout<<"Usage: ./udp_srv ip  port"<<endl;
        return -1;
    }
	cout<<"Server init success"<<endl;
    uint16_t srv_port = stoi(argv[2]);
    string srv_ip = argv[1];

    UdpSocket srv_sock;
    //创建套接字
    CHECK_RET(srv_sock.Socket());
    //绑定地址信息
    CHECK_RET(srv_sock.Bind(srv_ip,srv_port));

    while (1)
    {		
        string buf;
        string ip;
        uint16_t port;
        //发送数据
        CHECK_RET(srv_sock.Recv(&buf,&ip,&port));
        cout<<"Client["<<ip<<":"<<port<<"] say: "<<buf<<endl;
        //接收数据
        buf.clear();
        cout<<"Server say: ";
        cin>>buf;
		if (buf == "quit")
		{
			cout<<"quit"<<endl;
			break;
		}
		//谁发的给谁回复，因为在接收信息的时候已经保存了对端的ip和端口
        CHECK_RET(srv_sock.Send(buf,ip,port));
    }
    srv_sock.Close();
    //关闭套接字
    return 0;
}
