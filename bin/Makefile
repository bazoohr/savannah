# ====================================
# Makefile
#
# Hamid R. Bazoobandi
# Fri 13.7.2012
# ====================================
ROOT = ../

include $(ROOT)/rules.mak

LINK_LD = bin.ld

CFLAGS  += -I include/ -I $(ROOT)/include/ -I $(ROOT)/lib/include
ASFLAGS += -I include/ -I $(ROOT)/include/ -I $(ROOT)/lib/include
LDFLAGS += -n -T $(LINK_LD)

MYDIR = bin/
LIBDIR = $(ROOT)/lib/

SRC = console.c printk.c login.c start_off.S

OBJ = $(subst .S,.o,$(filter %.S,$(SRC))) $(subst .c,.o,$(filter %.c,$(SRC)))

login: $(OBJ)
	$(call silent_command, $(LD) $(LDFLAGS) -o $@ $(OBJ) $(LIBDIR)/$(USRLIB), "    LINK  $(MYDIR)$@")

-include $(subst .c,.d,$(filter %.c,$(SRC)))


.PHONY: clean
.PHONY: distclean
clean:
	$(call silent_command, rm -f dev/*.o dev/*.d *.o *.d login, "    CLEAN $(MYDIR)")
distclean:
	$(call silent_command, rm -f dev/*.o dev/*.d *.o *.d login, "    CLEAN ALL $(MYDIR)")
	$(call silent_command, rm -f include/asm_frame_const.h include/asm_procregs_const.h include/asm_kernel_args_const.h)
