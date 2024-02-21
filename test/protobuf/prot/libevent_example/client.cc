// 客户端向服务端进行发送数据，并接收服务器端的数据
// 客户端接收键盘的输入，将其发送

#include <stdio.h>
#include <event2/util.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <event.h>
#include <event2/event.h>
#include <arpa/inet.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>

#include "bike.pb.h"

#include <time.h>
#include <string>

using namespace std;
using namespace tutorial;

struct event_base* ev_base;

// 客户端发送 查询充值记录请求
// 服务端对其进行响应
 
int connect_server(const char* server_ip, int port);
 
 
void cmd_read_data(evutil_socket_t fd,short events,void* arg);
void socket_read_data(struct bufferevent*bev,void *arg);
void eventcb(struct bufferevent* bev,short events,void* arg);


int main(int argc,char **argv){
	int client_sock;
	
	if(argc != 3){
		perror("参数个数不对,请重新输入");
		exit(-1);
	}
	
	// 创建socket,并连接服务器端
	client_sock = connect_server(argv[1],atoi(argv[2]));
	if(client_sock == -1){
		perror("connect_server fail");
		exit(-1);
	}
	
	
	ev_base = event_base_new();
	
	struct bufferevent* bev = bufferevent_socket_new(ev_base,client_sock,BEV_OPT_CLOSE_ON_FREE);
	bufferevent_setcb(bev,socket_read_data,NULL,eventcb,NULL);
	// 生效
	bufferevent_enable(bev,EV_READ | EV_PERSIST);
	
	struct event* ev_cmd = event_new(ev_base,STDIN_FILENO,EV_READ|EV_PERSIST,cmd_read_data,(void*)&client_sock);
	event_add(ev_cmd,NULL);
	
	// 开启循环
	event_base_dispatch(ev_base);
	
	event_base_free(ev_base);
	
	

	printf("finished \n");
    return 0;
}

int connect_server(const char* server_ip, int port){
	evutil_socket_t client_sock;
	struct sockaddr_in client_addr;
	int ret = 0;
	client_sock = socket(AF_INET,SOCK_STREAM,0);
	
	if(client_sock == -1){
		return client_sock;
	}
	
	client_addr.sin_family = AF_INET;
	client_addr.sin_port = htons(port);
	inet_aton(server_ip,&client_addr.sin_addr);
	
	// 开始连接
	ret = connect(client_sock,(struct sockaddr*)&client_addr,sizeof(client_addr));
	if(ret == -1){
		ret = errno;
		close(client_sock);
		errno = ret;
		return -1;
	}
	
	return client_sock;
}


 
void cmd_read_data(evutil_socket_t fd,short events,void* arg)
{
	char msg[1024] = {'\0'};
	int len = 0;
	int sockfd = *((int*)arg);
	char *pch;
	string data;

	// 终端读取数据
	
	len = read(fd,msg,sizeof(msg)-1);
	if(len < 0){
		perror("read fail");
		exit(-1);
	}else if(len == 0){
		printf("读取完成\n");
		return;
	}
	
	pch = strchr(msg,'\n');
	if(pch != NULL){
		*pch = '\0';
	}else{
		msg[len] = '\0';
	}
	
	data = msg;
	list_account_records_request larr;
	
	// 除了输入quit之外 全部发送的是序列流
	if(strncmp(msg,"quit",4) != 0){	
		data.clear();
		larr.set_mobile("13523896587");
		// 这个是当有键盘输入时便开始发送请求
		// 进行序列化
		larr.SerializeToString(&data);
	}
	
	// cout << "data size:" << data.size() << endl;
	printf("write to server>>> [%s]\n", data.c_str());
	write(sockfd,data.c_str(),data.size());
}

void socket_read_data(struct bufferevent*bev,void *arg){
	char msg[1024];
	int len = 0;
	string data;
	
	 //为了简单起见，不考虑读一半数据的情况
    len = bufferevent_read(bev,msg,sizeof(msg));
    if( len == 0 )
    {
        printf("connection close. exit~\n");
        exit(1);
    }else if(len < 0){
		perror("read fail ");
		return ;
	}
 
    msg[len] = '\0';
	data = msg;
	
	// 读取数据
	list_account_records_response larr;
	// 解封装
	larr.ParseFromString(data);
	const list_account_records_response_account_record& ar = larr.records(0);
    printf("recv from server<<<<< [code :%d\t desc:%s\t limit:%d] \n", 
			larr.code(),larr.desc().c_str(),ar.limit());
	
}

void eventcb(struct bufferevent* bev,short events,void* arg){
	
	 if (events & BEV_EVENT_EOF)
        printf("connection closed\n");
    else if (events & BEV_EVENT_ERROR)
        printf("some other error\n");
 
    //这将自动close套接字和free读写缓冲区
    bufferevent_free(bev);

}
