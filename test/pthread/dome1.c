#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>

#define MAX_SIZE 80

// 主线程中输入字符串，子线程中打印字符串和其长度
sem_t sem;	// 信号量
char buf[MAX_SIZE];

static void* str_thread_handle(void* arg){
	int ret = 0;
	while(1){
		// 执行p操作
		ret = sem_wait(&sem);
		if(ret != 0){
			fprintf(stderr,"p fail.reason:%s\n",strerror(ret));
			exit(-1);
		}

		printf("string is :%slen:%d\n",buf,(int)strlen(buf));
		if(strcmp(buf,"end") == 0){
			printf("子线程结束\n");
			break;
		}
	}


}

int main(){
	pthread_t pth;  // 线程
	int ret = 0;
	void* thread_return;
	
	// 初始化信号量
	ret = sem_init(&sem,0,0); // 初始信号量为0
	// 现在信号量为0 ，p操作现在无法进入，p操作的现在现在是挂起状态
	if(ret != 0){
		fprintf(stderr,"sem init fail.reason:%s\n",strerror(ret));
		exit(-2);
	}
	
	// 创建线程
	ret = pthread_create(&pth,0,str_thread_handle,0);
	if(ret != 0){
		fprintf(stderr,"creat pthread failed.reason:%s\n",strerror(ret));
		exit(-1);
	}

	// 开始循环输入，当输入end时退出
	while(1){
		
		printf("请输入字符串：");
		fgets(buf,sizeof(buf),stdin); // 读取字符串，直到遇到换行符或者是文件结束符结束
		
		// 执行v操作
		if((ret = sem_post(&sem)) != 0){
			fprintf(stderr,"主线程执行v操作失败.原因:%s\n",strerror(ret));
			exit(-3);
		}

		if(strcmp(buf,"end") == 0){
			break;
		}
		sleep(1);
	}


	// 等待线程返回
	ret = pthread_join(pth,&thread_return);
	if(ret != 0){
		fprintf(stderr,"pthread_join fail.reason:%s\n",strerror(ret));
		exit(-2);
	}

	// 销毁信号量
	ret = sem_destroy(&sem);
	if(ret != 0){
		fprintf(stderr,"sem destroy fail.reason:%s\n",strerror(ret));
		exit(-2);
		
	}

	return 0;

}
