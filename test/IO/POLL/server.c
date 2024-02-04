// 使用select进行多路复用
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>

#define MAX_SIZE 8192
int cur_max_fd = 1;

void setcurfd(int sockfd){
	if(sockfd >= cur_max_fd){
		cur_max_fd = sockfd + 1;
	}
	
}

int main(int argc,char**argv){
	int sockfd;
	struct sockaddr_in server_addr;
	fd_set readfds;
	struct pollfd fds[MAX_SIZE];

	sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd == -1){
		fprintf(stderr,"sock create fail.reason:%s\n",strerror(errno));
		exit(-1);
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(6666); // 设置端口号
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	// 绑定
	bind(sockfd,(struct sockaddr*)&server_addr,sizeof(server_addr));

	// 开始监听
	listen(sockfd,5);

	// 初始化可读文件描述符集合
	//FD_ZERO(&readfds);
	//FD_SET(sockfd,&readfds);

	// 将sock作为下标设置监听数组
	setcurfd(sockfd);
	fds[sockfd].fd = sockfd;
	fds[sockfd].events = POLLIN;
	fds[sockfd].revents = 0;

	while(1){
		int result = 0;
		int fd = 0;
		int ret = 0;
		fd_set tempfds;
		struct sockaddr_in client_addr;
		int addrlen = sizeof(client_addr);
		int client_sock;
		int nread = 0;
		char ch[64];
		int i = 0;

		memset(&client_addr,0,sizeof(client_addr));
	
		tempfds = readfds;

		printf("server wait.\n");
		// (struct timeval*)0 表示时间无限  一直等待
		//result = select(FD_SETSIZE,&tempfds,(fd_set*)0,(fd_set*)0,(struct timeval*)0);
		
		result = poll(fds,cur_max_fd,1000); // 每1s监听一次
		if(result < 0){ // -1 失败  0-超时  
			perror("select errro");
			exit(-1);
		}else if(result == 0){
			continue;
		}
		
		for(i = 0;i < cur_max_fd;i++){
			if(fds[i].revents){ // 如果是就绪 poll会将revents设置为非0
				fd = fds[i].fd;
				if(fd == sockfd){
					// 客户端连接
					client_sock = accept(fd,(struct sockaddr*)&client_addr,&addrlen);
					if(client_sock < 0){
						perror("accept fail.");
						exit(-2);
					}
					
					// 加入到集合中
					//FD_SET(client_sock,&readfds);
					setcurfd(client_sock);
					fds[client_sock].fd = client_sock;
					fds[client_sock].events = POLLIN | POLLOUT;
					fds[client_sock].revents = 0;
					printf("\nclient connet to server,client sock:%d\n",client_sock);
				}else{ // 如果是客户端
					//ioctl(fd,FIONREAD,&nread);
					//if(nread == 0){ // 说明此时缓冲区中没有数据了
					//	close(fd);
					//	printf("read compelte,clear client form readfds\n");
						//FD_CLR(fd,&readfds);
					//}else{
						// 从缓冲区中读取数据
						memset(ch,0,sizeof(ch));
						ret = read(fd,ch,sizeof(ch));
						if(ret == 0){
							close(fd);
							memset(&fds[i],0,sizeof(struct pollfd));

							//fds[i].revents = 0;
							printf("\nread compelte,clear client form readfds\n");
							//FD_CLR(fd,&readfds);
						}else{
						ch[ret] = '\0';
						//sleep(5);
						printf("server recv data:%s\n",ch);
						printf("serving client on fd %d\n", fd);
						strcat(ch,"yes");
						// 在写回去
						write(fd,ch,strlen(ch));
					//}
						}
				}
				
			}
		}

	}



	close(sockfd);
	return 0;
}
