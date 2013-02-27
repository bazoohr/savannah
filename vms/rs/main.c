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

void
vm_main (void)
{
  while (1) {__asm__ __volatile__ ("cli;pause;\n\t");}
}
