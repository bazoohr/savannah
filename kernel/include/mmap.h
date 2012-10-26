#ifndef __MMAP_H__
#define __MMAP_H__

#include <proc.h>
#include <types.h>

void proc_vmem_init (struct proc *p);
void map_proc_pages (struct proc *p, phys_addr_t phys_addr, virt_addr_t virt_addr, int pages, int type);
void unmap_proc_pages (struct proc *p, virt_addr_t vaddr, size_t pages);
void free_proc_pages (struct proc *p);
void map_iomem (void);
phys_addr_t virt2phys (virt_addr_t vaddr, struct proc *p);

#endif /* __MMAP_H__ */
