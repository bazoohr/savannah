#ifndef __ASM_H__
#define __ASM_H__

#include <types.h>

bool has_long_mode (void);
bool has_x87_and_media_support (void);
void enable_x87_and_media (void);
void load_cr3 (phys_addr_t pml4_base_addr);
void halt (void);
bool has_1GBpage (void);

#endif
