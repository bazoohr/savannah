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
MYDIR    = drivers/pci/
LIBDIR = $(ROOT)/lib/

CFLAGS  += -I ./include/ -I $(ROOT)/include/ -I $(ROOT)/lib/include
ASFLAGS += -I ./include/ -I $(ROOT)/include/

AR=ar
ARFLAGS=cr

SRC = pci.c

OBJ = $(subst .S,.o,$(filter %.S,$(SRC))) $(subst .c,.o,$(filter %.c,$(SRC)))

$(PCILIB): $(OBJ)
	$(call silent_command, $(AR) $(ARFLAGS) $@ $(OBJ), "    ARCH  $(MYDIR)$@")

-include $(subst .c,.d,$(filter %.c,$(SRC)))


.PHONY: clean
.PHONY: distclean
clean:
	$(call silent_command, rm -f dev/*.o dev/*.d *.o *.d $(PCILIB), "    CLEAN $(MYDIR)")
distclean:
	$(call silent_command, rm -f dev/*.o dev/*.d *.o *.d $(PCILIB), "    CLEAN ALL $(MYDIR)")
	$(call silent_command, rm -f include/asm_frame_const.h include/asm_procregs_const.h include/asm_cpu_const.h include/asm_boot_stage2_args_const.h)
