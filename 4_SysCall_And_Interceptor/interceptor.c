#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/delay.h>
#include <asm/paravirt.h>

unsigned long **sys_call_table;
unsigned long original_cr0;

asmlinkage long (*ref_sys_display)(void);

asmlinkage long new_sys_display(void)
{	
	printk("Interceptor: SYSCALL intercepted\n");
	return 0;
}

static unsigned long **aquire_sys_call_table(void)
{
	unsigned long int offset = PAGE_OFFSET;
	unsigned long **sct;

	while (offset < ULLONG_MAX) {
		sct = (unsigned long **)offset;

		if (sct[__NR_close] == (unsigned long *) sys_close) 
			return sct;

		offset += sizeof(void *);
	}
	
	return NULL;
}

static int __init interceptor_start(void) 
{
	printk("Intercepter: ------------------------ INSERTED ------------------------\n"); 

	if(!(sys_call_table = aquire_sys_call_table()))
		return -1;
	
	original_cr0 = read_cr0();

	write_cr0(original_cr0 & ~0x00010000);
	ref_sys_display = (void *)sys_call_table[325];
	sys_call_table[325] = (unsigned long *)new_sys_display;
	write_cr0(original_cr0);
	
	return 0;
}

static void __exit interceptor_end(void) 
{
	if(!sys_call_table) {
		return;
	}
	
	write_cr0(original_cr0 & ~0x00010000);
	sys_call_table[325] = (unsigned long *)ref_sys_display;
	write_cr0(original_cr0);
	
	msleep(2000);

	printk("Intercepter: ------------------------ REMOVED ------------------------\n");
}

module_init(interceptor_start);
module_exit(interceptor_end);

MODULE_LICENSE("GPL");