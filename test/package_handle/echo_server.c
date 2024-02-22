#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <stdlib.h>

#define SERVER_PORT  6666

#define BUF_MALLOC_LEN 128
#define DATA_LEN 4	// 数据长度
const char* TAG = "HELX"; // 标识


int pack_read(int client_sock,char **buf){
	char *pchr;
	*buf = (char*)malloc(BUF_MALLOC_LEN);
	pchr = *buf;
	memset(pchr,'\0',BUF_MALLOC_LEN);
	
	int tag_len = strlen(TAG);
	
	// 开始读取
	read(client_sock,pchr,tag_len+DATA_LEN);
	printf("pchr:%s\n",pchr);
	
	if(strncmp(pchr,TAG,tag_len) == 0){
		int msg_len = *((int*)(pchr + tag_len));	// 获取到要读取数据的长度
		/*
		if(BUF_MALLOC_LEN < (msg_len + tag_len + DATA_LEN)){
			// 重新分配内存
			free(*buf);
			*buf = (char*)malloc(msg_len + tag_len + DATA_LEN);
			memset(buf,'\0',msg_len + tag_len + DATA_LEN);
			pchr = *buf;
		}*/
		printf("msg_len:%d\n",msg_len);
		//sleep(5);
		
		int count = 0;  // 已经读取的数据长度
		int len = 0; // 这次读取的数据长度
		
		while(count < msg_len){
			len = read(client_sock,pchr+count,msg_len-count);
			printf("rece len:%d\n",len);
			count = count +len;
		}
		return msg_len;
		
	}else{
		return 0;
	}
	
}

int main(void){
    // 一、接收段
    // 1、准备"信箱"，
    int sock;  // 套接字
    // 2、准备"标签"(接收端的ip和端口号)
    struct sockaddr_in server_add;


    // 网络地址    使用tcp协议
    sock = socket(AF_INET,SOCK_STREAM,0);
    
    // 把结构体清空(清空标签)
    bzero(&server_add,sizeof(struct sockaddr_in));
    
    // 写上地址和端口号
    server_add.sin_family = AF_INET;    // 选择协议家族IPv4
    // 如果是和本机通信需要写127.0.0.1
    // 绑定电脑上的所有ip地址(有几个网卡就有几个ip地址)
    server_add.sin_addr.s_addr = htonl(INADDR_ANY);      // 绑定ip地址
    server_add.sin_port = htons(SERVER_PORT);// 绑定端口号
                   
    // 3、将 "标签" 贴到 "信箱" 上
    bind(sock,(struct sockaddr* )&server_add,sizeof(server_add));

    // 将准备好信箱 挂到 小区中心
    listen(sock,64); // 64 同一时刻客户端连接服务端的数量，也可以写成其他整数，如128

    
    // 准备完成，等待来信
    printf("等待客户端的连接\n");
        
    int done = 1;
    while(done){
        struct sockaddr_in client; // 客户端
        int client_sock;  // 与客户端通信
        char client_ip[64]; // 保存客户端的ip
        //char buf[256]; // 保存读取客户端发来的数据
        char *buf = NULL;	// 用于分配内存
		int len; // 具体读取到的长度
		
        socklen_t client_addr_len;

        client_addr_len = sizeof(client);
        client_sock = accept(sock,(struct sockaddr*)&client,&client_addr_len);  // 接收,从sock"信箱"中接收

        // 打印客户端地址和端口号
        printf("client ip:%s\t port:%d\n",
                inet_ntop(AF_INET,&client.sin_addr.s_addr,client_ip,sizeof(client_ip)),
                ntohs(client.sin_port));

        
        // 读取客户端发送的数据
        len = pack_read(client_sock,&buf);
        buf[len] = '\0';
        printf("recive[%d]:%s\n",len,buf);
        
        // 往客户端进行写入
        len = write(client_sock,buf,len);
        printf("write finished.len:%d\n",len);
        // 关闭与客户端的通信
        close(client_sock);
        
    }
    return 0;



}
