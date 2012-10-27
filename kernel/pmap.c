#include <cdef.h>
#include <page.h>
#include <string.h>
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
