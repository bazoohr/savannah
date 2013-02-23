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
//  DEBUG  ("INIT: My info is in addr = %d\n", 0xD, cpuinfo->cpuid);

  char content[10];
  memset(content, 0, 10);

  int fd = open("2.txt", O_RDONLY);
  if (fd == -1)
    DEBUG ("INIT: Wrong fd\n", 0xD);

  read(fd, content, 10);

  DEBUG ("INIT: Content 1: %s\n", 0xD, content);

  //close(fd);

  int fd2 = open("1.txt", O_RDONLY);
  if (fd2 == -1)
    DEBUG ("INIT: Wrong fd\n", 0xD);

  read(fd2, &content, 10);

  DEBUG ("INIT: Content 2: %s\n", 0xD, content);

  DEBUG ("INIT: Fd1 = %d\n", 0xD, fd);
  DEBUG ("INIT: Fd2 = %d\n", 0xD, fd2);

  int pid = fork ();
  if (pid == -1) {
    DEBUG  ("Failed to fork!\n", 0x4);
  } else if (pid == 0) {
    for (i = 0 ; i < cpuinfo->cpuid ; i++) DEBUG("\n", 0x7);
    char *string[] = {"Hamid", "Francesco", NULL};
    DEBUG  ("I am the keyboard child pid = %d fd2 = %d fd1 = %d test = %x cpuid = %d\n", 0xD, pid, fd2, fd, test, cpuinfo->cpuid);
    exec("keyboard", string);
  }

  pid = fork();
  if (pid == -1) {
    DEBUG  ("Failed to fork!\n", 0x4);
  } else if (pid == 0) {
    for (i = 0 ; i < cpuinfo->cpuid ; i++) DEBUG("\n", 0x7);
    DEBUG  ("I am the console child pid = %d fd2 = %d fd1 = %d test = %x cpuid = %d\n", 0xD, pid, fd2, fd, test, cpuinfo->cpuid);
    exec("console", NULL);
  }

  for (i = 0 ; i < cpuinfo->cpuid ; i++) DEBUG("\n", 0x7);
  DEBUG  ("parent: child's PID = %d fd2 = %d fd = %d test = %x cpuid = %d\n", 0xD, pid, fd2, fd, test, cpuinfo->cpuid);
  //putc('@');

  pid = fork();
  if (pid == -1) {
    DEBUG  ("Failed to fork!\n", 0x4);
  } else if (pid == 0) {
    for (i = 0 ; i < cpuinfo->cpuid ; i++) DEBUG("\n", 0x7);
    DEBUG  ("I am the login child pid = %d fd2 = %d fd1 = %d test = %x cpuid = %d\n", 0xD, pid, fd2, fd, test, cpuinfo->cpuid);
    exec("login", NULL);
  }

  while (1) {__asm__ __volatile__ ("cli;pause;\n\t");}
}
