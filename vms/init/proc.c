#include <proc.h>
#include <const.h>
#include <page.h>
#include <printk.h>
#include <elf.h>
#include <string.h>
#include <pid.h>
#include <mmap.h>
#include <cdef.h>
#include <memory.h>
#include <resource.h>
#include <panic.h>
#include <lock.h>
#include <global.h>
#include <macro.h>
/* =========================================== */
struct proc *current = NULL;
/* =========================================== */
struct proc * __warn_unused_result
create_empty_proc (void)
{
  pid_t pid = alloc_pid ();
  struct proc *new_proc = alloc_pcb (pid);

  memset (new_proc, 0, sizeof (struct proc));
  new_proc->p_pid = pid;

  return new_proc;
}
/* ========================================================= */
void
create_idle_proc (void)
{
  struct proc *idle;

  idle = create_empty_proc ();

  if (idle->p_pid != IDLE_PID) {
    panic ("invalid idle pid");
  }

  idle->p_pml4  = kernel_pml4;
  idle->p_registers.cr3 = idle->p_pml4;
  idle->p_registers.cs  = SYS_CSEL;
  idle->p_registers.ss  = SYS_DSEL;
  idle->p_registers.ds  = SYS_DSEL;
  idle->p_registers.es  = SYS_DSEL;
  idle->p_registers.fs  = SYS_DSEL;
  idle->p_registers.gs  = SYS_DSEL;

  /* TODO:
   * replace 0 with a meaningful macro 
   */
  idle->p_type = 0;

  idle->p_remained_quantum = 0;
  idle->p_max_quantum = 0;

  idle->p_state = PROC_STATE_READY;

  create_ldt (&idle->p_ldt, KNL_PVL);

  create_ldt_descriptor (&idle->p_ldt, &idle->p_ldt_desc);
  create_tss_descriptor (&idle->p_tss, &idle->p_tss_desc);
}
/* ========================================================= */
/* TODO:
 *   1. converting memory size to page, must be done by a 
 *      small function or macro. This ugly mathmatical computation
 *      is not understandable.
 */
