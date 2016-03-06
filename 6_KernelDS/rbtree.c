#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/sched.h>
#include <linux/pid.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/mm_types.h>
#include <linux/rbtree.h>

MODULE_LICENSE("GPL");

int pid;
module_param(pid, int, 0);
extern struct task_struct *pid_task(struct pid *pid, enum pid_type);
extern int vm_is_stack_for_task(struct task_struct *t, struct vm_area_struct *vma);

/*
	SECTION
	Functions
*/
void navigate_curr_subtree(struct rb_root *root, struct rb_node *node)
{
	int size;
	struct vm_area_struct *vma;
	struct rb_node vm_rb;

	if(node->rb_left)
		navigate_curr_subtree(root, node->rb_left);
	
	vma = container_of(node, struct vm_area_struct, vm_rb);		
	size = (vma->vm_end - vma->vm_start)/1024;	
	printk("%016lx-%016lx %6uK \n", vma->vm_start, vma->vm_end, size);

	if(node->rb_right)
		navigate_curr_subtree(root, node->rb_right);
}

void DisplayVirtualMemory(struct task_struct *p)
{
	struct rb_root *root = &p->mm->mm_rb;
	navigate_curr_subtree(root, root->rb_node); 
}	

/*
	SECTION
	Entry Module
*/
static int __init main_init(void)
{
	printk("RBT: -----------------------INSERTED-------------------------\n");
	
	if(pid)
	{
		struct task_struct *p;
		struct pid *s_pid;	
		
		s_pid = find_vpid(pid);
		p = pid_task(s_pid, PIDTYPE_PID);
		
		if(p)
		{
			DisplayVirtualMemory(p);	
		}
		else
		{
			printk("RBT: Invalid Process-ID \n");
		}
	}
	else
	{
		printk("RBT: Process PID parameter not passed !\n");
	}
	return 0;
}

/*
	SECTION
	Exit Module
*/
static void __exit main_exit(void)
{
	printk("RBT: -----------------------REMOVED-------------------------\n");
}

/*
	SECTION
	Entry Point
*/
module_init(main_init)
module_exit(main_exit)