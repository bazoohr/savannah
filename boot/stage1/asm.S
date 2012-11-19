.file "asm.S"
.section .text
#define TRUE  1
#define FALSE 0
/*
 * bool cpu_has_long_mode (void);
 *
 * This function checks to see whether CPU
 * supports long mode or not?
 * Do not panic!! Nobody expects you to understand it :)
 * If you are very curious to know what it means, take a
 * look at AMD64 system programmer manual.
 */
  .p2align 4, 0x90
  .globl has_long_mode
  .type has_long_mode, @function
has_long_mode:
  pushl %ebp
  movl %esp, %ebp

  pushl %ebx

  movl $0x80000000, %eax
  cpuid
  cmpl $0x80000000, %eax
  jbe no_long_mode
  movl $0x80000001, %eax
  cpuid
  btl $29, %edx
  jnc no_long_mode
  movl $TRUE, %eax
  jmp finish
no_long_mode:
  movl $FALSE, %eax
finish:

  popl %ebx

  movl %ebp, %esp
  popl %ebp
  ret
/*
 * Support of x87 and media instructions is very important in 64-bit mode.
 * Without xmm* registers, most probably functions with variable number of arguments
 * will cause an Invalid Opcode Exception. this is because according to AMD64 ABI,
 * arguments of float type must be passed to functions in xmm* registers.
 * Of course from compiler to compiler generated code may vary.
 * But to be in safe margine, we'd better to make sure that CPU is supporting media
 * instructions, and has all needed features, and then enable these features. Because
 * support for media instructions is desabled by default.
 * If you are curious to know what this function does, take a look at AMD64 CPUID
 * programmer manual.
 */
  .p2align 4, 0x90
  .globl has_x87_and_media_support
  .type has_x87_and_media_support, @function
has_x87_and_media_support:
  pushl %ebp
  movl %esp, %ebp

  pushl %ebx

  movl $0x00000001, %eax
  cpuid
  btl $0, %edx
  jnc no_x87
  btl $25, %edx
  jnc no_sse
  btl $26, %edx
  jnc no_sse2
  btl $0, %ecx
  jnc no_sse3
  btl $23, %edx
  jnc no_mmx
  btl $24, %edx
  jnc no_fxsave_fxrstore
  /* It supports all, return true*/
  movl $TRUE, %eax
  jmp 1f
no_x87:
no_sse:
no_sse2:
no_sse3:
no_mmx:
no_fxsave_fxrstore:
  movl $FALSE, %eax
1:

  popl %ebx

  movl %ebp, %esp
  popl %ebp
  ret
/*
 * Enable media instructions. If you are curious to know how it works
 * take a look at AMD64 CPUID programmer manual.
 */
  .p2align 4, 0x90
  .globl enable_x87_and_media
  .type enable_x87_and_media, @function
enable_x87_and_media:
  /* let the processor know that we are going to use
   * media instructions, by setting cr4.osfxsr(bit number 9)*/
  movl %cr4, %eax
  btsl $9, %eax
  movl %eax, %cr4
  /* No processor emulate coprocessor 
   * by clearing cr0.em (bit number 2) */
  movl %cr0, %eax
  btrl $2, %eax
  movl %eax, %cr0
  /* enable monitor coprocessor by enabling
   * cr0.mp (bit number 1) */
  movl %cr0, %eax
  btsl $1, %eax
  movl %eax, %cr0
  ret
/*
 * This function stop the cpu!
 */
  .p2align 4, 0x90
  .globl halt
  .type halt, @function
halt:
  cli      /* No interrupt */
  hlt      /* Stop CPU */

  .p2align 4, 0x90
  .globl has_1GBpage
  .type has_1GBpage, @function
has_1GBpage:
  pushl %ebp
  movl %esp, %ebp

  pushl %ebx

  movl $0x80000001, %eax
  cpuid
  btl $26, %edx
  jnc no_1GBpage
  movl $TRUE, %eax
  jmp 1f
no_1GBpage:
  movl $FALSE, %eax
1:
  popl %ebx

  movl %ebp, %esp
  popl %ebp
  ret
