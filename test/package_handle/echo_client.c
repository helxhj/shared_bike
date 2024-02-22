#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define SERVER_PORT 6666
#define SERVER_IP "127.0.0.1"

#define DATA_LEN  4  // 数据长度
const char* TAG = "HELX";


int main(int argc,char** argv){

    // 准备"邮箱"
    int sockfd;
    char* message;
    struct sockaddr_in servaddr; // 服务端的ip
    int n;
    char* buf = NULL;


    // 执行的命令 echo_client "this is a test"
    if(argc != 2){ // 所以argc的值为2
        fputs("usage:./echo_client message\n",stderr);
        exit(1);
    }

    message = argv[1];
    printf("message:%s\n",message);
    
    // 创建socket
    sockfd = socket(AF_INET,SOCK_STREAM,0);
    
    memset(&servaddr,'0',sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    inet_pton(AF_INET,SERVER_IP,&servaddr.sin_addr); // 将字符ip地址，转换成网络能够接收的ip地址
    servaddr.sin_port = htons(SERVER_PORT);

    // 连接到接收端的邮箱
    connect(sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr));
	
	int tag_len = strlen(TAG);
	int msg_len = strlen(message);
	
	buf = (char *)malloc(tag_len + DATA_LEN + msg_len);
	memset(buf,'\0',tag_len + DATA_LEN + msg_len);
	
	strcpy(buf,TAG); // 拷贝标识头部
	// 指定数据长度，这时候表示数据长度为4字节就是int类型的大小
	*((int*)(buf+tag_len)) = msg_len;
	
	// 剩下的将数据内容进行拷贝
	memcpy(buf+tag_len+DATA_LEN,message,msg_len); 
	printf("buf ：%s,strlen :%d\n",buf+tag_len+DATA_LEN,strlen(buf));
    
    write(sockfd,buf,tag_len + DATA_LEN); // 将包头先写进去
	sleep(1);
	write(sockfd,buf+tag_len + DATA_LEN,4);
	sleep(3);
	write(sockfd,buf+tag_len + DATA_LEN + 4,msg_len-4);
    
	// memset(buf,'\0',tag_len + DATA_LEN + msg_len); // 先不加1 看看，会不会缺少'\0'
	char recv_buf[128] = {'\0'};
    n = read(sockfd,recv_buf,sizeof(recv_buf));
    if(n > 0){ // 接收数据成功
        buf[n] = '\0';
        printf("receive:%s\n",recv_buf);
    }else{
        perror("error!!!");
    }

    printf("finished\n");

    
    close(sockfd);
    return 0;
}
