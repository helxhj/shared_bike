#include <stdio.h>
#include <signal.h>

void myhandle(int sig){
	printf("Catch a signal : %d\n",sig);
}

int main(){
	struct sigaction act;
	// 设置响应函数
	act.sa_handler = myhandle;
	// 设置屏蔽信号集为空
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;

	sigaction(SIGINT,&act,0);
	while(1){
		sleep(1);
		printf("sleep 1 s\n");
	}

	
	return 0;
}
