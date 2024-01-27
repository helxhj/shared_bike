// 写入数据
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

struct Conn_stat{
	int count;
	char ip[64];

};

int main(){
	int shid = 0;
	void* share;
	struct Conn_stat stat = {0,"127.0.0.1"};
	struct Conn_stat* shm;
	int i = 0;
	int ret = 0;

	// 1、创建共享内存
	shid = shmget((key_t)123,sizeof(struct Conn_stat),0666 | IPC_CREAT);
	if(shid < 0){
		fprintf(stderr,"shared memory creat fail,reason:%s\n",strerror(errno));
		exit(-1);
	}

	// 2、挂起共享内存
	share = shmat(shid,0,0);
	if(share == (void*)-1){
		fprintf(stderr,"shmat fail.reason:%s\n",strerror(errno));
		exit(-2);
	}
	
	printf("shared memory addr:%p",share);
	shm = (struct Conn_stat*)share;
	
	memcpy(shm,&stat,sizeof(stat));

	while((i++) < 40){
		shm->count++;
		sleep(1);
	}
	// 3、 取消共享内存映射
	ret = shmdt(shm);
	if(ret < 0){
		fprintf(stderr,"shmdt error,reason:%s\n",strerror(errno));
		exit(-3);
	}
	return 0;
}
