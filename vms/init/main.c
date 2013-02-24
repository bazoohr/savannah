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

int test = 0x123;
void
vm_main (void)
{
  int i;

  for (i = 0 ; i < cpuinfo->cpuid ; i++) DEBUG ("\n", 0x7);

  if (cpuinfo->cpuid == 3) {
    DEBUG  ("THIS IS CHILD!\n", 0x2);
    while (1) {__asm__ __volatile__ ("cli;pause;\n\t");}
  }

  int pid = fork ();
  if (pid == -1) {
    DEBUG  ("Failed to fork!\n", 0x4);
  } else if (pid == 0) {
    for (i = 0 ; i < cpuinfo->cpuid ; i++) DEBUG("\n", 0x7);
    char *string[] = {"Hamid", "Francesco", NULL};
    DEBUG  ("I am the keyboard child pid = %d test = %x cpuid = %d\n", 0xD, pid, test, cpuinfo->cpuid);
    exec("keyboard", string);
  }

  pid = fork();
  if (pid == -1) {
    DEBUG  ("Failed to fork!\n", 0x4);
  } else if (pid == 0) {
    for (i = 0 ; i < cpuinfo->cpuid ; i++) DEBUG("\n", 0x7);
    DEBUG  ("I am the console child pid = %d test = %x cpuid = %d\n", 0xD, pid, test, cpuinfo->cpuid);
    exec("console", NULL);
  }

  for (i = 0 ; i < cpuinfo->cpuid ; i++) DEBUG("\n", 0x7);
  DEBUG  ("parent: child's PID = %d test = %x cpuid = %d\n", 0xD, pid, test, cpuinfo->cpuid);

  pid = fork();
  if (pid == -1) {
    DEBUG  ("Failed to fork!\n", 0x4);
  } else if (pid == 0) {
    for (i = 0 ; i < cpuinfo->cpuid ; i++) DEBUG("\n", 0x7);
    DEBUG  ("I am the login child pid = %d test = %x cpuid = %d\n", 0xD, pid, test, cpuinfo->cpuid);
    exec("login", NULL);
  }

  while (1) {__asm__ __volatile__ ("cli;pause;\n\t");}
}
