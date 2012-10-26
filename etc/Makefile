# ====================================
# Makefile
#
# Hamid R. Bazoobandi
# Fri 13.7.2012
# ====================================
ROOT= ..
CC=gcc

CFLAGS = -I $(ROOT)/kernel/include -I $(ROOT)/include

silent_command = $(if $(2), @echo $2 && $1, @$(1))

MYDIR += etc

.PHONY: all clean distclean
all: gen_asm_frame_const_h gen_asm_proc_const_h gen_asm_kernel_args_const_h

gen_asm_frame_const_h: gen_asm_frame_const_h.c $(ROOT)/kernel/include/frame.h
	$(CC) $(CFLAGS) -o $@ $@.c
	$(call silent_command, ./$@ > $(ROOT)/kernel/include/asm_frame_const.h, "    GEN    $(CURDIR)/kernel/include/asm_frame_const.h")
gen_asm_proc_const_h: gen_asm_proc_const_h.c $(ROOT)/kernel/include/proc.h
	$(CC) $(CFLAGS) -o $@ $@.c
	$(call silent_command, ./$@ > $(ROOT)/kernel/include/asm_proc_const.h, "    GEN    $(CURDIR)/kernel/include/asm_proc_const.h")
gen_asm_kernel_args_const_h: gen_asm_kernel_args_const_h.c $(ROOT)/include/kernel_args.h
	$(CC) $(CFLAGS) -o $@ $@.c
	$(call silent_command, ./$@ > $(ROOT)/kernel/include/asm_kernel_args_const.h, "    GEN    $(CURDIR)/kernel/include/asm_kernel_args_const.h")

clean:
	$(call silent_command, rm -f *.o *.d, "    CLEAN  $(MYDIR)")
	@rm -f `echo *.c | sed 's/ /\n/g' | sed 's/\(^.*\)\.c/\1/g'`
distclean:
	$(call silent_command, rm -f *.o *.d, "    CLEAN ALL  $(MYDIR)")
	@rm -f `echo *.c | sed 's/ /\n/g' | sed 's/\(^.*\)\.c/\1/g'`
