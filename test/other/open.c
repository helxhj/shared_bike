#include <stdio.h>

int main(void)
{
		FILE *out = freopen("stdout.txt", "w", stdout);
		printf("%s\n", "hello verybody!!!");

		return 0;
}

