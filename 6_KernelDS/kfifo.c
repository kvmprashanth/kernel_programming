#include <linux/init.h>
#include <linux/module.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/sched.h>
#include <linux/kfifo.h>

/*
	SECTION
	Declarations
*/
#define FIFO_SIZE 4096

static struct kfifo fifo;

int val = 7;
char character = '@';
char *name = "Prashanth";
void *out;

/*
	SECTION 
	handling functions
*/
void producer(void)
{
	struct task_struct *task;

	for_each_process(task)
	{
		kfifo_in(&fifo, &task->pid, sizeof(&task->pid));
	}
} 

void consumer(void)
{
	int buf;

	while(!kfifo_is_empty(&fifo)) 
	{
		kfifo_out(&fifo, &buf, sizeof(&buf));
		printk("%d ", buf);
	}
}

/*
	SECTION
	Entry Module
*/
static int __init main_init(void)
{
	int ret;
	struct task_struct *task;
	
	printk("FIFO: -----------------------INSERTED-------------------------\n");
	
	ret = kfifo_alloc(&fifo, FIFO_SIZE, GFP_KERNEL);	
	if(ret)
	{
		printk(KERN_ERR "error kfifo_alloc\n");
 		return ret;
	}

	producer();
	
	printk("OP:");
	consumer();
	printk("\n\n");

	printk("EP:");
	for_each_process(task)
	{
		printk("%d ", task->pid);
	}
	printk("\n\n");	

	kfifo_reset(&fifo);

	//enqueing elements into queue 
	kfifo_in(&fifo,&val,sizeof(int));
	kfifo_in(&fifo,&character,sizeof(char));
	kfifo_in(&fifo,&name,sizeof(char *));
	
	//dequeing elements from queue
	kfifo_out(&fifo,&out,sizeof(int));
	printk("%d \n",(int)out);

	//dequeing elements from queue
	kfifo_out(&fifo,&out,sizeof(char));
	printk("%c \n",(char)out);

	//dequeing elements from queue
	kfifo_out(&fifo,&out,sizeof(char *));
	printk("%s \n",(char *)out);
	
	return 0;
}

/*
	SECTION
	Exit Module
*/
static void __exit main_exit(void)
{
	kfifo_free(&fifo);
	printk("FIFO: -----------------------REMOVED -------------------------\n");
}

/*
	SECTION
	Entry Point
*/
module_init(main_init)
module_exit(main_exit)