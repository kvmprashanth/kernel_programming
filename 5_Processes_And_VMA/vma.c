#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/sched.h>
#include <linux/pid.h>
#include <linux/fs.h>
#include <linux/path.h>
#include <linux/dcache.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/mm_types.h>

MODULE_LICENSE("GPL");

int pid;
module_param(pid, int, 0);
extern struct task_struct *pid_task(struct pid *pid, enum pid_type);
extern int vm_is_stack_for_task(struct task_struct *t, struct vm_area_struct *vma);
/*
	SECTION
	Functions
*/

void DisplayProcessTree(struct task_struct *p)
{	
	printk("PP: PID-%d, Name-%s \n",p->pid, p->comm);
	printk("PP: Process Traversal ");
	
	while(p->pid)
	{
		printk("%d-",p->pid);
		p = p->parent;
	}
	printk("\n");
}

void DisplayVirtualMemory(struct task_struct *p)
{
	int total=0, size;
	struct vm_area_struct *vma;
	bool read, write, execute, shared, protected;
	
	vma = p->mm->mmap;
	
	do{
		read = vma->vm_flags & 1;
		write = vma->vm_flags & 2;
		execute = vma->vm_flags & 4;
		shared = vma->vm_flags & 8;
		protected = vma->vm_flags & 0x00001000;
		size = (vma->vm_end - vma->vm_start)/1024;
		total += size;
		
		printk("%016lx %6uK ", vma->vm_start, size);
		
		if(read)
			printk("r");
		else
			printk("-");
		
		if(write)
			printk("w");
		else
			printk("-");
			
		if(execute)
			printk("x");
		else
			printk("-");
		
		if(shared)
			printk("s");
		else
			printk("-");
		
		if(protected)
			printk("p");
		else
			printk("-");
		
		if(vma->vm_file!=NULL)
			printk(" %s ",vma->vm_file->f_path.dentry->d_iname);
		else if(vm_is_stack_for_task(p, vma))
			printk(" [ stack ] ");
		else
			printk(" [ anon ] ");
		
		printk("\n");
		vma = vma->vm_next;	
	}while(vma!=NULL);
	
	printk(" total           %6uK \n",total);    
}	

/*
	SECTION
	Entry Module
*/
static int __init main_init(void)
{
	printk("PP: -----------------------INSERTED-------------------------\n");
	
	if(pid)
	{
		struct task_struct *p;
		struct pid *s_pid;	
		
		s_pid = find_vpid(pid);
		p = pid_task(s_pid, PIDTYPE_PID);
		
		if(p)
		{
			DisplayProcessTree(p);
			DisplayVirtualMemory(p);	
		}
		else
		{
			printk("PP: Invalid Process-ID \n");
		}
	}
	else
	{
		printk("PP: Process PID parameter not passed !\n");
	}
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
