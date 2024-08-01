#!/bin/sh

insmod /lib/modules/mydrv_bt.ko 
echo 0 > /sys/devices/platform/mydrv_bt/power_enable
echo 1 > /sys/devices/platform/mydrv_bt/power_enable
brcm_patchram_plus --enable_hci --no2bytes --use_baudrate_for_download --tosleep 200000 --baudrate 1500000 --patchram /lib/firmware/brcm/BCM4345C0.hcd /dev/ttyS0 &
