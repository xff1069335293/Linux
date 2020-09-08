 #include "udpsocket.hpp"

#define CHECK_RET(q) if((q)==false){return false;}

int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		cout<<"Usage: ./udp_cli ip port"<<endl;
		return -1;
	}
	cout<<"Client connect success"<<endl;
	string srv_ip = argv[1];
	uint16_t srv_port = stoi(argv[2]);

	//1.�����׽���
	UdpSocket cli_sock;
	CHECK_RET(cli_sock.Socket());
	//2.�󶨵�ַ��Ϣ(�ͻ��˲��Ƽ����Լ���ַ)
	//3.��������
	while (1)
	{
		cout<<"Client say:";
		string buf;
		cin>>buf;
		if (buf == "quit")
		{
			cout<<"quit"<<endl;
			break;
		}
		CHECK_RET(cli_sock.Send(buf,srv_ip,srv_port));
	//4.��������
		CHECK_RET(cli_sock.Recv(&buf));
		cout<<"Server say: "<<buf<<endl;
	}
	//5.�ر��׽���
	cli_sock.Close();
	return 0;
}