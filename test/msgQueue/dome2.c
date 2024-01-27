// 接收信息
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>

#define MAX_SIZE 80

struct msg_queue{
	long int msg_type;
	char msg_ch[MAX_SIZE];
};


int main(){
	int mid;
	struct msg_queue msg;
	int ret = 0;

	mid = msgget((key_t)1234,0666 | IPC_CREAT);
	if(mid < 0){
		printf("msg queue creat failed.\n");
		exit(-1);
	}

	msg.msg_type = 0;
	ret = msgrcv(mid,&msg,sizeof(struct msg_queue)-sizeof(long int),0,0);
	if(ret < 0){
		printf("msg queue receive failed\n");
		exit(-2);
	}

	printf("reveive : %s\n",msg.msg_ch);
	
	// 将其从消息队列中删除
	ret = msgctl(mid,IPC_RMID,0);
	if(ret < 0){
		printf("remove msg from queue failed.\n");
		exit(-3);
	}
	return 0;
}
