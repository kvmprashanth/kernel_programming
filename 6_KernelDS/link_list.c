#include <linux/init.h>
#include <linux/module.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/sched.h>

/*
	SECTION
	Declarations
*/

struct task_list
{
	struct list_head list;
	int pid;
	int prio;
	int tgid;
	long state;
	char comm[16];
};

/*
	SECTION 
	handling functions
*/
void ListOperations(void)
{
	int i = 0;
	struct task_struct *task;
	struct task_list mylist;
	struct task_list *temp;
	struct list_head *p, *q;	
	
	INIT_LIST_HEAD(&mylist.list);

	for_each_process(task)
	{
		temp = (struct task_list *) kmalloc(sizeof(struct task_list), GFP_KERNEL);
		
		temp->pid = task->pid;
		temp->prio = task->prio;
		temp->tgid = task->tgid;
		temp->state = task->state;
		strcpy(temp->comm, task->comm);

		list_add_tail(&(temp->list), &(mylist.list));
	}	

	printk("\n TASK STRUCT LIST \n");
	for_each_process(task)
	{
		printk("PID:%5d TGID:%5d PRIO:%3d STATE:%ld NAME:%16s HEAD:%p \n", 
			task->pid, task->tgid, task->prio, task->state, task->comm, &task->tasks);
	}
	
	printk("\n CUSTOM LIST \n");
	list_for_each_entry(temp, &mylist.list, list)
	{
		 printk("PID:%5d TGID:%5d PRIO:%3d STATE:%ld NAME:%16s HEAD:%p \n", 
			temp->pid, temp->tgid, temp->prio, temp->state, temp->comm, &temp->list);
	}	

	list_for_each_safe(p, q, &mylist.list)
	{
		if(i%2==0)
		{
			temp = list_entry(p, struct task_list, list);
			list_del(p);
			kfree(temp);
		}
		i++;
	}


	printk("\n CUSTOM LIST AFTER ALTERNATE DELETE \n");
	list_for_each_entry(temp, &mylist.list, list)
	{
		 printk("PID:%5d TGID:%5d PRIO:%3d STATE:%ld NAME:%16s HEAD:%p \n", 
			temp->pid, temp->tgid, temp->prio, temp->state, temp->comm, &temp->list);
	}

	list_for_each_safe(p, q, &mylist.list)
	{
		temp = list_entry(p, struct task_list, list);
		list_del(p);
		kfree(temp);
	}
}


/*
	SECTION
	Entry Module
*/
static int __init main_init(void)
{
	printk("LL: -----------------------INSERTED-------------------------\n");
	
	ListOperations();

	return 0;
}

/*
	SECTION
	Exit Module
*/
static void __exit main_exit(void)
{
	printk("LL: -----------------------REMOVED -------------------------\n");
}

/*
	SECTION
	Entry Point
*/
module_init(main_init)
module_exit(main_exit)