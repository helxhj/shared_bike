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

 
int connect_server(const char* server_ip, int port);
 
 
void cmd_read_data(int fd, short events, void* arg);
void socket_read_data(int fd, short events, void *arg);
 


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
	
	struct event_base* ev_base = event_base_new(); // 创建事件集
	
	// 创建事件
	struct event* ev = event_new(ev_base,client_sock,EV_READ|EV_PERSIST,socket_read_data,NULL);
	// 添加事件
	event_add(ev,NULL);
	
	// 从终端中读取数据
	// 终端中读取的数据，通过文件句柄发送数据
	struct event* ev_cmd = event_new(ev_base,STDIN_FILENO,EV_READ|EV_PERSIST,cmd_read_data,(void*)&client_sock);
	event_add(ev_cmd,NULL);
	
	// 开启循环
	 event_base_dispatch(ev_base);

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


 
void cmd_read_data(int fd, short events, void* arg)
{
	char msg[1024];
	int len = 0;
	int sockfd = *((int*)arg);
	char *pch;
	
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
	
	printf("write to server>>> [%s]\n", msg);
    write(sockfd, msg, strlen(msg));
}

void socket_read_data(int fd, short events, void *arg){
	char msg[1024];
	int len = 0;

	 //为了简单起见，不考虑读一半数据的情况
    len = read(fd, msg, sizeof(msg)-1);
    if( len == 0 )
    {
        printf("connection close. exit~\n");
        exit(1);
    }else if(len < 0){
		perror("read fail ");
		return ;
	}
 
    msg[len] = '\0';
 
    printf("recv from server<<<<< [%s] \n", msg);
	
}