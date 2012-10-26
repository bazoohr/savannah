# ====================================
# Makefile
#
# Hamid R. Bazoobandi
# Fri 13.7.2012
# ====================================
ROOT = ..

include $(ROOT)/rules.mak

LINK_LD = boot.ld

CFLAGS  += -m32 -I. -I ../include
ASFLAGS += -m32 -I.
LDFLAGS += -melf_i386 -T $(LINK_LD)

SRC = boot.S loader.c string.c screen.c bootio.c asm.S
MYDIR += boot/

$(LOADER): $(subst .S,.o,$(filter %.S,$(SRC))) $(subst .c,.o,$(filter %.c,$(SRC)))
	$(call silent_command, $(LD) $(LDFLAGS) -o $@ $+, "    LINK  $(MYDIR)$@")

-include $(subst .c,.d,$(filter %.c,$(SRC)))


.PHONY: clean
.PHONY: distclean
clean:
	$(call silent_command, rm -f *.o *.d $(LOADER), "    CLEAN $(MYDIR)")
distclean:
	$(call silent_command, rm -f *.o *.d $(LOADER), "    CLEAN ALL $(MYDIR)")
