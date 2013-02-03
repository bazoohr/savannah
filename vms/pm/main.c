#include <cdef.h>
#include <dev/pic.h>
#include <types.h>
#include <printk.h>
#include <console.h>
#include <const.h>
#include <interrupt.h>
#include <asmfunc.h>
#include <memory.h>
#include <dev/keyboard.h>
#include <mp.h>
#include <dev/ioapic.h>
#include <dev/lapic.h>
#include <cpu.h>
#include <cpuinfo.h>
#include <fs.h>
#include <string.h>

void
vm_main (void)
{
  int i;
  con_init ();
  for (i = 0 ; i < cpuinfo->cpuid ; i++) printk("\n");

  cprintk ("PM: My info is in addr = %d\n", 0xD, cpuinfo->cpuid);

  char content[10];
  memset(&content, 0, 10);

  int fd = open("2.txt", O_RDONLY);
  if (fd == -1)
    cprintk("PM: Wrong fd\n", 0xD);

  read(fd, &content, 10);

  cprintk("PM: Content 1: %s\n", 0xD, content);

  //close(fd);

  int fd2 = open("1.txt", O_RDONLY);
  if (fd2 == -1)
    cprintk("PM: Wrong fd\n", 0xD);

  read(fd2, &content, 10);

  cprintk("PM: Content 2: %s\n", 0xD, content);

  cprintk("Fd1 = %d\n", 0xD, fd);
  cprintk("Fd2 = %d\n", 0xD, fd2);

  while (1) {__asm__ __volatile__ ("cli;pause;\n\t");}
}
