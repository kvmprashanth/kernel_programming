#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/sched.h>

MODULE_LICENSE("GPL");

extern struct task_struct *pid_task(struct pid *pid, enum pid_type);
/*
	SECTION
	Functions
*/

void DisplayAllTasks(void)
{ 
	struct task_struct *task;	
	
	for_each_process(task) {
		/* this pointlessly prints the name and PID of each task */
		printk("%6d %s\n", task->pid, task->comm);
	}

}	

/*
	SECTION
	Entry Module
*/
static int __init main_init(void)
{
	printk("PP: -----------------------INSERTED-------------------------\n");
	
	DisplayAllTasks();
	
	return 0;
}

/*
	SECTION
	Exit Module
*/
static void __exit main_exit(void)
{
	printk("PP: -----------------------REMOVED-------------------------\n");
}

/*
	SECTION
	Entry Point
*/
module_init(main_init)
module_exit(main_exit)
