#include "stdio.h"
#include <unistd.h>


int main(int argc, char** argv)
{
		for(int i = 0; i < 4; i++)
		{
				fprintf(stdout, "This is stdout[%d]\n", i);
				sleep(1);

		}
		//fprintf(stdout,"\n");
		sleep(1);

		for(int i = 0; i < 5; i++)
		{
				fprintf(stderr, "This is stderr[%d]", i);
				sleep(1);
		}
		//sleep(10);
		
		//fprintf(stdout,"\n");
		return 0;
}

