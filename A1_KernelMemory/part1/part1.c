#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/mm.h>
/* 
    SECTION    
    License 
*/
MODULE_LICENSE("GPL");

/*
    SECTION 
    Global declarations 
*/

int pid;
module_param(pid, int, 0);
extern struct task_struct *pid_task(struct pid *pid, enum pid_type);

/*
    SECTION
    Helper Functions
*/
struct page* softwalk_page_table(struct mm_struct *mm, unsigned long addr)
{
    pgd_t *pgd;
    pte_t *ptep, pte;
    pud_t *pud;
    pmd_t *pmd;
    struct page *page = NULL;

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

    return page;
    
 out:
    return page;
}

/* 
	SECTION
	Handler Function
*/
void MapVmaAddress(struct task_struct *tsk)
{
    unsigned long vma_rss, total_rss, vma_size, total_size, not_in_ram;
    unsigned long va, pa;
    struct vm_area_struct *vma;
    struct page *page = NULL;

    vma = tsk->mm->mmap;
    va = vma->vm_start;

    total_rss = 0;
    total_size = 0;

    for(vma = tsk->mm->mmap; vma != NULL; vma = vma->vm_next)
    {
        vma_rss = 0;
        vma_size = 0;
        not_in_ram = 0;

        for (va = vma->vm_start; va < vma->vm_end; va += PAGE_SIZE)
        {
            printk("VA:%016lx \t", va);
            page = softwalk_page_table(tsk->mm, va);
            vma_size++;

            if(page==NULL)
            {                
                printk("Page Not Mapped \n");
            }
            else if((pa = page_to_phys(page)))
            {
                printk("Physical page exists at PA:%016lx \n", pa);
                vma_rss++;
            }
            else
            {
                printk("Physical Page Not in RAM \n");
                not_in_ram++;
            }
        }
        printk("FOR CURRENT VMA,");
        printk("\n\tPresent in RAM (RSS): %lu (%lu kB)",vma_rss, vma_rss*4);
        printk("\n\tNot Present in RAM  : %lu (%lu kB)",not_in_ram, not_in_ram*4);
        printk("\n\tNot Mapped          : %lu (%lu kB)",(vma_size - vma_rss - not_in_ram), (vma_size - vma_rss - not_in_ram)*4);
        printk("\n\tTotal Pages         : %lu (%lu kB)",vma_size, vma_size*4);
        printk("\n\n");

        total_rss += vma_rss;
        total_size += vma_size;
    }
    printk("TOTAL VM,");
    printk("\n\tRSS : %lu (%lu kB)",total_rss, total_rss*4);
    printk("\n\tSize: %lu (%lu kB)",total_size, total_size*4);
    printk("\n");
}

/*
    SECTION
    Entry Module
*/
static int __init main_init(void)
{
    printk("Part-1: -----------------------INSERTED-------------------------\n");
    
    if(pid)
    {
        struct task_struct *tsk;
        struct pid *s_pid;  
        
        s_pid = find_vpid(pid);
        tsk = pid_task(s_pid, PIDTYPE_PID);
        
        if(tsk)
        {
            MapVmaAddress(tsk);    
        }
        else
        {
            printk("Part-1: Invalid Process-ID \n");
        }
    }
    else
    {
        printk("Part-1: Process PID parameter not passed !\n");
    }
    return 0;
}

/*
    SECTION
    Exit Module
*/
static void __exit main_exit(void)
{
    printk("Part-1: -----------------------REMOVED-------------------------\n");
}

/*
    SECTION
    Entry Point
*/
module_init(main_init)
module_exit(main_exit)