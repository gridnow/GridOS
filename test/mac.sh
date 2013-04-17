if [ ! -d /Volumes/TEST ]; then
	echo "Your TEST volume not mounted."
	exit 1
fi

cp -r ../release/ /Volumes/TEST/

qemu-system-i386 -L Bios -m 256  -boot c -usb -drive id=disk,file=hd0.img,if=none -device ahci,id=ahci -device ide-hd,drive=disk,bus=ahci.0  -localtime -soundhw ac97 -net nic,model=rtl8139
