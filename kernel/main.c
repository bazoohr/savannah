#include <cdef.h>
#include <dev/pic.h>
#include <types.h>
#include <kernel_args.h>
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
/* ========================================== */
static void
print_logo(void)
{
  cprintk("Welcome to\n", 0xB);
  cprintk("       v       v  u    u         oo      sss\n", 0xB);
  cprintk("        v     v   u    u        o  o    s\n", 0xB);
  cprintk("         v   v    u    u  ===   o  o     s\n", 0xB);
  cprintk("          v v     u    u        o  o      s\n", 0xB);
  cprintk("           v       uuuu          oo     sss\n", 0xB);
  cprintk("\n", 0xB);
}
void 
kmain (struct kernel_args *kargs)
{
  uint32_t rax;
  uint32_t rbx;
  uint32_t rcx;
  uint32_t rdx;

  con_init ();
  mm_init (kargs->ka_kernel_end_addr, kargs->ka_kernel_cr3, kargs->ka_memsz);
  mp_init ();

  cpuid (1, &rax, &rbx, &rcx, &rdx);
  cprintk ("rax = %x\n", 0xE, rax);
  cprintk ("rbx = %x\n", 0xE, rbx);
  cprintk ("rcx = %x\n", 0xE, rcx);
  cprintk ("rcx = %x\n", 0xE, rdx);
  if (rcx & 0x1) {
    cprintk ("SSE, SSE1 supported!\n", 0xE);
  } else {
    cprintk ("SSE, SSE1 NOT supported!\n", 0x4);
  }
  if (rcx & (1<<3)) {
    cprintk ("MWAIT supported!\n", 0xE);
  } else {
    cprintk ("MWAIT NOT supported!\n", 0x4);
  }

  if (rcx & (1<<9)) {
    cprintk ("SSSE3 is supported!\n", 0xE);
  } else {
    cprintk ("SSSE3 NOT supported!\n", 0x4);
  }

  cpuid (5, &rax, &rbx, &rcx, &rdx);
  cprintk ("rax = %x\n", 0xE, rax);
  cprintk ("rbx = %x\n", 0xE, rbx);
  cprintk ("rcx = %x\n", 0xE, rcx);
  cprintk ("rcx = %x\n", 0xE, rdx);
  interrupt_init (); /*idt*/
  __asm__ __volatile__ ("cli;hlt\n\t");

  pic_init ();
  ioapic_init ();
  lapic_init();
  kbd_init ();

  print_logo();
  mp_bootothers ();

  __asm__ __volatile__ ("sti\n");

  for (;;);
}
