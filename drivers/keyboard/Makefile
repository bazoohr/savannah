# ====================================
# Makefile
#
# Hamid R. Bazoobandi
# Fri 13.7.2012
# ====================================
ROOT = ../../

include $(ROOT)/config.mak
include $(ROOT)/rules.mak

LINK_LD  = ../drivers.ld
PROGNAME = keyboard
MYDIR    = drivers/keyboard/
LIBDIR = $(ROOT)/lib/

CFLAGS  += -I ./include/ -I $(ROOT)/include/ -I $(ROOT)/lib/include
ASFLAGS += -I ./include/ -I $(ROOT)/include/
LDFLAGS += -n -T $(LINK_LD)

SRC = start_off.S main.c console.c printk.c trap.S trap_handler.c \
			asmfunc.S \
			timer.c isr.S segment.c interrupt.c cpu.c\
			dev/i8253.c dev/i8259.c dev/keyboard.c dev/pic.c dev/ioapic.c dev/lapic.c

OBJ = $(subst .S,.o,$(filter %.S,$(SRC))) $(subst .c,.o,$(filter %.c,$(SRC)))

all: $(PROGNAME)

$(PROGNAME): $(OBJ)
	$(call silent_command, $(LD) $(LDFLAGS) -o $@ $(OBJ) $(LIBDIR)/$(USRLIB), "    LINK  $(MYDIR)$@")

-include $(subst .c,.d,$(filter %.c,$(SRC)))


.PHONY: clean
.PHONY: distclean
clean:
	$(call silent_command, rm -f dev/*.o dev/*.d *.o *.d $(PROGNAME), "    CLEAN $(MYDIR)")
distclean:
	$(call silent_command, rm -f dev/*.o dev/*.d *.o *.d $(PROGNAME), "    CLEAN ALL $(MYDIR)")
	$(call silent_command, rm -f include/asm_frame_const.h include/asm_procregs_const.h include/asm_cpu_const.h include/asm_boot_stage2_args_const.h)
