#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

void *thread_function(void *arg);

#define WORK_SIZE 1024
char work_area[WORK_SIZE];
int time_to_exit = 0;

int main() 
{
		int res;
		pthread_t a_thread;
		void *thread_result;

		res = pthread_create(&a_thread, NULL, thread_function, NULL);
		if (res != 0) {
				perror("Thread creation failed");
				exit(EXIT_FAILURE);
		}

		printf("Input some text. Enter 'end' to finish\n");
		while(!time_to_exit) {
				fgets(work_area, WORK_SIZE, stdin);
				while(1) {
						// 子线程将字符串的第一个字符置为0
						// 退出死循环，在继续输入
						if (work_area[0] != '\0') {
								sleep(1);
						}
						else {
								break;
						}
				}
		}

		printf("\nWaiting for thread to finish...\n");
		res = pthread_join(a_thread, &thread_result);
		if (res != 0) {
				perror("Thread join failed");
				exit(EXIT_FAILURE);
		}
		printf("Thread joined\n");

		exit(EXIT_SUCCESS);
}

void *thread_function(void *arg) 
{
		sleep(1);

		while(strncmp("end", work_area, 3) != 0) {
				printf("You input %d characters\n", strlen(work_area) -1);
				work_area[0] = '\0';
				sleep(1);
				while (work_area[0] == '\0' ) {
						sleep(1);
				}
		}
		time_to_exit = 1;
		work_area[0] = '\0';
		pthread_exit(0);
}

