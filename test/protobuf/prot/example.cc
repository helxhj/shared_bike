#include "bike.pb.h"
#include <string>
#include <iostream>

using namespace std;
using namespace tutorial;

int main(){
	// 用于存储要发送数据 序列化后的流
	string data;	// 存储序列化的消息

	{
		// 客户端发送登录验证
		login_request lr;
		lr.set_mobile("13223896587");
		lr.set_icode(511622);

		// 将数据序列化
		lr.SerializeToString(&data);
		// 客户端发送数据
		// send(sock,data.c_str(),data.length());
	}

	{
		// 服务器接收
		// recv(sock,data,sizeof(data));
		
		login_request lr;
		lr.ParseFromString(data);

		// 打印手机号，验证码
		cout << "客户端发来的手机号：" << lr.mobile() << endl;
		cout << "验证码：" << lr.icode() << endl;
		cout << "客户端发来的数据长度 "<< data.length() << " 数据data：" << data << endl;
		cout << "第一数据第一个字节：" << hex << (int)data[0] << endl;
		cout << "第一数据第二个字节：" << hex << (int)data[1] << endl;
		cout << "第二个数据第一字节：" << hex << (int)data[13] << endl;
		cout << "第二个数据第二字节：" << hex << (int)data[14] << endl;
		
	}

	return 0;
}
