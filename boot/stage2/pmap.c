#include <cdef.h>
#include <page.h>
#include <string.h>
#include <memory.h>
#include <panic.h>
#include <printk.h>
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

  /*
   * In the whole stage2, virtual addresses are equal to
   * physical addresses
   */
  paddr = memory_region_start_paddr;
  vaddr = memory_region_start_vaddr;

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

  if (page_size == _4KB_ && (paddr & (_4KB_ - 1))) {
    panic ("Address %x is not 4KB aligned", paddr);
  }

  if (page_size == _2MB_ && (paddr & (_2MB_ - 1))) {
    panic ("Address %x is not 2MB aligned", paddr);
  }

  if (page_size == _1GB_ && (paddr & (_1GB_ - 1))) {
    panic ("Address %x is not 1GB aligned", paddr);
  }

  if (flags == MAP_NEW) {
    pml4 = (page_table_entry_t *)calloc_align (sizeof (uint8_t), PAGE_TABLE_SIZE, PAGE_TABLE_SIZE);
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

  memory_region_size =  memory_region_end_vaddr - memory_region_start_vaddr;

  pages = memory_region_size / page_size + (memory_region_size % page_size ? 1 : 0);

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

      pml4[pml4_idx] = (phys_addr_t)pdpe | (PAGE_PRESENT | PAGE_RW);
    }

    pdpe = (page_table_entry_t *)((phys_addr_t)pml4[pml4_idx] & ~0xFFF);
    if (pdpe[pdpe_idx] == 0 && page_size < _1GB_) {
      pde = (page_table_entry_t *)calloc_align (sizeof (uint8_t), PAGE_TABLE_SIZE, 0x1000);
      if (pde == NULL) {
        panic ("BOOT STAGE2: Memory Allocation Failed line %d\n", __LINE__);
      }

      pdpe[pdpe_idx] = (phys_addr_t)pde | (PAGE_PRESENT | PAGE_RW);
    }
    if (page_size < _1GB_) {
      pde = (page_table_entry_t *)((phys_addr_t)pdpe[pdpe_idx] & ~0xFFF);

      if (pde[pde_idx] == 0 && page_size < _2MB_) {
        pte = (page_table_entry_t *)calloc_align (sizeof (uint8_t), PAGE_TABLE_SIZE, 0x1000);
        if (pte == NULL) {
          panic ("BOOT STAGE2: Memory Allocation Failed line %d\n", __LINE__);
        }

        pde[pde_idx] = (phys_addr_t)pte | (PAGE_PRESENT | PAGE_RW);
      }
    }
    if (page_size == _4KB_) {
      pte = (page_table_entry_t *)((phys_addr_t)pde[pde_idx] & ~0xFFF);
      pte[pte_idx] = (phys_addr_t)paddr | protection;
    } else if (page_size == _2MB_) {
      pde[pde_idx] = (phys_addr_t)paddr | protection;
    } else {  /* 1GB Pages */
      pdpe[pdpe_idx] = (phys_addr_t)paddr | protection;
    }

    page++;
    paddr += page_size;
    vaddr += page_size;
  }
}
/* =============================================== */
void
ept_map_memory (phys_addr_t *pml4_paddr,
                virt_addr_t memory_region_start_vaddr,
                virt_addr_t memory_region_end_vaddr,
                phys_addr_t memory_region_start_paddr,
                uint32_t page_size,
                uint8_t mtype,
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

  /*
   * In the whole stage2, virtual addresses are equal to
   * physical addresses
   */
  paddr = memory_region_start_paddr;
  vaddr = memory_region_start_vaddr;

  switch (page_size) {
    case _4KB_:
      break;
    case _2MB_:
    case _1GB_:
      protection |= EPT_PAGE_PSE;
      break;
    default:
      panic ("Second Boot Stage: Bad page size!");
  }

  if (page_size == _4KB_ && (paddr & (_4KB_ - 1))) {
    panic ("Address %x is not 4KB aligned", paddr);
  }

  if (page_size == _2MB_ && (paddr & (_2MB_ - 1))) {
    panic ("Address %x is not 2MB aligned", paddr);
  }

  if (page_size == _1GB_ && (paddr & (_1GB_ - 1))) {
    panic ("Address %x is not 1GB aligned", paddr);
  }

  if (flags == MAP_NEW) {
    pml4 = (page_table_entry_t *)calloc_align (sizeof (uint8_t), PAGE_TABLE_SIZE, PAGE_TABLE_SIZE);
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

  memory_region_size =  memory_region_end_vaddr - memory_region_start_vaddr;

  pages = memory_region_size / page_size + (memory_region_size % page_size ? 1 : 0);

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

      pml4[pml4_idx] = (phys_addr_t)pdpe | (EPT_PAGE_READ | EPT_PAGE_WRITE | EPT_PAGE_EXEC);
    }

    pdpe = (page_table_entry_t *)((phys_addr_t)pml4[pml4_idx] & ~0xFFF);
    if (pdpe[pdpe_idx] == 0 && page_size < _1GB_) {
      pde = (page_table_entry_t *)calloc_align (sizeof (uint8_t), PAGE_TABLE_SIZE, 0x1000);
      if (pde == NULL) {
        panic ("BOOT STAGE2: Memory Allocation Failed line %d\n", __LINE__);
      }

      pdpe[pdpe_idx] = (phys_addr_t)pde | (EPT_PAGE_READ | EPT_PAGE_WRITE | EPT_PAGE_EXEC);
    }
    if (page_size < _1GB_) {
      pde = (page_table_entry_t *)((phys_addr_t)pdpe[pdpe_idx] & ~0xFFF);

      if (pde[pde_idx] == 0 && page_size < _2MB_) {
        pte = (page_table_entry_t *)calloc_align (sizeof (uint8_t), PAGE_TABLE_SIZE, 0x1000);
        if (pte == NULL) {
          panic ("BOOT STAGE2: Memory Allocation Failed line %d\n", __LINE__);
        }

        pde[pde_idx] = (phys_addr_t)pte | (EPT_PAGE_READ | EPT_PAGE_WRITE | EPT_PAGE_EXEC);
      }
    }
    if (page_size == _4KB_) {
      pte = (page_table_entry_t *)((phys_addr_t)pde[pde_idx] & ~0xFFF);
      pte[pte_idx] = (phys_addr_t)paddr | (mtype << 3) | protection;
    } else if (page_size == _2MB_) {
      pde[pde_idx] = (phys_addr_t)paddr | (mtype << 3) | protection;
    } else {  /* 1GB Pages */
      pdpe[pdpe_idx] = (phys_addr_t)paddr | (mtype << 3) | protection;
    }

    page++;
    paddr += page_size;
    vaddr += page_size;
  }
}

