# ====================================
# Makefile
#
# Hamid R. Bazoobandi
# Fri 13.7.2012
# ====================================
ROOT= ..
include $(ROOT)/config.mak

silent_command = $(if $(2), @echo $2 && $1, @$(1))

MYDIR = initrd/

.PHONY: all clean distclean
all: create_initrd

create_initrd: create_initrd.c
	$(CC) -Wall -o $@ $@.c
	$(call silent_command, ./$@ 1.txt 2.txt $(ROOT)/bin/login $(ROOT)/bin/test $(ROOT)/bin/testfs $(ROOT)/bin/testcnl $(ROOT)/drivers/keyboard/keyboard $(ROOT)/drivers/console/console \
	$(ROOT)/drivers/junk/junk $(ROOT)/drivers/net/e1000/e1000 $(ROOT)/bin/hft, "    GEN   $(CURDIR)/$(INITRD)")
clean:
	$(call silent_command, rm -f *.o *.d $(INITRD), "    CLEAN $(MYDIR)")
	@rm -f `echo *.c | sed 's/ /\n/g' | sed 's/\(^.*\)\.c/\1/g'`
distclean:
	$(call silent_command, rm -f *.o *.d $(INITRD), "    CLEAN ALL $(MYDIR)")
	@rm -f `echo *.c | sed 's/ /\n/g' | sed 's/\(^.*\)\.c/\1/g'`
