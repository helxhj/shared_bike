#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>


int main(){
	int fd[2]; // 管道
	int ret = 0;
	pid_t pid;
    char msg[128];
	int len = 0;
	
	ret = pipe(fd);
	if(ret != 0){
		fprintf(stderr,"create pipe failed,reason:%s\n",strerror(errno));
		exit(-1);
	}

	// 创建进程
	pid = fork();
	if(pid < 0){
		fprintf(stderr,"create fork fail.reason:%s\n",strerror(errno));
		exit(-2);
	}else if(pid == 0){
		//char msg[128];
		//int len = 0;
		// 子进程只读
		close(fd[1]); // 关闭写端口
		//sleep(5);
		while(1){
			memset(msg,0,sizeof(msg));
			len = read(fd[0],msg,sizeof(msg));
			if(len <= 0){
				printf("-----child reveive stop-----\n");
				close(fd[0]);
				break;
			}
			printf("child reacevie msg:%s\n",msg);
			//sleep(1);
		}

	}else{
		// 父进程向子进程发送数据
		//char msg[128];

		// 关闭父进程的读端
		close(fd[0]);
		while(1){
			memset(msg,0,sizeof(msg));
			printf("please enter msg[exit quit]:");
			scanf("%s",msg);
			if(strcmp(msg,"exit") == 0){
				// 不在发送
				close(fd[1]);
				printf("------parent stop send msg------\n");
				break;
			}
			// 发送数据
			write(fd[1],msg,strlen(msg));
			printf("parent send msg :%s\n",msg);
			
		}
	}

	if(pid > 0){
		wait(NULL);
	}

	return 0;
}
