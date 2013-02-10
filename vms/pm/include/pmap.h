#ifndef __PMAP_H__
#define __PMAP_H__

#include <types.h>
#include <page.h>

#define VMM_PAGE_UNCACHABLE (PAGE_PRESENT | PAGE_RW | PAGE_PSE | PAGE_PCD) /* Page is not cachable */
#define VMM_PAGE_NORMAL (PAGE_PRESENT | PAGE_RW | PAGE_PSE)
#define VM_PAGE_UNCACHABLE (PAGE_PRESENT | PAGE_RW | PAGE_PSE | PAGE_PCD) /* Page is not cachable */
#define VM_PAGE_NORMAL (PAGE_PRESENT | PAGE_RW | PAGE_PSE)

#define MAP_NEW    0x0   /* Create New Page Tables */
#define MAP_UPDATE 0x1   /* Update existing Page Tables */

void map_memory (phys_addr_t *pml4_paddr,
            virt_addr_t memory_region_start_vaddr,
            virt_addr_t memory_region_end_vaddr,
            phys_addr_t memory_region_start_paddr,
            uint32_t page_size,
            uint16_t protection,
            int  flags);
void
EPT_map_memory (phys_addr_t *pml4_paddr,
                virt_addr_t memory_region_start_vaddr,
                virt_addr_t memory_region_end_vaddr,
                phys_addr_t memory_region_start_paddr,
                uint32_t page_size,
                uint16_t protection,
                int  flags);

#endif /* __PMAP_H__ */
