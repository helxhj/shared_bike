#include "bike.pb.h"
#include <string>
#include <iostream>
#include <time.h>

using namespace std;
using namespace tutorial;

int main(){
	// 用于存储要发送数据 序列化后的流
	string data;	// 存储序列化的消息

	{
		// 客户端发送登录验证
		list_account_records_response larr;
		larr.set_code(200);
		larr.set_desc("ok");

		for(int i = 0;i < 5;i++){
			list_account_records_response_account_record* ar = larr.add_records();
			ar->set_type(0);
			ar->set_limit((i+1)*100);
			ar->set_timestamp(time(NULL));
		}
		cout << "添加的字段数量：" << larr.records_size() << endl;
		// 将数据序列化
		larr.SerializeToString(&data);
		// 客户端发送数据
		// send(sock,data.c_str(),data.length());
	}

	{
		// 服务器接收
		// recv(sock,data,sizeof(data));
		list_account_records_response larr;
		larr.ParseFromString(data);
		
		cout << larr.code() << endl;
		cout << larr.desc() << endl;

		cout << "records size:" << larr.records_size() << endl;
		for(int i = 0;i < larr.records_size();i++){
			const list_account_records_response_account_record& ar = larr.records(i);
			cout << ar.limit() << endl; 
		}
	 	cout << "响应代号：" << hex << (int)data[0]	<< endl;
		cout << "data:" << data << endl;
		
	}

	return 0;
}
