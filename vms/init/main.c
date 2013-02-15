#include <cdef.h>
#include <types.h>
#include <printk.h>
#include <console.h>
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
  con_init ();

  for (i = 0 ; i < cpuinfo->cpuid ; i++) printk("\n");

  if (cpuinfo->cpuid == 3) {
    cprintk ("THIS IS CHILD!\n", 0x2);
    while (1) {__asm__ __volatile__ ("cli;pause;\n\t");}
  }
  cprintk ("INIT: My info is in addr = %d\n", 0xD, cpuinfo->cpuid);

  char content[10];
  memset(content, 0, 10);

  int fd = open("2.txt", O_RDONLY);
  if (fd == -1)
    cprintk("INIT: Wrong fd\n", 0xD);

  read(fd, content, 10);

  cprintk("INIT: Content 1: %s\n", 0xD, content);

  //close(fd);

  int fd2 = open("1.txt", O_RDONLY);
  if (fd2 == -1)
    cprintk("INIT: Wrong fd\n", 0xD);

  read(fd2, &content, 10);

  cprintk("INIT: Content 2: %s\n", 0xD, content);

  cprintk("INIT: Fd1 = %d\n", 0xD, fd);
  cprintk("INIT: Fd2 = %d\n", 0xD, fd2);

  int pid = fork ();
  if (pid == -1) {
    cprintk ("Failed to fork!\n", 0x4);
  } else if (pid == 0) {
    for (i = 0 ; i < cpuinfo->cpuid ; i++) printk("\n");
    char *string[] = {"Hamid", "Francesco", NULL};
    cprintk ("I am a child pid = %d fd2 = %d fd1 = %d test = %x\n", 0xD, pid, fd2, fd, test);
    exec("keyboard", 3, string);
  } else {
    for (i = 0 ; i < cpuinfo->cpuid ; i++) printk("\n");
    cprintk ("parent: child's PID = %d fd2 = %d fd = %d test = %x\n", 0xE, pid, fd2, fd, test);
  }

  while (1) {__asm__ __volatile__ ("cli;pause;\n\t");}
}
