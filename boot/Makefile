# ====================================
# Makefile
#
# Hamid R. Bazoobandi
# Fri 13.7.2012
# ====================================
ROOT = ..

include $(ROOT)/config.mak
include $(ROOT)/rules.mak

MYDIR += boot/

all:
	$(call silent_command, $(MAKE) -s --no-print-directory -C stage1)
	$(call silent_command, $(MAKE) -s --no-print-directory -C stage2)

.PHONY: clean
.PHONY: distclean
clean:
	$(call silent_command, rm -f *.o *.d , "    CLEAN")
	$(call silent_command, $(MAKE) clean --no-print-directory -C stage1)
	$(call silent_command, $(MAKE) clean --no-print-directory -C stage2)
distclean:
	$(call silent_command, rm -f *.o *.d , "    CLEAN ALL")
	$(call silent_command, $(MAKE) distclean --no-print-directory -C stage1)
	$(call silent_command, $(MAKE) distclean --no-print-directory -C stage2)
