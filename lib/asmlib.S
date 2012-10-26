#include <syscall.h>
  .section .text
/* =================================== */
  .p2align 4, 0x90
  .globl puts
  .type puts, @function
puts:
  pushq %rbx

  movq $SYS_PUTS, %rax      /* System call number */
  movq %rdi, %rbx    /* string */
  movq %rsi, %rcx    /* Color */
  int $0x80
  
  popq %rbx
  retq
  .size puts, .-puts
/* =================================== */
  .p2align 4, 0x90
  .globl putc
  .type putc, @function
putc:
  pushq %rbx

  movq $SYS_PUTC, %rax      /* System call number */
  movq %rdi, %rbx    /* Char */
  movq %rsi, %rcx    /* Color */
  int $0x80

  popq %rbx
  retq
  .size putc, .-putc
/* =================================== */
  .p2align 4, 0x90
  .globl gotoxy
  .type gotoxy, @function
gotoxy:
  pushq %rbx

  movq $SYS_GOTOXY, %rax     /* System call number */
  movq %rdi, %rbx   /* X */
  movq %rsi, %rcx   /* Y */
  int $0x80

  popq %rbx
  retq
  .size gotoxy, .-gotoxy
/* =================================== */
  .p2align 4, 0x90
  .globl wherexy
  .type wherexy, @function
wherexy:
  pushq %rbx
    
  movq $SYS_WHEREXY, %rax    /* System call number */
  movq %rdi, %rbx  /* &x */
  movq %rsi, %rcx  /* &y */
  int $0x80

  popq %rbx
  retq
  .size wherexy, .-wherexy
/* =================================== */
  .p2align 4, 0x90
  .globl fork
  .type fork, @function
fork:

  movq $SYS_FORK, %rax    /* System call number */
  int $0x80

  retq
  .size fork, .-fork
/* =================================== */
  .p2align 4, 0x90
  .globl clone
  .type clone, @function
clone:
  pushq %rbx

  movq $SYS_CLONE, %rax    /* System call number */
  movq %rdi, %rbx  /* thread routin, void *(func*)(void*) */  
  movq %rdx, %rdx  /* stack virtual address */
  movq %rcx, %rdi  /* stack size */
  movq %rsi, %rcx  /* thread argument, void *args */

  int $0x80

  popq %rbx
  retq
  .size clone, .-clone
/* =================================== */
  .p2align 4, 0x90
  .globl unclone
  .type unclone, @function
unclone:
  pushq %rbx

  movq $SYS_UNCLONE, %rax   /* System call number */
  movq %rdi, %rbx  /* returning status */
  int $0x80

  popq %rbx
  retq
  .size unclone, .-unclone
/* =================================== */
  .p2align 4, 0x90
  .globl exit
  .type exit, @function
exit:
  pushq %rbx

  movq $SYS_EXIT, %rax    /* System call number */
  movq %rdi, %rbx  /* exit statue */  
  int $0x80

  popq %rbx
  retq
  .size exit, .-exit
/* =================================== */
  .p2align 4, 0x90
  .globl waitpid
  .type waitpid, @function
waitpid:
  pushq %rbx

  movq $SYS_WAITPID, %rax    /* System call number */
  movq %rdi, %rbx  /* pid */  
  movq %rsi, %rcx  /* &status */  
  int $0x80

  popq %rbx
  retq
  .size waitpid, .-waitpid
/* =================================== */
  .p2align 4, 0x90
  .globl read
  .type read, @function
read:
  pushq %rbx
  
  movq $SYS_READ, %rax     /* System call number */
  movq %rdi, %rbx   /* device number */
  movq %rsi, %rcx   /* buffer pointer */
  movq %rdx, %rdx   /* size */
  int $0x80

  popq %rbx
  retq
  .size read, .-read
/* =================================== */
  .p2align 4, 0x90
  .globl sbrk
  .type sbrk, @function
sbrk:
  pushq %rbx

  movq $SYS_SBRK, %rax     /* System call number */
  movq %rdi, %rbx    /* size */
  int $0x80          /* call kernel */

  popq %rbx
  retq
  .size sbrk, .-sbrk
/* =================================== */
  .p2align 4, 0x90
  .globl brk
  .type brk, @function
brk:
  pushq %rbx

  movq $SYS_BRK, %rax     /* System call number */
  movq %rdi, %rbx    /* address */
  int $0x80          /* call kernel */

  popq %rbx
  retq
  .size brk, .-brk
