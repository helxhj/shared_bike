#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>


// 客户端

int main(){
	int sockid;
	struct sockaddr_in server_addr;
	char buff[128];

	sockid = socket(AF_INET,SOCK_DGRAM,0);

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(6666);
	server_addr.sin_addr.s_addr = inet_addr("192.168.85.128");


	// 发送信息
	printf("please enter your date:");
	fgets(buff,sizeof(buff),stdin);

	sendto(sockid,buff,strlen(buff),0,(struct sockaddr*)&server_addr,sizeof(server_addr));

	printf("send success\n");
	close(sockid);
	return 0;
}
