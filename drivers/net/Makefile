# ====================================
# Makefile
#
# Hamid R. Bazoobandi
# Fri 13.7.2012
# ====================================
ROOT = ../../

include $(ROOT)/config.mak
include $(ROOT)/rules.mak

MYDIR = drivers/net/

all:
	$(call silent_command, $(MAKE) -s --no-print-directory -C e1000)

.PHONY: clean
.PHONY: distclean
clean:
	$(call silent_command, rm -f *.o *.d , "    CLEAN $(MYDIR)")
	$(call silent_command, $(MAKE) clean --no-print-directory -C e1000)
distclean:
	$(call silent_command, rm -f *.o *.d , "    CLEAN ALL $(MYDIR)")
	$(call silent_command, $(MAKE) distclean --no-print-directory -C e1000)
