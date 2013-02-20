# ====================================
# Makefile
#
# Hamid R. Bazoobandi
# Fri 13.7.2012
# ====================================
ROOT     = ../../

include $(ROOT)/config.mak
include $(ROOT)/rules.mak

MYDIR    = boot/stage1/
PROGNAME = $(BOOT_STAGE1)


LINK_LD = stage1.ld
CFLAGS  += -m32 -I. -I $(ROOT)/include
ASFLAGS += -m32 -I.
LDFLAGS += -melf_i386 -T $(LINK_LD)

SRC = boot.S loader.c string.c screen.c bootio.c asm.S

all: $(PROGNAME)

$(PROGNAME): $(subst .S,.o,$(filter %.S,$(SRC))) $(subst .c,.o,$(filter %.c,$(SRC)))
	$(call silent_command, $(LD) $(LDFLAGS) -o $@ $+, "    LINK  $(MYDIR)$@")

-include $(subst .c,.d,$(filter %.c,$(SRC)))


.PHONY: clean
.PHONY: distclean
clean:
	$(call silent_command, rm -f *.out *.o *.d $(PROGNAME), "    CLEAN $(MYDIR)")
distclean:
	$(call silent_command, rm -f *.out *.o *.d $(PROGNAME), "    CLEAN ALL $(MYDIR)")
