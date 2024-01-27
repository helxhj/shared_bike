// 消息队列发送信息
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SIZE 80

struct msg_queue{
	long int msg_type;
	char msg_ch[MAX_SIZE];
};
int main(){
	struct msg_queue msg;
	int msgqid = 0;
	int ret = 0;
	
	msgqid = msgget((key_t)1234,0666 | IPC_CREAT);
	if(msgqid < 0){
		printf("msg queue creat error\n");
		exit(-1);
	}
	
	msg.msg_type = 1; // 设置消息队列类型为1
	strcpy(msg.msg_ch,"hello msg queue");
	// 0 阻塞直到消息队列有空间
	ret = msgsnd(msgqid,&msg,sizeof(struct msg_queue)-sizeof(long int),0);
	if(ret < 0){
		printf("msg queue send fail\n");
		exit(-2);
	}
	
	sleep(10);
	// 让收端将消息从消息队列中删除，发端在发送消息
	strcpy(msg.msg_ch,"hello msg queue 2");
	// 0 阻塞直到消息队列有空间
	ret = msgsnd(msgqid,&msg,sizeof(struct msg_queue)-sizeof(long int),0);
	if(ret < 0){
		printf("msg queue send fail\n");
		exit(-2);
	}

	return 0;
}
