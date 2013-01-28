# ====================================
# Makefile
#
# Hamid R. Bazoobandi
# Fri 13.7.2012
# ====================================
ROOT = ../../

include $(ROOT)/config.mak
include $(ROOT)/rules.mak

LINK_LD  = kernel.ld
PROGNAME = stage2
MYDIR    = boot/stage2/

CFLAGS  += -I ./include/ -I $(ROOT)/include/
ASFLAGS += -I ./include/ -I $(ROOT)/include/
LDFLAGS += -n -T $(LINK_LD)

SRC = start_off.S main.c console.c printk.c string.c panic.c trap.S trap_handler.c \
			asmfunc.S \
			timer.c isr.S segment.c interrupt.c mp.c cpu.c \
			dev/i8253.c dev/i8259.c dev/keyboard.c dev/pic.c dev/ioapic.c dev/lapic.c

OBJ = $(subst .S,.o,$(filter %.S,$(SRC))) $(subst .c,.o,$(filter %.c,$(SRC)))

all: $(BOOT_APS) $(PROGNAME)

$(PROGNAME): $(OBJ)
	$(call silent_command, $(LD) $(LDFLAGS) -o $@ $(OBJ), "    LINK  $(MYDIR)$@")

$(BOOT_APS): boot_aps.S
	$(call silent_command, $(AS) $(ASFLAGS) -m32 -c -o $@.o $<, "    AS    $<")
	$(call silent_command, $(LD) -melf_i386 -N -e _start -Ttext 0x0000 -o $(BOOT_APS).out $@.o, "    LINK  $(BOOT_APS)")
	$(call silent_command, objcopy -S -O binary $(BOOT_APS).out $@)

-include $(subst .c,.d,$(filter %.c,$(SRC)))


.PHONY: clean
.PHONY: distclean
clean:
	$(call silent_command, rm -f dev/*.o dev/*.d *.o *.d $(BOOT_APS) $(PROGNAME), "    CLEAN $(MYDIR)")
distclean:
	$(call silent_command, rm -f dev/*.o dev/*.d *.o *.d $(BOOT_APS) $(PROGNAME), "    CLEAN ALL $(MYDIR)")
	$(call silent_command, rm -f include/asm_frame_const.h include/asm_procregs_const.h include/asm_cpu_const.h include/asm_boot_stage2_args_const.h)
