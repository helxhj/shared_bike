#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
int main(){
	int client_sock;
	struct sockaddr_in server_addr;
	char buff[64] = {'\0'};
	int ret = 0;

	client_sock = socket(AF_INET,SOCK_STREAM,0);
	if(client_sock < 0){
		perror("sock fail.");
		exit(-1);
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(6666);
	inet_pton(AF_INET,"192.168.85.128",&server_addr.sin_addr.s_addr);

	connect(client_sock,(struct sockaddr*)&server_addr,sizeof(server_addr));


	// 开始写入
	printf("请输入数据:");
	fgets(buff,sizeof(buff),stdin);
	buff[strlen(buff)] = '\0';
	write(client_sock,buff,strlen(buff));
	printf("send %s success\n",buff);
	ret = read(client_sock,buff,sizeof(buff)-1);
	buff[ret] = '\0';
	printf("read from server data:%s\n",buff);
	
	sleep(5);

	printf("请输入数据:");
	fgets(buff,sizeof(buff),stdin);
	buff[strlen(buff)] = '\0';
	write(client_sock,buff,strlen(buff));
	printf("send %s success\n",buff);
	ret = read(client_sock,buff,sizeof(buff)-1);
	buff[ret] = '\0';
	printf("read from server data:%s\n",buff);

	

	close(client_sock);

	return 0;
}
