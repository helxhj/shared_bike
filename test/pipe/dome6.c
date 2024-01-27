// 把管道作为标准输入

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
int main(){
	int fd[2];
	int ret = 0;
	pid_t pid;

	ret = pipe(fd);
	if(ret < 0){
		fprintf(stderr,"pipe create fail.reason:%s\n",strerror(errno));
		exit(-1);
	}

	pid = fork();

	if(pid < 0){
		fprintf(stderr,"fork create fail,reason:%s\n",strerror(errno));
		exit(-2);
	}else if(pid == 0){
		// 子进程通过管道读端读取数据
		close(fd[1]); // 关闭管道写端
		
		close(0); // 关闭标准输入
		dup(fd[0]); // 将管道读端复制一份出来
		close(fd[0]); // 关闭管道读端

		// 通过复制出来的管道读端进行读取数据
		// 如果将所有写端(父进程 和子进程管道的写端关闭)
		// 这个复制出来的读端就会自动关闭
		execl("./od.exe","od.exe","-c",0);
		printf("excel error\n");
		exit(1);
	}else{
		char msg[128];

		close(fd[0]);
		
		// 开始写入
		strcpy(msg,"helloworld");
		write(fd[1],msg,strlen(msg));
			
		sleep(15);

		// 关闭写端
		close(fd[1]);
	}

	if(pid > 0){
		wait(NULL);
	}
	


	
	return 0;
}
