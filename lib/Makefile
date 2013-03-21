ROOT = ..

include $(ROOT)/rules.mak

MYDIR = lib/

CFLAGS  += -I ./include/ -I $(ROOT)/include/
ASFLAGS += -I ./include/ -I $(ROOT)/include/

AR=ar
ARFLAGS=cr

SRC = ipc.c misc.c string.c fs.c pm.c asm_lib.S lib_mem.c gdt.c printf.c\
      panic.c debug.c debug_console.c channel.c stdlib.c timer.c bitmap.c\
			timer_isr.S interrupt.c trap.S trap_handler.c

OBJ = $(subst .S,.o,$(filter %.S,$(SRC))) $(subst .c,.o,$(filter %.c,$(SRC)))

$(USRLIB): $(OBJ)
	$(call silent_command, $(AR) $(ARFLAGS) $@ $(OBJ), "    ARCH  $(MYDIR)$@")

-include $(subst .c,.d,$(filter %.c,$(SRC)))

.PHONY: clean
.PHONY: distclean
clean:
	$(call silent_command, rm -f *.o *.d *.a, "    CLEAN $(MYDIR)")
distclean:
	$(call silent_command, rm -f *.o *.d *.a, "    CLEAN ALL $(MYDIR)")

