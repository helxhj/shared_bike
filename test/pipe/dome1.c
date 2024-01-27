// 多进程使用管道通信
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

int main(){
	int fd[2]; // 管道文件描述符
	int ret = 0;
	pid_t fpid;
	char read_buf[1024]; // 读取
	char write_buf[1024]; // 写入

	ret = pipe(fd);
	if(ret != 0){ // 创建管道失败
		fprintf(stderr,"create pipe failed.reason:%s\n",strerror(errno));
		return -1;
	}
	printf("fd[0] : %d,fd[1] : %d\n",fd[0],fd[1]);
	// 创建进程
	fpid = fork();
	if(fpid < 0){
		fprintf(stderr,"create fork failed,reason:%s\n",strerror(errno));
		exit(1);
	}else if(fpid == 0){ // 子进程
		// 先读出
		read(fd[0],read_buf,sizeof(read_buf));
		printf("child receive:%s\n",read_buf);
		printf("child read_buf:%p\n",read_buf);
		// 后写入
		sleep(5);
		strcpy(write_buf,"hello parent");
		write(fd[1],write_buf,sizeof(write_buf));

		printf("child wirte_buf:%p\n",write_buf);


	}else{
		// 父进程
		// 父进程先写入
		strcpy(write_buf,"hello child");
		write(fd[1],write_buf,strlen(write_buf));
		printf("parent send message:%s\n",write_buf);
		printf("parent write_buf:%p\n",write_buf);
		// 后读出
		sleep(6);

		memset(read_buf,0,sizeof(read_buf));
		read(fd[0],read_buf,sizeof(read_buf));
		printf("parent receive:%s\n",read_buf);
		printf("parent read_buf:%p\n",read_buf);
		
	}
	printf("fd[0-%p]:%d,fd[1-%p]:%d\n",&fd[0],fd[0],&fd[1],fd[1]);
	if(fpid > 0){
		wait(NULL);	// 父进程等待子进程
	}


	return 0;
}
