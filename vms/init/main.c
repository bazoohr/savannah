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

void
vm_main (void)
{
#if 0
  int i;
  for (i = 0 ; i < cpuinfo->cpuid ; i++) DEBUG ("\n", 0x7);
  DEBUG ("This is init %d\n", 0xA, cpuinfo->cpuid);
  halt ();
#endif
  DEBUG ("init......\n", 0xF);
  int *ptr = (int *)((phys_addr_t)0x100);
  *ptr = 123;

  /* Launching keyboard driver */
  DEBUG  ("Starting keyboard driver... ", 0xF);
  int pid = fork ();
  if (pid == -1) {
    panic  ("init %d: Failed to fork for keyboard driver!\n", __LINE__);
  } else if (pid == 0) {
    exec("keyboard", NULL);
    DEBUG ("FAILED!", 0x4);
    halt ();
  }
  DEBUG  ("DONE!\n", 0xA);
  /* Launching junk driver */
  DEBUG  ("Starting flooding deamon... ", 0xF);
  pid = fork();
  if (pid == -1) {
    panic  ("init %d: Failed to fork for flooding deamon!\n", __LINE__);
  } else if (pid == 0) {
    exec("junk", NULL);
    DEBUG ("FAILED!", 0x4);
    halt ();
  }
  DEBUG  ("DONE!\n", 0xA);
  /* Launching console driver */
  DEBUG  ("Starting console driver... ", 0xF);
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
  DEBUG  ("DONE!\n", 0xA);
  /* Launching login */
  DEBUG  ("Starting login... ", 0xF);
  pid = fork();
  if (pid == -1) {
    panic  ("init %d: Failed to fork for login!\n", __LINE__);
  } else if (pid == 0) {
    exec("login", NULL);
    DEBUG ("FAILED!", 0x4);
    halt ();
  }
  DEBUG  ("DONE!\n", 0xA);

  while (1) {__asm__ __volatile__ ("cli;pause;\n\t");}
}
