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

SRC = login.c start_off.S
OBJ = $(subst .S,.o,$(filter %.S,$(SRC))) $(subst .c,.o,$(filter %.c,$(SRC)))

SRC2 = test.c start_off.S
OBJ2 = $(subst .S,.o,$(filter %.S,$(SRC2))) $(subst .c,.o,$(filter %.c,$(SRC2)))

SRC3 = testfs.c start_off.S
OBJ3 = $(subst .S,.o,$(filter %.S,$(SRC3))) $(subst .c,.o,$(filter %.c,$(SRC3)))

SRC4 = testcnl.c start_off.S
OBJ4 = $(subst .S,.o,$(filter %.S,$(SRC4))) $(subst .c,.o,$(filter %.c,$(SRC4)))

SRC5 = hft.c start_off.S hft_isr.S
OBJ5 = $(subst .S,.o,$(filter %.S,$(SRC5))) $(subst .c,.o,$(filter %.c,$(SRC5)))

all: login test testfs testcnl hft

login: $(OBJ)
	$(call silent_command, $(LD) $(LDFLAGS) -o $@ $(OBJ) $(LIBDIR)/$(USRLIB), "    LINK  $(MYDIR)$@")
test: $(OBJ2)
	$(call silent_command, $(LD) $(LDFLAGS) -o $@ $(OBJ2) $(LIBDIR)/$(USRLIB), "    LINK  $(MYDIR)$@")
testfs: $(OBJ3)
	$(call silent_command, $(LD) $(LDFLAGS) -o $@ $(OBJ3) $(LIBDIR)/$(USRLIB), "    LINK  $(MYDIR)$@")
testcnl: $(OBJ4)
	$(call silent_command, $(LD) $(LDFLAGS) -o $@ $(OBJ4) $(LIBDIR)/$(USRLIB), "    LINK  $(MYDIR)$@")
hft: $(OBJ5)
	$(call silent_command, $(LD) $(LDFLAGS) -o $@ $(OBJ5) $(LIBDIR)/$(USRLIB), "    LINK  $(MYDIR)$@")

-include $(subst .c,.d,$(filter %.c,$(SRC)))


.PHONY: clean
.PHONY: distclean
clean:
	$(call silent_command, rm -f dev/*.o dev/*.d *.o *.d login test testfs testcnl hft, "    CLEAN $(MYDIR)")
distclean:
	$(call silent_command, rm -f dev/*.o dev/*.d *.o *.d login test testfs testcnl hft, "    CLEAN ALL $(MYDIR)")
	$(call silent_command, rm -f include/asm_frame_const.h include/asm_procregs_const.h include/asm_kernel_args_const.h)
