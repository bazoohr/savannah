# ====================================
# Makefile
#
# Hamid R. Bazoobandi
# Fri 13.7.2012
# ====================================
ROOT = ../../..

include $(ROOT)/config.mak
include $(ROOT)/rules.mak

LINK_LD  = $(ROOT)/drivers/drivers.ld
PROGNAME = e1000
MYDIR    = drivers/net/e1000/
LIBDIR = $(ROOT)/lib/

CFLAGS  += -I ./include/ -I $(ROOT)/include/ -I $(ROOT)/lib/include -I $(ROOT)/lib/lwip/include/ -I $(ROOT)/lib/lwip/include/ipv4 -I $(ROOT)/lib/lwip/include/ipv6
ASFLAGS += -I ./include/ -I $(ROOT)/include/ -I $(ROOT)/lib/include
LDFLAGS += -n -T $(LINK_LD)

SRC = start_off.S main.c e1000_isr.S lwip-input.c

OBJ = $(subst .S,.o,$(filter %.S,$(SRC))) $(subst .c,.o,$(filter %.c,$(SRC)))

all: $(PROGNAME)

# TODO:
# e1000 driver should not be linked with lwip. I just put it here to test
# some functionalities. In case I forget to remove it myself, you remove it
# if you see this TODO.
$(PROGNAME): $(OBJ)
	$(call silent_command, $(LD) $(LDFLAGS) -o $@ $(OBJ) $(ROOT)/drivers/pci/$(PCILIB) $(LIBDIR)/$(LWIPLIB) $(LIBDIR)/$(USRLIB), "    LINK  $(MYDIR)$@")

-include $(subst .c,.d,$(filter %.c,$(SRC)))


.PHONY: clean
.PHONY: distclean
clean:
	$(call silent_command, rm -f dev/*.o dev/*.d *.o *.d $(PROGNAME), "    CLEAN $(MYDIR)")
distclean:
	$(call silent_command, rm -f dev/*.o dev/*.d *.o *.d $(PROGNAME), "    CLEAN ALL $(MYDIR)")
	$(call silent_command, rm -f include/asm_frame_const.h include/asm_procregs_const.h include/asm_cpu_const.h include/asm_boot_stage2_args_const.h)
