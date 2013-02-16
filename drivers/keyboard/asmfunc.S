/* ======================================== *
 * asmfunc.S                                *
 *                                          *
 * Hamid R. Bazoobandi 2012                 *
 * Amsterdam, The Netherlands               *
 * ======================================== */
.code64
.section .text
#include <const.h>
#include <dev/pic.h>
#include <asmlink.h>
/* ============================ */
ENTRY (cli)
  cli
  retq
END (cli)
/* ============================ */
ENTRY (inb)
	movw %di, %dx
	inb %dx
	retq
END (inb)
/* ============================ */
ENTRY (outb)
	movw %di, %ax
	movw %si, %dx
	outb %al, %dx
	retq
END(outb)
/* ================================== */
ENTRY (lidt)
  lidt (%rdi)
  retq
END (lidt)
/* ================================== */
ENTRY (load_cr3)
  movq %rdi, %rax
  movq %rax, %cr3
  retq
END (load_cr3)
