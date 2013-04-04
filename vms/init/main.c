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

struct fake {
  char none[MSG_DATA_SIZE];
} __packed;
struct fake tmp;
void
vm_main (void)
{
  uint64_t b, a;
  int pid;
  unsigned int aux = cpuinfo->cpuid;
#if 0
  if (sizeof (struct fake) != MSG_DATA_SIZE) {
    panic ("struct fake is not a cache line size!");
  }
  volatile uint64_t *s = cpuinfo->msg_ready_bitmap;
  uint64_t sum = 0;
  volatile uint64_t i;
  for (i = 0; i < 999999; i++);
#define MAX 10000
  for (i = 0; i < MAX; i++) {
    b = rdtscp (&aux);
    msg_send(PM, READ_IPC, &tmp, sizeof(struct fake));
    msg_receive(PM);
    a = rdtscp (&aux);
    sum += (a - b);
  }
  DEBUG ("written %x\n", 0xE, *s);
  DEBUG ("cpu %d sum %d Took %d cycles\n", 0xC, cpuinfo->cpuid, sum, sum / MAX);
  halt ();
#if 0
  int i;
  for (i = 0 ; i < cpuinfo->cpuid ; i++) DEBUG ("\n", 0x7);
  DEBUG ("This is init %d\n", 0xA, cpuinfo->cpuid);
  halt ();
  /* Launching keyboard driver */
  DEBUG  ("Starting keyboard driver... ", 0xF);
  b = rdtscp (&aux);
  pid = fork ();
  if (pid == -1) {
    panic  ("init %d: Failed to fork for keyboard driver!\n", __LINE__);
  } else if (pid == 0) {
    exec("keyboard", NULL);
    DEBUG ("FAILED!", 0x4);
    halt ();
  }
  a = rdtscp (&aux);
  DEBUG  ("DONE %d!\n", 0xA, a-b);
  /* Launching junk driver */
  uint64_t *s = cpuinfo->msg_ready_bitmap;
  uint64_t *rax;
  volatile uint64_t *monitor_area;
  uint64_t rdx, rcx;
  uint64_t sum = 0;
  monitor_area = rax = cpuinfo->msg_ready_bitmap;
  volatile uint64_t i;
  a = b = 0;
  for (i = 0; i < 999999; i++);
#define MAX 10000
  for (i = 0; i < MAX; i++) {
    rdx = rcx = 0;
    b = rdtsc ();
    *s = 0x123;
    while (*monitor_area == 0x123) {
      monitor (rax, rcx, rdx);
      if (*monitor_area == 0x123) {
        mwait ((uint64_t)rax, rcx);
      }
    }
    a = rdtsc ();
    sum += (a - b);
  }
  DEBUG ("written %x\n", 0xE, *monitor_area);
  DEBUG ("Took %d cycles\n", 0xE, a - b);
#endif

  //unsigned int aux = cpuinfo->cpuid;
  aux = cpuinfo->cpuid;
  s = cpuinfo->msg_ready_bitmap;
  //uint64_t *rax;
  //volatile uint64_t *monitor_area;
  //uint64_t rdx, rcx;
  //uint64_t sum = 0;
  //monitor_area = rax = cpuinfo->msg_ready_bitmap;
  //volatile uint64_t i;
  uint64_t cr0;
  cr0 = rcr0 ();
  DEBUG ("cr0 = %x id = %d\n\n", 0xA, cr0, cpuinfo->cpuid);
  a = b = 0;
#define MAX 10000
  for (i = 0; i < 999999; i++);
#if 0
  for (i = 0; i < MAX; i++) {
    rdx = rcx = 0;
    b = rdtscp (&aux);
    *s = 0x123;
    while (*monitor_area == 0x123) {
      monitor (rax, rcx, rdx);
      if (*monitor_area == 0x123) {
        mwait ((uint64_t)rax, rcx);
      }
    }
    a = rdtscp (&aux);
    sum += (a - b);
  }
#endif
  for (i = 0; i < MAX; i++) {
    b = rdtscp (&aux);
    *s = 0x123;
    while (*s== 0x123);
    a = rdtscp (&aux);
    sum += (a - b);
  }
  DEBUG ("written %x\n", 0xE, *s);
  DEBUG ("cpu %d sum %d Took %d cycles\n", 0xC, cpuinfo->cpuid, sum, sum / MAX);

  static volatile int hamid = 0;
  static volatile int francesco = 0;
  sum = 0;
  for (i = 0; i < MAX; i++) {
    b = rdtscp (&aux);
    hamid++;
    francesco = hamid;
    a = rdtscp (&aux);
    sum += (a - b);
  }
  DEBUG ("written %d\n", 0xE, francesco);
  DEBUG ("cpu %d sum %d Took %d cycles\n", 0xC, cpuinfo->cpuid, sum, sum / MAX);
  halt ();
#endif
  /* =============================================== */
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
