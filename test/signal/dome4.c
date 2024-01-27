//子进程在5秒钟之后给父进程发送一个SIGALR,父进程收到SIGALRM信号之后，“闹铃”
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

void hanle(int sig){
	printf("时间到了，该动动了\n");
}

int main(){
	pid_t pid;
	pid = fork();

	if(pid < 0){
		fprintf(stderr,"creat child process failed,pid:%d\n",pid);
		exit(0);
	}else if(pid == 0){ // 子进程
		while(1){ // 每5秒给父进程发一次
			sleep(5);
			kill(getppid(),SIGALRM); // getppid获取父进程的编号
		}
	}else{
		// 父进程
		struct sigaction act;

		act.sa_flags = 0;
		sigemptyset(&act.sa_mask);

		act.sa_handler = hanle;
		sigaction(SIGALRM,&act,0);

		while(1){
			
		}
		
	}


	return 0;
}
