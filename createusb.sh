#!/bin/sh -e

if [ "$(id -u)" != "0" ] ; then
	echo "This script must be run as root"
	exit 1
fi

GRUB_INSTALL=grub-install
DEV=sdb
TMPMNT=/tmp/mnt
ISO=/tmp/iso

mkfs.ext2 /dev/${DEV}1

mkdir -p $TMPMNT
mount /dev/${DEV}1 $TMPMNT

${GRUB_INSTALL} --no-floppy --root-directory=/tmp/mnt /dev/${DEV}

cat << EOF > ${ISO}/boot/grub/grub.cfg
menuentry "vuos" {
  insmod legacycfg
  legacy_configfile /boot/grub/menu.lst
}
EOF

cp ${ISO}/boot/grub/* ${TMPMNT}/boot/grub

umount ${TMPMNT}
