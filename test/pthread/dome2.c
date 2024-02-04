// 练习
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <errno.h>

//主线程每接收到一个字符串之后， 线程1就马上对该字符串进行处理。
//线程1的处理逻辑为：统计该字符串的个数，并记录当时的时间。
//线程1把该字符串处理完后，线程2马上就把处理结果写入文件result.txt
//直到用户输入exit.
#define MAX_SIZE 80

char buf[MAX_SIZE];

sem_t sem1; // 用于线程1的处理
sem_t sem2; // 用于线程2 

// 统计字符串个数
static void* pthread_str_count(void *arg){
	int ret = 0;
	time_t current_time;
	// 使用localtime将时间转换为本地时间结构体
	struct tm* time_info;
	char time_str[50] = {'\0'};

	while(1){
		ret = sem_wait(&sem1);
		if(ret != 0){
			 fprintf(stderr,"pth1 p fail.reason:%s\n",strerror(ret));
			 exit(-2);
		}

		// 获取当前时间
		time(&current_time);
		// 使用localtime将时间转换为本地时间结构体
		time_info = localtime(&current_time);
		// 格式化输出当前时间
		strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", time_info);

		// 打印当前时间
		printf("str count :%d,Current time: %s\n",(int)strlen(buf) ,time_str);

		// 使用v操作，将线程2开始执行
		
		ret = sem_post(&sem2);
		if(ret != 0){
			fprintf(stderr,"pth 2 v fail.reason:%s\n",strerror(ret));
			exit(-3);
		}
		
		if(strncmp(buf,"exit",4) == 0){
			break;
		}

	}


}

// 写入文件
static void* pthread_write_file(void* arg){
	FILE* file;
	int ret = 0;

	file = fopen("./result.txt","w");
	if(file == NULL){
		fprintf(stderr,"result.txt file open fail.reason:%s\n",strerror(errno));
		exit(-4);
	}

	while(1){
		// p操作
		ret = sem_wait(&sem2);
		if(ret != 0){
			fprintf(stderr,"pth1 p fail.reason:%s\n",strerror(ret));
			exit(-3);
		}
		// 写入文件
		ret = fwrite(buf,sizeof(char),strlen(buf),file);
		if(ret < 0){
			fprintf(stderr,"file wirite fail.reason:%s\n",strerror(errno));
			fclose(file);
			exit(-4);
		}
		printf("write to file success\n");
		if(strncmp(buf,"exit",4) == 0){
			break;
		}
		
	}

	fclose(file);

}

int main(){

	pthread_t pth1,pth2;
	int ret = 0;
	void* thread_return;

	// 开始对信号量初始化
	ret = sem_init(&sem1,0,0);
	if(ret != 0){
		fprintf(stderr,"sem1 init fail.reason:%s\n",strerror(ret));
		exit(-2);
	}
	
	// 开始对信号量初始化
	ret = sem_init(&sem2,0,0);
	if(ret != 0){
		fprintf(stderr,"sem2 init fail.reason:%s\n",strerror(ret));
		exit(-2);
	}


	// 线程1创建
	ret = pthread_create(&pth1,0,pthread_str_count,0);
	if(ret != 0){
		fprintf(stderr,"creat pthread1 failed.reason:%s\n",strerror(ret));
		exit(-1);
	}
	
	// 线程2创建
	ret = pthread_create(&pth2,0,pthread_write_file,0);
	if(ret != 0){
		fprintf(stderr,"creat pthread2 failed.reason:%s\n",strerror(ret));
		exit(-1);
	}

	// 主线程
	while(1){
		printf("请输入字符串：");
		fgets(buf,sizeof(buf),stdin);
		
		// v操作
		ret = sem_post(&sem1);
		if(ret != 0){
			fprintf(stderr,"main pro v fail.reason:%s\n",strerror(ret));
			exit(-3);
		}

		if(strncmp(buf,"exit",4) == 0){
			break;
		}
		sleep(1);

	}


	// 等待子线程销毁
 	ret = pthread_join(pth1,&thread_return);
    if(ret != 0){
		fprintf(stderr,"pthread_join pth1 fail.reason:%s\n",strerror(ret));
		exit(-2);
	}

	// 等待子线程销毁
 	ret = pthread_join(pth2,&thread_return);
    if(ret != 0){
		fprintf(stderr,"pthread_join pth2 fail.reason:%s\n",strerror(ret));
		exit(-2);
	}

	// 销毁信号量
	ret = sem_destroy(&sem1);
	if(ret != 0){
		fprintf(stderr,"sem1 destroy fail.reason:%s\n",strerror(ret));
		exit(-2);
	}

	// 销毁信号量
	ret = sem_destroy(&sem2);
	if(ret != 0){
		fprintf(stderr,"sem2 destroy fail.reason:%s\n",strerror(ret));
		exit(-2);
	}

	return 0;
}

