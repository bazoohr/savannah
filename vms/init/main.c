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
  int i;

#if 0
  for (i = 0 ; i < cpuinfo->cpuid ; i++) DEBUG ("\n", 0x7);
  DEBUG ("This is init %d\n", 0xA, cpuinfo->cpuid);
  halt ();
#endif

  DEBUG ("init......\n", 0xF);
  /* Launching keyboard driver */
  DEBUG  ("Starting keyboard driver... ", 0xF);
  int pid = fork ();
  if (pid == -1) {
    DEBUG  ("init %d: Failed to fork for keyboard driver!\n", 0x4, __LINE__);
  } else if (pid == 0) {
    for (i = 0 ; i < cpuinfo->cpuid ; i++) DEBUG("\n", 0x7);
    exec("keyboard", NULL);
    DEBUG ("FAILED!", 0x4);
    halt ();
  }
  DEBUG  ("DONE!\n", 0xA);
  /* Launching junk driver */
  DEBUG  ("Starting junk driver... ", 0xF);
  pid = fork();
  if (pid == -1) {
    DEBUG  ("init %d: Failed to fork for junk driver!\n", 0x4, __LINE__);
  } else if (pid == 0) {
    for (i = 0 ; i < cpuinfo->cpuid ; i++) DEBUG("\n", 0x7);
    exec("junk", NULL);
    DEBUG ("FAILED!", 0x4);
    halt ();
  }
  DEBUG  ("DONE!\n", 0xA);
  /* Launching console driver */
  DEBUG  ("Starting console driver... ", 0xF);
  pid = fork();
  if (pid == -1) {
    DEBUG  ("init %d: Failed to fork for console driver!\n", 0x4, __LINE__);
  } else if (pid == 0) {
    for (i = 0 ; i < cpuinfo->cpuid ; i++) DEBUG("\n", 0x7);
    exec("console", NULL);
    DEBUG ("FAILED!", 0x4);
    halt ();
  }
  DEBUG  ("DONE!\n", 0xA);

  DEBUG  ("Starting login... ", 0xF);
  pid = fork();
  if (pid == -1) {
    DEBUG  ("init %d: Failed to fork for login!\n", 0x4, __LINE__);
  } else if (pid == 0) {
    for (i = 0 ; i < cpuinfo->cpuid ; i++) DEBUG("\n", 0x7);
    exec("login", NULL);
    DEBUG ("FAILED!", 0x4);
    halt ();
  }
  DEBUG  ("DONE!\n", 0xA);

  while (1) {__asm__ __volatile__ ("cli;pause;\n\t");}
}
