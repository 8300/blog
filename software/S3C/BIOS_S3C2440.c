//---------------------BIOS_S3C2440.c------------------
//---------------------arm-linux-gcc
download arm-linux-gcc.tar
sudo vi /etc/profile
export PATH=$PATH:~/Downloads/tmp/usr/local/arm/4.3.2/bin
log out
 sudo ./Jflash-s3c2440 u-boot /t=5
//---------------------END-----------------------------