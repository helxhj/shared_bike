#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

// 有问题，为什么结束了 ，没有打印信息

#define FILE_NAME "./test.txt"

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
				//exit(-3);
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
				//exit(-4);
		}
		return ret;
}

int main(int argc,char* argv[]){
		pid_t pid;
		FILE * file;
		int i = 0;
		int semid = 0;
		int ret = 0;
		char buf[128];
		int flag = 0;
		
		pid = fork();
		if(pid < 0){
				fprintf(stderr,"fork creat fail.reason:%s\n",strerror(errno));
				exit(-1);
		}else if(pid == 0){
				flag = 0; // 学生
		}else{
				flag = 1; // 老师
		}

		semid = semget((key_t)123,1,IPC_CREAT);
		if(-1 == semid){
			fprintf(stderr,"sem creat fail.reason:%s\n",strerror(errno));
			exit(-3);
		}
		
		if(argc > 1){
			ret = sem_init(semid);
			exit(-4);
		}



		file = fopen(FILE_NAME,"w");
		if(NULL == file){
				fprintf(stderr," open %s file failed\n",FILE_NAME);
				exit(-2);
		}

		for(i = 0; i < 10;i++){
			// -----------临界 beg----------
			if(sem_p(semid) == -1)
			{
				fclose(file);
				exit(-3);
			}

			if(1 == flag){
				memset(buf,0,sizeof(buf));
				// 录入老师信息
				sprintf(buf,"王老师 年龄：42 性别：男\n");
				ret = fwrite(buf,sizeof(char),strlen(buf),file);
				if(ret < 0){
					fprintf(stderr,"teacher write failed,reason:%s\n",strerror(errno));
					fclose(file);
					exit(-4);
				}
				sleep(3);
			}else{
				memset(buf,0,sizeof(buf));
				// 录入学生信息
				sprintf(buf,"学生：李小红 年龄：15  性别：女\n");
				ret = fwrite(buf,sizeof(char),strlen(buf),file);
				if(ret < 0){
					fprintf(stderr,"stduent write failed,reason:%s\n",strerror(errno));
					fclose(file);
					exit(-5);
				}
				sleep(6);
			}
				// -----------临界 end-----------
			if(sem_v(semid) == -1){
				fclose(file);
				exit(-4);
			}
		}
		fclose(file);

		return 0;
}
