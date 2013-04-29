ROOT = ..

include $(ROOT)/rules.mak

MYDIR = lib/

CFLAGS  += -I ./include/ -I $(ROOT)/include/
ASFLAGS += -I ./include/ -I $(ROOT)/include/

AR=ar
ARFLAGS=cr

SRC = ipc.c misc.c string.c fs.c pm.c asm_lib.S lib_mem.c gdt.c printf.c\
      panic.c debug.c debug_console.c channel.c stdlib.c timer.c bitmap.c\
			interrupt.c trap.S trap_handler.c thread/thread.c thread/timer_isr.S\
			ioapic.c

OBJ = $(subst .S,.o,$(filter %.S,$(SRC))) $(subst .c,.o,$(filter %.c,$(SRC)))

all: $(USRLIB) $(LWIPLIB)

$(LWIPLIB):
	$(call silent_command, $(MAKE) --no-print-directory -C lwip)

$(USRLIB): $(OBJ)
	$(call silent_command, $(AR) $(ARFLAGS) $@ $(OBJ), "    ARCH  $(MYDIR)$@")

-include $(subst .c,.d,$(filter %.c,$(SRC)))

.PHONY: clean
.PHONY: distclean
clean:
	$(call silent_command, rm -f *.o *.d *.a, "    CLEAN $(MYDIR)")
	$(call silent_command, $(MAKE) clean --no-print-directory -C lwip)
distclean:
	$(call silent_command, rm -f *.o *.d *.a, "    CLEAN ALL $(MYDIR)")
	$(call silent_command, $(MAKE) distclean --no-print-directory -C lwip)

