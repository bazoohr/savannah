# ====================================
# Makefile
#
# Hamid R. Bazoobandi
# Fri 13.7.2012
# ====================================
ROOT = ..

include $(ROOT)/config.mak
include $(ROOT)/rules.mak

MYDIR = drivers/

all:
	$(call silent_command, $(MAKE) -s --no-print-directory -C keyboard)

.PHONY: clean
.PHONY: distclean
clean:
	$(call silent_command, rm -f *.o *.d , "    CLEAN $(MYDIR)")
	$(call silent_command, $(MAKE) clean --no-print-directory -C keyboard)
distclean:
	$(call silent_command, rm -f *.o *.d , "    CLEAN ALL $(MYDIR)")
	$(call silent_command, $(MAKE) distclean --no-print-directory -C keyboard)