void
ept_map_page_tables (phys_addr_t *ept, phys_addr_t pml4_paddr, const uint16_t protection)
{
  page_table_entry_t *pml4;
  page_table_entry_t *pdpe;
  page_table_entry_t *pde;
  int pml4_idx;
  int pdpe_idx;
  int pde_idx;

  if (ept == NULL || pml4_paddr == 0) {
    panic ("ept_map_page_tables: page tables are not yet created!");
  }

  pml4 = (page_table_entry_t *)pml4_paddr;
  ept_map_memory (ept,
      (virt_addr_t)pml4,
      (virt_addr_t)pml4 + PAGE_TABLE_SIZE,
      (phys_addr_t)pml4,
      USER_VMS_PAGE_SIZE,
      EPT_MTYPE_WB,
      protection,
      MAP_UPDATE);

  for (pml4_idx = 0; pml4_idx < 1/*PAGE_TABLE_ENTRIES*/; pml4_idx++) {
    pdpe = (page_table_entry_t *)((phys_addr_t)pml4[pml4_idx] & ~0xFFF);

    if (pdpe) {
      ept_map_memory (ept,
          (virt_addr_t)pdpe,
          (virt_addr_t)pdpe + PAGE_TABLE_SIZE,
          (phys_addr_t)pdpe,
          USER_VMS_PAGE_SIZE,
          EPT_MTYPE_WB,
          protection,
          MAP_UPDATE);
      /*
       * TODO:
       *    To be in the safe side, the upper bound should be PAGE_TABLE_ENTRIES,
       *    or it can be dynamically chosen according to the size of memory!
       */
      for (pdpe_idx = 0; pdpe_idx < 4/*PAGE_TABLE_ENTRIES*/; pdpe_idx++) {

        if (pdpe[pdpe_idx] && !(pdpe[pdpe_idx] & PAGE_PSE)) {  /* page < 1GB */
          pde = (page_table_entry_t *)((phys_addr_t)pdpe[pdpe_idx] & ~0xFFF);
          if (pde) {
            ept_map_memory (ept,
                (virt_addr_t)pde,
                (virt_addr_t)pde + PAGE_TABLE_SIZE,
                (phys_addr_t)pde,
                USER_VMS_PAGE_SIZE,
                EPT_MTYPE_WB,
                protection,
                MAP_UPDATE);
            for (pde_idx = 0; pde_idx < PAGE_TABLE_ENTRIES; pde_idx++) {
              if (pde[pde_idx] && !(pde[pde_idx] & PAGE_PSE)) { /* page < 2MB */
                phys_addr_t pte = ((phys_addr_t)pde[pde_idx] & ~0xFFF);

                ept_map_memory (ept,
                    (virt_addr_t)pte,
                    (virt_addr_t)pte + PAGE_TABLE_SIZE,
                    (phys_addr_t)pte,
                    USER_VMS_PAGE_SIZE,
                    EPT_MTYPE_WB,
                    protection,
                    MAP_UPDATE);
              } /* 4KB if */
            } /* pde for */
          } /* pde if */
        } /* 2MB if */
      } /* pdpe for */
    } /* pdpe if */
  } /* Pml4 for */
}
