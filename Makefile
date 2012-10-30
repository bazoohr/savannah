# ====================================
# Makefile
#
# Hamid R. Bazoobandi
# Fri 13.7.2012
# ====================================
ifneq ($(wildcard config.mak),)
.PHONY: all build-all clean distclean install
# Put the all: rule here so that config.mak can contain dependencies.
all: build-all
include config.mak
include rules.mak
config.mak: configure
	@echo $@ is out-of-date!
	@echo "please rerun configure and then try again."
	@exit 1;
else
config.mak:
	@echo "Please call configure before running make!"
	@exit 1
endif

build-all: 
	$(call silent_command, $(MAKE) -s --no-print-directory -C etc   )
	$(call silent_command, $(MAKE) -s --no-print-directory -C kernel)
	$(call silent_command, $(MAKE) -s --no-print-directory -C boot  )
	$(call silent_command, $(MAKE) -s --no-print-directory -C lib   )
	@echo "    Built Successfully!"

#
# TODO:
# Repalce all boot_aps with appropriate variable name
#
install: build-all
	@echo "    Installing..."
	@printf	"serial --unit =0 --stop=1 --speed=115200 --parity=no --word=8\n\
		terminal --timeout=0 serial console\n\
		default 0\n\
		timeout = 0\n\
		title \"My Kernel\"\n\
		kernel --type=multiboot /boot/grub/$(LOADER)\n\
		module /boot/grub/boot_aps\n\
		module /boot/grub/$(KERNEL)\n" > /tmp/menu.lst
	@rm -rf /tmp/iso
	@rm -f $(IMAGE)
	@mkdir -p /tmp/iso/boot/grub
	@cp etc/stage2_eltorito /tmp/iso/boot/grub
	@cp /tmp/menu.lst /tmp/iso/boot/grub
	@rm -f /tmp/menu.lst
	@cp boot/$(LOADER) /tmp/iso/boot/grub
	@cp boot/boot_aps /tmp/iso/boot/grub
	@cp kernel/$(KERNEL) /tmp/iso/boot/grub
	@genisoimage -quiet -input-charset ascii -R -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 \
             -boot-info-table -o $(IMAGE) /tmp/iso
	@echo "    Installing... OK"
clean:
	$(call silent_command, rm -f *.o *.d *.bin, "    CLEAN")
	$(call silent_command, rm -f $(IMAGE))
	$(call silent_command, $(MAKE) clean --no-print-directory -C etc   )
	$(call silent_command, $(MAKE) clean --no-print-directory -C boot  )
	$(call silent_command, $(MAKE) clean --no-print-directory -C kernel)
	$(call silent_command, $(MAKE) clean --no-print-directory -C lib   )
distclean:
	$(call silent_command, rm -f *.o $(CONFIG-MAK) *.d *.bin, "    CLEAN ALL")
	$(call silent_command, rm -f $(IMAGE))
	$(call silent_command, $(MAKE) distclean --no-print-directory -C etc     )
	$(call silent_command, $(MAKE) distclean --no-print-directory -C boot    )
	$(call silent_command, $(MAKE) distclean --no-print-directory -C kernel  )
	$(call silent_command, $(MAKE) distclean --no-print-directory -C lib     )
