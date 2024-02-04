#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
// 两个线程同时修改一个变量
int count = 0;

void* fun(void* val){
	int i = 0;
	for(;i < 100000;i++){
		count++;
	}
	sleep(3);
	// 线程退出
	pthread_exit(&count);
	//return NULL;
}

int main(void){
	pthread_t pth1,pth2;
	int ret = 0;
	void* val1 = 0;
	void* val2 = 0;

	// 创建线程1
	ret = pthread_create(&pth1,NULL,fun,NULL);
	if(ret != 0){
		printf("线程1创建失败。\n");
		exit(-1);
	}

	// 创建线程2
	ret = pthread_create(&pth2,NULL,fun,NULL);
	if(ret != 0){
		printf("线程2创建失败。\n");
		exit(-1);
	}

	// 等待线程1结束
	ret = pthread_join(pth1,&val1);
	if(ret != 0){
		printf("线程1 返回值失败。\n");
		exit(-2);
	}
	printf("线程1 的返回值：%d\n",*((int*)val1));
	// 等待线程2结束
	ret = pthread_join(pth2,&val2);
	if(ret != 0){
		printf("线程2 返回值失败。\n");
		exit(-2);
	}
	printf("线程2 的返回值：%d\n",*((int*)val2));

	printf("最终计数count:%d\n",count);
	return 0;
}
