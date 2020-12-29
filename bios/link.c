//--------------------- link.c ------------------------
0.//install jlink
downloads jlink5.0
cd /opt/SEGGER/JLink
./JLinkExe
0.//use jlink
usb
r
speed 12000
loadbin /home/ts/Downloads/init/init.bin 0
setpc 0
g
h
loadbin /home/ts/Downloads/tmp/vivi.bin 0x33f80000
#loadbin /home/ts/Downloads/tmp/u-boot_S35.bin 0x33f80000
setpc 0x33f80000
g

//--------------------- end ---------------------------
