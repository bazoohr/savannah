# ====================================
# Makefile
#
# Hamid R. Bazoobandi
# Fri 13.7.2012
# ====================================
ROOT = ..

include $(ROOT)/rules.mak

LINK_LD = vmm.ld

CFLAGS  += -I include/ -I $(ROOT)/include/ -I $(ROOT)/lib/include
ASFLAGS += -I include/ -I $(ROOT)/include/ -I $(ROOT)/lib/include
LDFLAGS += -n -T $(LINK_LD)

MYDIR = vmm/
LIBDIR = $(ROOT)/lib/

SRC = start_off.S main.c console.c printk.c string.c panic.c trap.S trap_handler.c \
			memory.c asmfunc.S pmap.c \
			timer.c isr.S segment.c interrupt.c mp.c cpu.c vmx.c \
			dev/i8253.c dev/i8259.c dev/keyboard.c dev/pic.c dev/ioapic.c dev/lapic.c

OBJ = $(subst .S,.o,$(filter %.S,$(SRC))) $(subst .c,.o,$(filter %.c,$(SRC)))

$(VMM): $(OBJ)
	$(call silent_command, $(LD) $(LDFLAGS) -o $@ $(OBJ) $(LIBDIR)/$(USRLIB), "    LINK  $(MYDIR)$@")

-include $(subst .c,.d,$(filter %.c,$(SRC)))


.PHONY: clean
.PHONY: distclean
clean:
	$(call silent_command, rm -f dev/*.o dev/*.d *.o *.d $(VMM), "    CLEAN $(MYDIR)")
distclean:
	$(call silent_command, rm -f dev/*.o dev/*.d *.o *.d $(VMM), "    CLEAN ALL $(MYDIR)")
	$(call silent_command, rm -f include/asm_frame_const.h include/asm_procregs_const.h include/asm_kernel_args_const.h)
