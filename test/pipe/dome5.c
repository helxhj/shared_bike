#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define CMD_EXIT  "exit"

int main(void) 
{
		int fd[2];
		int ret;
		char buff[80];
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
				close(fd[1]);

				while (1) {
						bzero(buff, sizeof(buff));
						if (read(fd[0], buff, sizeof(buff)) == 0) {
								break;
						}
						printf("received information:%s\n", buff);
				}
				printf("receive end!\n");
				close(fd[0]);
		} else {
				close (fd[0]);

				while(1) {
						scanf("%s", buff);
						if (strcmp(buff, CMD_EXIT) == 0) {
								break;
						}

						write(fd[1], buff, strlen(buff));
				}		

				close (fd[1]);		
		}

		if (pd > 0) {
				wait();
		}

		return 0;	
}


