#include <stdio.h>
#include <unistd.h>
#include <signal.h>

int flag = 0; // 小写

void handle_usr1(int sig){
	// 将小写改为大写
	flag = 1;
}

void handle_usr2(sig){ // 将大写改为小写
	flag = 0;
}

int main(){
	pid_t fpid;
	
	// 创建子进程
	fpid = fork();
	if(fpid < 0){
		fprintf(stderr,"create child process failed,fpid:%d\n",fpid);
	}else if(fpid == 0){
		// 子进程
		struct sigaction act;
		const char* msg;

		act.sa_flags = 0;
		sigemptyset(&act.sa_mask);
		
		// 设置处理函数 与信号
		act.sa_handler = handle_usr1;
		sigaction(SIGUSR1,&act,0);

		// 设置第二个处理函数 与信号
		act.sa_handler = handle_usr2;
		sigaction(SIGUSR2,&act,0);

		while(1){
			if(flag){ // 输出大写
				msg = "CHILD PROCESS WORK!";	
			}else{
				msg = "child process work!";
			}
			printf("%s\n",msg);
			sleep(1);
		}

	}else{
		char ch;
		while(1){
			ch = getchar();
			if(ch == 'A'){
				kill(fpid,SIGUSR1);
			}else if(ch == 'a'){
				kill(fpid,SIGUSR2);
			}
		}
	}



	return 0;
}
