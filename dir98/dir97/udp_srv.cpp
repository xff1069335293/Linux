#include"udpsocket.hpp"
#include<string.h>

#define CHECK_RET(q) if((q) == false){return false;}

int main(int argc, char* argv[])
{
	//���в��� ./srv 192.168.140.133 9000
    if (argc != 3)
    {
        cout<<"Usage: ./udp_srv ip  port"<<endl;
        return -1;
    }
	cout<<"Server init success"<<endl;
    uint16_t srv_port = stoi(argv[2]);
    string srv_ip = argv[1];

    UdpSocket srv_sock;
    //�����׽���
    CHECK_RET(srv_sock.Socket());
    //�󶨵�ַ��Ϣ
    CHECK_RET(srv_sock.Bind(srv_ip,srv_port));

    while (1)
    {		
        string buf;
        string ip;
        uint16_t port;
        //��������
        CHECK_RET(srv_sock.Recv(&buf,&ip,&port));
        cout<<"Client["<<ip<<":"<<port<<"] say: "<<buf<<endl;
        //��������
        buf.clear();
        cout<<"Server say: ";
        cin>>buf;
		if (buf == "quit")
		{
			cout<<"quit"<<endl;
			break;
		}
		//˭���ĸ�˭�ظ�����Ϊ�ڽ�����Ϣ��ʱ���Ѿ������˶Զ˵�ip�Ͷ˿�
        CHECK_RET(srv_sock.Send(buf,ip,port));
    }
    srv_sock.Close();
    //�ر��׽���
    return 0;
}
