#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#if defined(__GNU_LIBRARY__) && !defined(_SEM_SEMUN_UNDEFINED)						   
#else
union semun {
		int val;                             
		struct semid_ds *buf;    
		unsigned short int *array; 
		struct seminfo *__buf;  
};
#endif     


static int sem_init(int smid){
		int ret = 0;
		union semun  mun;

		mun.val = 1; // 设置信号量的初始值
		ret = semctl(smid,0,SETVAL,mun);
		if(ret < 0){
				fprintf(stderr,"信号量初始化失败，原因:%s\n",strerror(errno));
		}
		return ret;
}

static int sem_p(int smid){
	int ret = 0;
	struct sembuf sop;
	sop.sem_num = 0;
	sop.sem_op = -1;
	sop.sem_flg = SEM_UNDO;

	ret = semop(smid,&sop,1);
	if(ret < 0){
		fprintf(stderr,"信号量p操作失败，原因：%s\n",strerror(errno));
		exit(-3);
	}
	return ret;
}

static int sem_v(smid){
	int ret = 0;
	struct sembuf sop;

	sop.sem_num = 0;
	sop.sem_op = 1;
	sop.sem_flg = SEM_UNDO;
	ret = semop(smid,&sop,1);
	if(ret < 0){
		fprintf(stderr,"信号量v操作失败，原因：%s\n",strerror(errno));
		exit(-4);
	}
	return ret;
}

int main(int argc,char**argv[]){
		int smid = 0; // 信号量id
		int ret = 0;
		int i = 0;

		// 创建信号量
		smid = semget((key_t)123,1,0666|IPC_CREAT);
		if(smid == -1){
				fprintf(stderr,"获取信号量失败，原因：%s\n",strerror(errno));
				exit(-1);
		}
		if(argc > 1){
				// 当传递参数大于1，第一次调用，设置信号量
				ret = sem_init(smid);
				if(ret < 0){
						fprintf(stderr,"初始化失败。\n");
						exit(-2);
				}
		}
		//	printf("argc:%d\n",argc);

		for( i = 0;i < 5;i++){
				/* 模拟临界区----begin */
				// 对信号量进行p操作
				sem_p(smid);
				printf("Process(%d) In\n", getpid());		
				sleep(5);
				printf("Process(%d) Out\n", getpid());
				/* 模拟临界区----end */ 
				sem_v(smid);
				sleep(1);
		}
		return 0;
}
