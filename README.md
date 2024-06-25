
1、编译
1)
使用s5pv210板子
make 210_defconfig
使用rk3288板子
make 3288_defconfig
2)
make source
make V=1

2、使用tftp启动，方便调试
cp output/images/rootfs.squashfs ./../tftp
