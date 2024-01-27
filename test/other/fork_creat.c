//  进程创建
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
int main()
{
		// Pid_t 进程id
		pid_t fpid;//fpid表示fork函数返回的值
		int count=0;
		int stat = 0;	 // 获取子进程返回的状态

		fpid=fork();
		//fpid = fork();
		printf("fpid: %d\n",fpid);
		if (fpid < 0)
				printf("error in fork!");
		else if (fpid == 0) {
				printf("i am the child process, my process id is %d\n",getpid());
				printf("I’m children\n");
				// count +=2;
				exit(-2); // 结束子进程
		}
		else {
				printf("i am the parent process, my process id is %d\n",getpid());
				printf("I’m parent.\n");
				count++;
		}

		// printf("count addr:%p\n",&count);
		
		printf("统计结果是: %d\n",count);
		// 父进程捕捉子进程的状态
		wait(&stat);
		printf("子进程的状态:%d\n",WEXITSTATUS(stat));
		return 0;
}

