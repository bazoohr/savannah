#include <cdef.h>
#include <types.h>
#include <debug.h>
#include <const.h>
#include <asmfunc.h>
#include <cpu.h>
#include <string.h>
#include <ipc.h>
#include <fs.h>
#include <pm.h>
#include <panic.h>
#include <stdlib.h>
#include <timer.h>
#include <lapic.h>
#include <interrupt.h>
#include <gdt.h>

unsigned int aux;
uint64_t a, b;
#if 0
uint64_t sum = 0;
static volatile bool flag = false;
static void handle_ipi (void)
{
  sum += (rdtscp (&aux) - b);
  lapic_eoi ();
  flag = true;
}
#endif
void
vm_main (void)
{
  int pid;
  create_default_gdt ();
  interrupt_init();
  lapic_on ();
#if 0
  sti ();
  add_irq (34, &handle_ipi);

#define TIMES 100
  static volatile int i;
  for (i = 0; i < TIMES; i++) {
    b = rdtscp (&aux);
    sti ();
    lapic_send_ipi (34, FS);
    sti ();
    while (!flag);
    flag = false;
  }

  DEBUG ("Took %d cycles\n", 0xA, sum / TIMES);
  halt ();
#endif
#if 0
  int i;
  for (i = 0 ; i < cpuinfo->cpuid ; i++) DEBUG ("\n", 0x7);
  DEBUG ("This is init %d\n", 0xA, cpuinfo->cpuid);
  halt ();
  /* Launching keyboard driver */
  DEBUG  ("Starting keyboard driver... ", 0xF);
  b = get_cpu_cycle ();
  pid = fork ();
  if (pid == -1) {
    panic  ("init %d: Failed to fork for keyboard driver!\n", __LINE__);
  } else if (pid == 0) {
    exec("keyboard", NULL);
    DEBUG ("FAILED!", 0x4);
    halt ();
  }
  a = get_cpu_cycle ();
  DEBUG  ("DONE %d!\n", 0xA, a-b);
#endif
  DEBUG  ("Starting E1000 driver... ", 0xF);
  b = rdtscp(&aux);
  pid = fork();
  if (pid == -1) {
    panic  ("init %d: Failed to fork for flooding deamon!\n", __LINE__);
  } else if (pid == 0) {
    exec("e1000", NULL);
    DEBUG ("FAILED!", 0x4);
    halt ();
  }
  a = rdtscp(&aux);
  DEBUG  ("DONE %d!\n", 0xA, a-b);
  halt ();
  /* Launching junk driver */
  DEBUG  ("Starting flooding deamon... ", 0xF);
  b = rdtscp(&aux);
  pid = fork();
  if (pid == -1) {
    panic  ("init %d: Failed to fork for flooding deamon!\n", __LINE__);
  } else if (pid == 0) {
    exec("junk", NULL);
    DEBUG ("FAILED!", 0x4);
    halt ();
  }
  a = rdtscp(&aux);
  DEBUG  ("DONE %d!\n", 0xA, a-b);
  /* Launching console driver */
  DEBUG  ("Starting console driver... ", 0xF);
  b = rdtscp(&aux);
  pid = fork();
  if (pid == -1) {
    panic  ("init %d: Failed to fork for console driver!\n", __LINE__);
  } else if (pid == 0) {
    char x_str[3];
    char y_str[3];
    char *console_args[] = {x_str, y_str, NULL};
    uint32_t x_int;
    uint32_t y_int;
    debug_get_cursor_pos (&x_int, &y_int);
    y_int += 1;  /* We will print one more lines */
    snprintf (x_str, "%d", 3, x_int);
    snprintf (y_str, "%d", 3, y_int);
    exec("console", console_args);
    DEBUG ("FAILED!", 0x4);
    halt ();
  }
  a = rdtscp (&aux);
  DEBUG  ("DONE %d!\n", 0xA, a-b);
  /* Launching login */
  DEBUG  ("Starting login... ", 0xF);
  b = rdtscp (&aux);
  pid = fork();
  if (pid == -1) {
    panic  ("init %d: Failed to fork for login!\n", __LINE__);
  } else if (pid == 0) {
    exec("login", NULL);
    DEBUG ("FAILED!", 0x4);
    halt ();
  }
  a = rdtscp (&aux);
  DEBUG  ("DONE %d!\n", 0xA, a-b);

  while (1) {__asm__ __volatile__ ("cli;pause;\n\t");}
}
