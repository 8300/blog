//---------------------kernel.c------------------------
//---------------------arm-linu-gcc
sudo apt install gcc-arm-linux-gnueabi
sudo ln -s /usr/bin/arm-linux-gnueabi-gcc /usr/bin/arm-linux-gcc
sudo ln -s /usr/bin/arm-linux-gnueabi-ar /usr/bin/arm-linux-ar
sudo ln -s /usr/bin/arm-linux-gnueabi-ld /usr/bin/arm-linux-ld
sudo ln -s /usr/bin/arm-linux-gnueabi-nm /usr/bin/arm-linux-nm
sudo ln -s /usr/bin/arm-linux-gnueabi-strip /usr/bin/arm-linux-strip
sudo ln -s /usr/bin/arm-linux-gnueabi-objdump /usr/bin/arm-linux-objdump
sudo ln -s /usr/bin/arm-linux-gnueabi-objcopy /usr/bin/arm-linux-objcopy
//---------------------busybox

//
内核能够识别的所有设备都记录在原码树下的 Documentation/devices.txt 文件中
sudo mknod null c 1 3
sudo mknod console c 5 1
sudo mknod video0 c 81 0
sudo mknod i2c-0 c 89 0
sudo mknod i2c-0 c 89 1
sudo mknod i2c-1 c 89 1
sudo mknod mtd0 c 90 0
sudo mknod mtdr0 c 90 1
sudo mknod spi0 c 153 0
sudo mknod spi1 c 153 1
sudo mknod zero c 1 5
//
cd busybox-*
vi Makefile
【
ARCH    ?= arm
CROSS_COMPILE   ?= arm-linux-
】
:wq
make menuconfig
make
mkdir ../fs
make CONFIG_PREFIX=../fs install
cd ../fs
mkdir dev etc home lib media mnt opt proc sys tmp var root
#/lib
cd bin
arm-linux-readelf -d busybox
cd /usr/arm-linux-gnueabi/lib
ls -l libm.so.6
shell:libm.so.6 -> libm-2.27.so
ls -l libresolv.so.2
shell:libc.so.6 -> libc-2.27.so
ls -l ld-linux.so.3
shell:ld-linux.so.3 -> ld-2.27.so
cp libdl.so.2 libdl-2.27.so libcrypt.so.1 libcrypt-2.27.so libm.so.6 libm-2.27.so libresolv.so.2 libresolv-2.27.so ld-linux.so.3 ld-2.27.so libpthread.so fs/lib
#/etc
cd fs/etc
cp -r busybox*/examples/* .
//*/
vi init.d/rcS
#!/bin/sh

PATH=/sbin:/bin:/usr/sbin:/usr/bin
export PATH

#
#       Trap CTRL-C &c only in this shell so we can interrupt subprocesses.
#

mount -a
mkdir -p /dev/pts
mount -t devpts devpts /dev/pts
echo /sbin/mdev > /proc/sys/kernel/hotplug
mdev -s
mkdir -p /var/lock

hwclock -s
feed_wdg &

ifconfig lo 127.0.0.1
ifconfig eth0 192.168.2.99

/bin/hostname -F /etc/HOSTNAME
:wq
vi HOSTNAME
TS
:wq
vi fstab
#device         mount-point     type    options         dump    fsck order
proc            /proc           proc    defaults                0       0
tmpfs           /tmp            tmpfs   defaults                0       0
sysfs           /sys            sysfs   defaults                0       0
tmpfs           /dev            tmpfs   defaults                0       0
var             /dev            tmpfs   defaults                0       0
ramfs           /dev            ramfs   defaults                0       0
:wq
vi profile
USER="`id -un`"
LOGNAME=
USER PS1='[\u@\h \W]# '      # ???????????????????????[root@Blue etc]# PATH=
PATH

HOSTNAME=`/bin/hostname`

