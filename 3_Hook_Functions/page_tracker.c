#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>

MODULE_LICENSE("GPL");

int valid_page_faults;
static int faults[10000];

extern void (*page_tracker_ptr)(struct task_struct *);

/*
	SECTION
	Handler Function
*/
void page_fault (struct task_struct *p)
{
	valid_page_faults++;
	faults[p->pid]++;
}


/*
	SECTION
	Entry Module
*/
static int __init main_init(void)
{
	printk(KERN_ALERT"PT: ------------------------ INSERTED ------------------------\n");	
	
	/* Assigning Function pointer */
	page_tracker_ptr = &page_fault;

	return 0;
}

/*
	SECTION
	Exit Module
*/
static void __exit main_exit(void)
{
	/* Resetting Function pointer */
	int i;

	page_tracker_ptr = NULL;

	for(i=1;i<10000;i++)
	{
		if(faults[i]!=0)
			printk(KERN_ALERT"PT: \tPID-%d \tFaults-%d \n",i,faults[i]);	
	}


	printk(KERN_ALERT"PT: Total Page Faults Occured: %d \n",valid_page_faults);
	printk(KERN_ALERT"PT: ------------------------ REMOVED ------------------------\n");
}
/*
	SECTION
	Entry Point
*/
module_init(main_init)
module_exit(main_exit)