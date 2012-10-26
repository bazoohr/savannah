#!/bin/sh
# ==================================
# install.sh
#
# Hamid R. Bazoobandi 
# Fri 13.7.2012 Amsterdam
# ===================================
loader=./boot/loader.bin    # Our loader that loads kernel into memory
kernel=./kernel/kernel.bin  # Our actual kernel
init=./init/init            # Out init process
grub_stage_loc=./etc        # Location of Grub stage files! 
iso_tmp_dir=/tmp/iso        # temp location for iso directory
# ===================================
kernel_title="My Kernel"  # Title of our kernel in Grub menu
menu_file="menu.lst"      # grub menu
compiler="gcc"            # We're using gcc as default compiler
image_name="vuos.iso"     # We're using gcc as default compiler
show_help="no"            # Don't show help by default
# ===================================
die() {
  echo $1 1>&2
  exit 1;
}
# ===================================
# Check options
for opt do
  optarg=`expr "x$opt" : 'x[^=]*=\(.*\)'`
  case $opt in
  --help|-h) show_help="yes"
  ;;
  --compiler=*) 
    if test "$optarg" != "gcc" && test "$optarg" != "clang"; then
      echo "Compiler must be either \"gcc\" or \"clang\".";
      exit 1;
    fi
    compiler="$optarg"
  ;;
  --image-name=*) image_name="$optarg"
  ;;
  *) echo "ERROR: Unknown option $opt"; show_help="yes"
  ;;
  esac
done
if test "$show_help" = "yes"; then
  echo "Usage: install.sh [options]"
  echo
  echo "Options:"
  echo "--help           Print this message"
  echo "--compiler       Change default compiler. Default compiler is \"gcc\"."
  echo "--image-name     Change default image name. Default image is \"vuos.iso\"."
  exit 1;
fi
# if the given file name is a directory
if [ -d $image_name ]; then
  die "Kernel image can not be a directory!"
fi
# make sure given file exists && is a regular file
if [ -e $image_name -a ! -f $image_name ]; then
  die "File $image_name is not a regular file!"
fi
# Well! Seems intended file is OK to continue
echo "Running configure..."
if ! echo "$compiler" | ./configure; then
  die "Configuration failed!"
fi
echo "Running configure... OK!"
echo "========================================"

#echo "Copying splash image into the /boot/grub/image directory"
#if ! mcopy hubble.xpm.gz c:/boot/grub/images; then
#  echo "failed to copy splash image into /boot/grub/image"
#  exit 1;
#fi

echo "Compiling System..."
make clean
if ! make; then
  die "Compilation Failed!"
fi
echo "Compiling System... OK!"

echo "========================================"
echo "Creating grub menu..."
cat > $menu_file <<EOF
serial --unit =0 --stop=1 --speed=115200 --parity=no --word=8
terminal --timeout=0 serial console
# ==========================
default 0
timeout = 0
# ==========================
title $kernel_title
   # It should be a loader
   kernel --type=multiboot /boot/grub/`basename $loader`
   # We load 64-bit kernel as a module
   module /boot/grub/`basename $init`
   module /boot/grub/`basename $kernel`
EOF
echo "Creating grub menu... OK!"
echo "========================================"

if [ -e $iso_tmp_dir ]; then
  rm -rf iso_tmp_dir
fi
mkdir -p $iso_tmp_dir/boot/grub

echo "Copying grub stages into image..."
if ! cp $grub_stage_loc/stage2_eltorito $iso_tmp_dir/boot/grub; then
  die "failed to copy stages into image!"
fi
echo "Copying grub stages into image... OK!"
echo "========================================"

echo "Copying loader, kernel, and init into image..."
if ! cp $menu_file $iso_tmp_dir/boot/grub; then
  die "failed to copy $menu_file into image"
fi

if ! cp $loader $iso_tmp_dir/boot/grub; then
  die "Failed to copy loader into image!"
fi

if ! cp $kernel $iso_tmp_dir/boot/grub; then
  die "Failed to copy kernel into image!"
fi
if ! cp $init $iso_tmp_dir/boot/grub; then
  die "Failed to copy kernel into image!"
fi
rm -f $menu_file
echo "Copying loader, kernel, and init into image... OK!"

echo "========================================"
echo "Creating iso image..."
if ! genisoimage -quiet -input-charset ascii -R -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 \
             -boot-info-table -o $image_name $iso_tmp_dir; then
  die "Couldn't create iso image!"
fi
rm -rf $iso_tmp_dir
echo "Creating iso image... OK!"
echo "========================================"

echo "System installed  successfully!"
