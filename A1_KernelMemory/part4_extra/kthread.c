#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/mm_types.h>
#include <linux/kthread.h>
#include <linux/time.h>

/* 
    SECTION    
    License 
*/
MODULE_LICENSE("GPL");

/*
    SECTION 
    Global declarations 
*/
#define NO_OF_REGIONS 9
unsigned long kernel_region[NO_OF_REGIONS][2];

struct task_struct *kthread;
/*
    SECTION
    Helper Functions
*/
struct page* softwalk_page_table(unsigned long addr)
{
    pgd_t *pgd;
    pte_t *ptep, pte;
    pud_t *pud;
    pmd_t *pmd;
    struct page *page = NULL;
    struct mm_struct *mm = kthread->active_mm;

    //printk("\n TEST%p",mm);

    pgd = pgd_offset(mm, addr);
    if (pgd_none(*pgd) || pgd_bad(*pgd))
        goto out;
    
    pud = pud_offset(pgd, addr);
    if (pud_none(*pud) || pud_bad(*pud))
        goto out;
    
    pmd = pmd_offset(pud, addr);
    if (pmd_none(*pmd) || pmd_bad(*pmd))
        goto out;
    
    ptep = pte_offset_map(pmd, addr);
    if (!ptep)
        goto out;
    pte = *ptep;

    page = pte_page(pte);
    pte_unmap(ptep);
    
 out:
    return page;
}

/* 
	SECTION
	Handler Function
*/
void Initialize_kernel_regions(void)
{
    //Reserved for Hypervisor
    kernel_region[0][0] = 0xffff800000000000;
    kernel_region[0][1] = 0xffff87ffffffffff;

    //Direct Mappings of all physical memory
    kernel_region[1][0] = 0xffff880000000000;
    kernel_region[1][1] = 0xffffc7ffffffffff;

    //Hole

    //Vmalloc/ioremap space
    kernel_region[2][0] = 0xffffc90000000000;
    kernel_region[2][1] = 0xffffe8ffffffffff;

    //Hole

    //Virtual memory map
    kernel_region[3][0] = 0xffffea0000000000;
    kernel_region[3][1] = 0xffffeaffffffffff;

    //Kasan shadow memory
    kernel_region[4][0] = 0xffffec0000000000;
    kernel_region[4][1] = 0xfffffc0000000000;

    //Esp fixup stack
    kernel_region[5][0] = 0xffffff0000000000;
    kernel_region[5][1] = 0xffffff7fffffffff;

    //Kernel Text mapping
    kernel_region[6][0] = 0xffffffff80000000;
    kernel_region[6][1] = 0xffffffffa0000000;

    //Module mapping
    kernel_region[7][0] = 0xffffffffa0000000;
    kernel_region[7][1] = 0xffffffffff5fffff;

    //Vsyscall
    kernel_region[8][0] = 0xffffffffff600000;
    kernel_region[8][1] = 0xffffffffffdfffff;

    //Hole
}

int MapKernelAddress(void *p)
{
    int i; 
    unsigned long vma_rss, total_rss, vma_size, total_size, not_in_ram, total_not_in_ram;
    unsigned long va, pa;
    struct page *page;

    unsigned long j0,j1;
	int delay = 60*HZ;
	j0 = jiffies;
	j1 = j0 + delay;

    Initialize_kernel_regions();

    total_rss = 0;
    total_size = 0;
    total_not_in_ram = 0;

    for(i = 0; i < NO_OF_REGIONS; i++)
    {
        vma_rss = 0;
        vma_size = 0;
        not_in_ram = 0;

        for(va = kernel_region[i][0]; va < kernel_region[i][1]; va += PAGE_SIZE)
        {
        	page = softwalk_page_table(va);
            
            if(page==NULL)
            {                
                continue;
            }
            else if((pa = page_to_phys(page)))
            {
            	//Displaying Physical addresses of direct mapped regions
                if(i==0)
                    printk("Physical page exists at PA:%016lx \n", pa);
                vma_rss++;
            }
            else
            {
                //printk("Physical Page Not in RAM \n");
                not_in_ram++;
            }

            while (time_before(jiffies, j1))
	        	schedule();
        }
        vma_size = (kernel_region[i][1] - kernel_region[i][0] + 1)/PAGE_SIZE;
        printk("FOR CURRENT REGION,");
        printk("\n\tPresent in RAM (RSS): %lu (%lu kB)",vma_rss, vma_rss*4);
        printk("\n\tNot Present in RAM  : %lu (%lu kB)",not_in_ram, not_in_ram*4);
        printk("\n\tNot Mapped          : %lu (%lu kB)",(vma_size - vma_rss - not_in_ram), (vma_size - vma_rss - not_in_ram)*4);
        printk("\n\tTotal Pages         : %lu (%lu kB)",vma_size, vma_size*4);
        printk("\n\n");

        total_rss += vma_rss;
        total_not_in_ram += not_in_ram;
        total_size += vma_size;
    }
    printk("TOTAL KERNEL-VM,");
    printk("\n\tRSS        : %lu (%lu kB)",total_rss, total_rss*4);
    printk("\n\tMapped     : %lu (%lu kB)",total_rss+total_not_in_ram, (total_rss+total_not_in_ram)*4);
    printk("\n\tTotal Size : %lu (%lu TB) - Excluding hole regions",total_size, 4*total_size/(1024*1024*1024));
    printk("\n");

    return 0;
}

/*
    SECTION
    Entry Module
*/
static int kernel_init(void)
{
    printk("Kthread: -----------------------INSERTED-------------------------\n");
    
    kthread = kthread_run(MapKernelAddress,NULL,"Kthread KMemory-Map");

    return 0;
}

/*
    SECTION
    Exit Module
*/
static void kernel_exit(void)
{
	kthread_stop(kthread);
    printk("Kthread: -----------------------REMOVED-------------------------\n");
}

/*
    SECTION
    Entry Point
*/
module_init(kernel_init)
module_exit(kernel_exit)