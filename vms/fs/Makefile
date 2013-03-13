# ====================================
# Makefile
#
# Hamid R. Bazoobandi
# Fri 13.7.2012
# ====================================
ROOT = ../..

include $(ROOT)/rules.mak

LINK_LD = fs.ld

CFLAGS  += -I include/ -I $(ROOT)/include/ -I $(ROOT)/lib/include
ASFLAGS += -I include/ -I $(ROOT)/include/ -I $(ROOT)/lib/include
LDFLAGS += -n -T $(LINK_LD)

MYDIR = vms/fs/
LIBDIR = $(ROOT)/lib/

SRC = start_off.S main.c asmfunc.S interrupt.c trap.S trap_handler.c

OBJ = $(subst .S,.o,$(filter %.S,$(SRC))) $(subst .c,.o,$(filter %.c,$(SRC)))

$(FS): $(OBJ)
	$(call silent_command, $(LD) $(LDFLAGS) -o $@ $(OBJ) $(LIBDIR)/$(USRLIB), "    LINK  $(MYDIR)$@")

-include $(subst .c,.d,$(filter %.c,$(SRC)))


.PHONY: clean
.PHONY: distclean
clean:
	$(call silent_command, rm -f dev/*.o dev/*.d *.o *.d $(FS), "    CLEAN $(MYDIR)")
distclean:
	$(call silent_command, rm -f dev/*.o dev/*.d *.o *.d $(FS), "    CLEAN ALL $(MYDIR)")
	$(call silent_command, rm -f include/asm_frame_const.h include/asm_procregs_const.h include/asm_kernel_args_const.h)
