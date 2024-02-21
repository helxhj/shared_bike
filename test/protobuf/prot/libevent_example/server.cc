// 实现简单的客户端和服务器端通信
// 客户端从控制台逐行输入数据，按回车键后将数据发送给服务器端，
// 服务器端收到请求后，再将收到的数据回传给客户端显示.

#include <stdio.h>
#include <event2/util.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <event.h>
#include <event2/event.h>
#include <event2/listener.h>
#include <arpa/inet.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>

#include <string>
#include "bike.pb.h"

using namespace std;
using namespace tutorial;

#define PORT 8888
#define BUFLEN  1024

struct event_base* ev_base;  // 事件集

// 连接状态，用于保存时间类型，和收发数据
typedef struct _ConnectStat {
	struct bufferevent* bev;
	char buf[BUFLEN];
}ConnectStat;


//echo 服务实现相关代码
ConnectStat * stat_init(struct bufferevent *bev);

void do_echo_request(struct bufferevent*bev,void *arg);
void eventcb(struct bufferevent* bev,short events,void* arg);
 
void listener_cb(struct evconnlistener* listener,evutil_socket_t fd,struct sockaddr* addr,
int socklen,void* arg);

int main(){
	struct event* ev; // 事件
	struct sockaddr_in sin;
	
	sin.sin_family = AF_INET;
	sin.sin_port = htons(PORT);
	sin.sin_addr.s_addr = INADDR_ANY;
	

	// 当有事件来时开始连接
	// 创建事件集
	ev_base = event_base_new();
	
	// LEV_OPT_REUSEABLE|LEV_OPT_CLOSE_ON_FREE  地址重用  自动释放
	struct evconnlistener* listener = evconnlistener_new_bind(ev_base,listener_cb,ev_base,LEV_OPT_REUSEABLE|LEV_OPT_CLOSE_ON_FREE,20,(struct sockaddr*)&sin,sizeof(sin));
	
	printf("开始等待客户的连接\n");
	
	// 开始循环
	event_base_dispatch(ev_base);
	
	evconnlistener_free(listener);
	event_base_free(ev_base);
	
	return 0;
}

void listener_cb(struct evconnlistener* listener,evutil_socket_t fd,struct sockaddr* addr,
    int socklen,void* arg){
		
	printf("监听到客户端的连接,%d\n",fd);
	struct event_base* ev_base = (struct event_base*)arg;
	
	struct bufferevent* bev = bufferevent_socket_new(ev_base,fd,BEV_OPT_CLOSE_ON_FREE);
	
	ConnectStat* stat = stat_init(bev);
	
	bufferevent_setcb(bev,do_echo_request,NULL,eventcb,(void*)stat);
	
	// 生效
	bufferevent_enable(bev,EV_READ | EV_PERSIST);
}

ConnectStat * stat_init(struct bufferevent *bev){
	ConnectStat* temp = NULL;
	
	// 分配内存
	temp = (ConnectStat*)malloc(sizeof(ConnectStat));
	if(temp == NULL){
		perror("stat_init malloc fail");
		return temp;
	}
	memset(temp,0,sizeof(temp));
	temp->bev = bev;
	return temp;
}

// 开始对客户端的数据进行读取
void do_echo_request(struct bufferevent*bev,void *arg){
	string rece_data;  // 接收序列流
	string send_data;	// 发送序列流
	size_t len = 0;
	ConnectStat* stat = (ConnectStat*)arg;
	char *pch;
	
	 printf("do echo request ...\n");
	
	len = bufferevent_read(bev,stat->buf,sizeof(stat->buf)-1);
	// 出现的错误，是应为 ，使用strchr 获取到的'\n' 位置进行了替换
	// 因为出入的序列流中  中间位置出现的'\n' 所以会将后面的数据进行截断
	// 所以出现数据不匹配的错误
	
	rece_data = stat->buf;

		// 如果是quit退出
	if(strncmp(rece_data.c_str(),"quit",4) == 0){
		printf("客户端请求断开连接\n");
		// 释放
		bufferevent_free(bev);
		free(stat);
		return;
	}
	
	// 获取数据
	list_account_records_request larr;
	larr.ParseFromString(rece_data);
	
	//printf("recv from client<<<< %s\n", larr.mobile().c_str());
	cout << "recv from client:" << larr.mobile() << endl;

	
	// 回应
	list_account_records_response la;
	la.set_code(200);
	la.set_desc("ok");
	
	list_account_records_response_account_record* ar = la.add_records();
	ar->set_type(0);
	ar->set_limit(200);
	ar->set_timestamp(time(NULL));
	
	la.SerializeToString(&send_data);
	
	// 开始写入
	bufferevent_write(bev,send_data.c_str(),send_data.length());
	
	printf("write to client success\n");
}

void eventcb(struct bufferevent* bev,short events,void* arg){
	ConnectStat* stat = (ConnectStat*)arg;
	
	 if (events & BEV_EVENT_EOF)
        printf("connection closed\n");
    else if (events & BEV_EVENT_ERROR)
        printf("some other error\n");
 
    //这将自动close套接字和free读写缓冲区
    bufferevent_free(bev);
    free(stat);	
}