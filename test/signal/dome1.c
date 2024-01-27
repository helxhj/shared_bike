#include <stdio.h>
#include <stdlib.h>
#include <signal.h>


void myhandle(int sig) 
{
		printf("Catch a signal : %d\n", sig);
		signal(SIGINT,SIG_DFL); // SIG_DFL将此信号恢复成默认状态
		// 即第一次按下ctrl+c 接收此信号打印出一段换，下一次在
		// 按下ctrl+c 就会将程序终止
}

int main(void) 
{

		signal(SIGINT, myhandle);
		while (1) {
				printf("sleep 1 s\n");
				sleep(1);
		}
		return 0;
}

