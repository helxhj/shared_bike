// 读取内存
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <sys/shm.h>

struct Conn_stat{
	int count;
	char ip[64];

};

int main(){
	int shm = 0;
	void* share = NULL;
	struct Conn_stat* stat;
	int i = 0;
	int ret = 0;

	// 1、创建内存
	shm = shmget((key_t)123,sizeof(struct Conn_stat),0666 | IPC_CREAT);
	if(-1 == shm){
		fprintf(stderr,"shmget error,reason:%s\n",strerror(errno));
		exit(-1);
	}
	
	share = shmat(shm,0,0);
	if((void*)-1 == share){
		fprintf(stderr,"shmat error,reason:%s\n",strerror(errno));
		exit(-2);
	}

	stat = (struct Conn_stat*)share;
	while((i++) < 10){
		printf("count:%d,ip:%s\n",stat->count,stat->ip);
		sleep(1);
	}

	// 取消共享内存映射
	ret = shmdt(stat);
	if(ret < 0){
		fprintf(stderr,"shmdt error,reason:%s\n",strerror(errno));
		exit(-3);
	}

	/*
	// 删除内存
	ret = shmctl(shm,IPC_RMID,0);
	if(ret < 0){
		fprintf(stderr,"shmctl error ,reason:%s\n",strerror(errno));
		exit(-4);
	}
	*/
	return 0;
}
