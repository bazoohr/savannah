#!/bin/bash

#set -e

DEFAULT_OPTLVL=$(grep OPTLVL rules.mak | head -n1)

for level in 0 1 2 3 ; do
	sed -i "s/OPTLVL = -O./OPTLVL = -O${level}/1" rules.mak
	grep OPTLVL rules.mak | head -n1
	make clean
	make
	make install
	sudo make run
done

sed -i s/'OPTLVL = -O.'/"${DEFAULT_OPTLVL}"/  rules.mak
