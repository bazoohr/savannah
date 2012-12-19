# ==================================
# Makefile
#
# Hamid R. Bazoobandi
# Sat. 14 July 2012 Amsterdam
# ==================================
OPTLVL = -O3
AS = $(CC)
LD = ld

CFLAGS  := -m64 -fno-stack-protector -fno-builtin -nostdinc -mno-red-zone -Wall -Werror $(OPTLVL)
ASFLAGS := -m64 -Wall -D__ASSEMBLY__
LDFLAGS :=

# Since clang integrated as does not support .code16 YET ;)
# and we have no clue when they plan to support it, then we
# have to FORCE it not to use its integrated AS.
#
# BUT
# GCC does not understand this option. So we have to check to see
# which compiler are we using.
ifeq ($(CC),clang)
ASFLAGS += -no-integrated-as
endif
# Because of some strange reason, in gcc version 4.6.3
# in optimization level 3 (O3), gcc parallelizes some addition
# operations using mmx instructions, which cause #GP. It does not
# seem to be a problem of gcc, because the same code was 
# working before, and moreover we checked the manual of AMD64 and
# we do all what we have to do. Therefore we prefer to disable this
# feature for gcc at the moment. We try to enable it later, probably
# when this bug is fixed.
#ifeq ($(CC), gcc)
#CFLAGS += -mno-sse
#endif

MYDIR =

silent_command = $(if $(2), @echo $2 && $1, @$(1))
define make_depend 
	$(CC) $(CFLAGS) -MM -MP -MT $2 $1 > $3
endef

%.o: %.c
	$(call silent_command, $(call make_depend,$<,$@,$(subst .o,.d,$@)))
	$(call silent_command, $(CC) $(CFLAGS) -c -o $@ $<, "    CC    $(MYDIR)$<")
	$(call silent_command, $(CC) $(CFLAGS) -S $<)
%.o: %.S
	$(call silent_command, $(call make_depend,$<,$@,$(subst .o,.d,$@)))
	$(call silent_command, $(AS) $(ASFLAGS) -c -o $@ $<, "    AS    $(MYDIR)$<")
