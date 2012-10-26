#include <proc.h>
#include <cdef.h>
#include <page.h>
#include <mmap.h>
#include <string.h>
#include <asmfunc.h>
#include <memory.h>
#include <panic.h>
#include <printk.h>
#include <global.h>
extern phys_addr_t kernel_end_addr;

void
map_iomem (void)
{
  phys_addr_t paddr;
  phys_addr_t iomem_start;
  phys_addr_t iomem_end;
  virt_addr_t vaddr;
  page_table_entry_t *pml4;
  page_table_entry_t *pdpe;
  page_table_entry_t *pde;
  int pml4_idx;
  int pdpe_idx;
  int pde_idx;
  int page;
  int last_pages;
  size_t pages;
  size_t iomem_size;

  iomem_start = 0xFEC00000;
  iomem_end   = 0x100000000;

  iomem_size = iomem_end - iomem_start;
  pages = iomem_size / KNL_PAGE_SIZE + (iomem_size % KNL_PAGE_SIZE ? 1 : 0);

  pml4 = (page_table_entry_t *)kernel_pml4;

  paddr = iomem_start;
  /*
   * In the whole kernel, virtual addresses are equal to
   * physical addresses
   */
  vaddr = paddr;
  /* TODO:
   *  1. Replace all constants here with some meaningfull macros.
   */ 
  page = 0;
  last_pages = pages;
  while (page != last_pages) {
    /* TODO:
     *     A boundry check to make sure none of indices go beyond
     *     512.
     */
    pml4_idx = (vaddr >> 39) & 0x1FF;
    pdpe_idx = (vaddr >> 30) & 0x1FF;
    pde_idx  = (vaddr >> 21) & 0x1FF;

    pdpe = (page_table_entry_t *)((phys_addr_t)pml4[pml4_idx] & ~0xFFF);
    pde = (page_table_entry_t *)((phys_addr_t)pdpe[pdpe_idx] & ~0xFFF);

    if (pml4[pml4_idx] == 0) {
      panic ("IO mapped memory region can not be beyond 4GB");
    }
    if (pdpe[pdpe_idx] == 0) {
      pde = (page_table_entry_t *)alloc_mem_pages (1);

      memset (pde, 0, PAGE_TABLE_SIZE);

      pdpe[pdpe_idx] = (phys_addr_t)pde  | (PAGE_PRESENT | PAGE_RW);
    }
    pde[pde_idx] = paddr | (PAGE_PRESENT | PAGE_RW | PAGE_PSE);
    page++;
    paddr += KNL_PAGE_SIZE;
    vaddr += KNL_PAGE_SIZE;
  }
}
/*
 * FIXME:
 *     Define some mechanism to make sure that virtuall addresses are ALWAYS
 *     in canonical format. Any violation from canonical address format leads
 *     to a #GP exception (vector no. 13).
 *     This usually doesn't happen when we use very small virtuall adress space,
 *     but is definitely a potential danger.
 */
