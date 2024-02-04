#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>

pthread_mutex_t lock;

#define WORK_SIZE 1024

char work_area[WORK_SIZE];
int time_to_exit = 0;

static void* str_thread_handle(void* arg){
	
	pthread_mutex_lock(&lock);
	while(strncmp(work_area,"exit",4) != 0){
		printf("str len :%d\n",strlen(work_area) - 1);
		work_area[0] = '\0';

		// 解锁
		pthread_mutex_unlock(&lock);
		sleep(1);

		pthread_mutex_lock(&lock);
		while(work_area[0] == '\0'){
			pthread_mutex_unlock(&lock);
			sleep(1); // 主程序开始进入输入字符串
			pthread_mutex_lock(&lock);
		}
	}

	work_area[0] = '\0';
	time_to_exit = 1;
	pthread_mutex_unlock(&lock);
	pthread_exit(0);
}

int main(){
	pthread_t pth;
	void* thread_return;

	// 创建
	if(pthread_create(&pth,0,str_thread_handle,0) != 0){
		exit(-1);
	}
	
	pthread_mutex_init(&lock,NULL);

	// 上锁
	pthread_mutex_lock(&lock);
	while(!time_to_exit){
		fgets(work_area,sizeof(work_area),stdin);
		// 释放
		pthread_mutex_unlock(&lock);

		while(1){
		// 上锁
			pthread_mutex_lock(&lock);
			if(work_area[0] == '\0'){
				break;
			}else{
				pthread_mutex_unlock(&lock);
				sleep(1);
			}
		}
	}
	pthread_mutex_unlock(&lock);

	// 销毁线程
	pthread_join(pth,&thread_return);

	// 销毁锁
	pthread_mutex_destroy(&lock);
	
	return 0;
}
