/* ============================================ *
 * fork.c                                       *
 *                                              *
 * Hamid R. Bazoobandi                          *
 * August 5 2012 Amsterdam                      *
 * ============================================ */
#include <proc.h>
#include <mmap.h>
#include <frame.h>
#include <trap.h>
#include <string.h>
#include <printk.h>
#include <asmfunc.h>
#include <page.h>
#include <memory.h>
#include <lock.h>
#include <global.h>

static void
copy_registers (struct proc *child, struct trapframe *tf)
{
  /*
   *   We are NOT going to copy RAX here. Because after fork,
   *   RAX has two different values for parent, and child.
   *   We are returning PID to parrent, and 0 to child.
   */
  child->p_registers.rbx = tf->tf_rbx;
  child->p_registers.rcx = tf->tf_rcx;
  child->p_registers.rdx = tf->tf_rdx;
  child->p_registers.rdi = tf->tf_rdi;
  child->p_registers.rsi = tf->tf_rsi;
  child->p_registers.rbp = tf->tf_rbp;
  child->p_registers.rsp = tf->tf_rsp;
  child->p_registers.r8  = tf->tf_r8;
  child->p_registers.r9  = tf->tf_r9;
  child->p_registers.r10 = tf->tf_r10;
  child->p_registers.r11 = tf->tf_r11;
  child->p_registers.r12 = tf->tf_r12;
  child->p_registers.r13 = tf->tf_r13;
  child->p_registers.r14 = tf->tf_r14;
  child->p_registers.r15 = tf->tf_r15;

  child->p_registers.rip    = tf->tf_rip;
  child->p_registers.rsp    = tf->tf_rsp;
  child->p_registers.ss     = tf->tf_ss;
  child->p_registers.cs     = tf->tf_cs;
  child->p_registers.rflags = tf->tf_rflags;
}
void
do_fork (struct trapframe *tf)
{
  struct proc *child;
  struct proc *parent;
  int lock;
  int i;

  /*
   * XXX:
   *    I put this here just to remind that a thread
   *    also can fork a process. Then stack would be a
   *    serious problem.
   *    Currently if thread forks a process. then everything
   *    we are in an unknown situation.
   */
  if (current->p_flags & PROC_FLAGS_ISTHREAD) {
    parent = get_pcb (current->p_pid);
  } else {
    parent = current;
  }
  child = create_empty_proc ();
  /* Create page tables, and map kernel space */
  proc_vmem_init (child);
  /* code section */
  if (parent->p_text_pages > 0) {
    child->p_ptext = (phys_addr_t)alloc_mem_pages (parent->p_text_pages);
    if (!child->p_ptext) {
      cprintk ("failed to allocated memory code!\n", 0x4);
    }
    child->p_text_pages = parent->p_text_pages;
    child->p_vtext = parent->p_vtext;
    memcpy ((void*)child->p_ptext, (void*)parent->p_ptext, parent->p_text_pages * USR_PAGE_SIZE);
    map_proc_pages (child, child->p_ptext, child->p_vtext, child->p_text_pages, CODE_SEG);
  }
  /* data section */
  if (parent->p_data_pages > 0) {
    child->p_pdata = (phys_addr_t)alloc_mem_pages (parent->p_data_pages);
    if (!child->p_pdata) {
      cprintk ("failed to allocated memory! data\n", 0x4);
    }
    child->p_data_pages = parent->p_data_pages;
    child->p_vdata = parent->p_vdata;
    memcpy ((void*)child->p_pdata, (void*)parent->p_pdata, parent->p_data_pages * USR_PAGE_SIZE);
    map_proc_pages (child, child->p_pdata, child->p_vdata, child->p_data_pages, DATA_SEG);
  }

  /* rodata section */
  if (parent->p_rodata_pages > 0) {
    child->p_prodata = (phys_addr_t)alloc_mem_pages (parent->p_rodata_pages);
    child->p_rodata_pages = parent->p_rodata_pages;
    child->p_vrodata = parent->p_vrodata;
    memcpy ((void*)child->p_prodata, (void*)parent->p_prodata, parent->p_rodata_pages * USR_PAGE_SIZE);
    map_proc_pages (child, child->p_prodata, child->p_vrodata, child->p_rodata_pages, RODATA_SEG);
  }

  /* bss section */
  if (parent->p_bss_pages > 0) {
    child->p_pbss = (phys_addr_t)alloc_mem_pages (parent->p_bss_pages);
    if (!child->p_pbss) {
      cprintk ("ERROR: failed to allocate memory for bss\n", 0x4);
    }
    child->p_bss_pages = parent->p_bss_pages;
    child->p_vbss = parent->p_vbss;
    memcpy ((void*)child->p_pbss, (void*)parent->p_pbss, parent->p_bss_pages * USR_PAGE_SIZE);
    map_proc_pages (child, child->p_pbss, child->p_vbss, child->p_bss_pages, BSS_SEG);
  }

  child->p_pstack = (phys_addr_t)alloc_mem_pages (2 * (USTACK_SIZE / USR_PAGE_SIZE));
  if (!child->p_pstack) {
    cprintk ("ERROR: Failed to allocate memory for stack", 0x4);
  }

  child->p_vstack = parent->p_vstack;

  memcpy ((void*)child->p_pstack, (void*)parent->p_pstack, 2 * USTACK_SIZE);

  map_proc_pages (child, child->p_pstack + USTACK_SIZE, child->p_vstack + USTACK_SIZE, USTACK_SIZE / USR_PAGE_SIZE, STACK_SEG);
  map_proc_pages (child, child->p_pstack, child->p_pstack, USTACK_SIZE / USR_PAGE_SIZE, STACK_SEG);

  child->p_tss.tss_rsp0 = child->p_pstack + USTACK_SIZE;
  child->p_registers.rsp = child->p_vstack + 2 * USTACK_SIZE;

  child->p_heap_base = parent->p_heap_base;
  child->p_heap_end  = parent->p_heap_end;

  size_t heap_size = parent->p_heap_end - parent->p_heap_base;
  int heap_pages   = heap_size / USR_PAGE_SIZE + ((heap_size % USR_PAGE_SIZE) ? 1 : 0);

  for (i = 0; i < heap_pages; i++) {
    virt_addr_t current_heap_page_vaddr;
    phys_addr_t parent_heap_page;
    phys_addr_t child_heap_page;
    
    current_heap_page_vaddr = parent->p_heap_base + i * USR_PAGE_SIZE;

    parent_heap_page = virt2phys (current_heap_page_vaddr, parent);
    if (!parent_heap_page) {
      cprintk ("fork: virt2phys failed! bad heap virtual address!\n", 0x4);
    }
    
    child_heap_page = alloc_mem_pages (1);
    if (!child_heap_page) {
      cprintk ("fork: Failed to allocate memory for heap!\n", 0x4);
    }
    map_proc_pages (child, child_heap_page, current_heap_page_vaddr, 1, DATA_SEG);

    memcpy ((void *)child_heap_page, (void *)parent_heap_page, USR_PAGE_SIZE);
  }


  child->p_registers.cs = USR_CSEL;
  child->p_registers.ds = USR_DSEL;
  child->p_registers.es = USR_DSEL;
  child->p_registers.fs = USR_DSEL;
  child->p_registers.gs = USR_DSEL;
  child->p_registers.ss = USR_DSEL;

  child->p_registers.cr3 = child->p_pml4;

  child->p_remained_quantum = PROC_QUANTUM;
  child->p_max_quantum = PROC_QUANTUM;

  child->p_state = PROC_STATE_READY;

  child->p_flags = PROC_FLAGS_NONE;

  copy_registers (child, tf);

  child->p_ppid = parent->p_pid;

  child->p_registers.rax = 0;   /* return value to child */
  tf->tf_rax = child->p_pid;    /* return value to parent */

  child->p_type = 3;

  for (i = 0; i < RS_MAX; i++) {
    child->p_resources[i] = 0;
  }

  create_ldt (&child->p_ldt, USR_PVL);

  create_ldt_descriptor (&child->p_ldt, &child->p_ldt_desc);
  create_tss_descriptor (&child->p_tss, &child->p_tss_desc);

  lock_region (&lock);

  child->next = current->next;
  current->next = child;
  child->pre = current;

  unlock_region (&lock);
}
