# ====================================
# Makefile
#
# Hamid R. Bazoobandi
# Fri 13.7.2012
# ====================================
ROOT = ..

include $(ROOT)/rules.mak

LINK_LD = boot.ld
BOOT_APS = boot_aps
CFLAGS  += -m32 -I. -I ../include
ASFLAGS += -m32 -I.
LDFLAGS += -melf_i386 -T $(LINK_LD)

SRC = boot.S loader.c string.c screen.c bootio.c asm.S
MYDIR += boot/

all: $(LOADER) $(BOOT_APS)

$(LOADER): $(subst .S,.o,$(filter %.S,$(SRC))) $(subst .c,.o,$(filter %.c,$(SRC)))
	$(call silent_command, $(LD) $(LDFLAGS) -o $@ $+, "    LINK  $(MYDIR)$@")

#
# TODO:
# Clear this make file. Its a terrible mess at the moment.
#
$(BOOT_APS): boot_aps.o
	$(call silent_command, $(LD) $(LDFLAGS) -N -e _start -Ttext 0x0000 -o boot_aps.out $<, "    LINK   $(BOOT_APS)")
	$(call silent_command, objcopy -S -O binary boot_aps.out $@)

-include $(subst .c,.d,$(filter %.c,$(SRC)))


.PHONY: clean
.PHONY: distclean
clean:
	$(call silent_command, rm -f *.out *.o *.d $(LOADER) $(BOOT_APS), "    CLEAN $(MYDIR)")
distclean:
	$(call silent_command, rm -f *.out *.o *.d $(LOADER) $(BOOT_APS), "    CLEAN ALL $(MYDIR)")