export USER LOGNAME PS1 PATH
:wq
vi group
root:x:0:root
:wq
vi passwd
root::0:0:root:/:/bin/sh
:wq
#/dev
mknod console c 5 1
mknod null c 1 3
#jffs2
mkfs.jffs2 -n -s 512 -e 16KiB -d fs -o fs.jffs2 --pad=0xa00000
//mkfs.jffs2 -n -s 2048 -e 128KiB -d fs -o fs-jffs2.bin --pad=0xa00000
//mkfs.jffs2 -d fs -l -e 0x10000 -o fs.jffs2 
//          -d ??  -l  ???? , -e ??????? . -o ???????
#error
//Empty flash at 0x012dbffc ends at 0x012dc000
page size
各参数的意义：
(1)-r ：指定根文件系统目录.
(2)-o : 指定输出的根文件系统名.
(3)-s : 指定flash每一页的大小;
(3)-e : 指定flash的檫除块的大小，預設是64KB.
要注意，不同的flash, 其page size和block size會不一樣.，如果是K9F2G08U0M,在它的datasheet上有说明：
1 Page = (2K+64)Bytes
1 Block = (2K+64)Bytes = (128K+4K) Bytes
1 Device = (2K+64)B * 64Pages * 2048 Blocks = 2112 Mbits 
所以上面指定:"-s 2048 -e 128KiB"。
如果是K9F1208U0C，在它的datasheet上有说明：
1 Page = 528 Bytes
1 Block = 528 Bytes * 32 Page = (16K+512) Bytes
1 Device = 528 Bytes * 32 Pages * 4096 Blocks = 528 Mbits
则上面指定: "-s 512 -e 16KiB"
//Cowardly refusing to erase blocks on filesystem with no valid JFFS2 nodes
supervivi:
part del root 
part add root 0x00560000 0xa00000 0
part save
//CLEANMARKERnode found at 0x0042c000 has totlen 0xc != normal 0x0  的警告，则加上 -n 就会消失。
//返回的错误代码，在Linux源码目录/include/uapi/asm-generic的errno-base.h中定义
4.could not run '/etc/init.d/rcS': Permission denied
在编译主机上把权限改掉：chmod -R 777 *

//---------------------kernel
sudo ifconfig enp0s31f6 192.168.1.110 netmask 255.255.255.0
mtdparts=nandflash0:256k@0(bios),128k(params),5m(kernel),-(root)
sudo apt install flex bison libssl-dev
vi Makefile
【
ARCH    ?= arm
CROSS_COMPILE   ?= arm-linux-
】
make mini2440_defconfig
make menuconfig
//make ARCH=arm CROSS_COMPILE=arm-linux- LOADADDR=0x80008000 uImage
//make -j8
1.make zImage//viv
2.make uImage//uboot
cd secbulk
sudo insmod secbulk.ko
#cutting
ipv6
wireless
mouse
touchscreen

#term
RCU	Read-Copy Update//“随意读，但更新数据的时候，需要先复制一份副本，在副本上完成修改，再一次性地替换旧数据”。这是 Linux 内核实现的一种针对“读多写少”的共享数据的同步机制。
#error
//
NickBoot> setenv mtdparts mtdparts=nandflash0:256k@0(bios),128k(params),5m(kernel),-(root)
NickBoot> setenv bootargs noinitrd root=/dev/mtdblock3 rw init=/linuxrc console=ttySAC0,115200 rootfstype=jffs2
NickBoot> saveenv
NickBoot> tftp 30008000 uImage

