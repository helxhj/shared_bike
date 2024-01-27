#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

int main(){
	int fd[2]; // 读写描述句柄
	int ret = 0; // 结果
	char read_buf[1024]; // 保存读取数据
	char writ_buf[1024]; // 写数据

	ret = pipe(fd);
	if(ret != 0){
		fprintf(stderr,"pipe creat failed.reason:%s\n",strerror(errno));
		exit(-1);
	}

	// fd[0] 读取
	// fd[1] 写入

	strcpy(writ_buf,"hello world!");
	// 开始往写入端写入数据
	write(fd[1],writ_buf,strlen(writ_buf));
	printf("send %s success\n",writ_buf);

	// 读取数据
	memset(read_buf,0,sizeof(read_buf));
	read(fd[0],read_buf,sizeof(read_buf));
	printf("read %s success\n",read_buf);

	return 0;
}
