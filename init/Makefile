ROOT = ..

include $(ROOT)/rules.mak

MYDIR += init/

LINK_LD = link64.ld

CFLAGS  += -I $(ROOT)/lib/include -I $(ROOT)/include/ 
LDFLAGS += -n -T $(LINK_LD)
LIBDIR = $(ROOT)/lib/

SRC = main.c start_off.S

OBJ = $(subst .S,.o,$(filter %.S,$(SRC))) $(subst .c,.o,$(filter %.c,$(SRC)))

$(INIT): $(OBJ)
	$(call silent_command, $(LD) $(LDFLAGS) -o $@ $(OBJ) $(LIBDIR)/$(USRLIB), "    LINK  $(MYDIR)$@")

-include $(subst .c,.d,$(filter %.c,$(SRC)))


.PHONY: clean
.PHONY: distclean
clean:
	$(call silent_command, rm -f *.o *.d $(INIT), "    CLEAN $(MYDIR)")
distclean:
	$(call silent_command, rm -f *.o *.d $(INIT), "    CLEAN ALL $(MYDIR)")
