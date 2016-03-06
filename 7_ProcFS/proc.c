#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/jiffies.h>
#include <asm/uaccess.h>
#include <linux/slab.h>

#define MODULE_VERS "1.0"
#define MODULE_NAME "procfs_example"
#define FOOBAR_LEN 8

struct fb_data_t 
{
	char name[FOOBAR_LEN + 1];
	char value[FOOBAR_LEN + 1];
} foo_data, bar_data;

static struct proc_dir_entry *example_dir, *foo_file, *bar_file; 

int len,temp;
char *msg;

int proc_read_foobar(struct file *filp, char *buf, size_t count, loff_t *offp)
{
	if(count>temp)
		count=temp;
	
	temp=temp-count;
	copy_to_user(buf, msg, count);
	
	if(count==0)
		temp=len;
	   
	return count;
}

int proc_write_foobar(struct file *filp, const char *buf, size_t count, loff_t *offp)
{
	copy_from_user(msg, buf, count);
	printk(KERN_INFO "In write");
	len=count;
	temp=len;
	return count;
} 

static const struct file_operations foo_fops = {
	.owner = THIS_MODULE,
	.read  = (void *)proc_read_foobar,
	.write  = (void *)proc_write_foobar,
};

static const struct file_operations bar_fops = {
	.owner = THIS_MODULE,
	.read  = (void *)proc_read_foobar,
};

static int __init init_procfs_example(void)
{
	int rv = 0;
	
	//create directory
	example_dir = proc_mkdir(MODULE_NAME, NULL);
	if(example_dir == NULL) 
	{
		rv = -ENOMEM;
		goto out;
	}
	//example_dir->owner = THIS_MODULE;

	/*create jiffies using convenience function
	jiffies_file = proc_create_read_entry("jiffies", 0444, example_dir,
		proc_read_jiffies, NULL);

	if(jiffies_file == NULL) 
	{
		rv = -ENOMEM;
		goto no_jiffies;
	}
	jiffies_file->owner = THIS_MODULE;*/
	
	//create foo and bar files using same callback
	foo_file = proc_create("foo", 0644, example_dir, &foo_fops);
	if(foo_file == NULL) 
	{
		rv = -ENOMEM;
		goto no_foo;
	}
	
	strcpy(foo_data.name, "foo");
	strcpy(foo_data.value, "foo");
	msg = kmalloc(GFP_KERNEL, 100*sizeof(char));

	bar_file = proc_create("bar", 0644, example_dir, &bar_fops);
	if(bar_file == NULL) {
		rv = -ENOMEM;
		goto no_bar;
	}
	
	strcpy(bar_data.name, "bar");
	strcpy(bar_data.value, "bar");
	
	/*
	//create symlink
	symlink = proc_symlink("jiffies_too", example_dir, "jiffies");
	if(symlink == NULL) {
		rv = -ENOMEM;
		goto no_symlink;
	}
	symlink->owner = THIS_MODULE;
	*/

	printk(KERN_INFO "%s %s initialised\n", MODULE_NAME, MODULE_VERS);
	return 0;

	no_bar:
		remove_proc_entry("foo", example_dir);
	no_foo:
		remove_proc_entry("jiffies", example_dir);
	/*
	no_symlink:
		remove_proc_entry("bar", example_dir);
	no_jiffies:
		remove_proc_entry(MODULE_NAME, NULL);
	*/
	out:
		return rv;
}
static void __exit cleanup_procfs_example(void)
{
	//remove_proc_entry("jiffies_too", example_dir);
	remove_proc_entry("bar", example_dir);
	remove_proc_entry("foo", example_dir);
	//remove_proc_entry("jiffies", example_dir);
	remove_proc_entry(MODULE_NAME, NULL);
	printk(KERN_INFO "%s %s removed\n", MODULE_NAME, MODULE_VERS);
}

module_init(init_procfs_example);
module_exit(cleanup_procfs_example);

MODULE_AUTHOR("Prashanth");
MODULE_DESCRIPTION("Proc-FS Example");
MODULE_LICENSE("GPL");
