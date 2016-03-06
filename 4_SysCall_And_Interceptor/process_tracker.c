#include <stdio.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <unistd.h>

int main()
{
    long int rtn = syscall(325);
    printf("System call invoked: Parent Process-ID: %ld\n", rtn);
    return 0;
}