#ifndef __PROC_H__
#define __PROC_H__

#ifndef __ASSEMBLY__

#include <types.h>
#include <segment.h>
#include <cpu.h>
#include <const.h>
#include <pid.h>
#include <resource.h>

struct proc{
	struct tss p_tss;
	struct regs p_registers;
  struct ldt p_ldt;
  struct system_descriptor p_tss_desc;
  struct system_descriptor p_ldt_desc;
  /* Physical addresses of process segments */
	phys_addr_t p_ptext;
	phys_addr_t p_pdata;
  phys_addr_t p_prodata;
  phys_addr_t p_pbss;
	phys_addr_t p_pstack;
  /* Virtual addresses of process segments */
  virt_addr_t p_vtext;
  virt_addr_t p_vdata;
  virt_addr_t p_vrodata;
  virt_addr_t p_vbss;
  virt_addr_t p_vstack;

  virt_addr_t p_heap_base;
  virt_addr_t p_heap_end;

  phys_addr_t p_pml4;

  int p_remained_quantum;
  int p_max_quantum;

  size_t p_text_pages;      /* Number of Code segment pages */
  size_t p_data_pages;      /* Number of data segment pages */
  size_t p_rodata_pages;    /* Number of rodata segment pages */
  size_t p_bss_pages;       /* Number of bss segment pages */
	pid_t p_ppid;     /* process's parent PID */
	pid_t p_pid;      /* Process's PID */
#define PROC_FLAGS_NONE     0x0
#define PROC_FLAGS_WCHLDPID 0x1
#define PROC_FLAGS_WCHLDANY 0x2
#define PROC_FLAGS_ISTHREAD 0x3
	uint8_t p_flags;
#define PROC_STATE_BLOCKED 1
#define PROC_STATE_READY   2
#define PROC_STATE_ZOMBIE  3
	int p_state;
  int p_exit_status;
	int p_type;
  int p_resources[RS_MAX];
	struct proc *next;
	struct proc *pre;
};

struct proc *create_empty_proc (void);
void create_init_proc (phys_addr_t elf_addr);
void create_idle_proc (void);
#endif

#define INIT_PID 1       /* PROCESS ID of init */
#define IDLE_PID 0       /* Process ID of idle */

#define USTACK_SIZE 0x200000     /* Size of user process stack */

#define CODE_SEG     1
#define DATA_SEG     2
#define RODATA_SEG   3
#define BSS_SEG      4
#define STACK_SEG    5    

#define PROC_QUANTUM 10

#ifdef __ASSEMBLY__
#include <asm_proc_const.h>
#endif

#endif
