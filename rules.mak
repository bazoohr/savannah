# ==================================
# Makefile
#
# Hamid R. Bazoobandi
# Sat. 14 July 2012 Amsterdam
# ==================================
OPTLVL = -O0
AS = $(CC)
LD = ld

CFLAGS  := -m64 -fno-stack-protector -fno-builtin -nostdinc -mno-red-zone -Wall -Werror $(OPTLVL)
ASFLAGS := -m64 -Wall -D__ASSEMBLY__
LDFLAGS :=

MYDIR =

silent_command = $(if $(2), @echo $2 && $1, @$(1))
define make_depend 
	$(CC) $(CFLAGS) -MM -MP -MT $2 $1 > $3
endef

%.o: %.c
	$(call silent_command, $(call make_depend,$<,$@,$(subst .o,.d,$@)))
	$(call silent_command, $(CC) $(CFLAGS) -c -o $@ $<, "    CC    $(MYDIR)$<")
%.o: %.S
	$(call silent_command, $(call make_depend,$<,$@,$(subst .o,.d,$@)))
	$(call silent_command, $(AS) $(ASFLAGS) -c -o $@ $<, "    AS    $(MYDIR)$<")