void
proc_vmem_init (struct proc *p)
{
  int i;
  int kpages = kernel_end_addr / USR_PAGE_SIZE + (kernel_end_addr % USR_PAGE_SIZE != 0 ? 1 : 0);
  int npte   = kpages / PAGE_TABLE_ENTRIES + (kpages % PAGE_TABLE_ENTRIES != 0 ? 1 : 0);
  int npde   = npte / PAGE_TABLE_ENTRIES + (npte % PAGE_TABLE_ENTRIES != 0 ? 1 : 0);
  int npdpe  = npde / PAGE_TABLE_ENTRIES + (npde % PAGE_TABLE_ENTRIES != 0 ? 1 : 0);
  int npml4  = npdpe / PAGE_TABLE_ENTRIES + (npdpe % PAGE_TABLE_ENTRIES != 0 ? 1 : 0);
  
  int npgtb = npml4 + npdpe + npde + npte;
  page_table_entry_t *pgtb = (page_table_entry_t *)alloc_mem_pages (npgtb);

  memset (pgtb, 0, npgtb * PAGE_TABLE_SIZE);

  page_table_entry_t *pml4 = pgtb;
  page_table_entry_t *pdpe = pgtb + npml4 * PAGE_TABLE_ENTRIES;
  page_table_entry_t *pde  = pgtb + (npml4 + npdpe) * PAGE_TABLE_ENTRIES;
  page_table_entry_t *pte  = pgtb + (npml4 + npdpe + npde) * PAGE_TABLE_ENTRIES;

  /* With only one PDPE, we can map 512 GB of memory. So it is very unlikely that
   * we need to have a second PDPE. In this OS, we assume that 1GB of virtual memory is
   * didicated to kernel, and the rest of virtual address space, can be used by user processes.
   * In PML4, we have to set U/S bit, and ristrict user access in later stages.
   */
  for (i = 0; i < kpages; i++) {
    pte[i] = i * USR_PAGE_SIZE | (PAGE_PRESENT | PAGE_RW | PAGE_GLOBAL);
  }
  for (i = 0; i < npte; i++) {
    pde[i] = (phys_addr_t)&pte[i * PAGE_TABLE_ENTRIES] | PAGE_PRESENT | PAGE_RW;
  }
  for (i = 0; i < npde; i++) {
    pdpe[i] = (phys_addr_t)&pde[i * PAGE_TABLE_ENTRIES] | (PAGE_PRESENT | PAGE_RW);
  }
  for (i = 0; i < npdpe; i++) {
    pml4[i] = (phys_addr_t)&pdpe[i * PAGE_TABLE_ENTRIES] | (PAGE_PRESENT | PAGE_RW | PAGE_USR);
  }

  p->p_pml4 = (phys_addr_t)pml4;
}

void
map_proc_pages (struct proc *p, phys_addr_t phys_addr, virt_addr_t virt_addr, int pages, int type)
{
  phys_addr_t paddr;
  virt_addr_t vaddr;
  page_table_entry_t *pml4;
  page_table_entry_t *pdpe;
  page_table_entry_t *pde;
  page_table_entry_t *pte;
  int pml4_idx;
  int pdpe_idx;
  int pde_idx;
  int pte_idx;
  int page;
  int last_pages;
  uint64_t protect = 0;

  switch (type) {
    case CODE_SEG:
      /* Executable Read-Only Page */
      protect = PAGE_PRESENT | PAGE_USR;
      break;
    case DATA_SEG:
      /* Executable Writable Page */
      protect = PAGE_PRESENT | PAGE_RW | PAGE_USR;
      break;
    case RODATA_SEG:
      /* Non-Executable Read-Only Page */
      protect = PAGE_NX | PAGE_PRESENT | PAGE_USR;
      break;
    case BSS_SEG:
    case STACK_SEG:
      /* Non-Executable Writable Page */
      protect = PAGE_NX | PAGE_PRESENT | PAGE_RW | PAGE_USR;
      break;
    default:
      panic ("Undefined page type!\n");
  }
  pml4 = (page_table_entry_t *)p->p_pml4;

  /* TODO:
   *  1. Replace all constants here with some meaningfull macros.
   */ 
  page = 0;
  last_pages = pages;
  while (page != last_pages) {
    paddr = phys_addr + page * USR_PAGE_SIZE;
    vaddr = virt_addr + page * USR_PAGE_SIZE;

    /* TODO:
     *     A boundry check to make sure none of indices go beyond
     *     512.
     */
    pml4_idx = (vaddr >> 39) & 0x1FF;
    pdpe_idx = (vaddr >> 30) & 0x1FF;
    pde_idx  = (vaddr >> 21) & 0x1FF;
    pte_idx  = (vaddr >> 12) & 0x1FF;

    pdpe = (page_table_entry_t *)((phys_addr_t)pml4[pml4_idx] & ~0xFFF);
    pde = (page_table_entry_t *)((phys_addr_t)pdpe[pdpe_idx] & ~0xFFF);
    pte = (page_table_entry_t *)((phys_addr_t)pde[pde_idx] & ~0xFFF);

    if (pml4[pml4_idx] == 0) {
      pdpe = (page_table_entry_t *)alloc_mem_pages (1); 
      pde  = (page_table_entry_t *)alloc_mem_pages (1);
      pte  = (page_table_entry_t *)alloc_mem_pages (1);

      memset (pdpe, 0, PAGE_TABLE_SIZE);
      memset (pde,  0, PAGE_TABLE_SIZE);
      memset (pte,  0, PAGE_TABLE_SIZE);

      pml4[pml4_idx] = (phys_addr_t)pdpe | (PAGE_PRESENT | PAGE_RW | PAGE_USR);
      pdpe[pdpe_idx] = (phys_addr_t)pde  | (PAGE_PRESENT | PAGE_RW | PAGE_USR);
      pde[pde_idx]   = (phys_addr_t)pte  | (PAGE_PRESENT | PAGE_RW | PAGE_USR);
      pte[pte_idx]   = paddr | protect;
    } else if (pdpe[pdpe_idx] == 0) {
      pde = (page_table_entry_t *)alloc_mem_pages (1);
      pte = (page_table_entry_t *)alloc_mem_pages (1);

      memset (pde, 0, PAGE_TABLE_SIZE);
      memset (pte, 0, PAGE_TABLE_SIZE);

      pdpe[pdpe_idx] = (phys_addr_t)pde  | (PAGE_PRESENT | PAGE_RW | PAGE_USR);
      pde[pde_idx]   = (phys_addr_t)pte  | (PAGE_PRESENT | PAGE_RW | PAGE_USR);
      pte[pte_idx]   = paddr | protect;
    } else if (pde[pde_idx] == 0) {
      pte = (page_table_entry_t *)alloc_mem_pages (1);

      memset (pte, 0, PAGE_TABLE_SIZE);

      pde[pde_idx] = (phys_addr_t)pte  | (PAGE_PRESENT | PAGE_RW | PAGE_USR);
      pte[pte_idx] = paddr | protect;
    } else {
      pte[pte_idx] = paddr | protect;
    }
/*    cprintk ("v=%x p=%x 1=%x 2=%x 3=%x 4=%x\n", 0x5, vaddr, paddr, pml4[pml4_idx], pdpe[pdpe_idx], pde[pde_idx], pte[pte_idx]);*/
    page++;
  }
}

