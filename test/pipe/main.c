#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[]) 
{
		int fd;
		char buff[1024] = {0,};
		int i = 0;
		for(;i <= argc;i++){
			printf("argv[%d]: %s\n",i,argv[i]);
		}


		sscanf(argv[1], "%d", &fd);
		read(fd, buff, sizeof(buff));

		printf("Process(%d) received information:%s\n",  getpid(), buff);	
		return 0;	
}

