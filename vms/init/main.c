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
#include <string.h>
#include <ipc.h>
#include <fs.h>

void
vm_main (void)
{
  int i;
  con_init ();
  for (i = 0 ; i < cpuinfo->cpuid ; i++) printk("\n");

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

  while (1) {__asm__ __volatile__ ("cli;pause;\n\t");}
}
