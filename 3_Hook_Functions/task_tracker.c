#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>

MODULE_LICENSE("GPL");

extern void (*task_creation_ptr)(struct task_struct *);
extern void (*task_termination_ptr)(struct task_struct *);

/*
	SECTION
	Handler Function
*/
void task_creation (struct task_struct *p)
{
	printk(KERN_ALERT"TT \tACTION: Process-Created    \tPID: %d \tP-PID: %d\n",p->pid,p->parent->pid);
}

void task_termination (struct task_struct *p)
{
	printk(KERN_ALERT"TT \tACTION: Process-Terminated \tPID: %d \tP-PID: %d\n",p->pid,p->parent->pid);
}


/*
	SECTION
	Entry Module
*/
static int __init main_init(void)
{
	printk(KERN_ALERT"TT ------------------------ INSERTED ------------------------\n");	
	
	/* Assigning Function pointer */
	task_creation_ptr =  &task_creation;
	task_termination_ptr =  &task_termination;

	return 0;
}

/*
	SECTION
	Exit Module
*/
static void __exit main_exit(void)
{
	/* Resetting Function pointer */
	task_creation_ptr = NULL;
	task_termination_ptr = NULL;

	printk(KERN_ALERT"TT ------------------------ REMOVED ------------------------\n");
}
/*
	SECTION
	Entry Point
*/
module_init(main_init)
module_exit(main_exit)