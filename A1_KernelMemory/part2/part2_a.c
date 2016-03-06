#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/mm_types.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>

/* 
    SECTION    
    License 
*/
MODULE_LICENSE("GPL");

/*
    SECTION 
    Global declarations 
*/
int choice;
module_param(choice, int, 0);

struct node
{
    int data;
};

struct node *p;

/*
    SECTION
    Helper Functions
    virt_to_phys
*/
unsigned long softwalk_page_table(unsigned long addr)
{
    pgd_t *pgd;
    pte_t *ptep, pte;
    pud_t *pud;
    pmd_t *pmd;
    
    unsigned long pa;
    struct page *page = NULL;
    struct mm_struct *mm  = current->mm;
    
    unsigned long mask, last12bits;

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
    pte_unmap(ptep);

    printk("pgd-off:%lx pud-off:%lx pmd-off:%lx pte:%lx \n",pgd, pud, pmd, pte);

    page = pte_page(pte);
    pa = page_to_phys(page);

    mask = (1 << 12) - 1;
    last12bits = addr & mask; 
    //printk("Page-Address: %0lx VA: %016lx Mask: %016lx 12bits: %016lx \n", pa, addr, mask, last12bits);
    pa = (pa | last12bits);
    
    return pa;
    
 out:
    return 0;
}

/* 
    SECTION
    Handler Function
*/
void AllocateAndMap(void)
{
    unsigned long va, pa1, pa2;
    
    if(!choice)
        p = kmalloc(sizeof(struct node), GFP_KERNEL);
    else
        p = vmalloc(sizeof(struct node));

    p->data = 10;
    va = (unsigned long) p; 
    printk("Node-P: %p VA: %lu \n", p, va);

    pa1 = virt_to_phys(p);    
    pa2 = softwalk_page_table(va);

    printk("Physical Address Obtained from virt_to_phys : %016lx \n", pa1);
    printk("Physical Address Obtained from software walk: %016lx \n", pa2);
}

/*
    SECTION
    Entry Module
*/
static int __init main_init(void)
{
    printk("Part-2: -----------------------INSERTED-------------------------\n");
    
    AllocateAndMap();
    
    return 0;
}

/*
    SECTION
    Exit Module
*/
static void __exit main_exit(void)
{
    if(!choice)
        kfree(p);
    else
        vfree(p);

    printk("Part-2: -----------------------REMOVED-------------------------\n");
}

/*
    SECTION
    Entry Point
*/
module_init(main_init)
module_exit(main_exit)
