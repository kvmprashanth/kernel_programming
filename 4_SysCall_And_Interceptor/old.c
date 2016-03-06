#include <stdio.h>
#include <stdlib.h>

int lsproc()
{
	int ret;
	__asm__("movl $376, %eax");
	__asm__("int $0x80");
	__asm__("movl %eax, -4(%ebp)");
	return ret;
}

int main()
{
	long int ret;
	ret = lsproc();
	printf("System call sys_display return %ld\n", ret);

	return 0;
}