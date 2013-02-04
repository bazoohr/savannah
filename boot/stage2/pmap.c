#include <cdef.h>
#include <page.h>
#include <string.h>
#include <memory.h>
#include <panic.h>
#include <printk.h>
#include <global.h>
#include <macro.h>
#include <pmap.h>
/* ========================================== */
void
map_memory (phys_addr_t *pml4_paddr,
            virt_addr_t memory_region_start_vaddr,
            virt_addr_t memory_region_end_vaddr,
            phys_addr_t memory_region_start_paddr,
            uint32_t page_size,
            uint16_t protection,
            int  flags)
{
  phys_addr_t paddr;
  virt_addr_t vaddr;
  page_table_entry_t *pml4 = NULL;
  page_table_entry_t *pdpe = NULL;
  page_table_entry_t *pde  = NULL;
  page_table_entry_t *pte  = NULL;
  int pml4_idx;
  int pdpe_idx;
  int pde_idx;
  int pte_idx;
  int page;
  int last_pages;
  size_t pages;
  size_t memory_region_size;

  if (memory_region_start_vaddr > memory_region_end_vaddr) {
    panic ("mep_memory: memory start region can not be greater than end address");
  }

  if (memory_region_start_vaddr == memory_region_end_vaddr) {
    panic ("Can't map a memory region of zero size!");
  }

  if (flags == MAP_NEW) {
    pml4 = (page_table_entry_t *)calloc_align (sizeof (uint8_t), PAGE_TABLE_SIZE, _2MB_); /* Normally _4KB_ */
    if (pml4 == NULL) {
      panic ("BOOT STAGE2: Memory Allocation Failed line %d\n", __LINE__);
    }

    *pml4_paddr = (phys_addr_t)pml4;

  } else if (flags == MAP_UPDATE) {
    pml4 = (page_table_entry_t *)*pml4_paddr;

    if ((pml4[0] & (PAGE_PRESENT | PAGE_RW)) == 0) {
      cprintk ("Warning:\nSecond Boot Stage: Invalid PML4 used to map VMM Pages!", 0x4);
    }
    if (pml4[0] == 0) {
      panic ("Second Boot Stage: Trying to update null page tables!");
    }
  } else {
    panic ("Second Boot Stage: Undefined memory map flag!");
  }

  if (protection != VMM_PAGE_UNCACHABLE && protection != VMM_PAGE_NORMAL) {
    panic ("Second Boot Stage: Undefined VMM page protection!");
  }

  switch (page_size) {
    case _4KB_:
      break;
    case _2MB_:
    case _1GB_:
      protection |= PAGE_PSE;
      break;
    default:
      panic ("Second Boot Stage: Bad page size!");
  }

  memory_region_size =  memory_region_end_vaddr - memory_region_start_vaddr;

  pages = memory_region_size / page_size + (memory_region_size % page_size ? 1 : 0);

  paddr = memory_region_start_paddr;
  /*
   * In the whole stage2, virtual addresses are equal to
   * physical addresses
   */
  vaddr = memory_region_start_vaddr;
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
    pte_idx  = (vaddr >> 12) & 0x1FF;


    if (pml4[pml4_idx] == 0) {
      pdpe = (page_table_entry_t *)calloc_align (sizeof (uint8_t), PAGE_TABLE_SIZE, 0x1000);
      if (pdpe == NULL) {
        panic ("BOOT STAGE2: Memory Allocation Failed line %d\n", __LINE__);
      }

      pml4[pml4_idx] = (phys_addr_t)pdpe | 0x7;//(PAGE_PRESENT | PAGE_RW);
    }

    pdpe = (page_table_entry_t *)((phys_addr_t)pml4[pml4_idx] & ~0xFFF);
    if (pdpe[pdpe_idx] == 0 && page_size < _1GB_) {
      pde = (page_table_entry_t *)calloc_align (sizeof (uint8_t), PAGE_TABLE_SIZE, 0x1000);
      if (pde == NULL) {
        panic ("BOOT STAGE2: Memory Allocation Failed line %d\n", __LINE__);
      }

      pdpe[pdpe_idx] = (phys_addr_t)pde | 0x7;//| (PAGE_PRESENT | PAGE_RW);
    }
    if (page_size < _1GB_) {
      pde = (page_table_entry_t *)((phys_addr_t)pdpe[pdpe_idx] & ~0xFFF);

      if (pde[pde_idx] == 0 && page_size < _2MB_) {
        pte = (page_table_entry_t *)calloc_align (sizeof (uint8_t), PAGE_TABLE_SIZE, 0x1000);
        if (pte == NULL) {
          panic ("BOOT STAGE2: Memory Allocation Failed line %d\n", __LINE__);
        }

        pde[pde_idx] = (phys_addr_t)pte | 0x7;//(PAGE_PRESENT | PAGE_RW);
      }
    }
    if (page_size == _4KB_) {
      pte = (page_table_entry_t *)((phys_addr_t)pde[pde_idx] & ~0xFFF);
      pte[pte_idx] = (phys_addr_t)paddr | protection;
    } else if (page_size == _2MB_) {
      if (paddr & (_2MB_ - 1)) {
        panic ("Address %x is not 2MB aligned", paddr);
      }
      pde[pde_idx] = (phys_addr_t)paddr | protection;
    } else {  /* 1GB Pages */
      if (paddr & (_1GB_ - 1)) {
        panic ("Address %x is not 1GB aligned", paddr);
      }
      pdpe[pdpe_idx] = (phys_addr_t)paddr | 0x7 | (1 << 7);//protection;
    }

    page++;
    paddr += page_size;
    vaddr += page_size;
  }
}
/* =============================================== */
void
EPT_map_memory (phys_addr_t *pml4_paddr,
                virt_addr_t memory_region_start_vaddr,
                virt_addr_t memory_region_end_vaddr,
                phys_addr_t memory_region_start_paddr,
                uint32_t page_size,
                uint16_t protection,
                int  flags)
{
  phys_addr_t paddr;
  virt_addr_t vaddr;
  page_table_entry_t *pml4 = NULL;
  page_table_entry_t *pdpe = NULL;
  page_table_entry_t *pde  = NULL;
  page_table_entry_t *pte  = NULL;
  int pml4_idx;
  int pdpe_idx;
  int pde_idx;
  int pte_idx;
  int page;
  int last_pages;
  size_t pages;
  size_t memory_region_size;

  if (memory_region_start_vaddr > memory_region_end_vaddr) {
    panic ("mep_memory: memory start region can not be greater than end address");
  }

  if (memory_region_start_vaddr == memory_region_end_vaddr) {
    panic ("Can't map a memory region of zero size!");
  }

  if (flags == MAP_NEW) {
    pml4 = (page_table_entry_t *)calloc_align (sizeof (uint8_t), PAGE_TABLE_SIZE, _2MB_); /* Normally _4KB_ */
    if (pml4 == NULL) {
      panic ("BOOT STAGE2: Memory Allocation Failed line %d\n", __LINE__);
    }

    *pml4_paddr = (phys_addr_t)pml4;

  } else if (flags == MAP_UPDATE) {
    pml4 = (page_table_entry_t *)*pml4_paddr;

    if ((pml4[0] & (PAGE_PRESENT | PAGE_RW)) == 0) {
      cprintk ("Warning:\nSecond Boot Stage: Invalid PML4 used to map VMM Pages!", 0x4);
    }
    if (pml4[0] == 0) {
      panic ("Second Boot Stage: Trying to update null page tables!");
    }
  } else {
    panic ("Second Boot Stage: Undefined memory map flag!");
  }

  if (protection != VMM_PAGE_UNCACHABLE && protection != VMM_PAGE_NORMAL) {
    panic ("Second Boot Stage: Undefined VMM page protection!");
  }

  switch (page_size) {
    case _4KB_:
      break;
    case _2MB_:
    case _1GB_:
      protection |= PAGE_PSE;
      break;
    default:
      panic ("Second Boot Stage: Bad page size!");
  }

  memory_region_size =  memory_region_end_vaddr - memory_region_start_vaddr;

  pages = memory_region_size / page_size + (memory_region_size % page_size ? 1 : 0);

  paddr = memory_region_start_paddr;
  /*
   * In the whole stage2, virtual addresses are equal to
   * physical addresses
   */
  vaddr = memory_region_start_vaddr;
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
    pte_idx  = (vaddr >> 12) & 0x1FF;


    if (pml4[pml4_idx] == 0) {
      pdpe = (page_table_entry_t *)calloc_align (sizeof (uint8_t), PAGE_TABLE_SIZE, 0x1000);
      if (pdpe == NULL) {
        panic ("BOOT STAGE2: Memory Allocation Failed line %d\n", __LINE__);
      }

      pml4[pml4_idx] = (phys_addr_t)pdpe | 0x7; /* READ *//*| (PAGE_PRESENT | PAGE_RW)*/;
    }

    pdpe = (page_table_entry_t *)((phys_addr_t)pml4[pml4_idx] & ~0xFFF);
    if (pdpe[pdpe_idx] == 0 && page_size < _1GB_) {
      pde = (page_table_entry_t *)calloc_align (sizeof (uint8_t), PAGE_TABLE_SIZE, 0x1000);
      if (pde == NULL) {
        panic ("BOOT STAGE2: Memory Allocation Failed line %d\n", __LINE__);
      }

      pdpe[pdpe_idx] = (phys_addr_t)pde | 0x7;/*(PAGE_PRESENT | PAGE_RW)*/;
    }
    if (page_size < _1GB_) {
      pde = (page_table_entry_t *)((phys_addr_t)pdpe[pdpe_idx] & ~0xFFF);

      if (pde[pde_idx] == 0 && page_size < _2MB_) {
        pte = (page_table_entry_t *)calloc_align (sizeof (uint8_t), PAGE_TABLE_SIZE, 0x1000);
        if (pte == NULL) {
          panic ("BOOT STAGE2: Memory Allocation Failed line %d\n", __LINE__);
        }

        pde[pde_idx] = (phys_addr_t)pte | 0x7;/*(PAGE_PRESENT | PAGE_RW)*/;
      }
    }
    if (page_size == _4KB_) {
      pte = (page_table_entry_t *)((phys_addr_t)pde[pde_idx] & ~0xFFF);
      pte[pte_idx] = (phys_addr_t)paddr | 0x7;
    } else if (page_size == _2MB_) {
      if (paddr & (_2MB_ - 1)) {
        panic ("EPT MAP: Address %x is not 2MB aligned", paddr);
      }
      pde[pde_idx] = (phys_addr_t)paddr | 0x7 | (1 << 7);
    } else {  /* 1GB Pages */
      if (paddr & (_1GB_ - 1)) {
        panic ("EPT MAP: Address %x is not 1GB aligned", paddr);
      }
      pdpe[pdpe_idx] = (phys_addr_t)paddr | 0x7 | (1 << 7);/*| protection*/;
    }

    page++;
    paddr += page_size;
    vaddr += page_size;
  }
}