void
unmap_proc_pages (struct proc *p, virt_addr_t vaddr, size_t pages)
{
  page_table_entry_t *pml4;
  page_table_entry_t *pdpe;
  page_table_entry_t *pde;
  page_table_entry_t *pte;
  int pml4_idx;
  int pdpe_idx;
  int pde_idx;
  int pte_idx;
  int page;
  virt_addr_t current_page_vaddr;

  /* TODO:
   *     Take the right action here. Given address MUST be 4KB aligned
   */
  if (vaddr & 0xFFF) {
    cprintk ("unmap_proc_pages: Unaligned address passed to routine!\n", 0x4);
  }

  pml4 = (page_table_entry_t *)p->p_pml4;

  page = 0;
  current_page_vaddr = vaddr;
  while (page < pages) {
    pml4_idx = (current_page_vaddr >> 39) & 0x1FF;
    pdpe_idx = (current_page_vaddr >> 30) & 0x1FF;
    pde_idx  = (current_page_vaddr >> 21) & 0x1FF;
    pte_idx  = (current_page_vaddr >> 12) & 0x1FF;

    pdpe = (page_table_entry_t *)((phys_addr_t)pml4[pml4_idx] & ~0xFFF);
    pde = (page_table_entry_t *)((phys_addr_t)pdpe[pdpe_idx] & ~0xFFF);
    pte = (page_table_entry_t *)((phys_addr_t)pde[pde_idx] & ~0xFFF);

    pte[pte_idx] = 0;
    
    if (pte_idx == PAGE_TABLE_MAX_IDX) {
      pde[pde_idx] = 0;
    }
    if (pde_idx == PAGE_TABLE_MAX_IDX) {
      pdpe[pdpe_idx] = 0;
    }
    if (pdpe_idx == PAGE_TABLE_MAX_IDX) {
      pml4[pml4_idx] = 0;
    }
    page++;
    current_page_vaddr += USR_PAGE_SIZE;
  }
}

