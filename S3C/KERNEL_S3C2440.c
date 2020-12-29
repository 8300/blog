//---------------------KERNEL_S3C2440------------------
//---------------------uImage
make ARCH=arm CROSS_COMPILE=arm-linux- mini2440_defconfig
make ARCH=arm CROSS_COMPILE=arm-linux- menuconfig
make ARCH=arm CROSS_COMPILE=arm-linux- uImage
linux-5.1.15
//---------------------END-----------------------------