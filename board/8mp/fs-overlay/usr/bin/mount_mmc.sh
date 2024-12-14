#!/bin/sh

MMC_DEV=$1
MMC_PART=$2

echo "1=$MMC_DEV 2=$MMC_PART"

if [[ x"$MMC_DEV" = x ]] || [[ x"$MMC_PART" = x ]]; then
	echo "$0 mmc_dev mmc_part"
	exit
fi

mount /dev/mmcblk${MMC_DEV}p${MMC_PART} /media
