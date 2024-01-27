#include <stdio.h>

int main(){
	char msg[128];
	int ret = 0;

	// 其实就是从管道读端开始读取数据
	ret = scanf("%s",msg);
	printf("[ret = %d] %s\n",ret,msg);

	ret = scanf("%s",msg);
	printf("[ret = %d] %s\n",ret,msg);


}
