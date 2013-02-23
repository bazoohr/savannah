# ==================================
# Makefile
#
# Hamid R. Bazoobandi
# Sat. 14 July 2012 Amsterdam
# ==================================
OPTLVL = -O3
AS = $(CC)
LD = ld
DEBUG = yes

CFLAGS  := -m64 -fno-stack-protector -fno-builtin -nostdinc -mno-red-zone -Wall -msse2 -Werror $(OPTLVL)
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
# ===================================
ifeq ($(DEBUG),yes)
CFLAGS += -D__DEBUG__
ASFLAGS += -D__DEBUG__
endif
# ===================================
ifeq ($(CC),gcc)
CFLAGS  += -ftree-vectorize #-ftree-vectorizer-verbose=5
endif
# ===================================
MYDIR =

silent_command = $(if $(2), @echo $2 && $1, @$(1))
define make_depend
	$(CC) $(CFLAGS) -MM -MP -MT $2 $1 > $3
endef

%.o: %.c
	$(call silent_command, $(call make_depend,$<,$@,$(subst .o,.d,$@)))
	$(call silent_command, $(CC) $(CFLAGS) -c -o $@ $<, "    CC    $(MYDIR)$<")
	#$(call silent_command, $(CC) $(CFLAGS) -S $<, "    SS    $(MYDIR)$<")
%.o: %.S
	$(call silent_command, $(call make_depend,$<,$@,$(subst .o,.d,$@)))
	$(call silent_command, $(AS) $(ASFLAGS) -c -o $@ $<, "    AS    $(MYDIR)$<")
