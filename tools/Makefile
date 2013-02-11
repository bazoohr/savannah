# ====================================
# Makefile
#
# Hamid R. Bazoobandi
# Fri 13.7.2012
# ====================================
ROOT= ..
CC=gcc

CFLAGS = -I $(ROOT)/boot/stage2/include -I $(ROOT)/include

silent_command = $(if $(2), @echo $2 && $1, @$(1))

MYDIR = tools/

.PHONY: all clean distclean
all: gen_asm_frame_const_h gen_asm_boot_stage2_args_const_h gen_asm_cpu_const_h

gen_asm_frame_const_h: gen_asm_frame_const_h.c $(ROOT)/boot/stage2/include/frame.h
	$(CC) $(CFLAGS) -o $@ $@.c
	$(call silent_command, ./$@ > $(ROOT)/boot/stage2/include/asm_frame_const.h, "    GEN    $(CURDIR)/boot/stage2/include/asm_frame_const.h")
	$(call silent_command, ./$@ > $(ROOT)/vmm/include/asm_frame_const.h, "    GEN    $(CURDIR)/vmm/include/asm_frame_const.h")
	$(call silent_command, ./$@ > $(ROOT)/vms/pm/include/asm_frame_const.h, "    GEN    $(CURDIR)/vms/pm/include/asm_frame_const.h")
	$(call silent_command, ./$@ > $(ROOT)/vms/fs/include/asm_frame_const.h, "    GEN    $(CURDIR)/vms/fs/include/asm_frame_const.h")
	$(call silent_command, ./$@ > $(ROOT)/vms/init/include/asm_frame_const.h, "    GEN    $(CURDIR)/vms/init/include/asm_frame_const.h")
gen_asm_boot_stage2_args_const_h: gen_asm_boot_stage2_args_const_h.c $(ROOT)/include/boot_stage2_args.h
	$(CC) $(CFLAGS) -o $@ $@.c
	$(call silent_command, ./$@ > $(ROOT)/boot/stage2/include/asm_boot_stage2_args_const.h, "    GEN    $(CURDIR)/boot/stage2/include/asm_boot_stage2_args_const.h")
gen_asm_cpu_const_h: gen_asm_cpu_const_h.c $(ROOT)/include/cpu.h
	$(CC) $(CFLAGS) -o $@ $@.c
	$(call silent_command, ./$@ > $(ROOT)/boot/stage2/include/asm_cpu_const.h, "    GEN    $(CURDIR)/boot/stage2/include/asm_cpu_const.h")
	$(call silent_command, ./$@ > $(ROOT)/lib/include/asm_cpu_const.h, "    GEN    $(CURDIR)/lib/include/asm_cpu_const.h")
clean:
	$(call silent_command, rm -f *.o *.d, "    CLEAN $(MYDIR)")
	@rm -f `echo *.c | sed 's/ /\n/g' | sed 's/\(^.*\)\.c/\1/g'`
distclean:
	$(call silent_command, rm -f *.o *.d, "    CLEAN ALL $(MYDIR)")
	@rm -f `echo *.c | sed 's/ /\n/g' | sed 's/\(^.*\)\.c/\1/g'`