void
free_proc_pages (struct proc *p)
{
  page_table_entry_t *pml4 = (page_table_entry_t *)p->p_pml4;
  page_table_entry_t *pdpe = (page_table_entry_t *)((phys_addr_t)pml4[0] & ~0xFFF);
  int pdpe_idx;
  int pde_idx;

  if (p->p_heap_end <= p->p_vtext) {
    panic ("free_proc_pages: vtext can not be littler than heap_end!\n");
  }

  free_mem_pages (p->p_ptext);
  free_mem_pages (p->p_pdata);
  free_mem_pages (p->p_pbss);
  free_mem_pages (p->p_prodata);
  free_mem_pages (p->p_pstack);
  /* Free page tables regarding kernel */
  free_mem_pages ((phys_addr_t)pml4); 
  /* Free all other remained page tables */
  /*
   * We go through all page tables, and free allocated pages for them. 
   * The reason I'm doing it this way is that we have some stacks (system stack)
   * mapped exactly to their physical addresses. Usually we have a clear pattern
   * for virtual addresses given to different segments of processes.
   * Virtual addresses start from 0x40000000, and go on. Moreover we have kernel
   * address space mapped for all processes. Freeing kernel is very trivial, because
   * we know its size (currently we don't have any dynamic allocation in kernel, so
   * kernel size never changes), therefore, we can allocate its memory pages at once
   * and simplify freeing process. The line before this comment, frees the kernel
   * virtual address space of exiting process.
   * The only thing that disrupt this clean pattern in virtual memory allocation
   * is system stack. We map system stacks exactly to their physical address space.
   * So, we can not predict where they are mapped. Therefore we have to search for
   * page tables allocated to map them through all existing page tables. Therefore
   * we have this O(n^2) algorithm for freeing all remaining page tables.
   */ 
  pdpe_idx = 0;
  for (pdpe_idx = 0; pdpe_idx < PAGE_TABLE_MAX_IDX; pdpe_idx++) {
    page_table_entry_t *pde; 

    if (pdpe[pdpe_idx]) {
      pde = (page_table_entry_t *)((phys_addr_t)pdpe[pdpe_idx] & ~0xFFF);

      for (pde_idx = 0; pde_idx < PAGE_TABLE_MAX_IDX; pde_idx++) {
        if (pde[pde_idx]) {
          phys_addr_t pte = ((phys_addr_t)pde[pde_idx] & ~0xFFF);
          free_mem_pages (pte);
        }
      }
      free_mem_pages ((phys_addr_t)pde);
    }
  }
}

phys_addr_t
virt2phys (virt_addr_t vaddr, struct proc *p)
{
  page_table_entry_t *pml4;
  page_table_entry_t *pdpe;
  page_table_entry_t *pde;
  page_table_entry_t *pte;
  int pml4_idx;
  int pdpe_idx;
  int pde_idx;
  int pte_idx;
  phys_addr_t paddr;

  pml4 = (page_table_entry_t *)p->p_pml4;

  pml4_idx = (vaddr >> 39) & 0x1FF;
  pdpe_idx = (vaddr >> 30) & 0x1FF;
  pde_idx  = (vaddr >> 21) & 0x1FF;
  pte_idx  = (vaddr >> 12) & 0x1FF;

  pdpe = (page_table_entry_t *)((phys_addr_t)pml4[pml4_idx] & ~0xFFF);
  pde = (page_table_entry_t *)((phys_addr_t)pdpe[pdpe_idx] & ~0xFFF);
  pte = (page_table_entry_t *)((phys_addr_t)pde[pde_idx] & ~0xFFF);
  paddr = pte[pte_idx] & ~(0xFFF | PAGE_NX);

  if (paddr) {
    paddr |= (vaddr & 0xFFF);
  }
  return paddr;
}
