# TODO
# Figure out if we can remove this unuseful file
.globl _start
_start:
  movq %rdi, cpuinfo

  call vmm_main