//make: *** arch/arm: Is a directory. Stop.
Makefile中ARCH=arm这一句后面多了两个空格
//
grep -rn run_init_process
nit/main.c:1093:	if (!try_to_run_init_process("/sbin/init") ||

//NOW, Booting Linux..
Boot options  --->
() Default kernel command string  --->
填入以下内核启动参数：
noinitrd root=/dev/mtdblock3 rw init=/linuxrc console=ttySAC0,115200 rootfstype=jffs2
//noinitrd root=/dev/mtdblock3 rw init=/linuxrc console=ttySAC0,115200
如果改为以下启动参数，可同时将内核启动信息打印到串口和LCD显示屏
//noinitrd root=/dev/mtdblock3 rw init=/linuxrc console=ttySAC0,115200 console=tty0
启动参数中rw不可忽略，否则可能造成根文件系统挂载完成后无法进入系统。我遇到过不填写rw时，yaffs2根文件系统被挂载为ro（只读），导致无法进入系统
其他配置可先不修改，直接退出保存
//undefined reference to `stdscr'`wmove'`acs_map' `waddch' `waddnstr'`wmove' `acs_map'
解决办法：
sudo apt-get install libncurses5-dev
//error verifying int64_t uses long long
错误是因为没有装g++ 导致，安装g++可解决
//multiple (or no) load addresses
关闭：Boot option--->
    Use appended device tree blob to zImage
//启动文件系统时Kernel panic - not syncing: Attempted to kill init!的错误，原因arm-linux-gcc交叉编译器为4.4.3需要在linux kernel配置里面增加EABI的支持。

Kernel Features  --->
            [*] Use the ARM EABI to compile the kernel                   
            [*]   Allow old ABI binaries to run with this kernel (EXPERIMENTA)
//No filesystem could mount root, tried: ext2 cramfs vfat msdos romfs
./patch-ker.sh c m your-linux-kernel-directory
File system--->[*]Miscellaneous filesystems--->Yaffs2 file system support

Supervivi> param set linux_cmd_line "console=ttySAC0 root=/dev/nfs nfsroot=192.168.1.111:/opt/FriendlyARM/mini2440/rootfs_qtopia_qt4 ip=192.168.1.70:192.168.1.111:192.168.1.111:255.255.255.0:sbc2440.arm9.net:eth0:off"
//cc1: error: unrecognized command line option "-m64"
cc1: error: unrecognized command line option "-mno-red-zone"
cc1: error: unrecognized command line option "-mcmodel=kernel"
cc1: error: unrecognized command line option "-maccumulate-outgoing-args"修改makefile arch=arm
//Can't use 'defined(@array)' (Maybe you should just omit the defined()?)
Can't use '!defined(@array)' (Maybe you should just omit the defined()?) at kernel/timeconst.pl line 373.
其实，提示的错误信息已经明确告诉你了，你应该省略defined().
这里，我们打开 kernel/timeconst.pl
@val = @{$canned_values{$hz}};
if (!defined(@val)) {
@val = compute_values($hz);
}
output($hz, @val);
将if (!defined(@val)) 改为if (!(@val))，再次编译就可以通过了。
查了一下更新，发现其中有一项是perl版本升级到了 v5.22.1，然后查了perl官方文档，发现官网因为一个bug，该版本将defined(@array)去掉了。可以直接使用数组判断非空。
// implicit declaration of function 'irq_to_gpio' 
编辑drivers/mfd/ezx-pcap.c 这个文件
在应该是205这一行做如下替换

} while (gpio_get_value(irq_to_gpio(pcap->spi->irq)));
 
-->} while (gpio_get_value(pxa_irq_to_gpio(pcap->spi->irq)));

然后需要在在预编译部分#include <linux/gpio.h>下面添加包含gpio-pxa.h

