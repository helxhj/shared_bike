#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
int main(){
	int sockid;
	struct sockaddr_in server_addr;	
	struct sockaddr_in clent_addr;
	int addrlen = sizeof(server_addr);
	char buff[128] = {'\0'};

	// 1、创建套接字
	sockid = socket(AF_INET,SOCK_DGRAM,0);
	//sockid = socket(AF_INET,SOCK_DGRAM|SOCK_NONBLOCK ,0);
	if(-1 == sockid){
		fprintf(stderr,"sock create fail.reason:%s\n",strerror(errno));
		exit(-1);
	}

	// 2、准备标签
	// 清空标签
	bzero(&server_addr,0);

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(6666);
	server_addr.sin_addr.s_addr = INADDR_ANY; // 监听所有地址

	// 3、将标签绑定到套接字上
	bind(sockid,(struct sockaddr*)&server_addr,addrlen);
	fcntl(sockid,F_SETFL,fcntl(sockid,F_GETFL,0) | O_NONBLOCK);
	while(1){
		int len = 0;
		// 接收来自客户端发来的信息
		printf("server wait\n");
		len = recvfrom(sockid,buff,sizeof(buff)-1,0,(struct sockaddr*)&clent_addr,&addrlen);
		if(len < 0){
			if(errno == EAGAIN | errno == EWOULDBLOCK){
				sleep(1);
				continue;
			}

			fprintf(stderr,"recv fail.reason:%s\n",strerror(errno));
			exit(-2);
		}
		buff[len] = '\0';
		printf("recv buff:%s\n",buff);
		
	}

	close(sockid);
	return 0;
}
