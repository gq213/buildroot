
1、编译

make 3288_defconfig

make source

make V=1


2、烧录

烧录output/images/rootfs.squashfs到Micro SD卡的第1分区

sudo dd if=rootfs.squashfs of=/dev/sdb1
