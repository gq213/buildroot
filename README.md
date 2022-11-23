
1、编译

使用s5pv210板子

make 210_defconfig

使用rk3288板子

make 3288_defconfig

make source

make V=1


2、烧录

烧录output/images/rootfs.squashfs到Micro SD卡的第1分区

sudo dd if=rootfs.squashfs of=/dev/sdb1
