#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>

MODULE_LICENSE("GPL");

int flag;
module_param(flag, int, 0);
/*
	SECTION
	Entry Module
*/
static int __init main_init(void)
{
	printk(KERN_INFO"Kernel Module Inserted\n");	
	
	if(flag==0)
		printk(KERN_ALERT"Flag is false\n");
	else
		printk(KERN_ALERT"Flag is true\n");
	
	return 0;
}

/*
	SECTION
	Exit Module
*/
static void __exit main_exit(void)
{
	printk(KERN_INFO"Kernel Module Removed\n");
}

/*
	SECTION
	Entry Point
*/
module_init(main_init)
module_exit(main_exit)