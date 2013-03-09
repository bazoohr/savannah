/* ================================== *
 * page.h
 *
 * Hamid R. Bazoobandi
 * 17.07.2012 Amsterdam
 * ================================== */
#ifndef __PAGE_H__
#define __PAGE_H__

#include <const.h>
/* =========================================== */
#if ! defined __ASSEMBLY__

#include <types.h>

typedef uint64_t page_table_entry_t;
#define PAGE_TABLE_ENTRY_SIZE (sizeof (page_table_entry_t))

#else

#define PAGE_TABLE_ENTRY_SIZE (8)

#endif  /* __ASSEMBLY__ */
/* =========================================== */
#define PAGE_TABLE_SIZE       _4KB_
#define PAGE_TABLE_ENTRIES    (PAGE_TABLE_SIZE / PAGE_TABLE_ENTRY_SIZE)
#define PAGE_TABLE_MAX_IDX    (PAGE_TABLE_ENTRIES - 1)

#define PML4_SHIFT  39
#define PDPE_SHIFT  30
#define PDE_SHIFT   21

#define _PAGE_BIT_PRESENT	  0	  /* is present */
#define _PAGE_BIT_RW		    1	  /* writeable */
#define _PAGE_BIT_USER		  2	  /* userspace addressable */
#define _PAGE_BIT_PWT		    3	  /* page write through */
#define _PAGE_BIT_PCD		    4	  /* page cache disabled */
#define _PAGE_BIT_ACCESSED	5	  /* was accessed (raised by CPU) */
#define _PAGE_BIT_DIRTY		  6	  /* was written to (raised by CPU) */
#define _PAGE_BIT_PSE		    7	  /* 4 MB (or 2MB) page */
#define _PAGE_BIT_PAT		    7	  /* on 4KB pages */
#define _PAGE_BIT_GLOBAL	  8	  /* Global TLB entry PPro+ */
#define _PAGE_BIT_UNUSED1	  9	  /* available for programmer */
#define _PAGE_BIT_IOMAP		  10  /* flag used to indicate IO mapping */
#define _PAGE_BIT_HIDDEN	  11  /* hidden by kmemcheck */
#define _PAGE_BIT_PAT_LARGE	12	/* On 2MB or 1GB pages */
#define _PAGE_BIT_NX        63  /* No execute: only valid after cpuid check */

#define PAGE_PRESENT  (1 << _PAGE_BIT_PRESENT)
#define PAGE_PSE      (1 << _PAGE_BIT_PSE)
#define PAGE_RW       (1 << _PAGE_BIT_RW)
#define PAGE_PWT      (1 << _PAGE_BIT_PWT)
#define PAGE_USR      (1 << _PAGE_BIT_USER)
#define PAGE_GLOBAL   (1 << _PAGE_BIT_GLOBAL)
#define PAGE_PCD      (1 << _PAGE_BIT_PCD)
/*
 * Because GCC complains about shifting to last bit of uint64_t, we set NX bit
 * by using an OR operation with this constant, which is exactly the same.
 */
#define PAGE_NX       (0x8000000000000000UL)
/* =========================================== *
 * EPT stuffs
 * =========================================== */
#define _EPT_PAGE_BIT_READ     0
#define _EPT_PAGE_BIT_WRITE    1
#define _EPT_PAGE_BIT_EXEC     2
#define _EPT_PAGE_BIT_MTYPE    3
#define _EPT_PAGE_BIT_PSE      7

#define EPT_PAGE_READ     (1 << _EPT_PAGE_BIT_READ)
#define EPT_PAGE_WRITE    (1 << _EPT_PAGE_BIT_WRITE)
#define EPT_PAGE_EXEC     (1 << _EPT_PAGE_BIT_EXEC)
#define EPT_PAGE_MTYPE(x) ((x) << _EPT_PAGE_BIT_MTYPE)
#define EPT_PAGE_PSE      (1 << _EPT_PAGE_BIT_PSE)

/* EPT memory types */
#define EPT_MTYPE_UC 0  /* Uncachable */
#define EPT_MTYPE_WC 1  /* Write Combining */
#define EPT_MTYPE_WT 4  /* Write Through */
#define EPT_MTYPE_WP 5  /* Write Protected */
#define EPT_MTYPE_WB 6  /* Write Back */
#endif

