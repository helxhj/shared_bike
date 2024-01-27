#include <stdio.h>
#include <stdlib.h>
#include <signal.h>


void myhandle(int sig) 
{
		printf("Catch a signal : %d\n", sig);
}

int main(void) 
{

		signal(SIGINT, myhandle);
		while (1) {
				sleep(1);
		}
		return 0;
}