#include <linux/gpio.h>
#include <linux/gpio-pxa.h>
//16%无法下载，更改虚拟机CPU个数，大于1  //原因应该是cpu和多线程
#error---end---------
//----------------------jffs2--------------------------------
这个链接 http://catb.org/~esr/faqs/smart-questions.html是一篇关于如何在互联网上寻求帮助的很好的文章
//-------------------make menuconfig-----------------
#
Linux/arm 5.0.2 Kernel Configuration
----/----/----/----General setup-------------
Compile also drivers which will not load    //同时编译不加载的驱动程序
Local version - append to kernel release    //在内核版本后面加自定义字符
Automatically append version information to the version string  //自动生成版本信息
Build ID Salt//SALT值属于随机值。用户注册时，系统用来和用户密码进行组合而生成的随机数值，称作salt值，通称为加盐值。
kernel compression mode (Gzip) --->内核镜像要用的压缩模式
((none))Default hostname
Support for paging of anonymous memory (swap)//Linux中的虚拟内存即SWAP分区
System V IPC//System V进程间通信（IPC）支持，许多程序需要这个功能@@@@@@@@@@
POSIX Message Queues//这是POSIX的消息队列，它同样是一种IPC。建议你最好将它选上。
Enable process_vm_readv/writev syscalls//
uselib syscall
Auditing support
IRQ subsystem --->
Timers subsystem --->
Preemption Model (No Forced Preemption(Server)) --->
{
No Forced Preemption (Server)，这种模式等同于没有使能抢占选项的标准内核，主要适用于科学计算等服务器环境。
Voluntary Kernel Preemption (Desktop)，这种模式使能了自愿抢占，但仍然失效抢占内核选项，它通过增加抢占点缩减了抢占延迟，因此适用于一些需要较好的响应性的环境，如桌面环境，当然这种好的响应性是以牺牲一些吞吐率为代价的。
Preemptible Kernel (Low-Latency Desktop)，这种模式既包含了自愿抢占，又使能了可抢占内核选项，因此有很好的响应延迟，实际上在一定程度上已经达到了软实时性。它主要适用于桌面和一些嵌入式系统，但是吞吐率比模式2更低。
}
CPU/Task time and stats accounting --->
CPU isolation
RCU Subsystem --->//一个高性能的锁机制RCU 子系统
Kernel .config support//将.config配置信息保存在内核中，选上它及它的子项使得其它用户能从/proc中得到内核的配置,选上，重新配置内核时可以利用已有配置
Kernel log buffer size (16=> 64KB, 17=>128KB)//内核日志缓存的大小
CPU kernel log buffer size contribution(13=>8KB,17=>128KB)
Temporary per-CPU printk log buffer size (12=>4KB,13=>8KB)
Control Group support --->
Namespaces support --->//命名空间支持，允许服务器为不同的用户信息提供不 同的用户名空间服务
Checkpoint/restore support
Automatic process group scheduling
Enable deprecated sysfs features to support old userspace tools//在某些文件系统上(比如debugfs)提供从内核空间向用户空间传递大量数据的接口
Kernel->user space relay support (formerly relayfs)
Initial RAM filesystem and RAM disk (initramfs/initrd) support//初始RAM的文件和RAM磁盘（ initramfs /initrd）支持（如果要采用initrd启动则要选择，否则可以不选),不需要，不用选。
Initramfs source file(s)
Support initial ramdisk/ramfs compressed using gzip 
Support initial ramdisk/ramfs compressed using bzip2
Support initial ramdisk/ramfs compressed using LZMA
Support initial ramdisk/ramfs compressed using XZ
Support initial ramdisk/ramfs compressed using LZO
Support initial ramdisk/ramfs compressed using LZ4
Compiler optimization level (Optimize for performance)  --->  
Configure standard kernel features (expert users)  ---> //这是为了编译某些特殊的内核使用的，例如引导盘，通常你可以不选择这一选项，你也不用对它下面的子项操心了
Enable bpf() system call
Permanently enable BPF JIT and remove BPF interpreter
Enable userfaultfd() system call 
Enable rseq() system call (NEW)  
 Enabled debugging of rseq() system call (NEW)
Embedded system
PC/104 support
Kernel Performance Events And Counters  --->     
Enable VM event counters for /proc/vmstat   
Enable SLUB debugging support  
Enable memcg SLUB sysfs support by default
Disable heap randomization//禁用随机heap（heap堆是一个应用层的概念，即堆对CPU是不可见的，它的实现方式有多种，可以由OS 实现，也可以由运行库实现,如果你愿意，你也可以在一个栈中来实现一个堆），选不选均可。
Choose SLAB allocator (SLUB (Unqueued Allocator))  --->//选择内存分配管 理器（强烈推荐使用SLUB）     
Allow slab caches to be merged  
 SLAB freelist randomization 