void create_init_proc (phys_addr_t elf_addr)
{
	Elf64_Ehdr *elf_hdr = (Elf64_Ehdr*)elf_addr;  /* Start address of executable */
	Elf64_Phdr *s = (Elf64_Phdr*)((uint8_t*)elf_addr + elf_hdr->e_phoff);
  struct proc* init;
  virt_addr_t last_sect_vaddr = 0;
  size_t last_sect_pages = 0;
  int lock;
  int i;

  if (elf_hdr->e_phnum != 4) {
    panic ("Unexpected init Header!");
  }

  init = create_empty_proc ();

  /* Create page tables, and map kernel space */
  proc_vmem_init (init);

  if (init->p_pid != INIT_PID) {
    panic ("Invalid init pid!");
  }

  /* code section */
  init->p_text_pages = s->p_memsz / USR_PAGE_SIZE + (s->p_memsz % USR_PAGE_SIZE != 0 ? 1: 0);
  if (init->p_text_pages > 0) {
    init->p_ptext = (phys_addr_t)alloc_mem_pages (init->p_text_pages);
    init->p_vtext = s->p_vaddr;
    init->p_registers.rip = s->p_vaddr;
    memcpy ((void*)init->p_ptext, (void*)(elf_addr + s->p_offset), s->p_memsz);
    map_proc_pages (init, init->p_ptext, init->p_vtext, init->p_text_pages, CODE_SEG);
    last_sect_vaddr = init->p_vtext;
    last_sect_pages = init->p_text_pages;
  }
  s++;  /* Goto next section */
  /* data + rodata section */
  init->p_data_pages = s->p_memsz / USR_PAGE_SIZE + (s->p_memsz % USR_PAGE_SIZE != 0 ? 1: 0);
  if (init->p_data_pages > 0) {
    init->p_pdata = (phys_addr_t)alloc_mem_pages (init->p_data_pages);
    init->p_vdata = s->p_vaddr;
    memcpy ((void*)init->p_pdata, (void*)(elf_addr + s->p_offset), s->p_memsz);
    map_proc_pages (init, init->p_pdata, init->p_vdata, init->p_data_pages, DATA_SEG);
    last_sect_vaddr = init->p_vdata;
    last_sect_pages = init->p_data_pages;
  }

  s++;  /* Goto next section */
  /* rodata section */
  init->p_rodata_pages = s->p_memsz / USR_PAGE_SIZE + (s->p_memsz % USR_PAGE_SIZE != 0 ? 1: 0);
  if (init->p_rodata_pages > 0) {
    init->p_prodata = (phys_addr_t)alloc_mem_pages (init->p_rodata_pages);
    init->p_vrodata = s->p_vaddr;
    memcpy ((void*)init->p_prodata, (void*)(elf_addr + s->p_offset), s->p_memsz);
    map_proc_pages (init, init->p_prodata, init->p_vrodata, init->p_rodata_pages, RODATA_SEG);
    last_sect_vaddr = init->p_vrodata;
    last_sect_pages = init->p_rodata_pages;
  }

  s++;  /* Goto next section */
  /* bss section */
  init->p_bss_pages = s->p_memsz / USR_PAGE_SIZE + (s->p_memsz % USR_PAGE_SIZE != 0 ? 1: 0);
  if (init->p_bss_pages > 0) {
    init->p_pbss = (phys_addr_t)alloc_mem_pages (init->p_bss_pages);
    init->p_vbss = s->p_vaddr;
    if (!init->p_pbss) {
      cprintk ("ERROR: failed to allocate memory for bss\n", 0x4);
    }
    memset ((void*)init->p_pbss, 0, s->p_memsz);
    map_proc_pages (init, init->p_pbss, init->p_vbss, init->p_bss_pages, BSS_SEG);
    last_sect_vaddr = init->p_vbss;
    last_sect_pages = init->p_bss_pages;
  }
  virt_addr_t vstack = last_sect_vaddr + last_sect_pages * USR_PAGE_SIZE;

  init->p_pstack = (phys_addr_t)alloc_mem_pages (2 * (USTACK_SIZE / USR_PAGE_SIZE));
  init->p_vstack = vstack;
  if (!init->p_pstack) {
    cprintk ("ERROR: Failed to allocate memory", 0x4);
  }

  map_proc_pages (init, init->p_pstack + USTACK_SIZE, init->p_vstack + USTACK_SIZE, USTACK_SIZE / USR_PAGE_SIZE, STACK_SEG);
  map_proc_pages (init, init->p_pstack, init->p_pstack, USTACK_SIZE / USR_PAGE_SIZE, STACK_SEG);
  init->p_tss.tss_rsp0 = init->p_pstack + USTACK_SIZE;
  init->p_registers.rsp = init->p_vstack + 2 * USTACK_SIZE;

  /* 
   * Currently There is nothing in heap. so base and end
   * are pointing to the same address
   */
  init->p_heap_base = init->p_vstack + 2 * USTACK_SIZE;
  init->p_heap_end  = init->p_heap_base;

  /* TODO:
   * replace 1 << 9 a meaningful macro 
   */
  init->p_registers.rflags = 1 << 9;

  init->p_registers.cs = USR_CSEL;
  init->p_registers.ds = USR_DSEL;
  init->p_registers.es = USR_DSEL;
  init->p_registers.fs = USR_DSEL;
  init->p_registers.gs = USR_DSEL;
  init->p_registers.ss = USR_DSEL;

  init->p_registers.cr3 = init->p_pml4;

  init->p_remained_quantum = PROC_QUANTUM;
  init->p_max_quantum = PROC_QUANTUM;

  init->p_state = PROC_STATE_READY;

  init->p_flags = PROC_FLAGS_NONE;
  /* TODO:
   * replace 0 with a meaningful macro 
   */

  init->p_type = 3;

  for (i = 0; i < RS_MAX; i++) {
    init->p_resources[i] = 0;
  }  

  create_ldt (&init->p_ldt, USR_PVL);

  create_ldt_descriptor (&init->p_ldt, &init->p_ldt_desc);
  create_tss_descriptor (&init->p_tss, &init->p_tss_desc);

  /* FIXME:
   *   We have to initialize the list in a way that immediatly after
   *   kernel primary stuffs, idle is the first process which is scheduled
   */
  lock_region (&lock);

  init->next = init;
  init->pre  = init;

  unlock_region (&lock);
}
