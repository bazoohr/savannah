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
	$(call silent_command, $(MAKE) -s --no-print-directory -C tools)
	$(call silent_command, $(MAKE) -s --no-print-directory -C lib   )
	$(call silent_command, $(MAKE) -s --no-print-directory -C drivers)
	$(call silent_command, $(MAKE) -s --no-print-directory -C bin)
	$(call silent_command, $(MAKE) -s --no-print-directory -C initrd)
	$(call silent_command, $(MAKE) -s --no-print-directory -C boot  )
	$(call silent_command, $(MAKE) -s --no-print-directory -C vmm)
	$(call silent_command, $(MAKE) -s --no-print-directory -C vms)
	@echo "    Built Successfully!"

install: build-all
	@echo "    Installing..."
#	@printf	"serial --unit =0 --stop=1 --speed=115200 --parity=no --word=8\n\
#		terminal --timeout=0 serial console\n\
#		timeout = 20\n\
#		title \"Anarchix\"\n\
#		root (cd)\n\
#		kernel --type=multiboot /boot/grub/$(BOOT_STAGE1) root=cd\n\
#		module /boot/grub/$(BOOT_STAGE2)\n\
#		module /boot/grub/$(BOOT_APS)\n\
#		module /boot/grub/$(VMM)\n\
#		module /boot/grub/$(PM)\n\
#		module /boot/grub/$(FS)\n\
#		module /boot/grub/$(INIT)\n\
#		module /boot/grub/$(INITRD)\n" > /tmp/menu.lst
	@printf "timeout 0\n\
	  title vuos\n\
		kernel --type=multiboot /boot/grub/$(BOOT_STAGE1)\n\
		module /boot/grub/$(BOOT_STAGE2)\n\
		module /boot/grub/$(BOOT_APS)\n\
		module /boot/grub/$(VMM)\n\
		module /boot/grub/$(PM)\n\
		module /boot/grub/$(FS)\n\
		module /boot/grub/$(RS)\n\
		module /boot/grub/$(INIT)\n\
		module /boot/grub/$(INITRD)\n" > /tmp/menu.lst
	@rm -rf /tmp/iso
	@rm -f $(IMAGE)
	@mkdir -p /tmp/iso/boot/grub
	@cp tools/stage2_eltorito /tmp/iso/boot/grub
	@cp /tmp/menu.lst /tmp/iso/boot/grub
	@rm -f /tmp/menu.lst
	@cp boot/stage1/$(BOOT_STAGE1) /tmp/iso/boot/grub
	@cp boot/stage2/$(BOOT_STAGE2) /tmp/iso/boot/grub
	@cp boot/stage2/$(BOOT_APS) /tmp/iso/boot/grub
	@cp vmm/$(VMM) /tmp/iso/boot/grub
	@cp vms/pm/$(PM) /tmp/iso/boot/grub
	@cp vms/fs/$(FS) /tmp/iso/boot/grub
	@cp vms/rs/$(RS) /tmp/iso/boot/grub
	@cp vms/init/$(INIT) /tmp/iso/boot/grub
	@cp initrd/$(INITRD) /tmp/iso/boot/grub
	@genisoimage -quiet -input-charset ascii -R -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 \
		-boot-info-table -o $(IMAGE) /tmp/iso
	@echo "    Installing... OK"
run:
	@bochs -q -f bochsrc.txt
clean:
	$(call silent_command, rm -f *.o *.d *.bin, "    CLEAN")
	$(call silent_command, rm -f $(IMAGE))
	$(call silent_command, $(MAKE) clean --no-print-directory -C tools)
	$(call silent_command, $(MAKE) clean --no-print-directory -C lib   )
	$(call silent_command, $(MAKE) clean --no-print-directory -C drivers)
	$(call silent_command, $(MAKE) clean --no-print-directory -C bin)
	$(call silent_command, $(MAKE) clean --no-print-directory -C initrd)
	$(call silent_command, $(MAKE) clean --no-print-directory -C boot  )
	$(call silent_command, $(MAKE) clean --no-print-directory -C vmm)
	$(call silent_command, $(MAKE) clean --no-print-directory -C vms)
distclean:
	$(call silent_command, rm -f *.o *.d *.bin, "    CLEAN ALL")
	$(call silent_command, rm -f $(IMAGE))
	$(call silent_command, $(MAKE) distclean --no-print-directory -C tools)
	$(call silent_command, $(MAKE) distclean --no-print-directory -C lib     )
	$(call silent_command, $(MAKE) distclean --no-print-directory -C drivers  )
	$(call silent_command, $(MAKE) distclean --no-print-directory -C bin  )
	$(call silent_command, $(MAKE) distclean --no-print-directory -C initrd)
	$(call silent_command, $(MAKE) distclean --no-print-directory -C boot    )
	$(call silent_command, $(MAKE) distclean --no-print-directory -C vmm  )
	$(call silent_command, $(MAKE) distclean --no-print-directory -C vms  )
	$(call silent_command, rm -f $(CONFIG-MAK))
