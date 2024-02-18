/***********************************
使用epoll实现多路复用

************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

// 创建并服务端sock 并将sock初始化
int sock_init(){
	int sock;
	struct sockaddr_in sock_addr;
	sock = socket(AF_INET,SOCK_STREAM,0);
	if(-1 == sock){
		perror("sock create fail.");
		exit(-1);
	}
	
	bzero(&sock_addr,0);
	
	sock_addr.sin_family = AF_INET;
	sock_addr.sin_port = htons(80);   // 80 http默认端口
	sock_addr.sin_addr.s_addr = htons();
	
	
}

int main(){
	int server_sock;
	
	server_sock = sock_init();
	
	
	return 0;
}