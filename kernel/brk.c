#include <memory.h>
#include <mmap.h>
#include <page.h>
#include <proc.h>
#include <printk.h>
#include <panic.h>
#include <lock.h>
#include <global.h>
#include <macro.h>

/* ======================================== */
void * __warn_unused_result
do_sbrk (size_t size)
{
  virt_addr_t heap_end_addr;
  struct proc *main_proc;
  int requested_bytes;
  int available_bytes;
  int needed_heap_pages;
  int needed_bytes; 
  int lock;
  int i;

  /*
   * We have to lock this routine against race condition between a process and
   * its threads. Because the process itself and all its threads can extend the heap,
   * and we have to make sure that at any time, only one of them is in this routine.
   */
  lock_region (&lock);
  /*
   * Clone system call, does not assign any value to thread's heap_end feild. Because
   * thread shares its address space with its parent. Therefore to get the end of heap,
   * we have look at its parent's heap information.
   */
  main_proc = current->p_flags != PROC_FLAGS_ISTHREAD ? current : get_pcb (current->p_ppid);
  heap_end_addr = main_proc->p_heap_end;

  if (size == 0) {
    unlock_region (&lock);
    return (void *)heap_end_addr;
  }

  requested_bytes = size;
  available_bytes = ALIGN (heap_end_addr, USR_PAGE_SIZE) - heap_end_addr;

  needed_bytes = requested_bytes < available_bytes ?
                 available_bytes - requested_bytes : 
                 requested_bytes;

  if (requested_bytes > available_bytes) {
    needed_heap_pages = needed_bytes / USR_PAGE_SIZE + (needed_bytes % USR_PAGE_SIZE ? 1 : 0);
  } else {
    needed_heap_pages = 0;
  }

#if 0
  cprintk ("available pages = %d requested_bytes = %d, needed bytes %d needed pages %d\n", 0xB, available_bytes, requested_bytes, needed_bytes, needed_heap_pages);
#endif
  if (needed_heap_pages > 0) {
    phys_addr_t heap_last_addr = ALIGN (heap_end_addr, USR_PAGE_SIZE);

    for (i = 0; i < needed_heap_pages; i++) {
      phys_addr_t new_heap_page;
      new_heap_page = alloc_mem_pages (1);
      if (!new_heap_page) {
        int j;
     /*   cprintk ("do_sbrk: failed to allocate page!\n", 0x4);*/
        /* Free all alocated pages */
        for (j = 0; j < needed_heap_pages - i; j++) {
          phys_addr_t page_paddr;
          virt_addr_t page_vaddr;

          page_vaddr = heap_end_addr - j * USR_PAGE_SIZE;

          page_paddr = virt2phys (page_vaddr, main_proc);
          if (!page_paddr) {
            panic ("do_sbrk: virt2phys failed!");
          }
          /*
           * TODO: These pages should be unmapped as well.
           */
          free_mem_pages (page_paddr);
        }
        unlock_region (&lock);
        return (void *)-1;
      }

      map_proc_pages (main_proc, new_heap_page, heap_last_addr, 1, DATA_SEG);
      heap_last_addr += USR_PAGE_SIZE;
    }
    heap_end_addr += requested_bytes;
/*    cprintk ("heap_end_addr = %x last_addr %x\n", 0xD, heap_end_addr, heap_last_addr);*/
  } else {
    heap_end_addr += requested_bytes;
/*    cprintk ("heap_end_addr = %x\n", 0x8, heap_end_addr);*/
  }

  main_proc->p_heap_end = heap_end_addr;

  unlock_region (&lock);
  return (void *)(heap_end_addr - requested_bytes);
}

int __warn_unused_result
do_brk (virt_addr_t addr)
{
  virt_addr_t heap_start_vaddr;
  virt_addr_t heap_end_vaddr;
  virt_addr_t current_heap_page_vaddr;
  struct proc *main_proc;
  size_t heap_length;
  int heap_pages;
  int lock;
  int i;

  lock_region (&lock);
  /*
   * Clone system call, does not assign any value to thread's heap_end feild. Because
   * thread shares its address space with its parent. Therefore to get the end of heap,
   * we have look at its parent's heap information.
   */
  main_proc = !(current->p_flags & PROC_FLAGS_ISTHREAD) ? current : get_pcb (current->p_ppid);
  heap_end_vaddr = main_proc->p_heap_end;

  heap_start_vaddr = main_proc->p_heap_base;
  heap_end_vaddr   = main_proc->p_heap_end;

  if (addr < heap_start_vaddr) {
    unlock_region (&lock);
    return -1;
  }

  if (addr != heap_start_vaddr) {
    cprintk ("do_brk Warning: given address is not the heap's begining!\n", 0x4);
  }

  if (addr > heap_end_vaddr) {
    if (do_sbrk (addr - heap_end_vaddr) == (void *)-1) {
      cprintk ("brk: failed to extend heap!\n", 0x4);
      unlock_region (&lock);
      return -1;
    }
  }

  heap_length = heap_end_vaddr - heap_start_vaddr;

  heap_pages = heap_length / USR_PAGE_SIZE + (addr % USR_PAGE_SIZE ? 0 : 1);
  current_heap_page_vaddr = heap_start_vaddr;
  for (i = 0; i < heap_pages; i++) {
    phys_addr_t page_paddr;

    page_paddr = virt2phys (current_heap_page_vaddr, main_proc);
    if (!page_paddr) {
      cprintk ("do_brk: Failed to translate heap virtual page address %x!\n", 0x4, current_heap_page_vaddr);
      unlock_region (&lock);
      return -1;
    }
    free_mem_pages (page_paddr);
    unmap_proc_pages (main_proc, current_heap_page_vaddr, 1);
    current_heap_page_vaddr += USR_PAGE_SIZE;
  }

  main_proc->p_heap_end = addr;
  
  unlock_region (&lock);
  return 0;
}

