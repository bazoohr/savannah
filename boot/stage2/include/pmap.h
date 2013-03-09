#ifndef __PMAP_H__
#define __PMAP_H__

#include <types.h>


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
ept_map_memory (phys_addr_t *pml4_paddr,
                virt_addr_t memory_region_start_vaddr,
                virt_addr_t memory_region_end_vaddr,
                phys_addr_t memory_region_start_paddr,
                uint32_t page_size,
                uint8_t mtype,
                uint16_t protection,
                int  flags);

void ept_map_page_tables (phys_addr_t *ept, phys_addr_t pml4_paddr, const uint16_t protection);
#endif /* __PMAP_H__ */
