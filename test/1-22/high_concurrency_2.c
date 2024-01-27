// 多进程高并发
#define _GNU_SOURCE   // 多进程高并发用的宏
#include <sched.h>		// 头文件
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

static void start_work_processes(int n);
typedef void (*spawn_proce_pt) (void*); // 函数指针
pid_t spawn_process(spawn_proce_pt proce,void* data,char* name);
static void worker_process_cycle(void* data);
static void worker_process_init(int data);	

int main(void){
	// 开始工作子进程
	start_work_processes(4); // 启动资格子进程
	
	//wait(NULL); // 主进程等待子进程
	for(;;)sleep(1);
	return 0;
}

static void start_work_processes(int n){
	int i = 0;
	for(;i < n;i++){
		spawn_process(worker_process_cycle,(void *)(intptr_t)i,"work process");
	}
}

pid_t spawn_process(spawn_proce_pt proce,void* data,char* name){
	pid_t fpid;
	fpid = fork();

	switch(fpid){
		case -1:
			fprintf(stderr,"fork() failed while spawning \"%s\"\n",name);
			break;
		case 0: // 子进程
			proce(data);
		default:
			break; // 主进程
	}

	printf("fpid :%d\n",fpid);
	return fpid;
	
}

static void worker_process_cycle(void* data){
	int worker = (intptr_t)data;
	// 对子进程进程cpu绑定
	worker_process_init(worker);

	// 开始干活
	//for(;;){
	//	sleep(5); // 休眠5s
	//	 printf("pid %ld ,doing ...\n",(long int)getpid()); // 打印进程编号
	// }
	exit(1);
	
}	
static void worker_process_init(int data){
	cpu_set_t cpu_affinity;
	CPU_ZERO(&cpu_affinity);
	CPU_SET(data % 2,&cpu_affinity); // 将4个子进程绑定到2个核上
	if(sched_setaffinity(0,sizeof(cpu_set_t),&cpu_affinity) == -1){
		fprintf(stderr,"sched_setaffinity() failed\n");
	}
	
}	
