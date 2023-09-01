#!/bin/sh

bootloader_image="/opt/firmware/bootloader/u-boot-sunxi-with-spl.bin"

dd if=${bootloader_image} of=/dev/mmcblk1 bs=1024 seek=8 conv=notrunc

if [ $? -ne 0 ]; then
	echo "failed to update bootloader"
	exit 1
else
	echo "bootloader was updated successfully."
	exit 0
fi
