stm32 程序调试_linux_openocd
作者 qq ： 210214599
date ： Wed 14 Apr 2021 04:15:54 AM EDT
1. 硬件：
电脑系统： debian-10.8.0-amd64-xfce-CD-1
单片机： stm32f103c8t6
jlink ： j-OB V2
2. 软件：
openocd 0.10.0
sudo apt install openocd
3. openocd 用法
3.1 -f：加载配置文件
openocd -f interface/stlink.cfg -f target/st_nucleo_l4xx.cfg
board/ interface/ target/ 等都在 openocd 的 share 目录下的 script 文件夹里
我使用的配置文件：
cp /usr/share/openocd/scripts/target/stm32f1x.cfg openocd.cfg
vi openocd.cfg
开头添加如下内容：
telnet_port 4444
gdb_port 3333
interface jlink
transport select swd;
3.2 telnet
flash 下载基本操作：
telnet localhost 4444
>reset
>flash
>flash
>reset
或者
>halt
>flash
>reset
halt
erase_sector 0 0 last
write_image xxx.hex/bin/elf
//要先挂起才能开始擦除
//全部擦除
//完成
write_image erase xxx.bin/hex/elf 0
//自动擦除并从 0 地址开始
//啦啦啦快看你的板子～
下面这个操作最舒服，一条搞定：
>program xx.elf
>reset
// look your board
烧录 elf 文件和 hex 文件后面不用写擦除地址，因为它们包含有这些信息，而 bin 文件只有要执行的二进制文件，
后面要加地址项。
