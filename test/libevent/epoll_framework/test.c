#include <stdio.h>
#include <string.h>
int main(){
	char ch[] = "12345";
	char* p = ch;
	*p++;
	*p++;

	printf("ch len:%d\n",strlen(ch));
	printf("p:%s\n",p);
	
	printf("p - ch:%d\n",(p-ch));
	return 0;
}