Harden slab freelist metadata 
SLUB per cpu partial cache  
Profiling support //剖面支持，用一个工具来扫描和提供计算机的剖面图。支持系统评测（对于大多数用户来说并不是必须的）
---------------System Type  -------------
 MMU-based Paged Memory Management Support  
 ARM system type (Allow multiple platforms to be selected)  --->    
Multiple platform selection  --->  
 Dummy Virtual Machine (NEW)      
Actions Semi SoCs (NEW)  ----  
Annapurna Labs Alpine platform (NEW)   
---------------Bus support  -------------

---------------Kernel Features-----------
---------------Boot options -------------

---------------CPU Power Management------

---------------Floating point emulation--

---------------ARM Accelerated Cryptographic Algorithms

---------------Virtuallization-----------

---------------General architecture-dependent options

---------------Enable the block layer----

---------------IO Schedulers-------------

---------------Executable file formats---
---------------Memory Management options-

---------------Networking support--------

---------------Device Drivers------------
[*] PCI support  ---> 
 <M> PCCard (PCMCIA/CardBus) support  --->
<*> RapidIO support  --->  
   Bus devices  --->     
{*} Connector - unified userspace <-> kernelspace linker  --->   
 < > GNSS receiver support  ----   
<M> Memory Technology Device (MTD) support  --->
-*- Device Tree and Open Firmware support  ---> 
 <M> Parallel port support  --->  
[*] Block devices  ---> 
    NVME Support  ---> 
    Misc devices  --->  
< > ATA/ATAPI/MFM/RLL support (DEPRECATED)  ----  
    SCSI device support  --->     
<*> Serial ATA and Parallel ATA drivers (libata)  ---> 
[*] Multiple devices driver support (RAID and LVM)  ---> 
<M> Generic Target Core Mod (TCM) and ConfigFS Infrastructure  ---> 
[*] Fusion MPT device support  ---> 

    IEEE 1394 (FireWire) support  --->       
 -*- Network device support  --->    
[*] Open-Channel SSD target support  --->     
    Input device support  --->     
    Character devices  ---> 
    I2C support  ---> 
< > I3C support  ----    
[*] SPI support  --->  
 <M> SPMI support  ----    
  
---------------File systems -------------

---------------Security options----------

---------------Cryprographic API---------
---------------Library routines----------

---------------Kernel hacking------------
//---------------------boot
#error
//Starting kernel ...
很多时候，在配置内核的时候，一不小心就造成kernel无法启动，比如出现：

。。。省略。。。
Kernel image @ 0x82000000 [ 0x000000 - 0x464080 ]
## Flattened Device Tree blob at 88000000
   Booting using the fdt blob at 0x88000000
   Using Device Tree in place at 88000000, end 880094d5

Starting kernel ...
假如想解决问题，就必须知道问题出现在了哪里，因此一般通过内核启动的时候打印的信息来定位问题产生的原因。 
但是如上所示，没有得到提示信息，直接悬挂在启动内核的位置上。 
为解决这个提问： 
这里首先想到的是打开使能early_printk，再次启动然后看是否可以得到有效的提示信息，假如仍然不可以的话，那就你直接读取缓存 ，缓存的地址为 __log_buf其地址在system.map被标记出来，我们可以找到具体的位置。具体如下:

首先去内核编译目录找到System.map文件。
接下来遭到System.map中__log_buf对应地址，执行：

cat  ./System.map | grep -n  __log_buf
如何可能得到如下结果： 
64584:c0770be8 b __log_buf 
此时记住c0770be8 这个数字。 
3. 启动内核，内核此时卡在Starting kernel …位置。 
4. 重新启动系统（注意:不要断电启动），进入UBoot命令行界面 
5. 在命令行输入（数字是刚才得到的数字）：
md 0xc0770be8 
1
6、不停的敲击回车键，直到为全为0

通过此方法就可以看到问题出现在哪个位置。
//---------------------end-----------------------------


