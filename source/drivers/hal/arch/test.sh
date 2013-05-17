#!/bin/sh

make ARCH=arm

arm-linux-gnueabi-ld -o arm.bin arm.o ../../../libs/common/misc.o ../libs/*.o -Tarm/lds32.txt -entry=stext
cp arm.bin arm.elf
arm-linux-gnueabi-objcopy -O binary -S arm.bin

#mkimage -A arm -O linux -C none -a 0x50080000 -e 0x50080000 -n GridOS -d arm.bin gridos.bin
cp arm.bin /private/tftpboot/C0A80163.img
