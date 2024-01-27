#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(void) 
{
		int fd[2];
		int ret;
		char buff1[1024];
		char buff2[1024];
		pid_t pd;

		ret = pipe(fd);
		if (ret !=0) {
				printf("create pipe failed!\n");
				exit(1);
		}

		pd = fork();
		if (pd == -1) {
				printf("fork error!\n");
				exit(1);
		} else if (pd == 0) {
				//		close(fd[1]);
				// 子类写

				strcpy(buff1, "Hello!");
				write(fd[1],buff1,strlen(buff1));

				bzero(buff2, sizeof(buff2));
				read(fd[0], buff2, sizeof(buff2));
				printf("process(%d) received information:%s\n", getpid(), buff2);
		} else {
				strcpy(buff1, "Hello!");
				//close (fd[0]);
				//write(fd[1], buff1, strlen(buff1)); 
				//printf("process(%d) send information:%s\n", getpid(), buff1);

				close (fd[1]);		
				// 读取
				read(fd[0],buff2,sizeof(buff2));
				printf("parent:%s\n",buff2);
	
		}

		if (pd > 0) {
				wait();
		}

		return 0;	
}


