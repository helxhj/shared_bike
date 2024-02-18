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
#include <arpa/inet.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>



#define PORT 8888
#define BUFLEN  1024

struct event_base* ev_base;  // 事件集

// 连接状态，用于保存时间类型，和收发数据
typedef struct _ConnectStat {
	struct event*  ev;
	char buf[BUFLEN];
}ConnectStat;


//echo 服务实现相关代码
ConnectStat * stat_init(struct event *ev);
void accept_connection(int fd, short events, void* arg);
void do_echo_request(int fd, short events, void *arg);
void do_echo_response(int fd, short events, void *arg);
 
int tcp_server_init(int port, int listen_num);

int main(){
	struct event* ev; // 事件
	
	int ser_sock = tcp_server_init(PORT,64);
	if(ser_sock == -1){
		perror("tcp_server_init fail.");
		exit(-1);
	}
	
	// 当有事件来时开始连接
	// 创建事件集
	ev_base = event_base_new();
	
	// 创建事件
	ev = event_new(ev_base,ser_sock,EV_READ | EV_PERSIST,accept_connection,(void*)ev_base);
	
	// 添加时间
	event_add(ev,NULL);
	
	// 开始循环
	event_base_dispatch(ev_base);
	
	return 0;
}

// 创建sock 并开始监听
int tcp_server_init(int port, int listen_num){
	// 为了保证其跨平台 
	evutil_socket_t server;// int listener;
	struct sockaddr_in  server_add;
	int error_save = 0;
	
	server = socket(AF_INET,SOCK_STREAM,0);
	if(-1 == server ){
		return -1;
	}
	
	// 设置地址重用
	evutil_make_listen_socket_reuseable(server);
	
	server_add.sin_family = AF_INET;
	server_add.sin_port = htons(port);
	server_add.sin_addr.s_addr = INADDR_ANY;
	
	// 开始绑定
	error_save = bind(server,(struct sockaddr*)&server_add,sizeof(server_add));
	if(-1 == error_save){
		goto error;
	}
	
	// 开始监听
	if(listen(server,listen_num)<0)
	{
		goto error;
	}
	
	 //跨平台统一接口，将套接字设置为非阻塞状态
    evutil_make_socket_nonblocking(server);
	
	return server;
	
error: 
	// 先保存全局errno
	error_save = errno;
	evutil_closesocket(server);
	errno = error_save;
	return -1;
}

// 连接客户端，并对客户端进行监听
void accept_connection(int fd, short events, void* arg){
	
	evutil_socket_t client_sock = 0;
	struct sockaddr_in client_addr;
	int len = sizeof(client_addr);
	
	// 获取事件集
	struct event_base* ev_base = (struct event_base*)arg;
	
	// 监听客户端
	client_sock = accept(fd,(struct sockaddr*)&client_addr,&len);
	
	printf("监听到客户端的连接\n");
	
	// 设置非阻塞
	evutil_make_socket_nonblocking(client_sock);
	
	// 创建一个未初始化的事件
	struct event* ev = event_new(NULL,-1,0,NULL,NULL);
	ConnectStat* stat = stat_init(ev);
	
	event_assign(ev,ev_base,client_sock,EV_READ,do_echo_request,(void*)stat);
	// 添加到事件集合中
	event_add(ev,NULL);
}

ConnectStat * stat_init(struct event *ev){
	ConnectStat* temp = NULL;
	
	// 分配内存
	temp = (ConnectStat*)malloc(sizeof(ConnectStat));
	if(temp == NULL){
		perror("stat_init malloc fail");
		return temp;
	}
	memset(temp,0,sizeof(temp));
	temp->ev = ev;
	return temp;
}

// 开始对客户端的数据进行读取
void do_echo_request(int fd, short events, void *arg){
	int len = 0;
	ConnectStat* stat = (ConnectStat*)arg;
	char *pchr;
	
	len = read(fd,stat->buf,sizeof(stat->buf)-1);
	if(len < 0){
		perror("read form client fail.");
		goto EXIT_ERR;
	}else if(len == 0){ // 读取完毕，关闭连接
		goto EXIT_ERR;
	}
	
	pchr = strchr(stat->buf,'\n');
	if(pchr != NULL){
		*pchr = '\0';
	}else{
		stat->buf[len] = '\0';
	}
	
	printf("recv form client <<< %s\n",stat->buf);
	
	if(strncmp(stat->buf,"quit",4) == 0){
		printf("断开连接\n");
		goto EXIT_ERR;
	}
	
	// 重新设置事件
	event_set(stat->ev,fd,EV_WRITE,do_echo_response,(void*)stat);
	stat->ev->ev_base = ev_base;
	event_add(stat->ev,NULL);
	return;
	
	
EXIT_ERR:
	event_free(stat->ev);
	close(fd);
    free(stat);
	return;
}

void do_echo_response(int fd, short events, void *arg){
	ConnectStat* stat = (ConnectStat*)arg;
	int len = 0;
	
	printf("write to client>>>> %s\n", stat->buf);
	// 开始向客户端进行写数据
	len = write(fd,stat->buf,strlen(stat->buf));
	
	if(len > 0){
		printf("write successfully.\n");
	}else if(len == 0){ // 句柄被关闭了
		fprintf(stderr,"Remote connection[fd: %d] has been closed\n", fd);
		goto EXIT_RESP;
	}else{
		// 写入失败
		fprintf(stderr,"read faield[fd: %d], reason:%s [%d]\n",fd , strerror(errno), len);
		goto EXIT_RESP;
	}
	
	// 更改事件
	event_set(stat->ev,fd,EV_READ,do_echo_request,(void*)stat);
	stat->ev->ev_base = ev_base;
	event_add(stat->ev,NULL);
	
	return;
	
EXIT_RESP:
	event_free(stat->ev);
	close(fd);
	free(stat);
}