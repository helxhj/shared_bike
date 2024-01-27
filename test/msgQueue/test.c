// 练习

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define MAX_SIZE 128
struct msg_queue{
	long int msg_type;
	char msg_text[MAX_SIZE];
};

int main(){
	pid_t pid;
	

	pid =fork();

	if(pid < 0){
		fprintf(stderr,"fork creat failed.reason：%s\n",strerror(errno));
		exit(-1);
	}else if(pid == 0){
		// 子线程接收信息
		int mid = 0;
		struct msg_queue msg;
		int ret = 0;
		mid = msgget((key_t)123, 0666|IPC_CREAT);
		if(mid < 0){
			fprintf(stderr,"child msg queue creat fail.reason:%s\n",strerror(errno));
			exit(-3);
		}

		while(1){ // 循环接收打印数据
			memset(&msg,0,sizeof(msg));
			ret = msgrcv(mid,&msg,sizeof(msg)-sizeof(long int),0,0);
			if(ret < 0){
				fprintf(stderr,"child reaceive fail.reason:%s\n",strerror(errno));
				exit(-4);
			}
			if(strcmp(msg.msg_text,"exit") == 0){
				printf("stop recive and send\n");
				// 将消息从消息队列中删除
				ret = msgctl(mid,IPC_RMID,0);
				if(ret < 0){
					fprintf(stderr,"remove form queue fail.reason:%s\n",strerror(errno));
					exit(-5);
				}
				break;
			}
			printf("child receive :%s\n",msg.msg_text);
		}
		exit(2);

	}else{
		int mid = 0;
		//char text[128];
		int ret = 0;
		struct msg_queue msg;
		// 父线程发送信息
		
		// 获取消息队列
		mid = msgget((key_t)123, 0666 | IPC_CREAT);
		if(mid < 0){
			fprintf(stderr,"parent msg queue creat fail.reason:%s\n",strerror(errno));
			exit(-2);
		}

		while(1){
			memset(&msg,0,sizeof(msg));
			printf("please enter msg for send:");
			scanf("%s",msg.msg_text);
			msg.msg_type = 1;
			// 发送信息
			ret = msgsnd(mid,&msg,sizeof(msg) - sizeof(long int),0);
			if(ret < 0){
				fprintf(stderr,"parent send msg fail.reason:%s\n",strerror(errno));
				exit(-6);
			}
			if(strcmp(msg.msg_text,"exit") == 0){
				break;
			}
			sleep(1);
		}

		printf("parent stop send msg\n");
	}

	if(pid > 0){
		wait(NULL);
	}



	return 0;
}
