initramfs
2.initrd
Linux初始RAM磁盘（initrd）是在系统引导过程中挂载的一个临时根文件系统，用来支持两阶段的引导过程。initrd文件中包含了各种可执行程序和驱动程序，它们可以用来挂载实际的根文件系统，然后再将这个 initrd RAM磁盘卸载，并释放内存。在很多嵌入式Linux系统中，initrd 就是最终的根文件系统。

3.initramfs概述
initramfs与initrd类似，也是初始化好了且存在于ram中的，可以压缩也可以不压缩。但是目前initramfs只支持cpio包格式，它会被populate_rootfs->unpack_to_rootfs(&__initramfs_start, &__initramfs_end - &__initramfs_start, 0)函数(解压缩、)解析、安装。

initramfs与initrd区别
(1) Linux内核只认cpio格式的initramfs文件包(因为unpack_to_rootfs只能解析cpio格式文件)，非cpio格式的 initramfs文件包将被系统抛弃，而initrd可以是cpio包也可以是传统的镜像(image)文件，实际使用中initrd都是传统镜像文件。

(2) initramfs在编译内核的同时被编译并与内核连接成一个文件，它被链接到地址__initramfs_start处，与内核同时被 bootloader加载到ram中，而initrd是另外单独编译生成的，是一个独立的文件，它由bootloader单独加载到ram中内核空间外的地址，比如加载的地址为addr(是物理地址而非虚拟地址)，大小为8MB，那么只要在命令行加入"initrd=addr,8M"命令，系统就可以找到 initrd(当然通过适当修改Linux的目录结构，makefile文件和相关代码，以上两种情况都是可以相通的)。

(3) initramfs被解析处理后原始的cpio包(压缩或非压缩)所占的空间(&__initramfs_start - &__initramfs_end)是作为系统的一部分直接保留在系统中，不会被释放掉，而对于initrd镜像文件，如果没有在命令行中设置"keepinitd"命令，那么initrd镜像文件被处理后其原始文件所占的空间(initrd_end - initrd_start)将被释放掉。

(4) initramfs可以独立ram disk单独存在，而要支持initrd必须要先支持ram disk，即要配置CONFIG_BLK_DEV_INITRD选项 -- 支持initrd，必须先要配置CONFIG_BLK_DEV_RAM -- 支持ram disk ，因为initrd image实际就是初始化好了的ramdisk镜像文件，最后都要解析、写入到ram disk设备/dev/ram或/dev/ram0中。
注: 使用initramfs，命令行参数将不需要"initrd="和"root="命令？ YES



initramfs利弊:
------------------------------------------------------
由于initramfs使用cpio包格式，所以很容易将一个单一的文件、目录、node编译链接到系统中去，这样很简单的系统中使用起来很方便，不需要另外挂接文件系统。
但是因为cpio包实际是文件、目录、节点的描述语言包，为了描述一个文件、目录、节点，要增加很多额外的描述文字开销，特别是对于目录和节点，本身很小额外添加的描述文字却很多，这样使得cpio包比相应的image文件大很多。




使用initramfs的内核配置(使用initramfs做根文件系统):
------------------------------------------------------
General setup  --->
[*] Initial RAM filesystem and RAM disk (initramfs/initrd) support
(/rootfs_dir) Initramfs source file(s)   //输入根文件系统的所在目录 

使用initramfs的内核启动参数
不需要"initrd="和"root="参数,但是必须在initramfs中创建/init文件或者修改内核启动最后代码(init文件是软连接，指向什么? init -> bin/busybox，否则内核启动将会失败)

链接入内核的initramfs文件在linux-2.6.24/usr/initramfs_data.cpio.gz


使用initrd的内核配置(使用网口将根文件系统下载到RAM -- tftp addr ramdisk.gz):
------------------------------------------------------
1. 配置initrd
General setup  --->
[*] Initial RAM filesystem and RAM disk (initramfs/initrd) support
() Initramfs source file(s)   //清空根文件系统的目录配置 

2. 配置ramdisk
Device Drivers  --->   
Block devices  --->
<*> RAM disk support
(16)  Default number of RAM disks   // 内核在/dev/目录下生成16个ram设备节点
(4096) Default RAM disk size (kbytes)
(1024) Default RAM disk block size (bytes)

使用 initrd的内 核启动参数:
initrd=addr,0x400000 root=/dev/ram rw
注:
(1) addr是根文件系统的下载地址；
(2) 0x400000是根文件系统的大小，该大小需要和内核配置的ramdisk size 4096 kbytes相一致；
(3) /dev/ram是ramdisk的设备节点，rw表示根文件系统可读、可写；



根文件系统存放在FLASH分区：
------------------------------------------------------
1. 内核启动参数不需要"initrd="(也可以写成"noinitrd")；
root=/dev/mtdblock2 (/dev/mtdblock2 -- 根文件系统所烧写的FLASH分区)

2. 内核配置不需要ram disk；也不需要配置initramfs或者initrd
[ ] Initial RAM filesystem and RAM disk (initramfs/initrd) support 

注: boot的FLASH分区要和kernel的FLASH分区匹配(而非一致)，需要进一步解释。








TCP reno registered 停在这里
---------------------------------------------
rootfs太大，检测发现rootfs下有垃圾(里面有包含了一份rootfs)


下面这个大小启动成功了
---------------------------------------------
root@zengxiaolong:/home/zengxiaolong/soft/s3c2410/rootfs# du -h
4.0K    ./dev
4.0K    ./tmp
8.0K    ./etc/init.d
40K    ./etc
4.0K    ./usr
4.0K    ./var
4.0K    ./sbin
2.1M    ./bin
4.0K    ./sys
116K    ./lib/modules
2.8M    ./lib
4.0K    ./proc
4.9M    .


initrd and initramfs
http://blog.sina.com.cn/s/blog_622013d90100fr04.html
---------------------------------------------
#
# General setup
#
...
CONFIG_BLK_DEV_INITRD=y
CONFIG_INITRAMFS_SOURCE="<path_to>/target_fs>"
...
#
# UBI - Unsorted block images
#
...
CONFIG_BLK_DEV_RAM=y
CONFIG_BLK_DEV_RAM_COUNT=1
CONFIG_BLK_DEV_RAM_SIZE=8192
CONFIG_BLK_DEV_RAM_BLOCKSIZE=1024


Installation initramfs

No special installation like with initrd is necessary. The initramfs is already in the kernel. If you start the kernel, the initramfs is already there. Therefore, there is no root=/dev/ram0 rw initrd=0x87000000,8M bootargs option necessary. Remove this if you still have it!

3.精通initramfs构建step by step

来源: ChinaUnix博客 　日期： 2009.06.24 18:19　(共有条评论) 我要评论   
                                                
（一）hello world 
一、initramfs是什么 
在2.6版本的linux内核中，都包含一个压缩过的cpio格式的打包文件。当内核启动时，会从这个打包文件中导出文件到内核的rootfs文件系统， 然后内核检查rootfs中是否包含有init文件，如果有则执行它，作为PID为1的第一个进程。这个init进程负责启动系统后续的工作，包括定位、 挂载“真正的”根文件系统设备（如果有的话）。如果内核没有在 rootfs中找到init文件，则内核会按以前版本的方式定位、挂载根分区，然后执行/sbin/init程序完成系统的后续初始化工作。 
这个压缩过的cpio格式的打包文件就是initramfs。编译2.6版本的linux内核时，编译系统总会创建initramfs，然后把它与编译好 的内核连接在一起。内核源代码树中的usr目录就是专门用于构建内核中的initramfs的，其中的initramfs_data.cpio.gz文件 就是initramfs。缺省情况下，initramfs是空的，X86架构下的文件大小是134个字节。 
二、构建第一个initramfs：hello world 
从C语言开始，学习计算机编程语言的第一个程序几乎都是hello world，因此我们也构建一个最简单的hello world式的initramfs，以说明initramfs的基本构建方法。 
initramfs的灵魂是init文件（或者叫程序，因为它会被内核第一个执行），我们先写一个简单的init程序，它会在内核的console中打印出经典的hello world信息。 
hello.c： 
#include  
#include  
int main(int argc,char argv[]) 
{ 
printf("hello world, from initramfs.\n"); 
sleep(9999999); 
return 0; 
} 
其中的sleep（）函数语句是为了避免执行时内核很快打出panic的信息，并非功能上的需要。 
接着把hello.c编译成静态连接程序： 
gcc -o hello_static -static -s hello.c 
命令行中的-s参数表示编译后的程序不包含调试定位信息，目的是减少编译出来的程序文件的大小。 
再创建一个initramfs的构建源文件目录image，把hello_static程序拷入这个目录，并改名为init。 
在image目录下，创建一个dev/console的设备文件，否init程序无法在内核console中输出信息： 
mknod -m 600 dev/console c 5 1 
注意，执行这个命令需要有root权限。 
好了，现在可以设置内核配置参数，进行initramfs的构建了： 
在general setup配置目录下的initramfs sources配置项下输入image的路径名，比如我的路径就是/home/wyk/initramfs-test/image。因为我们的init程 序是ELF格式的，所以内核需要支持ELF的可执行文件，否则启动这个init程序会失败。在内核的 Executable file formats配置目录下，选择 kernel support for ELF binaries，则可使内核支持ELF格式的可执行文件。其他内核配置参数根据实际需要设置即可，不过，为了减少内核编译时间，可参考这篇文章
http://linuxman.blog.ccidnet.com/blog-htm-do-showone-uid-60710-type-blog-itemid-293122.html
设置一个最简单的内核配置。 
内核配置参数设置完成后，按常规的内核编译方法进行编译，initramfs就自动连接到编译好的内核映像文件中了。 
三、试验环境搭建 
试验initramfs需要经常重启系统，所以使用CPU模拟器是不错的选择。我们可以选用qemu，它支持直接启动linux内核，无需在模拟器中安装OS。从方便使用的角度考虑，我们采用qemu launcher设置qemu的各项参数，它的安装可参考
http://linuxman.blog.ccidnet.com/blog-htm-do-showone-uid-60710-type-blog-itemid-612280.html
。 
在qemu launcher的linux配置标签中，打勾直接启动linux，然后在下面的文本框中填上刚才编译好的内核映像文件的路径名。因为qemu的运行还需要设置硬盘映像文件，所以还需要在左边的配置标签中新建一个硬盘映像文件，但实际上我们并不使用硬盘。 
配置好qemu的参数后，点击launcher按钮，内核就开始在qemu中运行了。内核输出一堆内核运行信息后，最后打出了 
hello world, from initramfs. 
哈哈，我们构建的initramfs已经能够正常工作了！ 
（二）initramfs的前世今生 
四、什么是rootfs和ramfs 
所有的2.6版本linux内核都有一个特殊的文件系统 rootfs，是内核启动的初始始根文件系统，initramfs的文件会复制到rootfs。如果把initramfs比作种子，那么rootfs就是 它生长的土壤。大部分linux系统正常运行后都会安装另外的文件系统，然后忽略rootfs。 
rootfs是ramfs文件系统的一个特殊实例。ramfs是一种非常简单的文件系统，是基于内存的文件系统。ramfs文件系统没有容量大小的限制，它可以根据需要动态增加容量。 
ramfs 直接利用了内核的磁盘高速缓存机制。所有的文件的读写数据都会在内存中做高速缓存（cache），当系统再次使用文件数据时，可以直接从内存中读写，以提 供系统的I/O性能。高速缓存中的写入数据会在适当的时候回写到对应的文件系统设备（如磁盘等）中，这时它的状态就标识为clean，这样系统在必要时可 以释放掉这些内存。ramfs没有对应文件系统设备，所以它的数据永远都不会回写回去，也就不会标识为clean，因此系统也永远不会释放ramfs所占 用的内存。 
因为ramfs直接使用了内核已有的磁盘高速缓存机制，所以它的实现代码非常小。也由于这个原因，ramfs特性不能通过内核配置参数删除，它是内核的天然特性。 
五、ramfs不是ramdisk 
ramdisk 是在一块内存区域中创建的块设备，用于存放文件系统。ramdisk的容量是固定的，不能象ramfs一样动态增长。ramdisk需要内核的文件系统驱 动程序（如ext2）来操作其上的数据，而ramfs则是内核的天然特性，无需额外的驱动程序。ramdisk也象其他文件系统设备一样，需要在块设备和 内存中的磁盘高速缓存之间复制数据，而这种数据复制实际不必要的。 
六、从ramfs派生的文件系统tmpfs 
ramfs 的一个缺点是它可能不停的动态增长直到耗尽系统的全部内存，所以只有root或授权用户允许使用ramfs。为了解决这个问题，从ramfs派生出了 tmpfs文件系统，增加了容量大小的限制，而且允许把数据写入交换分区。由于增加了这两个特性，所以tmpfs允许普通用户使用。 
关于tmpfs文件系统更多的信息，可以看内核源码中的 Documentation/filesystems/tmpfs.txt 文档。 
综上所述，initramfs是一种ramfs文件系统，在内核启动完成后把它复制到rootfs中，作为内核初始的根文件系统，它的任务是挂载系统真正的根文件系统。这就是initramfs的前世今生。 

（三）：busybox 
七、什么是busybox 
busybox号称是嵌入式Linux中的瑞士军刀——小巧、功能齐全。它把许多常用的Linux命令都集成到一个单一的可执行程序中，只用这一个可执行 程序（即busybox）加上Linux内核就可以构建一个基本的 Linux系统。busybox程序非常小巧，包含全部命令可执行文件大小也只有750多K。busybox是完全模块化的，可以很容易地在编译时增加、 删除其中包含的命令。 
由于busybox的这些特点，它广泛应用于LiveCD、应急修复盘、安装盘等系统中。我们也是以它为基础，构建initramfs。 
八、busybox的配置、编译和安装 
（1）去
http://busybox.net
去下载最新的源码，解压展开。 
（2）用 
make menuconfig 
命令启动配置界面配置，配置busybox的特性、选择要包含在busybox的命令（busybox称为applet）； 
也可以用 
make defconfig 
命令做缺省配置，包含全部的applet。 
另外两个配置命令是 
make allyesconfig——最大配置 
make allnoconfig——最小配置 
它们和make defconfig命令都可以用来作为自定义配置的初始配置，然后再用make menuconfing命令做定制化配置。 
为了简单，我们用make defconfig做缺省配置。 
（3）用 
make 
命令编译busybox软件。 
（4）用 
make CONFIG_PREFIX= install 
命令安装。如果在命令行中省略CONFIG_PREFIX变量的赋值，则会安装缺省值 ./_install 目录下。CONFIG_PREFIX可以在make menuconfig的配置界面中修改。 
我们用make CONFIG_PREFIX=~/initramfs-test/image 命令把busybox安装到initramfs的构建目录中。 
（5）缺省配置下，busybox动态链接到glibc，所以要把它用到的动态库复制到initramfs的构建目录中。用ldd命令查看busybox用到了哪些动态库文件及相应的文件路径，然后把它们复制到相应的目录下即可。 
我们编译的busybox需要向image/lib目录下复制 
ld-linux.so.2 
libc.so.6 
libcrypt.so.1 
libm.so.6 
动态库文件。 
九、在image下创建必要的目录和设备文件 
（1）在imgae目录下创建 
proc ， sys ， etc ，mnt 
四个目录 
（2）hello world 已经创建了console 设备文件，我们再用 
mknod -m 600 dev/null c 1 3 
命令创建另一个基本的设备文件。 
十、试验一下 
busybox的构建和准备工作做完了，我们试验一下吧： 
在image目录下以root用户权限—— 
（1）用 
mount -vt proc proc =proc 
mount -vt sysfs sysfs =sys 
命令安装内核虚拟文件系统 
（2）用 
mount -v -o bind /dev dev 
命令绑定/dev的设备文件到image/dev 
（3）用 
chroot . /bin/sh 
命令进入busybox的环境。出现shell的命令提示符，可以试着输入几个命令，看看执行结果。例如，输入 fdisk -l 命令看看是否能显示硬盘的分区。 

（四）：mini linux 
十一、自动生成/dev下的设备文件 
上节用chroot方法试验busybox时，为了简单，是用“绑定”的方式把主机的/dev中的设备文件映射到image目录下的dev目录。在initramfs上，这种方法显然不能使用。 
生成系统的设备文件，现在通常都是用udev动态生成，而initramfs为了做到通用，动态生成的要求是必须的。在busybox中有一个mdev命令，就是用来动态生成设备文件，填充到/dev目录的。 
在系统启动时，用 
mdev -s 
命令可以根据内核的sysfs文件系统在/dev目录中自动生成相应的设备文件。命令执行前，需要先挂载内核的proc和sysfs虚拟文件系统。 
十二、初始身手 
解决了自动生成设备文件的问题后，我们可以试着做一个最简单的可运行的linux系统了： 
（1）在image目录下写一个最简单的init脚本。 
#!/bin/sh 
mount -t proc proc /proc 
mount -t sysfs sysfs /sys 
mdev -s 
/bin/sh 
（2）为init脚本设置可执行权限，否则内核不会去执行它。 
chmod +x init 
（3）有些busybox配置中，mdev命令需要读取/etc/mdev.conf文件，为了避免出错信息，我们创建一个空文件。 
touch etc/mdev.conf 
（4）在内核源码目录下，执行 
make 
命令，重新编译内核，生成新的initramfs。 
好了，在QEMU模拟环境下启动这个新的内核，系统初始化后，会进入SHELL环境。在这个SHELL环境下，试验一些常用命令，看看是否可以正常运行。 
十三、can't access tty 
上一步创建的简单linux系统在进入SHELL环境时，会打出下面这一句出错信息： 
/bin/sh: can't access tty; job controll off 
虽然不影响使用，但终究不够完美。 
产生这个错误的原因是我们的SHELL是直接运行在内核的console上的，而console是不能提供控制终端（terminal）功能的，所以必须 把 SHELL运行在tty设备上，才能消除这个错误。解决问题的办法是使用正规init机制，在执行SHELL前打开tty设备。 
另外，这个简单系统的reboot、halt等命令是不起作用的，也必须通过init方式解决。 
十四、busybox的缺省init模式 
busybox支持init功能，当系统没有/etc/inittab文件时，它有一套缺省的模式，按下面配置执行： 
::sysinit:/etc/init.d/rcS 
::askfirst:/bin/sh 
::ctrlaltdel:/sbin/reboot 
::shutdown:/sbin/swapoff -a 
::shutdown:/bin/umount -a -r 
::restart:/sbin/init 
如果busybox检测到/dev/console不是串口控制台，init还要执行下面的动作： 
tty2::askfirst:/bin/sh 
tty3::askfirst:/bin/sh 
tty4::askfirst:/bin/sh 
我们试试这种模式是否可以解决我们的问题。 
（1）写/etc/init.d/rcS脚本 
这个脚本实际是要执行系统的初始化操作。我们把前面的init脚本改造一下，将最后的/bin/sh命令删除，然后移到 etc/init.d目录下，改名为rcS。 
（2）initramfs不需要linuxrc，而且如果没有init文件，内核就不认为它是一个有效的initramfs，因而不安装它，导致内核panic。于是，我们在image目录下，把busybox安装的linuxrc改名为init 
mv linuxrc init 
（3）重新编译内核，生成新的initramfs 
（4）用QEMU试验一下新编译的内核。系统启动后，会打出一句话“please press Enter to active this console”——感觉还不错。但是按下回车键后，系统依然会打出错误信息“-/bin/sh: 
can't access tty; job controll off ”。用tty命令看看当前的终端设备文件名： 
# tty 
/dev/console 
它还是console，不是tty设备，所以问题没有解决。不过，reboot和halt命令倒是可以正常工作了。 
经过验证，busybox的缺省init模式无法满足我们的要求，我们还是要写inittab，定制自己的init初始化流程。 
十五、busybox的inittab文件格式说明 
要写自己的inittab，需要理解busybox的inittab文件格式。 
busybox的inittab文件与通常的inittab不同，它没有runlevel的概念，语句功能上也有限制。inittab语句的标准格式是 
::: 
各字段的含义如下 
: 
id字段与通常的inittab中的含义不同，它代表的是这个语句中process执行所在的tty设备，内容就是/dev目录中tty设备的文件名。由于是运行process的tty设备的文件名，所以也不能象通常的inittab那样要求每条语句id的值唯一。 
: 
busybox不支持runlevel，所以此字段完全被忽略。 
: 
为下列这些值之一： 
sysinit, respawn, askfirst, wait,once, restart, ctrlaltdel, shutdown 
其含义与通常的inittab的定义相同。特别提一下askfirst，它的含义与respawn相同，只是在运行process前，会打出一句话 “please press Enter to active this console”，然后等用户在终端上敲入回车键后才运行process。 
： 
指定要运行的process的命令行。 
十六、写mini linux的inittab 
理解了busybox的inittab格式，我们就可以写mini linux的inittab： 
::sysinit:/etc/init.d/rcS 
tty1::askfirst:/bin/sh 
tty2::askfirst:/bin/sh 
tty3::askfirst:/bin/sh 
tty4::askfirst:/bin/sh 
tty5::askfirst:/bin/sh 
tty6::askfirst:/bin/sh 
::restart:/sbin/init 
::ctrlaltdel:/sbin/reboot 
::shutdown:/bin/umount -a -r 
把这个文件放到image的etc目录下。为了执行reboot命令时避免提示找不到/etc/fstab文件，我们再在etc目录下创建一个空文件 
touch fstab 
做好了这些，就可以重新编译内核，生成新的initramfs了。在QEMU试验环境下验证新生成的mini linux，系统运行正常，而且象通常的linux系统一样，用ALT＋F1～F6键可以在6个终端间切换。 

（五）initrd 
十七、配置内核支持initrd 
到目前为止，我们的initramfs都由内核编译系统生成的，并链接到内核中。其实我们也可以用cpio命令生成单独的initramfs，与内核编译脱钩，在内核运行时以initrd的形式加载到内核，以增加灵活性。 
首先配置内核使用单独的initrd：在 Device Driver / Block device / 配置目录下，选择 RAM filesystem and RAMdisk ( initramfs/initrd ) support 配置项；再到 General Setup 配置目录项下，将 initramfs source file(s) 配置项原有的内容清空。然后把内核源码树的usr目录下已由内核编译生成的initramfs文件initramfs_data.cpio.gz拷贝到 ~/initramfs-test 目录下，我们先直接用这个文件试验一下 initrd 方式的initramfs的效果。最后，执行make命令重新编译内核后，在QEMU试验环境中，把initrd配置框（linux配置框的下面）的内容 写为 ~/initramfs-test/initramfs_data.cpio.gz，指定initrd的文件路径。 
好了，试验一下新的initrd方式的initramfs吧，效果跟先前的完全一样。 
十八、用cpio命令生成initramfs 
cpio 命令有三种操作模式：copy－out、copy－in、copy－pass，生成initramfs用的是它的copy－out模式，即把文件打包的操 作模式。cpio的copy－out操作模式使用 -o 命令行选项指定。缺省情况下，cpio从标准输入读取输入数据，向标准输出写入输出数据。使用 -I 选项可以指定文件名代替标准输入，使用 -O 选项可以指定文件名代替标准输出，而 -F 选项指定的文件名则根据cpio操作模式的不同可代替标准输入或标准输出。 
把~/initramfs-test/image目录下的文件打包成initramfs，执行下面的命令： 
find . | cpio -o -H newc | gzip > ../image.cpio.gz 
命令执行完毕后，在~/initramfs-test目录下就会生成文件名为imgae.cpio.gz的initramfs。 
上面cpio命令的 -H 选项指定打包文件的具体格式，要生成initramfs，只能用newc 格式，如果使用其他格式，内核会打出这样的出错信息：Unpacking initramfs... kernel panic - not syncing: no cpio magic 
在QEMU试验环境下试验一下新的initrd方式的initramfs，效果跟先前的完全一样。 
十九、cpio命令的其他用法 
如果我们要解开一个cpio格式的打包文件，则要使用cpio命令的copy－in操作模式。cpio的copy－out操作模式使用 -i 命令行选项指定。例如，我们想把前一步从内核源码树 usr目录下拷贝的initramfs_data.cpio.gz 展开到~/initramfs-test/initramfs_data目录下，则使用下列命令： 
mkdir ~/initramfs-test/initramfs_data 
cd ~/initramfs-test/initramfs_data 
cpio -i -F ../initramfs_data.cpio.gz --no-absolute-filename 
命令执行完毕后，initramfs_data目录下出现多个目录和文件，用diff命令比较initramfs_data与image目录，两者的完全一样。 
上面cpio命令的 --no-absolute-filename 选项的作用是展开文件时，去掉文件路径最前面的"/"，把绝对路径名变为相对路径名。内核编译时生成的initramfs使用了绝对路径名，所以这个选项 必须使用，否则initramfs内文件展开到"/"目录去了，如果你是root用户或有"/"目录的写权限，那么展开的文件就有可能覆盖同名的文件（在 文件修改时间新于原有文件），那就糟糕了！ 
展开文件前，你可能会想先看看打包文件里都有哪些文件，这时就要用 -t 选项了。例如，我们想看看内核编译时生成的initramfs_data.cpio.gz中都有哪些文件，我们就可以用下面的命令： 
zcat initramfs_data.cpio.gz | cpio -t 
在标准输出中打出文件名列表。 
使用 -v 选项可以在cpio命令执行时输出详细信息：在打包或展开文件时，输出已处理的文件名；与 -t 选项连用时，则显示文件的详细信息，类似 ls -l 的输出内容。-V 选项则用打点的方式，显示cpio命令的执行进度信息，一个点代表处理一个文件。 
（六）switch_root 
二十、switch_root 命令 
除了基于initramfs的系统（如第四节的mini linux），通常initramfs都是为安装最终的根文件系统做准备工作，它的最后一步需要安装最终的根文件系统，然后切换到新根文件系统上去。以往 的基于ramdisk 的initrd 使用pivot_root命令切换到新的根文件系统，然后卸载ramdisk。但是initramfs是rootfs，而rootfs既不能 pivot_root，也不能umount。为了从initramfs中切换到新根文件系统，需要作如下处理： 
（1）删除rootfs的全部内容，释放空间 
find -xdev / -exec rm '{}' ';' 
（2）安装新的根文件系统，并切换 
cd /newmount; mount --move . /; chroot . 
（3）把stdin/stdout/stderr 附加到新的/dev/console，然后执行新文件系统的init程序 
上述步骤比较麻烦，而且要解决一个重要的问题：第一步删除rootfs的所有内容也删除了所有的命令，那么后续如何再使用这些命令完成其他步骤？busybox的解决方案是，提供了switch_root命令，完成全部的处理过程，使用起来非常方便。 
switch_root命令的格式是： 
switch_root [-c /dev/console] NEW_ROOT NEW_INIT [ARGUMENTS_TO_INIT] 
其中NEW_ROOT是实际的根文件系统的挂载目录，执行switch_root命令前需要挂载到系统中；NEW_INIT是实际根文件系统的init程序的路径，一般是/sbin/init；-c /dev/console是可选参数，用于重定向实际的根文件系统的设备文件，一般情况我们不会使用；而ARGUMENTS_TO_INIT则是传递给实际的根文件系统的init程序的参数，也是可选的。 

需要特别注意的是：switch_root命令必须由PID=1的进程调用，也就是必须由initramfs的init程序直接调用，不能由init派生的其他进程调用，否则会出错，提示： 
switch_root: not rootfs 
也是同样的原因，init脚本调用switch_root命令必须用exec命令调用，否则也会出错，提示： 
switch_root: not rootfs 
二十一、实践：用initramfs安装CLFS根文件系统 
现在实践一下switch_root命令，用它切换一个CLFS的根文件系统硬盘分区。我的CLFS安装在/dev/sda8硬盘分区，我们就以此为例说明。 
我们还是在以前的image目录中构建 
（1）改写init脚本 
#!/bin/sh 
mount -t proc proc /proc 
mount -t sysfs sysfs /sys 
mdev -s 
mount /dev/sda8 /mnt （注意：为了简单，我们直接把CLFS分区写死在init脚本中了） 
exec switch_root /mnt /sbin/init 
（2）生成新的initrd 
按上一节
“精通initramfs构建step by step （五）：initrd”
描述的cpio命令生成新的initrd。 
（3）把新的initrd拷贝到CLFS分区的/boot目录下，改名为clfs-initrd 
（4）在GRUB的menu.lst配置文件中增加一个启动项 
#test for initramfs of CLFS 
title test for initramfs of CLFS (on /dev/sda8) 
root (hd0,7) 
kernel /boot/clfskernel-2.6.17.13 （注意：并没有向内核传递root参数信息） 
initrd /boot/clfs-initrd 
全部做完后，重启机器，选择 test for initramfs of CLFS 启动项，机器顺利进入了CLFS系统，我们构建的initramfs用switch_root命令完成了CLFS实际根文件系统的安装和切换。 

（七）modules
二十二、内核模块支持 
到目前为止，我们在构建initramfs时还没有涉及内核模块的支持，所用到的硬件驱动程序都是直接编译到内核中。现在我们就看看如何使initramfs支持内核模块。 
首先，内核配置要支持模块，并支持内核模块的自动加载功能：在内核配置菜单中的激活下面的配置项，编译进内核 Load module support / Enable loadable module support / Automatic kernel loading ； 
然后把需要的硬件驱动程序配置模块形式，比如把我的机器上的硬盘控制器的驱动编译成模块，则选择 
Device Driver 
|---->SCSI device support 
|---->SCSI disk support 
|----->verbose SCSI error reporting (不是必须的，但可方便问题定位) 
|----->SCSI low-level drivers 
|---->Serial ATA (SATA) support 
|---->intel PIIX/ICH SATA support 
把它们配置成模块。 
最后，编译内核，并把编译好的内核模块安装到image的目录下： 
make 
make INSTALL_MOD_PATH=~/initramfs-test/image modules_install 
命令执行完毕后，在image/lib/modules/2.6.17.13/kernel/drivers/scsi目录下安装了4个内核模文 件：scsi_mod.ko、sd_mod.ko、ata_piix.ko、libata.ko，它们就是所需的硬盘控制器的驱动程序。 
好了，都准备好了，可以用cpio命令生成inintramfs了。不过，为了方便后面的试验，我们再把init脚本改成 
#!/bin/sh 
mount -t proc proc /proc 
mount -t sysfs sysfs /sys 
mdev -s 
exec /bin/sh 
使系统启动后进入shell环境，并且用exec调用的方式，使shell的pid为1，能够执行switch_root命令。 
二十三、试验：用initramfs中的内核模块安装硬盘文件系统 
用新生成的initramfs启动系统，内核并没有自动加载硬盘控制器的驱动程序，所以 /dev目录下也没有sda等硬盘设备文件。好吧，我们自己加载内核模块文件。不幸的是，busybox的modprobe命令执行不正常，不能加载内核 模块。怀疑是busybox的modprobe命令配置或编译有问题，后续再花时间定位吧，先用insmod命令依次加载。查看/lib/modules /2.6.17.13/modules.dep，弄清楚了4个模块的依赖关系，执行下面的命令加载： 
insmod scsi_mod 
insmod libata 
insmod ata_piix 
insmod sd_mod 
然后再用 
mdev -s 
命令生成硬盘的设备文件。 
好了，可以安装CLFS的硬盘分区，并把根文件系统切换到CLFS的硬盘分区： 
mount /dev/sda8 /mnt 
exec switch_root /mnt /sbin/init 
系统正常启动到了CLFS，我们可以做到用initramfs中的硬盘控制器的驱动模块安装硬盘分区了。 
二十四、mdev的hotplug模式 
上面的试验中，我们在加载完驱动模块后调用了mdev -s 命令来生成硬盘的设备文件。其实，可以使用mdev的hotplug模式在加载内核时自动生成对应的设备文件： 
在执行insmod命令前，用 
echo /sbin/mdev > /proc/sys/kernel/hotplug 
命令设置系统的hotplug程序为mdev。 
后续使用insmod命令加载模块时，系统自动调用mdev生成相应的设备文件。 
注意：内核必须配置支持hotplug功能，而前面提到的CLFS最简内核配置方案是没有配置hotplug支持的。 
（八）coldplug 
二十五、udev的coldplug模式 
内核在启动时已经检测到了系统的硬件设备，并把硬件设备信息通过sysfs内核虚拟文件系统导出。udev扫描sysfs文件系统，根据硬件设备信息生成 热插拔（hotplug）事件，udev再读取这些事件，生成对应的硬件设备文件。由于没有实际的硬件插拔动作，所以这一过程被称为coldplug。我 们的initramfs就是利用这一机制，加载硬件设备的驱动程序模块。 
udev完成coldplug操作，需要下面三个程序： 
udevd——作为deamon，记录hotplug事件，然后排队后再发送给udev，避免事件冲突（race conditions）。 
udevtrigger——扫描sysfs文件系统，生成相应的硬件设备hotplug事件。 
udevsettle——查看udev事件队列，等队列内事件全部处理完毕才退出。 
在initramfs的init脚本中可以执行下面的语句实现coldplug功能： 
mkdir -p /dev/.udev/db 
udevd --daemon 
mkdir -p /dev/.udev/queue 
udevtrigger 
udevsettle 
许多文档提到的在udevd --daemon 命令前要执行 
echo > /proc/sys/kernel/hotplug 
命令，经验证，在我们的initramfs环境下的coldplug功能中并不需要。 
二十六、试验：用udev自动加载设备驱动模块 
了解了udev的coldplug的机理，我们就试验一下用udev自动加载设备驱动模块，并生成硬件设备文件。 
（1）从 /sbin 目录下拷贝udevd、udevtrigger、udevsettle程序到image目录下的sbin目录下，并用ldd命令找到它们所需要的动态库文件，拷贝到image目录下的lib目录下。 
（2）修改init脚本，增加coldplug功能： 
#!/bin/sh 
mount -t proc proc /proc 
mount -t sysfs sysfs /sys 
mdev -s 
#using udev autoload hard disk driver module 
mkdir -p /dev/.udev/db 
udevd --daemon 
mkdir -p /dev/.udev/queue 
udevtrigger 
udevsettle 
mount /dev/sda8 /mnt 
killall udevd 
exec switch_root /mnt /sbin/init 
注意：在切换到真正根文件系统前，要把udevd进程杀掉，否则会和真正根文件系统中的udev脚本的执行相冲突。这就是上面killall udevd 语句的作用。 
（3）编写udev规则文件 
规则文件是udev的灵魂，没有规则文件，udev无法自动加载硬件设备的驱动模块。为了简单，我们直接使用CLFS中的40- modprobe.rules，把它拷贝到image目录下的etc/udev/rules.d目录。有关udev的规则文件编写，已超出了本文的范围， 后续我有可能专文描述。 
######################################################################## 
# 
# Description : 40-modprobe.rules 
# 
# Authors : Based on Open Suse Udev Rules 
# 
kay.sievers@suse.de

# 
# Adapted to : Jim Gifford 
# LFS : Alexander E. Patrakov 
# 
# Version : 00.01 
# 
# Notes : 
# 
######################################################################## 
# hotplug 
ENV{MODALIAS}=="?*", RUN+="/sbin/modprobe $env{MODALIAS}" 
# scsi 
SUBSYSTEM=="scsi_device", ACTION=="add", SYSFS{device/type}=="0|7|14", RUN+="/sbin/modprobe sd_mod" 
SUBSYSTEM=="scsi_device", ACTION=="add", SYSFS{device/type}=="1", SYSFS{device/vendor}=="On[sS]tream", RUN+="/sbin/modprobe osst" 
SUBSYSTEM=="scsi_device", ACTION=="add", SYSFS{device/type}=="1", RUN+="/sbin/modprobe st" 
SUBSYSTEM=="scsi_device", ACTION=="add", SYSFS{device/type}=="[45]", RUN+="/sbin/modprobe sr_mod" 
SUBSYSTEM=="scsi_device", ACTION=="add", RUN+="/sbin/modprobe sg" 
# floppy 
KERNEL=="nvram", ACTION=="add", RUN+="load_floppy_module.sh" 
注意：上面的 
ENV{MODALIAS}=="?*", RUN+="/sbin/modprobe $env{MODALIAS}" 
语句是实现自动加载硬件设备驱动模块功能的关键，它根据sysfs文件系统中记录 的模块aliases数据，用modprobe命令加载对应的内核模块。有关模块aliases的进一步说明，可参考CLFS手册（CLFS- 1.0.0-x86）中的11.5.2.4. Module Loading一节的描述。 
（4）拷贝modprobe命令 
前一节提到过，busybox的modprobe 命令不能正常使用，所以我们需要拷贝 /sbin 目录下的modprobe命令到image目录下的sbin目录，供udev加载内核模块使用。再用ldd命令检查一下 /sbin/modprobe 命令所需的动态库文件，如果有则拷贝到image/lib目录下。（我的检查结果是，除了libc6外，不需要其他动态库，所以不需要拷贝） 
好了，重新生成initramfs，启动CLFS系统，initramfs能够自动加载硬盘设备的驱动模块，系统顺利地从initramfs切换到了真正的CLFS的根文件系统。 

（九）内核编译时构建initramfs补遗 
二十七、直接把cpio打包文件编译进内核 
如果我们有一个已经做好的cpio格式的initramfs，可以在内核编译时直接编译进内核。回忆一下
第一节
的内容，我们在内核配置参数中的initramfs sources配置项下输入构建initramfs的目录路径。其实我们也可以直接输出现成的initramfs的文件名，这样在内核编译时，就可以把它编译进内核了。 
使用这种方法，有两点需要注意： 
（1）cpio文件不能压缩。一般作为initrd的cpio文件都经过了压缩，所以编译前需要先把压缩过的文件解压。 
（2）cpio文件的后缀名必须是 .cpio。内核编译通过 .cpio的后缀名来识别此文件是cpio打包文件，而其他文件后缀名则会被认为是initramfs构建的描述文件（关于描述文件，下面后详细说明）。 
二十八、用描述文件构建initramfs 
用内核编译工具构建initramfs的第三种方法是使用描述文件。在内核配置参数中的initramfs sources配置项下可以输入initramfs构建描述文件的文件名，内核编译工具根据描述文件完成initramfs的构建。 
描述文件的语法格式的说明如下： 
# a comment 
file      
dir     
nod        
slink      
pipe     
sock    
name of the file/dir/nod/etc in the archive 
location of the file in the current filesystem 
link target 
mode/permissions of the file 
user id (0=root) 
group id (0=root) 
device type (b=block, c=character) 
major number of nod 
minor number of nod 
例子： 
我们用描述文件的方式，构建第一节中的hello world的initramfs。 
hello-init.desp: 
dir /dev 0755 0 0 
nod /dev/console 0600 0 0 c 5 1 
file /init /home/wyk/initramfs-test/hello_static 0755 0 0 
在内核配置项initramfs sources中指定描述文件hello-init.desp，编译内核时就会生成hello world的initramfs，运行效果与第一节用指定构建目录的方法构建的initramfs的完全相同。 
注意：在内核帮助文件中，提到initramfs sources配置项可以指定多个目录或描述文件，内核会汇集这些目录或文件生成一个initramfs。但从我的试验来看，initramfs sources只接受单一的目录名或文件名，输出多个目录名或文件名（之间用空格分隔），内核编译时就会出错。也许是我的方法有误，还望读者指正。 

（十）uclibc 
二十九、toolchain 
在initramfs中使用uclibc库，关键是构建uclibc的工具链toolchain。构建uclibc 的 toolchain 有两种主要方式：（1）用buildroot工具（ 
http://buildroot.uclibc.org/
）自动构建，这也是uclibc的官方标准做法。（2）用CLFS Embedded手册的方法手工创建。目前CLFS Embedded还在开发中，可在
http://cross-lfs.org/view/clfs-embedded/x86/
中查阅。 
我们简单地说明用buildroot工具构建uclbic的toolchain的步骤： 
（1）获取buildroot。 
推荐用svn命令从它的版本库中下载： 
svn co svn://uclibc.org/trunk/buildroot 
要求使用svn命令，需要先安装subversion软件包。下载过程中，可能会出现连接异常中断的情况，这时重新执行上述命令，继续进行下载，有可能要重复多次。 
（2）配置buildroot 
因为我们只是创建toolchain，所以需要做相应的配置。在buildroot的顶层目录下，执行 
make menuconfig 
命令，在缺省配置的基础上做如下配置 
Target Architecture: i386 
Target Architecture Variant: i686 
Package Selection for the target: 取消BusyBox的选项（缺省是选中的） 
Target filesystem options: 取消 ext2 root filesystem（缺省是选中的） 
Toolchain --> Toolchain type: Buildroot toolchain 
（3）编译 
执行 
make 
命令，buildroot工具会自动下载所需要的源文件并自动编译，等一两个小时后，toolchain就编译好了。编译好的toolchain位于 
buildroot/build_i686/staging_dir/usr/bin/ 
目录下。工具命令的前缀是 i686-linux- 。 
三十、编译Busybox静态连接uclibc库 
一般而言，使用uclibc库是为了把它静态连接到busybox中。具体步骤是： 
（1）把uclibc toolchain的目录添加到PATH中。 
在~/.bash_profile文件中添加： 
#set PATH so it includes uclibc toolchain if it exist 
if [ -d ~/buildroot/build_i686/staging_dir/usr/bin ] ; then 
PATH="${PATH}":~/buildroot/build_i686/staging_dir/usr/bin 
fi 
（2）配置busybox静态连接库。 
在busybox的配置界面中，选择： 
Build Options --> Build BusyBox as a static binary (no shared libs) 
（3）编译 
执行 
make CROSS_COMPILE=i686-linux- 
命令“交叉编译”busybox。 
最后编译生成的是静态连接的可执行文件，不需要在initramfs中拷贝库文件。 
三十一、用buildroot自动构建initramfs 
buildroot 工具实际是一个功能强大的根文件系统构建工具，它以uclibc和busybox作为系统构建的基础，toolchain只是它构建系统的中间产品。 initramfs是一种特殊的根文件系统，当然也可以用buildroot工具自动构建，下面是构建方法的简要描述： 
（1）配置 
在buildroot的配置界面下做如下的配置： 
Package Selection for the target: 选择 
Busybox 
Run Busybox's own full installation 
Use minimal target skeleton 
Target filesystem options --> cpio the root filesystem --> comprassion method: gzip 
（2）编译 
执行 
make 
命令，进行编译。 
（3）输出 
构建好的cpio文件是 
buildroot/binaries/rootfs.i686.cpio.gz 
同一目录下还包含一个未压缩的文件：rootfs.i686.cpio 
构建目录则是 
buildroot/project_build_i686/uclibc/root 
可以在这个目录下对原始的initramfs进行修改调整，然后自己用cpio命令打包生成新的initramfs。 
（4）调整 
直接用buildroot生成的root.i686.cpio.gz作为initramfs，运行时会出现 
can't open /dev/tty1: No such file or directory 
can't open /dev/tty2: No such file or directory 
can't open /dev/tty3: No such file or directory 
错误信息的循环输出，系统不能正常运行。 
错误的原因是没有在initramfs的/dev目录下生成相应的设备文件。需要做如下的调整： 
1）在构建目录（buildroot/project_build_i686/uclibc/root）下的etc/init.d目录中新增一个初始化脚本文件S10mountfs 
#!/bin/sh 
mount -t proc proc /proc 
mount -t sysfs sysfs /sys 
mdev -s 
2）更改busybox的setuid属性，否则无法执行mount命令。在构建目录（buildroot/project_build_i686/uclibc/root）下执行 
chmod -s bin/busybox 
命令。 
这两项调整工作做完后，在构建目录（buildroot/project_build_i686/uclibc/root）下执行 
find . | cpio -o -H newc |gzip > ../initramfs.cpio.gz 
命令，重新生成initramfs的cpio打包文件。 
（5）运行效果 
运行新的initramfs，系统出现登录提示。输入用户名 root，密码为空，即可进入一个mini的linux系统。 
buildroot是功能强大、配置灵活的自动化构建工具，它的详细使用和配置方法超出了本文的范围，后续可能会专文描述，此处就从略了。
4.TINY4412】LINUX移植笔记：（3）Initramfs文件系统
2018年04月20日 22:26:26 jwy2014 阅读数：154更多
个人分类： 编程
宿主机 ： 虚拟机 Ubuntu 16.04 LTS / X64 
目标板[底板]： Tiny4412SDK - 1506 
目标板[核心板]： Tiny4412 - 1412 
LINUX内核： 4.12.0 
交叉编译器： gcc-arm-none-eabi-5_4-2016q3 
日期： 2017-7-9 11:23:51 
作者： SY
简介
    • 
Initramfs文件系统默认集成到Linux中，是一种内存文件系统
    • 
    • 
制作Initramfs文件系统，首先需要制作最小文件系统，一般都是使用BusyBox。
    • 
测试
    • 
烧录
    • 
    • 
        . 
>> 开发板
        . 
        . 
>>   TINY4412 # 
        . 
        . 
>>   TINY4412 # dfu 0 ram 0
        . 
        . 
>>   USB PHY0 Enable
        . 
        . 
 
        . 
        . 
>> 电脑
        . 
        . 
>>   root@ubuntu:/opt/linux-4.12# dfu-util -D ./arch/arm/boot/uImage 
        . 
        . 
>>   dfu-util 0.8
        . 
        . 
 
        . 
        . 
Copyright 2005-2009 Weston Schmidt, Harald Welte and OpenMoko Inc.
        . 
        . 
Copyright 2010-2014 Tormod Volden and Stefan Schmidt
        . 
        . 
This program is Free Software and has ABSOLUTELY NO WARRANTY
        . 
        . 
Please report bugs to dfu-util@lists.gnumonks.org
        . 
        . 
 
        . 
        . 
dfu-util: Invalid DFU suffix signature
        . 
        . 
dfu-util: A valid DFU suffix will be required in a future dfu-util        release!!!
        . 
        . 
Opening DFU capable USB device...
        . 
        . 
ID 18d1:4e30
        . 
        . 
Run-time device DFU version 0110
        . 
        . 
Claiming USB DFU Interface...
        . 
        . 
Setting Alternate Setting #0 ...
        . 
        . 
Determining device status: state = dfuIDLE, status = 0
        . 
        . 
dfuIDLE, continuing
        . 
        . 
DFU mode device DFU version 0110
        . 
        . 
Device returned transfer size 4096
        . 
        . 
Copying data from PC to DFU device
        . 
        . 
Download        [=========================] 100%      4792264 bytes
        . 
        . 
Download done.
        . 
        . 
state(7) = dfuMANIFEST, status(0) = No error condition is present
        . 
        . 
state(2) = dfuIDLE, status(0) = No error condition is present
        . 
        . 
Done!
        . 
        . 
root@ubuntu:/opt/linux-4.12#
        . 
        . 
 
        . 
        . 
>> 开发板
        . 
        . 
>> TINY4412 # bootm 
        . 
        . 
 
        . 
        . 
## Booting kernel from Legacy Image at 40008000 ...
        . 
        . 
 
        . 
        . 
 Image Name:   Linux-4.12.0-g193655b
        . 
        . 
 Image Type:   ARM Linux Kernel Image (uncompressed)
        . 
        . 
 Data Size:    4792200 Bytes = 4.6 MiB
        . 
        . 
 Load Address: 40008000
        . 
        . 
 Entry Point:  40008000
        . 
        . 
 Verifying Checksum ... OK
        . 
        . 
EHCI failed to shut down host controller.
        . 
        . 
 Loading Kernel Image ... OK
        . 
        . 
 
        . 
        . 
Starting kernel ...
        . 
        ◦ 1
        ◦ 2
        ◦ 3
        ◦ 4
        ◦ 5
        ◦ 6
        ◦ 7
        ◦ 8
        ◦ 9
        ◦ 10
        ◦ 11
        ◦ 12
        ◦ 13
        ◦ 14
        ◦ 15
        ◦ 16
        ◦ 17
        ◦ 18
        ◦ 19
        ◦ 20
        ◦ 21
        ◦ 22
        ◦ 23
        ◦ 24
        ◦ 25
        ◦ 26
        ◦ 27
        ◦ 28
        ◦ 29
        ◦ 30
        ◦ 31
        ◦ 32
        ◦ 33
        ◦ 34
        ◦ 35
        ◦ 36
        ◦ 37
        ◦ 38
        ◦ 39
        ◦ 40
        ◦ 41
        ◦ 42
        ◦ 43
        ◦ 44
        ◦ 45
        ◦ 46
        ◦ 47
        ◦ 48
        ◦ 1
        ◦ 2
        ◦ 3
        ◦ 4
        ◦ 5
        ◦ 6
        ◦ 7
        ◦ 8
        ◦ 9
        ◦ 10
        ◦ 11
        ◦ 12
        ◦ 13
        ◦ 14
        ◦ 15
        ◦ 16
        ◦ 17
        ◦ 18
        ◦ 19
        ◦ 20
        ◦ 21
        ◦ 22
        ◦ 23
        ◦ 24
        ◦ 25
        ◦ 26
        ◦ 27
        ◦ 28
        ◦ 29
        ◦ 30
        ◦ 31
        ◦ 32
        ◦ 33
        ◦ 34
        ◦ 35
        ◦ 36
        ◦ 37
        ◦ 38
        ◦ 39
        ◦ 40
        ◦ 41
        ◦ 42
        ◦ 43
        ◦ 44
        ◦ 45
        ◦ 46
        ◦ 47
        ◦ 48
    • 
开发板死机了，打开调试输出
    • 
    • 
        . 
root@ubuntu:/opt/linux-4.12# make menuconfig
        . 
        . 
|-- Kernel hacking
        . 
        . 
    |-- [*] Kernel low-level debugging functions (read help!) 
        . 
        . 
    |-- Kernel low-level debugging port (Use Samsung S3C UART 0 for low-level debug)
        . 
        . 
|-- [*] Early printk
        . 
        ◦ 1
        ◦ 2
        ◦ 3
        ◦ 4
        ◦ 5
        ◦ 1
        ◦ 2
        ◦ 3
        ◦ 4
        ◦ 5
    • 
烧录到开发板测试
    • 
    • 
        . 
TINY4412 # bootm
        . 
        . 
 
        . 
        . 
## Booting kernel from Legacy Image at 40008000 ...
        . 
        . 
 
        . 
        . 
 Image Name:   Linux-4.12.0-g98c9281-dirty
        . 
        . 
 Image Type:   ARM Linux Kernel Image (uncompressed)
        . 
        . 
 Data Size:    4827896 Bytes = 4.6 MiB
        . 
        . 
 Load Address: 40008000
        . 
        . 
 Entry Point:  40008000
        . 
        . 
 Verifying Checksum ... OK
        . 
        . 
 Loading Kernel Image ... OK
        . 
        . 
 
        . 
        . 
Starting kernel ...
        . 
        . 
 
        . 
        . 
Uncompressing Linux... done, booting the kernel.
        . 
        . 
 
        . 
        . 
Error: unrecognized/unsupported machine ID (r1 = 0x00001200).
        . 
        . 
        . 
Available machine support:
        . 
        . 
        . 
ID (hex)        NAME
        . 
        . 
ffffffff        Generic DT based system
        . 
        . 
ffffffff        SAMSUNG EXYNOS (Flattened Device Tree)
        . 
        . 
        . 
Please check your kernel config and/or bootloader.
        . 
        ◦ 1
        ◦ 2
        ◦ 3
        ◦ 4
        ◦ 5
        ◦ 6
        ◦ 7
        ◦ 8
        ◦ 9
        ◦ 10
        ◦ 11
        ◦ 12
        ◦ 13
        ◦ 14
        ◦ 15
        ◦ 16
        ◦ 17
        ◦ 18
        ◦ 19
        ◦ 20
        ◦ 21
        ◦ 22
        ◦ 23
        ◦ 24
        ◦ 25
        ◦ 1
        ◦ 2
        ◦ 3
        ◦ 4
        ◦ 5
        ◦ 6
        ◦ 7
        ◦ 8
        ◦ 9
        ◦ 10
        ◦ 11
        ◦ 12
        ◦ 13
        ◦ 14
        ◦ 15
        ◦ 16
        ◦ 17
        ◦ 18
        ◦ 19
        ◦ 20
        ◦ 21
        ◦ 22
        ◦ 23
        ◦ 24
        ◦ 25
        ◦ 查找资料，可能是linux的 dts文件和Initramfs文件系统没有加载。
    • 
配置 initramfs
    • 
    • 
        . 
General setup  ---> 
        . 
        . 
    [*] Initial RAM filesystem and RAM disk (initramfs/initrd) support
        . 
        . 
(/dev/rootfs)    Initramfs source file(s)
        . 
        . 
Device Drivers  --->   
        . 
        . 
    [*] Block devices  --->
        . 
        . 
    <*>   RAM block device support
        . 
        . 
        (16)    Default number of RAM disks 
        . 
        . 
        (8192)  Default RAM disk size (kbytes)
        . 
        ◦ 1
        ◦ 2
        ◦ 3
        ◦ 4
        ◦ 5
        ◦ 6
        ◦ 7
        ◦ 8
        ◦ 1
        ◦ 2
        ◦ 3
        ◦ 4
        ◦ 5
        ◦ 6
        ◦ 7
        ◦ 8
    • 
进入开发板
    • 
TINY4412 # setenv dfu_alt_info uImage ram 0x41000000 0xA00000\;exynos4412-tiny4412.dtb ram 0x42000000 0x100000
    • 
        ◦ 1
        ◦ 1
        ◦ U-BOOT加载Linux内核、Ramdisk、内核dts设备树是有一定的顺序的，查看bootm源码：
        . 
/* we overload the cmd field with our state machine info instead of a
        . 
        . 
 * function pointer */
        . 
        . 
static cmd_tbl_t cmd_bootm_sub[] = {
        . 
        . 
    U_BOOT_CMD_MKENT(start, 0, 1, (void *)BOOTM_STATE_START, "", ""),
        . 
        . 
    U_BOOT_CMD_MKENT(loados, 0, 1, (void *)BOOTM_STATE_LOADOS, "", ""),
        . 
        . 
 
        . 
        . 
#ifdef CONFIG_SYS_BOOT_RAMDISK_HIGH
        . 
        . 
 
        . 
        . 
    U_BOOT_CMD_MKENT(ramdisk, 0, 1, (void *)BOOTM_STATE_RAMDISK, "", ""),
        . 
        . 
 
        . 
        . 
#endif
        . 
        . 
 
        . 
        . 
 
        . 
        . 
#ifdef CONFIG_OF_LIBFDT
        . 
        . 
 
        . 
        . 
    U_BOOT_CMD_MKENT(fdt, 0, 1, (void *)BOOTM_STATE_FDT, "", ""),
        . 
        . 
 
        . 
        . 
#endif
        . 
        . 
 
        . 
        . 
    U_BOOT_CMD_MKENT(cmdline, 0, 1, (void *)BOOTM_STATE_OS_CMDLINE, "", ""),
        . 
        . 
    U_BOOT_CMD_MKENT(bdt, 0, 1, (void *)BOOTM_STATE_OS_BD_T, "", ""),
        . 
        . 
    U_BOOT_CMD_MKENT(prep, 0, 1, (void *)BOOTM_STATE_OS_PREP, "", ""),
        . 
        . 
    U_BOOT_CMD_MKENT(fake, 0, 1, (void *)BOOTM_STATE_OS_FAKE_GO, "", ""),
        . 
        . 
    U_BOOT_CMD_MKENT(go, 0, 1, (void *)BOOTM_STATE_OS_GO, "", ""),
        . 
        . 
};
        . 
        ◦ 1
        ◦ 2
        ◦ 3
        ◦ 4
        ◦ 5
        ◦ 6
        ◦ 7
        ◦ 8
        ◦ 9
        ◦ 10
        ◦ 11
        ◦ 12
        ◦ 13
        ◦ 14
        ◦ 15
        ◦ 16
        ◦ 17
        ◦ 18
        ◦ 19
        ◦ 20
        ◦ 21
        ◦ 22
        ◦ 23
        ◦ 24
        ◦ 25
        ◦ 1
        ◦ 2
        ◦ 3
        ◦ 4
        ◦ 5
        ◦ 6
        ◦ 7
        ◦ 8
        ◦ 9
        ◦ 10
        ◦ 11
        ◦ 12
        ◦ 13
        ◦ 14
        ◦ 15
        ◦ 16
        ◦ 17
        ◦ 18
        ◦ 19
        ◦ 20
        ◦ 21
        ◦ 22
        ◦ 23
        ◦ 24
        ◦ 25
        ◦ 首先加载loados，也就是Linxu内核。然后加载ramdisk，也就是文件系统。最后加载fdt，也就是内核设备树文件。
    • 
重新烧录
    • 
    • 
        . 
TINY4412 # bootm 41000000 - 42000000
        . 
        . 
 
        . 
        . 
## Booting kernel from Legacy Image at 41000000 ...
        . 
        . 
 
        . 
        . 
 Image Name:   Linux-4.12.0-g98c9281-dirty
        . 
        . 
 Image Type:   ARM Linux Kernel Image (uncompressed)
        . 
        . 
 Data Size:    5958168 Bytes = 5.7 MiB
        . 
        . 
 Load Address: 40008000
        . 
        . 
 Entry Point:  40008000
        . 
        . 
 Verifying Checksum ... OK
        . 
        . 
 
        . 
        . 
## Flattened Device Tree blob at 42000000
        . 
        . 
 
        . 
        . 
 Booting using the fdt blob at 0x42000000
        . 
        . 
 Loading Kernel Image ... OK
        . 
        . 
 Loading Device Tree to 4fff1000, end 4ffff56c ... OK
        . 
        . 
 
        . 
        . 
Starting kernel ...
        . 
        . 
 
        . 
        . 
Uncompressing Linux... done, booting the kernel.
        . 
        ◦ 1
        ◦ 2
        ◦ 3
        ◦ 4
        ◦ 5
        ◦ 6
        ◦ 7
        ◦ 8
        ◦ 9
        ◦ 10
        ◦ 11
        ◦ 12
        ◦ 13
        ◦ 14
        ◦ 15
        ◦ 16
        ◦ 17
        ◦ 18
        ◦ 19
        ◦ 20
        ◦ 1
        ◦ 2
        ◦ 3
        ◦ 4
        ◦ 5
        ◦ 6
        ◦ 7
        ◦ 8
        ◦ 9
        ◦ 10
        ◦ 11
        ◦ 12
        ◦ 13
        ◦ 14
        ◦ 15
        ◦ 16
        ◦ 17
        ◦ 18
        ◦ 19
        ◦ 20
        ◦ bootm中的-表示跳过 Ramdisk，又死机了!
    • 
经过排除，应该是bootargs参数未设置
    • 
setenv bootargs root=/dev/ram0 rw console=ttySAC0,115200 init=/linuxrc earlyprintk
    • 
        ◦ 1
        ◦ 1
    • 
重新烧录
    • 
    • 
        . 
TINY4412 # bootm 41000000 - 42000000
        . 
        . 
 
        . 
        . 
## Booting kernel from Legacy Image at 41000000 ...
        . 
        . 
 
        . 
        . 
 Image Name:   Linux-4.12.0-g98c9281-dirty
        . 
        . 
 Image Type:   ARM Linux Kernel Image (uncompressed)
        . 
        . 
 Data Size:    5958168 Bytes = 5.7 MiB
        . 
        . 
 Load Address: 40008000
        . 
        . 
 Entry Point:  40008000
        . 
        . 
 Verifying Checksum ... OK
        . 
        . 
 
        . 
        . 
## Flattened Device Tree blob at 42000000
        . 
        . 
 
        . 
        . 
 Booting using the fdt blob at 0x42000000
        . 
        . 
 Loading Kernel Image ... OK
        . 
        . 
 Loading Device Tree to 4fff1000, end 4ffff56c ... OK
        . 
        . 
 
        . 
        . 
Starting kernel ...
        . 
        . 
 
        . 
        . 
Uncompressing Linux... done, booting the kernel.
        . 
        . 
[    0.000000] Booting Linux on physical CPU 0xa00
        . 
        . 
[    0.000000] Linux version 4.12.0-g98c9281-dirty (root@ubuntu) (gcc version 5.4.1 20160919 (release) [ARM/embedded-5-branch revision 240496] (GNU Tools for ARM Embedded Processors) ) #4 SMP PREEMPT Sun Jul 9 10:00:46 CST 2017
        . 
        . 
[    0.000000] CPU: ARMv7 Processor [413fc090] revision 0 (ARMv7), cr=10c5387d
        . 
        . 
[    0.000000] CPU: PIPT / VIPT nonaliasing data cache, VIPT aliasing instruction cache
        . 
        . 
[    0.000000] OF: fdt: Machine model: FriendlyARM TINY4412 board based on Exynos4412
        . 
        . 
[    0.000000] bootconsole [earlycon0] enabled
        . 
        . 
[    0.000000] Memory policy: Data cache writealloc
        . 
        . 
[    0.000000] cma: Reserved 96 MiB at 0x7a000000
        . 
        . 
[    0.000000] Samsung CPU ID: 0xe4412011
        . 
        . 
[    0.000000] percpu: Embedded 16 pages/cpu @ef78f000 s34968 r8192 d22376 u65536
        . 
        . 
[    0.000000] Built 1 zonelists in Zone order, mobility grouping on.  Total pages: 260608
        . 
        . 
[    0.000000] Kernel command line: root=/dev/ram0 rw console=ttySAC0,115200 init=/linuxrc earlyprintk
        . 
        . 
[    0.000000] PID hash table entries: 4096 (order: 2, 16384 bytes)
        . 
        . 
[    0.000000] Dentry cache hash table entries: 131072 (order: 7, 524288 bytes)
        . 
        . 
[    0.000000] Inode-cache hash table entries: 65536 (order: 6, 262144 bytes)
        . 
        . 
[    0.000000] Memory: 927808K/1048576K available (7168K kernel code, 334K rwdata, 2448K rodata, 2048K init, 326K bss, 22464K reserved, 98304K cma-reserved, 163840K highmem)
        . 
        . 
[    0.000000] Virtual kernel memory layout:
        . 
        . 
[    0.000000]     vector  : 0xffff0000 - 0xffff1000   (   4 kB)
        . 
        . 
[    0.000000]     fixmap  : 0xffc00000 - 0xfff00000   (3072 kB)
        . 
        . 
[    0.000000]     vmalloc : 0xf0800000 - 0xff800000   ( 240 MB)
        . 
        . 
[    0.000000]     lowmem  : 0xc0000000 - 0xf0000000   ( 768 MB)
        . 
        . 
[    0.000000]     pkmap   : 0xbfe00000 - 0xc0000000   (   2 MB)
        . 
        . 
[    0.000000]     modules : 0xbf000000 - 0xbfe00000   (  14 MB)
        . 
        . 
[    0.000000]       .text : 0xc0008000 - 0xc0800000   (8160 kB)
        . 
        . 
[    0.000000]       .init : 0xc0b00000 - 0xc0d00000   (2048 kB)
        . 
        . 
[    0.000000]       .data : 0xc0d00000 - 0xc0d538d0   ( 335 kB)
        . 
        . 
[    0.000000]        .bss : 0xc0d5b0e4 - 0xc0daca20   ( 327 kB)
        . 
        . 
[    0.000000] SLUB: HWalign=64, Order=0-3, MinObjects=0, CPUs=4, Nodes=1
        . 
        . 
[    0.000000] Preemptible hierarchical RCU implementation.
        . 
        . 
[    0.000000]  RCU restricting CPUs from NR_CPUS=8 to nr_cpu_ids=4.
        . 
        . 
[    0.000000] RCU: Adjusting geometry for rcu_fanout_leaf=16, nr_cpu_ids=4
        . 
        . 
[    0.000000] NR_IRQS:16 nr_irqs:16 16
        . 
        . 
[    0.000000] L2C: platform modifies aux control register: 0x02070000 -> 0x3e470001
        . 
        . 
[    0.000000] L2C: platform provided aux values permit register corruption.
        . 
        . 
[    0.000000] L2C: DT/platform modifies aux control register: 0x02070000 -> 0x3e470001
        . 
        . 
[    0.000000] L2C-310 enabling early BRESP for Cortex-A9
        . 
        . 
[    0.000000] L2C-310: enabling full line of zeros but not enabled in Cortex-A9
        . 
        . 
[    0.000000] L2C-310 dynamic clock gating enabled, standby mode enabled
        . 
        . 
[    0.000000] L2C-310 cache controller enabled, 16 ways, 1024 kB
        . 
        . 
[    0.000000] L2C-310: CACHE_ID 0x4100c4c8, AUX_CTRL 0x4e470001
        . 
        . 
[    0.000000] Exynos4x12 clocks: sclk_apll = 700000000, sclk_mpll = 800000000
        . 
        . 
[    0.000000]  sclk_epll = 96035156, sclk_vpll = 108035156, arm_clk = 1400000000
        . 
        . 
[    0.000000] Switching to timer-based delay loop, resolution 41ns
        . 
        . 
[    0.000000] clocksource: mct-frc: mask: 0xffffffff max_cycles: 0xffffffff, max_idle_ns: 79635851949 ns
        . 
        . 
[    0.000003] sched_clock: 32 bits at 24MHz, resolution 41ns, wraps every 89478484971ns
        . 
        . 
[    0.008061] Console: colour dummy device 80x30
        . 
        . 
[    0.012419] Calibrating delay loop (skipped), value calculated using timer frequency.. 48.00 BogoMIPS (lpj=240000)
        . 
        . 
[    0.022835] pid_max: default: 32768 minimum: 301
        . 
        . 
[    0.027578] Mount-cache hash table entries: 2048 (order: 1, 8192 bytes)
        . 
        . 
[    0.034199] Mountpoint-cache hash table entries: 2048 (order: 1, 8192 bytes)
        . 
        . 
[    0.041626] CPU: Testing write buffer coherency: ok
        . 
        . 
[    0.046603] CPU0: thread -1, cpu 0, socket 10, mpidr 80000a00
        . 
        . 
[    0.084752] Setting up static identity map for 0x40100000 - 0x40100060
        . 
        . 
[    0.144729] smp: Bringing up secondary CPUs ...
        . 
        . 
[    0.204790] CPU1: thread -1, cpu 1, socket 10, mpidr 80000a01
        . 
        . 
[    0.284781] CPU2: thread -1, cpu 2, socket 10, mpidr 80000a02
        . 
        . 
[    0.364780] CPU3: thread -1, cpu 3, socket 10, mpidr 80000a03
        . 
        . 
[    0.364825] smp: Brought up 1 node, 4 CPUs
        . 
        . 
[    0.386283] SMP: Total of 4 processors activated (192.00 BogoMIPS).
        . 
        . 
[    0.392612] CPU: All CPU(s) started in SVC mode.
        . 
        . 
[    0.397892] devtmpfs: initialized
        . 
        . 
[    0.407787] VFP support v0.3: implementor 41 architecture 3 part 30 variant 9 rev 4
        . 
        . 
[    0.415523] clocksource: jiffies: mask: 0xffffffff max_cycles: 0xffffffff, max_idle_ns: 19112604462750000 ns
        . 
        . 
[    0.425273] futex hash table entries: 1024 (order: 4, 65536 bytes)
        . 
        . 
[    0.434382] pinctrl core: initialized pinctrl subsystem
        . 
        . 
[    0.440068] /lcd0-power-domain@10023C80 has as child subdomain: /tv-power-domain@10023C20.
        . 
        . 
[    0.448811] NET: Registered protocol family 16
        . 
        . 
[    0.454479] DMA: preallocated 256 KiB pool for atomic coherent allocations
        . 
        . 
[    0.462304] cpuidle: using governor menu
        . 
        . 
[    0.485066] SCSI subsystem initialized
        . 
        . 
[    0.488871] usbcore: registered new interface driver usbfs
        . 
        . 
[    0.494309] usbcore: registered new interface driver hub
        . 
        . 
[    0.499717] usbcore: registered new device driver usb
        . 
        . 
[    0.505315] Advanced Linux Sound Architecture Driver Initialized.
        . 
        . 
[    0.511989] clocksource: Switched to clocksource mct-frc
        . 
        . 
[    0.524402] missing cooling_device property
        . 
        . 
[    0.528489] failed to build thermal zone cpu-thermal: -2
        . 
        . 
[    0.533947] NET: Registered protocol family 2
        . 
        . 
[    0.538640] TCP established hash table entries: 8192 (order: 3, 32768 bytes)
        . 
        . 
[    0.545666] TCP bind hash table entries: 8192 (order: 5, 163840 bytes)
        . 
        . 
[    0.552320] TCP: Hash tables configured (established 8192 bind 8192)
        . 
        . 
[    0.558708] UDP hash table entries: 512 (order: 2, 24576 bytes)
        . 
        . 
[    0.564647] UDP-Lite hash table entries: 512 (order: 2, 24576 bytes)
        . 
        . 
[    0.571150] NET: Registered protocol family 1
        . 
        . 
[    0.575722] RPC: Registered named UNIX socket transport module.
        . 
        . 
[    0.581549] RPC: Registered udp transport module.
        . 
        . 
[    0.586342] RPC: Registered tcp transport module.
        . 
        . 
[    0.591097] RPC: Registered tcp NFSv4.1 backchannel transport module.
        . 
        . 
[    0.648336] audit: initializing netlink subsys (disabled)
        . 
        . 
[    0.653932] audit: type=2000 audit(0.651:1): state=initialized audit_enabled=0 res=1
        . 
        . 
[    0.654027] workingset: timestamp_bits=30 max_order=18 bucket_order=0
        . 
        . 
[    0.658831] NFS: Registering the id_resolver key type
        . 
        . 
[    0.658844] Key type id_resolver registered
        . 
        . 
[    0.658846] Key type id_legacy registered
        . 
        . 
[    0.658867] romfs: ROMFS MTD (C) 2007 Red Hat, Inc.
        . 
        . 
[    0.662060] bounce: pool size: 64 pages
        . 
        . 
[    0.662087] Block layer SCSI generic (bsg) driver version 0.4 loaded (major 248)
        . 
        . 
[    0.662092] io scheduler noop registered
        . 
        . 
[    0.662094] io scheduler deadline registered
        . 
        . 
[    0.662109] io scheduler cfq registered (default)
        . 
        . 
[    0.662112] io scheduler mq-deadline registered
        . 
        . 
[    0.662114] io scheduler kyber registered
        . 
        . 
[    0.672233] dma-pl330 12680000.pdma: Loaded driver for PL330 DMAC-141330
        . 
        . 
[    0.672239] dma-pl330 12680000.pdma:         DBUFF-32x4bytes Num_Chans-8 Num_Peri-32 Num_Events-32
        . 
        . 
[    0.674089] dma-pl330 12690000.pdma: Loaded driver for PL330 DMAC-141330
        . 
        . 
[    0.674094] dma-pl330 12690000.pdma:         DBUFF-32x4bytes Num_Chans-8 Num_Peri-32 Num_Events-32
        . 
        . 
[    0.674792] dma-pl330 12850000.mdma: Loaded driver for PL330 DMAC-141330
        . 
        . 
[    0.674797] dma-pl330 12850000.mdma:         DBUFF-64x8bytes Num_Chans-8 Num_Peri-1 Num_Events-32
        . 
        . 
[    0.772781] Serial: 8250/16550 driver, 4 ports, IRQ sharing disabled
        . 
        . 
[    0.780286] 13800000.serial: ttySAC0 at MMIO 0x13800000 (irq = 46, base_baud = 0) is a S3C6400/10
        . 
        . 
[    0.789142] console [ttySAC0] enabled
        . 
        . 
[    0.789142] console [ttySAC0] enabled
        . 
        . 
[    0.796470] bootconsole [earlycon0] disabled
        . 
        . 
[    0.796470] bootconsole [earlycon0] disabled
        . 
        . 
[    0.805305] 13810000.serial: ttySAC1 at MMIO 0x13810000 (irq = 47, base_baud = 0) is a S3C6400/10
        . 
        . 
[    0.805628] 13820000.serial: ttySAC2 at MMIO 0x13820000 (irq = 48, base_baud = 0) is a S3C6400/10
        . 
        . 
[    0.805941] 13830000.serial: ttySAC3 at MMIO 0x13830000 (irq = 49, base_baud = 0) is a S3C6400/10
        . 
        . 
[    0.818642] brd: module loaded
        . 
        . 
[    0.824203] loop: module loaded
        . 
        . 
[    0.824920] libphy: Fixed MDIO Bus: probed
        . 
        . 
[    0.825067] usbcore: registered new interface driver r8152
        . 
        . 
[    0.825301] usbcore: registered new interface driver asix
        . 
        . 
[    0.830687] usbcore: registered new interface driver ax88179_178a
        . 
        . 
[    0.836791] usbcore: registered new interface driver cdc_ether
        . 
        . 
[    0.842616] usbcore: registered new interface driver smsc75xx
        . 
        . 
[    0.848311] usbcore: registered new interface driver smsc95xx
        . 
        . 
[    0.854048] usbcore: registered new interface driver net1080
        . 
        . 
[    0.859667] usbcore: registered new interface driver cdc_subset
        . 
        . 
[    0.865596] usbcore: registered new interface driver zaurus
        . 
        . 
[    0.871136] usbcore: registered new interface driver cdc_ncm
        . 
        . 
[    0.877270] ehci_hcd: USB 2.0 'Enhanced' Host Controller (EHCI) Driver
        . 
        . 
[    0.883280] ehci-exynos: EHCI EXYNOS driver
        . 
        . 
[    0.887524] ohci_hcd: USB 1.1 'Open' Host Controller (OHCI) Driver
        . 
        . 
[    0.893608] ohci-exynos: OHCI EXYNOS driver
        . 
        . 
[    0.898067] usbcore: registered new interface driver usb-storage
        . 
        . 
[    0.904447] s3c-rtc 10070000.rtc: failed to find rtc source clock
        . 
        . 
[    0.909833] s3c-rtc: probe of 10070000.rtc failed with error -2
        . 
        . 
[    0.915897] i2c /dev entries driver
        . 
        . 
[    0.920839] s3c2410-wdt 10060000.watchdog: watchdog inactive, reset disabled, irq disabled
        . 
        . 
[    0.927835] device-mapper: ioctl: 4.35.0-ioctl (2016-06-23) initialised: dm-devel@redhat.com
        . 
        . 
[    0.937359] sdhci: Secure Digital Host Controller Interface driver
        . 
        . 
[    0.942039] sdhci: Copyright(c) Pierre Ossman
        . 
        . 
[    0.946633] s3c-sdhci 12530000.sdhci: clock source 2: mmc_busclk.2 (20000000 Hz)
        . 
        . 
[    1.012035] mmc0: SDHCI controller on samsung-hsmmc [12530000.sdhci] using ADMA
        . 
        . 
[    1.012199] Synopsys Designware Multimedia Card Interface Driver
        . 
        . 
[    1.016727] s5p-secss 10830000.sss: s5p-sss driver registered
        . 
        . 
[    1.017187] usbcore: registered new interface driver usbhid
        . 
        . 
[    1.017242] usbhid: USB HID core driver
        . 
        . 
[    1.023542] NET: Registered protocol family 10
        . 
        . 
[    1.024176] Segment Routing with IPv6
        . 
        . 
[    1.026223] sit: IPv6, IPv4 and MPLS over IPv4 tunneling driver
        . 
        . 
[    1.032452] NET: Registered protocol family 17
        . 
        . 
[    1.036529] NET: Registered protocol family 15
        . 
        . 
[    1.040987] Key type dns_resolver registered
        . 
        . 
[    1.045363] Registering SWP/SWPB emulation handler
        . 
        . 
[    1.061029] mmc0: new high speed SDHC card at address 0007
        . 
        . 
[    1.061408] mmcblk0: mmc0:0007 SD16G 14.5 GiB 
        . 
        . 
[    1.061730] hctosys: unable to open rtc device (rtc0)
        . 
        . 
[    1.065527] ALSA device list:
        . 
        . 
[    1.067854]   No soundcards found.
        . 
        . 
[    1.072546] List of all partitions:
        . 
        . 
[    1.074716] 0100            8192 ram0 
        . 
        . 
[    1.074719]  (driver?)
        . 
        . 
[    1.080790] 0101            8192 ram1 
        . 
        . 
[    1.080793]  (driver?)
        . 
        . 
[    1.086867] 0102            8192 ram2 
        . 
        . 
[    1.086870]  (driver?)
        . 
        . 
[    1.092958] 0103            8192 ram3 
        . 
        . 
[    1.092961]  (driver?)
        . 
        . 
[    1.099018] 0104            8192 ram4 
        . 
        . 
[    1.099021]  (driver?)
        . 
        . 
[    1.105095] 0105            8192 ram5 
        . 
        . 
[    1.105097]  (driver?)
        . 
        . 
[    1.111170] 0106            8192 ram6 
        . 
        . 
[    1.111173]  (driver?)
        . 
        . 
[    1.117247] 0107            8192 ram7 
        . 
        . 
[    1.117249]  (driver?)
        . 
        . 
[    1.123415] 0108            8192 ram8 
        . 
        . 
[    1.123418]  (driver?)
        . 
        . 
[    1.129398] 0109            8192 ram9 
        . 
        . 
[    1.129400]  (driver?)
        . 
        . 
[    1.135475] 010a            8192 ram10 
        . 
        . 
[    1.135477]  (driver?)
        . 
        . 
[    1.141637] 010b            8192 ram11 
        . 
        . 
[    1.141639]  (driver?)
        . 
        . 
[    1.147806] 010c            8192 ram12 
        . 
        . 
[    1.147811]  (driver?)
        . 
        . 
[    1.153993] 010d            8192 ram13 
        . 
        . 
[    1.153998]  (driver?)
        . 
        . 
[    1.160129] 010e            8192 ram14 
        . 
        . 
[    1.160133]  (driver?)
        . 
        . 
[    1.166293] 010f            8192 ram15 
        . 
        . 
[    1.166299]  (driver?)
        . 
        . 
[    1.172484] b300        15179776 mmcblk0 
        . 
        . 
[    1.172490]  driver: mmcblk
        . 
        . 
[    1.179221] No filesystem could mount root, tried: 
        . 
        . 
[    1.179225]  ext3
        . 
        . 
[    1.184109]  ext4
        . 
        . 
[    1.185990]  ext2
        . 
        . 
[    1.187900]  cramfs
        . 
        . 
[    1.189809]  vfat
        . 
        . 
[    1.191893]  msdos
        . 
        . 
[    1.193823]  romfs
        . 
        . 
[    1.195799] 
        . 
        . 
[    1.199276] Kernel panic - not syncing: VFS: Unable to mount root fs on unknown-block(1,0)
        . 
        . 
[    1.207523] CPU: 2 PID: 1 Comm: swapper/0 Not tainted 4.12.0-g98c9281-dirty #4
        . 
        . 
[    1.214722] Hardware name: SAMSUNG EXYNOS (Flattened Device Tree)
        . 
        . 
[    1.220835] [<c010f268>] (unwind_backtrace) from [<c010bcb0>] (show_stack+0x10/0x14)
        . 
        . 
[    1.228538] [<c010bcb0>] (show_stack) from [<c034b068>] (dump_stack+0x88/0x9c)
        . 
        . 
[    1.235746] [<c034b068>] (dump_stack) from [<c01a7f88>] (panic+0xe8/0x268)
        . 
        . 
[    1.242603] [<c01a7f88>] (panic) from [<c0b012dc>] (mount_block_root+0x220/0x268)
        . 
        . 
[    1.250060] [<c0b012dc>] (mount_block_root) from [<c0b01524>] (mount_root+0x114/0x11c)
        . 
        . 
[    1.257957] [<c0b01524>] (mount_root) from [<c0b016c8>] (prepare_namespace+0x19c/0x1e0)
        . 
        . 
[    1.265944] [<c0b016c8>] (prepare_namespace) from [<c0b00e8c>] (kernel_init_freeable+0x1f4/0x200)
        . 
        . 
[    1.274804] [<c0b00e8c>] (kernel_init_freeable) from [<c0744b30>] (kernel_init+0x8/0x110)
        . 
        . 
[    1.282958] [<c0744b30>] (kernel_init) from [<c0108578>] (ret_from_fork+0x14/0x3c)
        . 
        . 
[    1.290513] CPU3: stopping
        . 
        . 
[    1.293193] CPU: 3 PID: 0 Comm: swapper/3 Not tainted 4.12.0-g98c9281-dirty #4
        . 
        . 
[    1.300393] Hardware name: SAMSUNG EXYNOS (Flattened Device Tree)
        . 
        . 
[    1.306486] [<c010f268>] (unwind_backtrace) from [<c010bcb0>] (show_stack+0x10/0x14)
        . 
        . 
[    1.314206] [<c010bcb0>] (show_stack) from [<c034b068>] (dump_stack+0x88/0x9c)
        . 
        . 
[    1.321408] [<c034b068>] (dump_stack) from [<c010e2b8>] (handle_IPI+0x1a0/0x1b0)
        . 
        . 
[    1.328785] [<c010e2b8>] (handle_IPI) from [<c01014e8>] (gic_handle_irq+0x94/0x98)
        . 
        . 
[    1.336336] [<c01014e8>] (gic_handle_irq) from [<c010c78c>] (__irq_svc+0x6c/0xa8)
        . 
        . 
[    1.343795] Exception stack(0xef0a1f78 to 0xef0a1fc0)
        . 
        . 
[    1.348830] 1f60:                                                       00000001 00000000
        . 
        . 
[    1.356994] 1f80: 00000000 c0115740 ef0a0000 c0d054c0 c0d05464 c0c7b138 ef0a1fd0 00000000
        . 
        . 
[    1.365153] 1fa0: 00000000 00000000 f081b640 ef0a1fc8 c0108fd4 c0108fd8 60000013 ffffffff
        . 
        . 
[    1.373314] [<c010c78c>] (__irq_svc) from [<c0108fd8>] (arch_cpu_idle+0x38/0x3c)
        . 
        . 
[    1.380701] [<c0108fd8>] (arch_cpu_idle) from [<c015175c>] (do_idle+0x16c/0x200)
        . 
        . 
[    1.388072] [<c015175c>] (do_idle) from [<c0151aa8>] (cpu_startup_entry+0x18/0x1c)
        . 
        . 
[    1.395621] [<c0151aa8>] (cpu_startup_entry) from [<4010158c>] (0x4010158c)
        . 
        . 
[    1.402559] CPU0: stopping
        . 
        . 
[    1.405253] CPU: 0 PID: 0 Comm: swapper/0 Not tainted 4.12.0-g98c9281-dirty #4
        . 
        . 
[    1.412453] Hardware name: SAMSUNG EXYNOS (Flattened Device Tree)
        . 
        . 
[    1.418545] [<c010f268>] (unwind_backtrace) from [<c010bcb0>] (show_stack+0x10/0x14)
        . 
        . 
[    1.426264] [<c010bcb0>] (show_stack) from [<c034b068>] (dump_stack+0x88/0x9c)
        . 
        . 
[    1.433466] [<c034b068>] (dump_stack) from [<c010e2b8>] (handle_IPI+0x1a0/0x1b0)
        . 
        . 
[    1.440844] [<c010e2b8>] (handle_IPI) from [<c01014e8>] (gic_handle_irq+0x94/0x98)
        . 
        . 
[    1.448393] [<c01014e8>] (gic_handle_irq) from [<c010c78c>] (__irq_svc+0x6c/0xa8)
        . 
        . 
[    1.455853] Exception stack(0xc0d01f40 to 0xc0d01f88)
        . 
        . 
[    1.460894] 1f40: 00000001 00000000 00000000 c0115740 c0d00000 c0d054c0 c0d05464 c0c7b138
        . 
        . 
[    1.469053] 1f60: c0d01f98 00000000 00000000 00000000 000000e6 c0d01f90 c0108fd4 c0108fd8
        . 
        . 
[    1.477207] 1f80: 60000013 ffffffff
        . 
        . 
[    1.480685] [<c010c78c>] (__irq_svc) from [<c0108fd8>] (arch_cpu_idle+0x38/0x3c)
        . 
        . 
[    1.488067] [<c0108fd8>] (arch_cpu_idle) from [<c015175c>] (do_idle+0x16c/0x200)
        . 
        . 
[    1.495443] [<c015175c>] (do_idle) from [<c0151aa8>] (cpu_startup_entry+0x18/0x1c)
        . 
        . 
[    1.502995] [<c0151aa8>] (cpu_startup_entry) from [<c0b00c90>] (start_kernel+0x38c/0x394)
        . 
        . 
[    1.511151] [<c0b00c90>] (start_kernel) from [<4000807c>] (0x4000807c)
        . 
        . 
[    1.517656] CPU1: stopping
        . 
        . 
[    1.520349] CPU: 1 PID: 0 Comm: swapper/1 Not tainted 4.12.0-g98c9281-dirty #4
        . 
        . 
[    1.527549] Hardware name: SAMSUNG EXYNOS (Flattened Device Tree)
        . 
        . 
[    1.533640] [<c010f268>] (unwind_backtrace) from [<c010bcb0>] (show_stack+0x10/0x14)
        . 
        . 
[    1.541361] [<c010bcb0>] (show_stack) from [<c034b068>] (dump_stack+0x88/0x9c)
        . 
        . 
[    1.548563] [<c034b068>] (dump_stack) from [<c010e2b8>] (handle_IPI+0x1a0/0x1b0)
        . 
        . 
[    1.555940] [<c010e2b8>] (handle_IPI) from [<c01014e8>] (gic_handle_irq+0x94/0x98)
        . 
        . 
[    1.563491] [<c01014e8>] (gic_handle_irq) from [<c010c78c>] (__irq_svc+0x6c/0xa8)
        . 
        . 
[    1.570950] Exception stack(0xef09df78 to 0xef09dfc0)
        . 
        . 
[    1.575986] df60:                                                       00000001 00000000
        . 
        . 
[    1.584150] df80: 00000000 c0115740 ef09c000 c0d054c0 c0d05464 c0c7b138 ef09dfd0 00000000
        . 
        . 
[    1.592309] dfa0: 00000000 00000000 ef7a48dc ef09dfc8 c0108fd4 c0108fd8 60000013 ffffffff
        . 
        . 
[    1.600469] [<c010c78c>] (__irq_svc) from [<c0108fd8>] (arch_cpu_idle+0x38/0x3c)
        . 
        . 
[    1.607851] [<c0108fd8>] (arch_cpu_idle) from [<c015175c>] (do_idle+0x16c/0x200)
        . 
        . 
[    1.615227] [<c015175c>] (do_idle) from [<c0151aa8>] (cpu_startup_entry+0x18/0x1c)
        . 
        . 
[    1.622776] [<c0151aa8>] (cpu_startup_entry) from [<4010158c>] (0x4010158c)
        . 
        . 
[    1.629727] ---[ end Kernel panic - not syncing: VFS: Unable to mount root fs on unknown-block(1,0)
        . 
        ◦ 1
        ◦ 2
        ◦ 3
        ◦ 4
        ◦ 5
        ◦ 6
        ◦ 7
        ◦ 8
        ◦ 9
        ◦ 10
        ◦ 11
        ◦ 12
        ◦ 13
        ◦ 14
        ◦ 15
        ◦ 16
        ◦ 17
        ◦ 18
        ◦ 19
        ◦ 20
        ◦ 21
        ◦ 22
        ◦ 23
        ◦ 24
        ◦ 25
        ◦ 26
        ◦ 27
        ◦ 28
        ◦ 29
        ◦ 30
        ◦ 31
        ◦ 32
        ◦ 33
        ◦ 34
        ◦ 35
        ◦ 36
        ◦ 37
        ◦ 38
        ◦ 39
        ◦ 40
        ◦ 41
        ◦ 42
        ◦ 43
        ◦ 44
        ◦ 45
        ◦ 46
        ◦ 47
        ◦ 48
        ◦ 49
        ◦ 50
        ◦ 51
        ◦ 52
        ◦ 53
        ◦ 54
        ◦ 55
        ◦ 56
        ◦ 57
        ◦ 58
        ◦ 59
        ◦ 60
        ◦ 61
        ◦ 62
        ◦ 63
        ◦ 64
        ◦ 65
        ◦ 66
        ◦ 67
        ◦ 68
        ◦ 69
        ◦ 70
        ◦ 71
        ◦ 72
        ◦ 73
        ◦ 74
        ◦ 75
        ◦ 76
        ◦ 77
        ◦ 78
        ◦ 79
        ◦ 80
        ◦ 81
        ◦ 82
        ◦ 83
        ◦ 84
        ◦ 85
        ◦ 86
        ◦ 87
        ◦ 88
        ◦ 89
        ◦ 90
        ◦ 91
        ◦ 92
        ◦ 93
        ◦ 94
        ◦ 95
        ◦ 96
        ◦ 97
        ◦ 98
        ◦ 99
        ◦ 100
        ◦ 101
        ◦ 102
        ◦ 103
        ◦ 104
        ◦ 105
        ◦ 106
        ◦ 107
        ◦ 108
        ◦ 109
        ◦ 110
        ◦ 111
        ◦ 112
        ◦ 113
        ◦ 114
        ◦ 115
        ◦ 116
        ◦ 117
        ◦ 118
        ◦ 119
        ◦ 120
        ◦ 121
        ◦ 122
        ◦ 123
        ◦ 124
        ◦ 125
        ◦ 126
        ◦ 127
        ◦ 128
        ◦ 129
        ◦ 130
        ◦ 131
        ◦ 132
        ◦ 133
        ◦ 134
        ◦ 135
        ◦ 136
        ◦ 137
        ◦ 138
        ◦ 139
        ◦ 140
        ◦ 141
        ◦ 142
        ◦ 143
        ◦ 144
        ◦ 145
        ◦ 146
        ◦ 147
        ◦ 148
        ◦ 149
        ◦ 150
        ◦ 151
        ◦ 152
        ◦ 153
        ◦ 154
        ◦ 155
        ◦ 156
        ◦ 157
        ◦ 158
        ◦ 159
        ◦ 160
        ◦ 161
        ◦ 162
        ◦ 163
        ◦ 164
        ◦ 165
        ◦ 166
        ◦ 167
        ◦ 168
        ◦ 169
        ◦ 170
        ◦ 171
        ◦ 172
        ◦ 173
        ◦ 174
        ◦ 175
        ◦ 176
        ◦ 177
        ◦ 178
        ◦ 179
        ◦ 180
        ◦ 181
        ◦ 182
        ◦ 183
        ◦ 184
        ◦ 185
        ◦ 186
        ◦ 187
        ◦ 188
        ◦ 189
        ◦ 190
        ◦ 191
        ◦ 192
        ◦ 193
        ◦ 194
        ◦ 195
        ◦ 196
        ◦ 197
        ◦ 198
        ◦ 199
        ◦ 200
        ◦ 201
        ◦ 202
        ◦ 203
        ◦ 204
        ◦ 205
        ◦ 206
        ◦ 207
        ◦ 208
        ◦ 209
        ◦ 210
        ◦ 211
        ◦ 212
        ◦ 213
        ◦ 214
        ◦ 215
        ◦ 216
        ◦ 217
        ◦ 218
        ◦ 219
        ◦ 220
        ◦ 221
        ◦ 222
        ◦ 223
        ◦ 224
        ◦ 225
        ◦ 226
        ◦ 227
        ◦ 228
        ◦ 229
        ◦ 230
        ◦ 231
        ◦ 232
        ◦ 233
        ◦ 234
        ◦ 235
        ◦ 236
        ◦ 237
        ◦ 238
        ◦ 239
        ◦ 240
        ◦ 241
        ◦ 242
        ◦ 243
        ◦ 244
        ◦ 245
        ◦ 246
        ◦ 247
        ◦ 248
        ◦ 249
        ◦ 250
        ◦ 251
        ◦ 252
        ◦ 253
        ◦ 254
        ◦ 255
        ◦ 256
        ◦ 257
        ◦ 258
        ◦ 259
        ◦ 260
        ◦ 261
        ◦ 262
        ◦ 263
        ◦ 264
        ◦ 265
        ◦ 266
        ◦ 267
        ◦ 268
        ◦ 269
        ◦ 270
        ◦ 271
        ◦ 272
        ◦ 273
        ◦ 274
        ◦ 275
        ◦ 276
        ◦ 277
        ◦ 278
        ◦ 279
        ◦ 280
        ◦ 281
        ◦ 282
        ◦ 283
        ◦ 284
        ◦ 285
        ◦ 286
        ◦ 1
        ◦ 2
        ◦ 3
        ◦ 4
        ◦ 5
        ◦ 6
        ◦ 7
        ◦ 8
        ◦ 9
        ◦ 10
        ◦ 11
        ◦ 12
        ◦ 13
        ◦ 14
        ◦ 15
        ◦ 16
        ◦ 17
        ◦ 18
        ◦ 19
        ◦ 20
        ◦ 21
        ◦ 22
        ◦ 23
        ◦ 24
        ◦ 25
        ◦ 26
        ◦ 27
        ◦ 28
        ◦ 29
        ◦ 30
        ◦ 31
        ◦ 32
        ◦ 33
        ◦ 34
        ◦ 35
        ◦ 36
        ◦ 37
        ◦ 38
        ◦ 39
        ◦ 40
        ◦ 41
        ◦ 42
        ◦ 43
        ◦ 44
        ◦ 45
        ◦ 46
        ◦ 47
        ◦ 48
        ◦ 49
        ◦ 50
        ◦ 51
        ◦ 52
        ◦ 53
        ◦ 54
        ◦ 55
        ◦ 56
        ◦ 57
        ◦ 58
        ◦ 59
        ◦ 60
        ◦ 61
        ◦ 62
        ◦ 63
        ◦ 64
        ◦ 65
        ◦ 66
        ◦ 67
        ◦ 68
        ◦ 69
        ◦ 70
        ◦ 71
        ◦ 72
        ◦ 73
        ◦ 74
        ◦ 75
        ◦ 76
        ◦ 77
        ◦ 78
        ◦ 79
        ◦ 80
        ◦ 81
        ◦ 82
        ◦ 83
        ◦ 84
        ◦ 85
        ◦ 86
        ◦ 87
        ◦ 88
        ◦ 89
        ◦ 90
        ◦ 91
        ◦ 92
        ◦ 93
        ◦ 94
        ◦ 95
        ◦ 96
        ◦ 97
        ◦ 98
        ◦ 99
        ◦ 100
        ◦ 101
        ◦ 102
        ◦ 103
        ◦ 104
        ◦ 105
        ◦ 106
        ◦ 107
        ◦ 108
        ◦ 109
        ◦ 110
        ◦ 111
        ◦ 112
        ◦ 113
        ◦ 114
        ◦ 115
        ◦ 116
        ◦ 117
        ◦ 118
        ◦ 119
        ◦ 120
        ◦ 121
        ◦ 122
        ◦ 123
        ◦ 124
        ◦ 125
        ◦ 126
        ◦ 127
        ◦ 128
        ◦ 129
        ◦ 130
        ◦ 131
        ◦ 132
        ◦ 133
        ◦ 134
        ◦ 135
        ◦ 136
        ◦ 137
        ◦ 138
        ◦ 139
        ◦ 140
        ◦ 141
        ◦ 142
        ◦ 143
        ◦ 144
        ◦ 145
        ◦ 146
        ◦ 147
        ◦ 148
        ◦ 149
        ◦ 150
        ◦ 151
        ◦ 152
        ◦ 153
        ◦ 154
        ◦ 155
        ◦ 156
        ◦ 157
        ◦ 158
        ◦ 159
        ◦ 160
        ◦ 161
        ◦ 162
        ◦ 163
        ◦ 164
        ◦ 165
        ◦ 166
        ◦ 167
        ◦ 168
        ◦ 169
        ◦ 170
        ◦ 171
        ◦ 172
        ◦ 173
        ◦ 174
        ◦ 175
        ◦ 176
        ◦ 177
        ◦ 178
        ◦ 179
        ◦ 180
        ◦ 181
        ◦ 182
        ◦ 183
        ◦ 184
        ◦ 185
        ◦ 186
        ◦ 187
        ◦ 188
        ◦ 189
        ◦ 190
        ◦ 191
        ◦ 192
        ◦ 193
        ◦ 194
        ◦ 195
        ◦ 196
        ◦ 197
        ◦ 198
        ◦ 199
        ◦ 200
        ◦ 201
        ◦ 202
        ◦ 203
        ◦ 204
        ◦ 205
        ◦ 206
        ◦ 207
        ◦ 208
        ◦ 209
        ◦ 210
        ◦ 211
        ◦ 212
        ◦ 213
        ◦ 214
        ◦ 215
        ◦ 216
        ◦ 217
        ◦ 218
        ◦ 219
        ◦ 220
        ◦ 221
        ◦ 222
        ◦ 223
        ◦ 224
        ◦ 225
        ◦ 226
        ◦ 227
        ◦ 228
        ◦ 229
        ◦ 230
        ◦ 231
        ◦ 232
        ◦ 233
        ◦ 234
        ◦ 235
        ◦ 236
        ◦ 237
        ◦ 238
        ◦ 239
        ◦ 240
        ◦ 241
        ◦ 242
        ◦ 243
        ◦ 244
        ◦ 245
        ◦ 246
        ◦ 247
        ◦ 248
        ◦ 249
        ◦ 250
        ◦ 251
        ◦ 252
        ◦ 253
        ◦ 254
        ◦ 255
        ◦ 256
        ◦ 257
        ◦ 258
        ◦ 259
        ◦ 260
        ◦ 261
        ◦ 262
        ◦ 263
        ◦ 264
        ◦ 265
        ◦ 266
        ◦ 267
        ◦ 268
        ◦ 269
        ◦ 270
        ◦ 271
        ◦ 272
        ◦ 273
        ◦ 274
        ◦ 275
        ◦ 276
        ◦ 277
        ◦ 278
        ◦ 279
        ◦ 280
        ◦ 281
        ◦ 282
        ◦ 283
        ◦ 284
        ◦ 285
        ◦ 286
    • 
估计是设置bootargs参数设置问题。
    • 
TINY4412 # setenv bootargs root=/dev/ram0 rw rootfstype=ramfs console=ttySAC0,115200 init=/linuxrc earlyprintk
    • 
        ◦ 1
        ◦ 1
    • 
重新烧录
    • 
    • 
        . 
TINY4412 # bootm 41000000 - 42000000
        . 
        . 
 
        . 
        . 
## Booting kernel from Legacy Image at 41000000 ...
        . 
        . 
 
        . 
        . 
 Image Name:   Linux-4.12.0-g98c9281-dirty
        . 
        . 
 Image Type:   ARM Linux Kernel Image (uncompressed)
        . 
        . 
 Data Size:    5957944 Bytes = 5.7 MiB
        . 
        . 
 Load Address: 40008000
        . 
        . 
 Entry Point:  40008000
        . 
        . 
 Verifying Checksum ... OK
        . 
        . 
 
        . 
        . 
## Flattened Device Tree blob at 42000000
        . 
        . 
 
        . 
        . 
 Booting using the fdt blob at 0x42000000
        . 
        . 
 Loading Kernel Image ... OK
        . 
        . 
 Loading Device Tree to 4fff1000, end 4ffff56c ... OK
        . 
        . 
 
        . 
        . 
Starting kernel ...
        . 
        . 
 
        . 
        . 
Uncompressing Linux... done, booting the kernel.
        . 
        . 
[    0.000000] Booting Linux on physical CPU 0xa00
        . 
        . 
[    0.000000] Linux version 4.12.0-g98c9281-dirty (root@ubuntu) (gcc version 5.4.1 20160919 (release) [ARM/embedded-5-branch revision 240496] (GNU Tools for ARM Embedded Processors) ) #7 SMP PREEMPT Sun Jul 9 17:02:43 CST 2017
        . 
        . 
[    0.000000] CPU: ARMv7 Processor [413fc090] revision 0 (ARMv7), cr=10c5387d
        . 
        . 
[    0.000000] CPU: PIPT / VIPT nonaliasing data cache, VIPT aliasing instruction cache
        . 
        . 
[    0.000000] OF: fdt: Machine model: FriendlyARM TINY4412 board based on Exynos4412
        . 
        . 
[    0.000000] Memory policy: Data cache writealloc
        . 
        . 
[    0.000000] cma: Reserved 96 MiB at 0x7a000000
        . 
        . 
[    0.000000] Samsung CPU ID: 0xe4412011
        . 
        . 
[    0.000000] percpu: Embedded 16 pages/cpu @ef78f000 s34968 r8192 d22376 u65536
        . 
        . 
[    0.000000] Built 1 zonelists in Zone order, mobility grouping on.  Total pages: 260608
        . 
        . 
[    0.000000] Kernel command line: root=/dev/ram0 rw console=ttySAC0,115200 init=/linuxrc rootfstype=ramfs
        . 
        . 
[    0.000000] PID hash table entries: 4096 (order: 2, 16384 bytes)
        . 
        . 
[    0.000000] Dentry cache hash table entries: 131072 (order: 7, 524288 bytes)
        . 
        . 
[    0.000000] Inode-cache hash table entries: 65536 (order: 6, 262144 bytes)
        . 
        . 
[    0.000000] Memory: 927808K/1048576K available (7168K kernel code, 334K rwdata, 2448K rodata, 2048K init, 326K bss, 22464K reserved, 98304K cma-reserved, 163840K highmem)
        . 
        . 
[    0.000000] Virtual kernel memory layout:
        . 
        . 
[    0.000000]     vector  : 0xffff0000 - 0xffff1000   (   4 kB)
        . 
        . 
[    0.000000]     fixmap  : 0xffc00000 - 0xfff00000   (3072 kB)
        . 
        . 
[    0.000000]     vmalloc : 0xf0800000 - 0xff800000   ( 240 MB)
        . 
        . 
[    0.000000]     lowmem  : 0xc0000000 - 0xf0000000   ( 768 MB)
        . 
        . 
[    0.000000]     pkmap   : 0xbfe00000 - 0xc0000000   (   2 MB)
        . 
        . 
[    0.000000]     modules : 0xbf000000 - 0xbfe00000   (  14 MB)
        . 
        . 
[    0.000000]       .text : 0xc0008000 - 0xc0800000   (8160 kB)
        . 
        . 
[    0.000000]       .init : 0xc0b00000 - 0xc0d00000   (2048 kB)
        . 
        . 
[    0.000000]       .data : 0xc0d00000 - 0xc0d538d0   ( 335 kB)
        . 
        . 
[    0.000000]        .bss : 0xc0d5b0e4 - 0xc0daca20   ( 327 kB)
        . 
        . 
[    0.000000] SLUB: HWalign=64, Order=0-3, MinObjects=0, CPUs=4, Nodes=1
        . 
        . 
[    0.000000] Preemptible hierarchical RCU implementation.
        . 
        . 
[    0.000000]  RCU restricting CPUs from NR_CPUS=8 to nr_cpu_ids=4.
        . 
        . 
[    0.000000] RCU: Adjusting geometry for rcu_fanout_leaf=16, nr_cpu_ids=4
        . 
        . 
[    0.000000] NR_IRQS:16 nr_irqs:16 16
        . 
        . 
[    0.000000] L2C: platform modifies aux control register: 0x02070000 -> 0x3e470001
        . 
        . 
[    0.000000] L2C: platform provided aux values permit register corruption.
        . 
        . 
[    0.000000] L2C: DT/platform modifies aux control register: 0x02070000 -> 0x3e470001
        . 
        . 
[    0.000000] L2C-310 enabling early BRESP for Cortex-A9
        . 
        . 
[    0.000000] L2C-310: enabling full line of zeros but not enabled in Cortex-A9
        . 
        . 
[    0.000000] L2C-310 dynamic clock gating enabled, standby mode enabled
        . 
        . 
[    0.000000] L2C-310 cache controller enabled, 16 ways, 1024 kB
        . 
        . 
[    0.000000] L2C-310: CACHE_ID 0x4100c4c8, AUX_CTRL 0x4e470001
        . 
        . 
[    0.000000] Exynos4x12 clocks: sclk_apll = 700000000, sclk_mpll = 800000000
        . 
        . 
[    0.000000]  sclk_epll = 96035156, sclk_vpll = 108035156, arm_clk = 1400000000
        . 
        . 
[    0.000000] Switching to timer-based delay loop, resolution 41ns
        . 
        . 
[    0.000000] clocksource: mct-frc: mask: 0xffffffff max_cycles: 0xffffffff, max_idle_ns: 79635851949 ns
        . 
        . 
[    0.000003] sched_clock: 32 bits at 24MHz, resolution 41ns, wraps every 89478484971ns
        . 
        . 
[    0.000151] Console: colour dummy device 80x30
        . 
        . 
[    0.000164] Calibrating delay loop (skipped), value calculated using timer frequency.. 48.00 BogoMIPS (lpj=240000)
        . 
        . 
[    0.000174] pid_max: default: 32768 minimum: 301
        . 
        . 
[    0.000238] Mount-cache hash table entries: 2048 (order: 1, 8192 bytes)
        . 
        . 
[    0.000245] Mountpoint-cache hash table entries: 2048 (order: 1, 8192 bytes)
        . 
        . 
[    0.000551] CPU: Testing write buffer coherency: ok
        . 
        . 
[    0.000744] CPU0: thread -1, cpu 0, socket 10, mpidr 80000a00
        . 
        . 
[    0.040017] Setting up static identity map for 0x40100000 - 0x40100060
        . 
        . 
[    0.119995] smp: Bringing up secondary CPUs ...
        . 
        . 
[    0.200065] CPU1: thread -1, cpu 1, socket 10, mpidr 80000a01
        . 
        . 
[    0.280048] CPU2: thread -1, cpu 2, socket 10, mpidr 80000a02
        . 
        . 
[    0.360047] CPU3: thread -1, cpu 3, socket 10, mpidr 80000a03
        . 
        . 
[    0.360092] smp: Brought up 1 node, 4 CPUs
        . 
        . 
[    0.360105] SMP: Total of 4 processors activated (192.00 BogoMIPS).
        . 
        . 
[    0.360110] CPU: All CPU(s) started in SVC mode.
        . 
        . 
[    0.360647] devtmpfs: initialized
        . 
        . 
[    0.367318] VFP support v0.3: implementor 41 architecture 3 part 30 variant 9 rev 4
        . 
        . 
[    0.367481] clocksource: jiffies: mask: 0xffffffff max_cycles: 0xffffffff, max_idle_ns: 19112604462750000 ns
        . 
        . 
[    0.367493] futex hash table entries: 1024 (order: 4, 65536 bytes)
        . 
        . 
[    0.370429] pinctrl core: initialized pinctrl subsystem
        . 
        . 
[    0.370971] /lcd0-power-domain@10023C80 has as child subdomain: /tv-power-domain@10023C20.
        . 
        . 
[    0.371547] NET: Registered protocol family 16
        . 
        . 
[    0.372959] DMA: preallocated 256 KiB pool for atomic coherent allocations
        . 
        . 
[    0.374004] cpuidle: using governor menu
        . 
        . 
[    0.392782] SCSI subsystem initialized
        . 
        . 
[    0.392929] usbcore: registered new interface driver usbfs
        . 
        . 
[    0.392964] usbcore: registered new interface driver hub
        . 
        . 
[    0.393014] usbcore: registered new device driver usb
        . 
        . 
[    0.393524] Advanced Linux Sound Architecture Driver Initialized.
        . 
        . 
[    0.394210] clocksource: Switched to clocksource mct-frc
        . 
        . 
[    0.399822] missing cooling_device property
        . 
        . 
[    0.399831] failed to build thermal zone cpu-thermal: -2
        . 
        . 
[    0.399897] NET: Registered protocol family 2
        . 
        . 
[    0.400286] TCP established hash table entries: 8192 (order: 3, 32768 bytes)
        . 
        . 
[    0.400346] TCP bind hash table entries: 8192 (order: 5, 163840 bytes)
        . 
        . 
[    0.400463] TCP: Hash tables configured (established 8192 bind 8192)
        . 
        . 
[    0.400553] UDP hash table entries: 512 (order: 2, 24576 bytes)
        . 
        . 
[    0.400581] UDP-Lite hash table entries: 512 (order: 2, 24576 bytes)
        . 
        . 
[    0.400701] NET: Registered protocol family 1
        . 
        . 
[    0.400966] RPC: Registered named UNIX socket transport module.
        . 
        . 
[    0.400973] RPC: Registered udp transport module.
        . 
        . 
[    0.400977] RPC: Registered tcp transport module.
        . 
        . 
[    0.400982] RPC: Registered tcp NFSv4.1 backchannel transport module.
        . 
        . 
[    0.451667] audit: initializing netlink subsys (disabled)
        . 
        . 
[    0.451821] audit: type=2000 audit(0.447:1): state=initialized audit_enabled=0 res=1
        . 
        . 
[    0.451899] workingset: timestamp_bits=30 max_order=18 bucket_order=0
        . 
        . 
[    0.456880] NFS: Registering the id_resolver key type
        . 
        . 
[    0.456901] Key type id_resolver registered
        . 
        . 
[    0.456906] Key type id_legacy registered
        . 
        . 
[    0.456930] romfs: ROMFS MTD (C) 2007 Red Hat, Inc.
        . 
        . 
[    0.457505] bounce: pool size: 64 pages
        . 
        . 
[    0.457534] Block layer SCSI generic (bsg) driver version 0.4 loaded (major 248)
        . 
        . 
[    0.457541] io scheduler noop registered
        . 
        . 
[    0.457546] io scheduler deadline registered
        . 
        . 
[    0.457563] io scheduler cfq registered (default)
        . 
        . 
[    0.457569] io scheduler mq-deadline registered
        . 
        . 
[    0.457574] io scheduler kyber registered
        . 
        . 
[    0.461557] dma-pl330 12680000.pdma: Loaded driver for PL330 DMAC-141330
        . 
        . 
[    0.461568] dma-pl330 12680000.pdma:         DBUFF-32x4bytes Num_Chans-8 Num_Peri-32 Num_Events-32
        . 
        . 
[    0.463254] dma-pl330 12690000.pdma: Loaded driver for PL330 DMAC-141330
        . 
        . 
[    0.463263] dma-pl330 12690000.pdma:         DBUFF-32x4bytes Num_Chans-8 Num_Peri-32 Num_Events-32
        . 
        . 
[    0.463827] dma-pl330 12850000.mdma: Loaded driver for PL330 DMAC-141330
        . 
        . 
[    0.463836] dma-pl330 12850000.mdma:         DBUFF-64x8bytes Num_Chans-8 Num_Peri-1 Num_Events-32
        . 
        . 
[    0.498875] Serial: 8250/16550 driver, 4 ports, IRQ sharing disabled
        . 
        . 
[    0.500052] 13800000.serial: ttySAC0 at MMIO 0x13800000 (irq = 46, base_baud = 0) is a S3C6400/10
        . 
        . 
[    1.151210] console [ttySAC0] enabled
        . 
        . 
[    1.155115] 13810000.serial: ttySAC1 at MMIO 0x13810000 (irq = 47, base_baud = 0) is a S3C6400/10
        . 
        . 
[    1.163901] 13820000.serial: ttySAC2 at MMIO 0x13820000 (irq = 48, base_baud = 0) is a S3C6400/10
        . 
        . 
[    1.172773] 13830000.serial: ttySAC3 at MMIO 0x13830000 (irq = 49, base_baud = 0) is a S3C6400/10
        . 
        . 
[    1.230362] brd: module loaded
        . 
        . 
[    1.265840] loop: module loaded
        . 
        . 
[    1.266545] libphy: Fixed MDIO Bus: probed
        . 
        . 
[    1.266698] usbcore: registered new interface driver r8152
        . 
        . 
[    1.266780] usbcore: registered new interface driver asix
        . 
        . 
[    1.266868] usbcore: registered new interface driver ax88179_178a
        . 
        . 
[    1.267690] usbcore: registered new interface driver cdc_ether
        . 
        . 
[    1.273513] usbcore: registered new interface driver smsc75xx
        . 
        . 
[    1.279305] usbcore: registered new interface driver smsc95xx
        . 
        . 
[    1.284988] usbcore: registered new interface driver net1080
        . 
        . 
[    1.290605] usbcore: registered new interface driver cdc_subset
        . 
        . 
[    1.296523] usbcore: registered new interface driver zaurus
        . 
        . 
[    1.302085] usbcore: registered new interface driver cdc_ncm
        . 
        . 
[    1.308212] ehci_hcd: USB 2.0 'Enhanced' Host Controller (EHCI) Driver
        . 
        . 
[    1.314220] ehci-exynos: EHCI EXYNOS driver
        . 
        . 
[    1.318465] ohci_hcd: USB 1.1 'Open' Host Controller (OHCI) Driver
        . 
        . 
[    1.324544] ohci-exynos: OHCI EXYNOS driver
        . 
        . 
[    1.329009] usbcore: registered new interface driver usb-storage
        . 
        . 
[    1.335387] s3c-rtc 10070000.rtc: failed to find rtc source clock
        . 
        . 
[    1.340773] s3c-rtc: probe of 10070000.rtc failed with error -2
        . 
        . 
[    1.346836] i2c /dev entries driver
        . 
        . 
[    1.351755] s3c2410-wdt 10060000.watchdog: watchdog inactive, reset disabled, irq disabled
        . 
        . 
[    1.358759] device-mapper: ioctl: 4.35.0-ioctl (2016-06-23) initialised: dm-devel@redhat.com
        . 
        . 
[    1.368326] sdhci: Secure Digital Host Controller Interface driver
        . 
        . 
[    1.372959] sdhci: Copyright(c) Pierre Ossman
        . 
        . 
[    1.377611] s3c-sdhci 12530000.sdhci: clock source 2: mmc_busclk.2 (20000000 Hz)
        . 
        . 
[    1.444256] mmc0: SDHCI controller on samsung-hsmmc [12530000.sdhci] using ADMA
        . 
        . 
[    1.444419] Synopsys Designware Multimedia Card Interface Driver
        . 
        . 
[    1.448951] s5p-secss 10830000.sss: s5p-sss driver registered
        . 
        . 
[    1.449415] usbcore: registered new interface driver usbhid
        . 
        . 
[    1.449472] usbhid: USB HID core driver
        . 
        . 
[    1.455757] NET: Registered protocol family 10
        . 
        . 
[    1.456371] Segment Routing with IPv6
        . 
        . 
[    1.458446] sit: IPv6, IPv4 and MPLS over IPv4 tunneling driver
        . 
        . 
[    1.464686] NET: Registered protocol family 17
        . 
        . 
[    1.468752] NET: Registered protocol family 15
        . 
        . 
[    1.473214] Key type dns_resolver registered
        . 
        . 
[    1.477612] Registering SWP/SWPB emulation handler
        . 
        . 
[    1.493259] mmc0: new high speed SDHC card at address 0007
        . 
        . 
[    1.493641] mmcblk0: mmc0:0007 SD16G 14.5 GiB 
        . 
        . 
[    1.494133] hctosys: unable to open rtc device (rtc0)
        . 
        . 
[    1.497747] ALSA device list:
        . 
        . 
[    1.500078]   No soundcards found.
        . 
        . 
[    1.504066] VFS: Mounted root (ramfs filesystem) on device 0:13.
        . 
        . 
[    1.509486] devtmpfs: error mounting -2
        . 
        . 
[    1.514563] Freeing unused kernel memory: 2048K
        . 
        . 
[    1.517957] Kernel panic - not syncing: Requested init /linuxrc failed (error -2).
        . 
        . 
[    1.525341] CPU: 3 PID: 1 Comm: swapper/0 Not tainted 4.12.0-g98c9281-dirty #7
        . 
        . 
[    1.532541] Hardware name: SAMSUNG EXYNOS (Flattened Device Tree)
        . 
        . 
[    1.538647] [<c010f268>] (unwind_backtrace) from [<c010bcb0>] (show_stack+0x10/0x14)
        . 
        . 
[    1.546352] [<c010bcb0>] (show_stack) from [<c034b068>] (dump_stack+0x88/0x9c)
        . 
        . 
[    1.553560] [<c034b068>] (dump_stack) from [<c01a7f88>] (panic+0xe8/0x268)
        . 
        . 
[    1.560417] [<c01a7f88>] (panic) from [<c0744bcc>] (kernel_init+0xa4/0x110)
        . 
        . 
[    1.567355] [<c0744bcc>] (kernel_init) from [<c0108578>] (ret_from_fork+0x14/0x3c)
        . 
        . 
[    1.574907] CPU2: stopping
        . 
        . 
[    1.577593] CPU: 2 PID: 0 Comm: swapper/2 Not tainted 4.12.0-g98c9281-dirty #7
        . 
        . 
[    1.584795] Hardware name: SAMSUNG EXYNOS (Flattened Device Tree)
        . 
        . 
[    1.590879] [<c010f268>] (unwind_backtrace) from [<c010bcb0>] (show_stack+0x10/0x14)
        . 
        . 
[    1.598601] [<c010bcb0>] (show_stack) from [<c034b068>] (dump_stack+0x88/0x9c)
        . 
        . 
[    1.605804] [<c034b068>] (dump_stack) from [<c010e2b8>] (handle_IPI+0x1a0/0x1b0)
        . 
        . 
[    1.613182] [<c010e2b8>] (handle_IPI) from [<c01014e8>] (gic_handle_irq+0x94/0x98)
        . 
        . 
[    1.620733] [<c01014e8>] (gic_handle_irq) from [<c010c78c>] (__irq_svc+0x6c/0xa8)
        . 
        . 
[    1.628195] Exception stack(0xef09ff78 to 0xef09ffc0)
        . 
        . 
[    1.633231] ff60:                                                       00000001 00000000
        . 
        . 
[    1.641392] ff80: 00000000 c0115740 ef09e000 c0d054c0 c0d05464 c0c7b138 ef09ffd0 00000000
        . 
        . 
[    1.649551] ffa0: 00000000 00000000 00000126 ef09ffc8 c0108fd4 c0108fd8 60000013 ffffffff
        . 
        . 
[    1.657711] [<c010c78c>] (__irq_svc) from [<c0108fd8>] (arch_cpu_idle+0x38/0x3c)
        . 
        . 
[    1.665097] [<c0108fd8>] (arch_cpu_idle) from [<c015175c>] (do_idle+0x16c/0x200)
        . 
        . 
[    1.672469] [<c015175c>] (do_idle) from [<c0151aa8>] (cpu_startup_entry+0x18/0x1c)
        . 
        . 
[    1.680019] [<c0151aa8>] (cpu_startup_entry) from [<4010158c>] (0x4010158c)
        . 
        . 
[    1.686959] CPU0: stopping
        . 
        . 
[    1.689652] CPU: 0 PID: 0 Comm: swapper/0 Not tainted 4.12.0-g98c9281-dirty #7
        . 
        . 
[    1.696853] Hardware name: SAMSUNG EXYNOS (Flattened Device Tree)
        . 
        . 
[    1.702938] [<c010f268>] (unwind_backtrace) from [<c010bcb0>] (show_stack+0x10/0x14)
        . 
        . 
[    1.710660] [<c010bcb0>] (show_stack) from [<c034b068>] (dump_stack+0x88/0x9c)
        . 
        . 
[    1.717863] [<c034b068>] (dump_stack) from [<c010e2b8>] (handle_IPI+0x1a0/0x1b0)
        . 
        . 
[    1.725240] [<c010e2b8>] (handle_IPI) from [<c01014e8>] (gic_handle_irq+0x94/0x98)
        . 
        . 
[    1.732792] [<c01014e8>] (gic_handle_irq) from [<c010c78c>] (__irq_svc+0x6c/0xa8)
        . 
        . 
[    1.740254] Exception stack(0xc0d01f40 to 0xc0d01f88)
        . 
        . 
[    1.745292] 1f40: 00000001 00000000 00000000 c0115740 c0d00000 c0d054c0 c0d05464 c0c7b138
        . 
        . 
[    1.753450] 1f60: c0d01f98 00000000 00000000 00000000 00000130 c0d01f90 c0108fd4 c0108fd8
        . 
        . 
[    1.761607] 1f80: 60000113 ffffffff
        . 
        . 
[    1.765082] [<c010c78c>] (__irq_svc) from [<c0108fd8>] (arch_cpu_idle+0x38/0x3c)
        . 
        . 
[    1.772462] [<c0108fd8>] (arch_cpu_idle) from [<c015175c>] (do_idle+0x16c/0x200)
        . 
        . 
[    1.779839] [<c015175c>] (do_idle) from [<c0151aa8>] (cpu_startup_entry+0x18/0x1c)
        . 
        . 
[    1.787396] [<c0151aa8>] (cpu_startup_entry) from [<c0b00c90>] (start_kernel+0x38c/0x394)
        . 
        . 
[    1.795546] CPU1: stopping
        . 
        . 
[    1.798239] CPU: 1 PID: 0 Comm: swapper/1 Not tainted 4.12.0-g98c9281-dirty #7
        . 
        . 
[    1.805440] Hardware name: SAMSUNG EXYNOS (Flattened Device Tree)
        . 
        . 
[    1.811524] [<c010f268>] (unwind_backtrace) from [<c010bcb0>] (show_stack+0x10/0x14)
        . 
        . 
[    1.819247] [<c010bcb0>] (show_stack) from [<c034b068>] (dump_stack+0x88/0x9c)
        . 
        . 
[    1.826450] [<c034b068>] (dump_stack) from [<c010e2b8>] (handle_IPI+0x1a0/0x1b0)
        . 
        . 
[    1.833827] [<c010e2b8>] (handle_IPI) from [<c01014e8>] (gic_handle_irq+0x94/0x98)
        . 
        . 
[    1.841379] [<c01014e8>] (gic_handle_irq) from [<c010c78c>] (__irq_svc+0x6c/0xa8)
        . 
        . 
[    1.848841] Exception stack(0xef09df78 to 0xef09dfc0)
        . 
        . 
[    1.853876] df60:                                                       00000001 00000000
        . 
        . 
[    1.862037] df80: 00000000 c0115740 ef09c000 c0d054c0 c0d05464 c0c7b138 ef09dfd0 00000000
        . 
        . 
[    1.870196] dfa0: 00000000 00000000 f081b440 ef09dfc8 c0108fd4 c0108fd8 60000013 ffffffff
        . 
        . 
[    1.878356] [<c010c78c>] (__irq_svc) from [<c0108fd8>] (arch_cpu_idle+0x38/0x3c)
        . 
        . 
[    1.885736] [<c0108fd8>] (arch_cpu_idle) from [<c015175c>] (do_idle+0x16c/0x200)
        . 
        . 
[    1.893113] [<c015175c>] (do_idle) from [<c0151aa8>] (cpu_startup_entry+0x18/0x1c)
        . 
        . 
[    1.900663] [<c0151aa8>] (cpu_startup_entry) from [<4010158c>] (0x4010158c)
        . 
        . 
[    1.907612] ---[ end Kernel panic - not syncing: Requested init /linuxrc failed (error -2).
        . 
        ◦ 1
        ◦ 2
        ◦ 3
        ◦ 4
        ◦ 5
        ◦ 6
        ◦ 7
        ◦ 8
        ◦ 9
        ◦ 10
        ◦ 11
        ◦ 12
        ◦ 13
        ◦ 14
        ◦ 15
        ◦ 16
        ◦ 17
        ◦ 18
        ◦ 19
        ◦ 20
        ◦ 21
        ◦ 22
        ◦ 23
        ◦ 24
        ◦ 25
        ◦ 26
        ◦ 27
        ◦ 28
        ◦ 29
        ◦ 30
        ◦ 31
        ◦ 32
        ◦ 33
        ◦ 34
        ◦ 35
        ◦ 36
        ◦ 37
        ◦ 38
        ◦ 39
        ◦ 40
        ◦ 41
        ◦ 42
        ◦ 43
        ◦ 44
        ◦ 45
        ◦ 46
        ◦ 47
        ◦ 48
        ◦ 49
        ◦ 50
        ◦ 51
        ◦ 52
        ◦ 53
        ◦ 54
        ◦ 55
        ◦ 56
        ◦ 57
        ◦ 58
        ◦ 59
        ◦ 60
        ◦ 61
        ◦ 62
        ◦ 63
        ◦ 64
        ◦ 65
        ◦ 66
        ◦ 67
        ◦ 68
        ◦ 69
        ◦ 70
        ◦ 71
        ◦ 72
        ◦ 73
        ◦ 74
        ◦ 75
        ◦ 76
        ◦ 77
        ◦ 78
        ◦ 79
        ◦ 80
        ◦ 81
        ◦ 82
        ◦ 83
        ◦ 84
        ◦ 85
        ◦ 86
        ◦ 87
        ◦ 88
        ◦ 89
        ◦ 90
        ◦ 91
        ◦ 92
        ◦ 93
        ◦ 94
        ◦ 95
        ◦ 96
        ◦ 97
        ◦ 98
        ◦ 99
        ◦ 100
        ◦ 101
        ◦ 102
        ◦ 103
        ◦ 104
        ◦ 105
        ◦ 106
        ◦ 107
        ◦ 108
        ◦ 109
        ◦ 110
        ◦ 111
        ◦ 112
        ◦ 113
        ◦ 114
        ◦ 115
        ◦ 116
        ◦ 117
        ◦ 118
        ◦ 119
        ◦ 120
        ◦ 121
        ◦ 122
        ◦ 123
        ◦ 124
        ◦ 125
        ◦ 126
        ◦ 127
        ◦ 128
        ◦ 129
        ◦ 130
        ◦ 131
        ◦ 132
        ◦ 133
        ◦ 134
        ◦ 135
        ◦ 136
        ◦ 137
        ◦ 138
        ◦ 139
        ◦ 140
        ◦ 141
        ◦ 142
        ◦ 143
        ◦ 144
        ◦ 145
        ◦ 146
        ◦ 147
        ◦ 148
        ◦ 149
        ◦ 150
        ◦ 151
        ◦ 152
        ◦ 153
        ◦ 154
        ◦ 155
        ◦ 156
        ◦ 157
        ◦ 158
        ◦ 159
        ◦ 160
        ◦ 161
        ◦ 162
        ◦ 163
        ◦ 164
        ◦ 165
        ◦ 166
        ◦ 167
        ◦ 168
        ◦ 169
        ◦ 170
        ◦ 171
        ◦ 172
        ◦ 173
        ◦ 174
        ◦ 175
        ◦ 176
        ◦ 177
        ◦ 178
        ◦ 179
        ◦ 180
        ◦ 181
        ◦ 182
        ◦ 183
        ◦ 184
        ◦ 185
        ◦ 186
        ◦ 187
        ◦ 188
        ◦ 189
        ◦ 190
        ◦ 191
        ◦ 192
        ◦ 193
        ◦ 194
        ◦ 195
        ◦ 196
        ◦ 197
        ◦ 198
        ◦ 199
        ◦ 200
        ◦ 201
        ◦ 202
        ◦ 203
        ◦ 204
        ◦ 205
        ◦ 206
        ◦ 207
        ◦ 208
        ◦ 209
        ◦ 210
        ◦ 211
        ◦ 212
        ◦ 213
        ◦ 214
        ◦ 215
        ◦ 216
        ◦ 217
        ◦ 218
        ◦ 219
        ◦ 220
        ◦ 221
        ◦ 222
        ◦ 223
        ◦ 224
        ◦ 225
        ◦ 226
        ◦ 227
        ◦ 228
        ◦ 229
        ◦ 230
        ◦ 231
        ◦ 232
        ◦ 233
        ◦ 234
        ◦ 235
        ◦ 236
        ◦ 1
        ◦ 2
        ◦ 3
        ◦ 4
        ◦ 5
        ◦ 6
        ◦ 7
        ◦ 8
        ◦ 9
        ◦ 10
        ◦ 11
        ◦ 12
        ◦ 13
        ◦ 14
        ◦ 15
        ◦ 16
        ◦ 17
        ◦ 18
        ◦ 19
        ◦ 20
        ◦ 21
        ◦ 22
        ◦ 23
        ◦ 24
        ◦ 25
        ◦ 26
        ◦ 27
        ◦ 28
        ◦ 29
        ◦ 30
        ◦ 31
        ◦ 32
        ◦ 33
        ◦ 34
        ◦ 35
        ◦ 36
        ◦ 37
        ◦ 38
        ◦ 39
        ◦ 40
        ◦ 41
        ◦ 42
        ◦ 43
        ◦ 44
        ◦ 45
        ◦ 46
        ◦ 47
        ◦ 48
        ◦ 49
        ◦ 50
        ◦ 51
        ◦ 52
        ◦ 53
        ◦ 54
        ◦ 55
        ◦ 56
        ◦ 57
        ◦ 58
        ◦ 59
        ◦ 60
        ◦ 61
        ◦ 62
        ◦ 63
        ◦ 64
        ◦ 65
        ◦ 66
        ◦ 67
        ◦ 68
        ◦ 69
        ◦ 70
        ◦ 71
        ◦ 72
        ◦ 73
        ◦ 74
        ◦ 75
        ◦ 76
        ◦ 77
        ◦ 78
        ◦ 79
        ◦ 80
        ◦ 81
        ◦ 82
        ◦ 83
        ◦ 84
        ◦ 85
        ◦ 86
        ◦ 87
        ◦ 88
        ◦ 89
        ◦ 90
        ◦ 91
        ◦ 92
        ◦ 93
        ◦ 94
        ◦ 95
        ◦ 96
        ◦ 97
        ◦ 98
        ◦ 99
        ◦ 100
        ◦ 101
        ◦ 102
        ◦ 103
        ◦ 104
        ◦ 105
        ◦ 106
        ◦ 107
        ◦ 108
        ◦ 109
        ◦ 110
        ◦ 111
        ◦ 112
        ◦ 113
        ◦ 114
        ◦ 115
        ◦ 116
        ◦ 117
        ◦ 118
        ◦ 119
        ◦ 120
        ◦ 121
        ◦ 122
        ◦ 123
        ◦ 124
        ◦ 125
        ◦ 126
        ◦ 127
        ◦ 128
        ◦ 129
        ◦ 130
        ◦ 131
        ◦ 132
        ◦ 133
        ◦ 134
        ◦ 135
        ◦ 136
        ◦ 137
        ◦ 138
        ◦ 139
        ◦ 140
        ◦ 141
        ◦ 142
        ◦ 143
        ◦ 144
        ◦ 145
        ◦ 146
        ◦ 147
        ◦ 148
        ◦ 149
        ◦ 150
        ◦ 151
        ◦ 152
        ◦ 153
        ◦ 154
        ◦ 155
        ◦ 156
        ◦ 157
        ◦ 158
        ◦ 159
        ◦ 160
        ◦ 161
        ◦ 162
        ◦ 163
        ◦ 164
        ◦ 165
        ◦ 166
        ◦ 167
        ◦ 168
        ◦ 169
        ◦ 170
        ◦ 171
        ◦ 172
        ◦ 173
        ◦ 174
        ◦ 175
        ◦ 176
        ◦ 177
        ◦ 178
        ◦ 179
        ◦ 180
        ◦ 181
        ◦ 182
        ◦ 183
        ◦ 184
        ◦ 185
        ◦ 186
        ◦ 187
        ◦ 188
        ◦ 189
        ◦ 190
        ◦ 191
        ◦ 192
        ◦ 193
        ◦ 194
        ◦ 195
        ◦ 196
        ◦ 197
        ◦ 198
        ◦ 199
        ◦ 200
        ◦ 201
        ◦ 202
        ◦ 203
        ◦ 204
        ◦ 205
        ◦ 206
        ◦ 207
        ◦ 208
        ◦ 209
        ◦ 210
        ◦ 211
        ◦ 212
        ◦ 213
        ◦ 214
        ◦ 215
        ◦ 216
        ◦ 217
        ◦ 218
        ◦ 219
        ◦ 220
        ◦ 221
        ◦ 222
        ◦ 223
        ◦ 224
        ◦ 225
        ◦ 226
        ◦ 227
        ◦ 228
        ◦ 229
        ◦ 230
        ◦ 231
        ◦ 232
        ◦ 233
        ◦ 234
        ◦ 235
        ◦ 236
        ◦ 提示linuxrc有问题，看有人说过initramfs默认第一个程序是/init，无论U-BOOT的命令行设置init=/linuxrc还是其他什么。
    • 
修改linuxrc为init
    • 
    • 
        . 
root@ubuntu:/opt# cp -a rootfs/ initramfs
        . 
        . 
root@ubuntu:/opt/initramfs# mv linuxrc init
        . 
        . 
root@ubuntu:/opt/initramfs# ls
        . 
        . 
bin  dev  etc  init  mnt  proc  root  sbin  sys  tmp
        . 
        ◦ 1
        ◦ 2
        ◦ 3
        ◦ 4
        ◦ 1
        ◦ 2
        ◦ 3
        ◦ 4
    • 
修改linux
    • 
    • 
        . 
General setup  ---> 
        . 
        . 
    [*] Initial RAM filesystem and RAM disk (initramfs/initrd) support
        . 
        . 
(/dev/initramfs)    Initramfs source file(s)
        . 
        ◦ 1
        ◦ 2
        ◦ 3
        ◦ 1
        ◦ 2
        ◦ 3
    • 
重新编译内核，烧录
    • 
    • 
        . 
TINY4412 # bootm 41000000 - 42000000
        . 
        . 
 
        . 
        . 
## Booting kernel from Legacy Image at 41000000 ...
        . 
        . 
 
        . 
        . 
 Image Name:   Linux-4.12.0-g98c9281-dirty
        . 
        . 
 Image Type:   ARM Linux Kernel Image (uncompressed)
        . 
        . 
 Data Size:    5957952 Bytes = 5.7 MiB
        . 
        . 
 Load Address: 40008000
        . 
        . 
 Entry Point:  40008000
        . 
        . 
 Verifying Checksum ... OK
        . 
        . 
 
        . 
        . 
## Flattened Device Tree blob at 42000000
        . 
        . 
 
        . 
        . 
 Booting using the fdt blob at 0x42000000
        . 
        . 
 Loading Kernel Image ... OK
        . 
        . 
 Loading Device Tree to 4fff1000, end 4ffff56c ... OK
        . 
        . 
 
        . 
        . 
Starting kernel ...
        . 
        . 
 
        . 
        . 
Uncompressing Linux... done, booting the kernel.
        . 
        . 
[    0.000000] Booting Linux on physical CPU 0xa00
        . 
        . 
[    0.000000] Linux version 4.12.0-g98c9281-dirty (root@ubuntu) (gcc version 5.4.1 20160919 (release) [ARM/embedded-5-branch revision 240496] (GNU Tools for ARM Embedded Processors) ) #9 SMP PREEMPT Sun Jul 9 17:59:59 CST 2017
        . 
        . 
[    0.000000] CPU: ARMv7 Processor [413fc090] revision 0 (ARMv7), cr=10c5387d
        . 
        . 
[    0.000000] CPU: PIPT / VIPT nonaliasing data cache, VIPT aliasing instruction cache
        . 
        . 
[    0.000000] OF: fdt: Machine model: FriendlyARM TINY4412 board based on Exynos4412
        . 
        . 
[    0.000000] bootconsole [earlycon0] enabled
        . 
        . 
[    0.000000] Memory policy: Data cache writealloc
        . 
        . 
[    0.000000] cma: Reserved 96 MiB at 0x7a000000
        . 
        . 
[    0.000000] Samsung CPU ID: 0xe4412011
        . 
        . 
[    0.000000] percpu: Embedded 16 pages/cpu @ef78f000 s34968 r8192 d22376 u65536
        . 
        . 
[    0.000000] Built 1 zonelists in Zone order, mobility grouping on.  Total pages: 260608
        . 
        . 
[    0.000000] Kernel command line: root=/dev/ram0 rw rootfstype=ramfs console=ttySAC0,115200 init=/linuxrc earlyprintk
        . 
        . 
[    0.000000] PID hash table entries: 4096 (order: 2, 16384 bytes)
        . 
        . 
[    0.000000] Dentry cache hash table entries: 131072 (order: 7, 524288 bytes)
        . 
        . 
[    0.000000] Inode-cache hash table entries: 65536 (order: 6, 262144 bytes)
        . 
        . 
[    0.000000] Memory: 927808K/1048576K available (7168K kernel code, 334K rwdata, 2448K rodata, 2048K init, 326K bss, 22464K reserved, 98304K cma-reserved, 163840K highmem)
        . 
        . 
[    0.000000] Virtual kernel memory layout:
        . 
        . 
[    0.000000]     vector  : 0xffff0000 - 0xffff1000   (   4 kB)
        . 
        . 
[    0.000000]     fixmap  : 0xffc00000 - 0xfff00000   (3072 kB)
        . 
        . 
[    0.000000]     vmalloc : 0xf0800000 - 0xff800000   ( 240 MB)
        . 
        . 
[    0.000000]     lowmem  : 0xc0000000 - 0xf0000000   ( 768 MB)
        . 
        . 
[    0.000000]     pkmap   : 0xbfe00000 - 0xc0000000   (   2 MB)
        . 
        . 
[    0.000000]     modules : 0xbf000000 - 0xbfe00000   (  14 MB)
        . 
        . 
[    0.000000]       .text : 0xc0008000 - 0xc0800000   (8160 kB)
        . 
        . 
[    0.000000]       .init : 0xc0b00000 - 0xc0d00000   (2048 kB)
        . 
        . 
[    0.000000]       .data : 0xc0d00000 - 0xc0d538d0   ( 335 kB)
        . 
        . 
[    0.000000]        .bss : 0xc0d5b0e4 - 0xc0daca20   ( 327 kB)
        . 
        . 
[    0.000000] SLUB: HWalign=64, Order=0-3, MinObjects=0, CPUs=4, Nodes=1
        . 
        . 
[    0.000000] Preemptible hierarchical RCU implementation.
        . 
        . 
[    0.000000]  RCU restricting CPUs from NR_CPUS=8 to nr_cpu_ids=4.
        . 
        . 
[    0.000000] RCU: Adjusting geometry for rcu_fanout_leaf=16, nr_cpu_ids=4
        . 
        . 
[    0.000000] NR_IRQS:16 nr_irqs:16 16
        . 
        . 
[    0.000000] L2C: platform modifies aux control register: 0x02070000 -> 0x3e470001
        . 
        . 
[    0.000000] L2C: platform provided aux values permit register corruption.
        . 
        . 
[    0.000000] L2C: DT/platform modifies aux control register: 0x02070000 -> 0x3e470001
        . 
        . 
[    0.000000] L2C-310 enabling early BRESP for Cortex-A9
        . 
        . 
[    0.000000] L2C-310: enabling full line of zeros but not enabled in Cortex-A9
        . 
        . 
[    0.000000] L2C-310 dynamic clock gating enabled, standby mode enabled
        . 
        . 
[    0.000000] L2C-310 cache controller enabled, 16 ways, 1024 kB
        . 
        . 
[    0.000000] L2C-310: CACHE_ID 0x4100c4c8, AUX_CTRL 0x4e470001
        . 
        . 
[    0.000000] Exynos4x12 clocks: sclk_apll = 700000000, sclk_mpll = 800000000
        . 
        . 
[    0.000000]  sclk_epll = 96035156, sclk_vpll = 108035156, arm_clk = 1400000000
        . 
        . 
[    0.000000] Switching to timer-based delay loop, resolution 41ns
        . 
        . 
[    0.000000] clocksource: mct-frc: mask: 0xffffffff max_cycles: 0xffffffff, max_idle_ns: 79635851949 ns
        . 
        . 
[    0.000003] sched_clock: 32 bits at 24MHz, resolution 41ns, wraps every 89478484971ns
        . 
        . 
[    0.008063] Console: colour dummy device 80x30
        . 
        . 
[    0.012419] Calibrating delay loop (skipped), value calculated using timer frequency.. 48.00 BogoMIPS (lpj=240000)
        . 
        . 
[    0.022834] pid_max: default: 32768 minimum: 301
        . 
        . 
[    0.027578] Mount-cache hash table entries: 2048 (order: 1, 8192 bytes)
        . 
        . 
[    0.034199] Mountpoint-cache hash table entries: 2048 (order: 1, 8192 bytes)
        . 
        . 
[    0.041624] CPU: Testing write buffer coherency: ok
        . 
        . 
[    0.046602] CPU0: thread -1, cpu 0, socket 10, mpidr 80000a00
        . 
        . 
[    0.084745] Setting up static identity map for 0x40100000 - 0x40100060
        . 
        . 
[    0.144723] smp: Bringing up secondary CPUs ...
        . 
        . 
[    0.204783] CPU1: thread -1, cpu 1, socket 10, mpidr 80000a01
        . 
        . 
[    0.284776] CPU2: thread -1, cpu 2, socket 10, mpidr 80000a02
        . 
        . 
[    0.364775] CPU3: thread -1, cpu 3, socket 10, mpidr 80000a03
        . 
        . 
[    0.364819] smp: Brought up 1 node, 4 CPUs
        . 
        . 
[    0.386275] SMP: Total of 4 processors activated (192.00 BogoMIPS).
        . 
        . 
[    0.392603] CPU: All CPU(s) started in SVC mode.
        . 
        . 
[    0.397885] devtmpfs: initialized
        . 
        . 
[    0.407825] VFP support v0.3: implementor 41 architecture 3 part 30 variant 9 rev 4
        . 
        . 
[    0.415557] clocksource: jiffies: mask: 0xffffffff max_cycles: 0xffffffff, max_idle_ns: 19112604462750000 ns
        . 
        . 
[    0.425308] futex hash table entries: 1024 (order: 4, 65536 bytes)
        . 
        . 
[    0.434429] pinctrl core: initialized pinctrl subsystem
        . 
        . 
[    0.440106] /lcd0-power-domain@10023C80 has as child subdomain: /tv-power-domain@10023C20.
        . 
        . 
[    0.448857] NET: Registered protocol family 16
        . 
        . 
[    0.454552] DMA: preallocated 256 KiB pool for atomic coherent allocations
        . 
        . 
[    0.462368] cpuidle: using governor menu
        . 
        . 
[    0.485163] SCSI subsystem initialized
        . 
        . 
[    0.488968] usbcore: registered new interface driver usbfs
        . 
        . 
[    0.494404] usbcore: registered new interface driver hub
        . 
        . 
[    0.499814] usbcore: registered new device driver usb
        . 
        . 
[    0.505403] Advanced Linux Sound Architecture Driver Initialized.
        . 
        . 
[    0.512080] clocksource: Switched to clocksource mct-frc
        . 
        . 
[    0.524574] missing cooling_device property
        . 
        . 
[    0.528663] failed to build thermal zone cpu-thermal: -2
        . 
        . 
[    0.534120] NET: Registered protocol family 2
        . 
        . 
[    0.538811] TCP established hash table entries: 8192 (order: 3, 32768 bytes)
        . 
        . 
[    0.545832] TCP bind hash table entries: 8192 (order: 5, 163840 bytes)
        . 
        . 
[    0.552487] TCP: Hash tables configured (established 8192 bind 8192)
        . 
        . 
[    0.558870] UDP hash table entries: 512 (order: 2, 24576 bytes)
        . 
        . 
[    0.564813] UDP-Lite hash table entries: 512 (order: 2, 24576 bytes)
        . 
        . 
[    0.571313] NET: Registered protocol family 1
        . 
        . 
[    0.575886] RPC: Registered named UNIX socket transport module.
        . 
        . 
[    0.581714] RPC: Registered udp transport module.
        . 
        . 
[    0.586507] RPC: Registered tcp transport module.
        . 
        . 
[    0.591262] RPC: Registered tcp NFSv4.1 backchannel transport module.
        . 
        . 
[    0.648438] audit: initializing netlink subsys (disabled)
        . 
        . 
[    0.654044] audit: type=2000 audit(0.651:1): state=initialized audit_enabled=0 res=1
        . 
        . 
[    0.654127] workingset: timestamp_bits=30 max_order=18 bucket_order=0
        . 
        . 
[    0.658920] NFS: Registering the id_resolver key type
        . 
        . 
[    0.658937] Key type id_resolver registered
        . 
        . 
[    0.658939] Key type id_legacy registered
        . 
        . 
[    0.658961] romfs: ROMFS MTD (C) 2007 Red Hat, Inc.
        . 
        . 
[    0.662169] bounce: pool size: 64 pages
        . 
        . 
[    0.662195] Block layer SCSI generic (bsg) driver version 0.4 loaded (major 248)
        . 
        . 
[    0.662200] io scheduler noop registered
        . 
        . 
[    0.662202] io scheduler deadline registered
        . 
        . 
[    0.662217] io scheduler cfq registered (default)
        . 
        . 
[    0.662220] io scheduler mq-deadline registered
        . 
        . 
[    0.662223] io scheduler kyber registered
        . 
        . 
[    0.672363] dma-pl330 12680000.pdma: Loaded driver for PL330 DMAC-141330
        . 
        . 
[    0.672369] dma-pl330 12680000.pdma:         DBUFF-32x4bytes Num_Chans-8 Num_Peri-32 Num_Events-32
        . 
        . 
[    0.674201] dma-pl330 12690000.pdma: Loaded driver for PL330 DMAC-141330
        . 
        . 
[    0.674207] dma-pl330 12690000.pdma:         DBUFF-32x4bytes Num_Chans-8 Num_Peri-32 Num_Events-32
        . 
        . 
[    0.674916] dma-pl330 12850000.mdma: Loaded driver for PL330 DMAC-141330
        . 
        . 
[    0.674921] dma-pl330 12850000.mdma:         DBUFF-64x8bytes Num_Chans-8 Num_Peri-1 Num_Events-32
        . 
        . 
[    0.797667] Serial: 8250/16550 driver, 4 ports, IRQ sharing disabled
        . 
        . 
[    0.805210] 13800000.serial: ttySAC0 at MMIO 0x13800000 (irq = 46, base_baud = 0) is a S3C6400/10
        . 
        . 
[    0.814062] console [ttySAC0] enabled
        . 
        . 
[    0.814062] console [ttySAC0] enabled
        . 
        . 
[    0.821362] bootconsole [earlycon0] disabled
        . 
        . 
[    0.821362] bootconsole [earlycon0] disabled
        . 
        . 
[    0.830224] 13810000.serial: ttySAC1 at MMIO 0x13810000 (irq = 47, base_baud = 0) is a S3C6400/10
        . 
        . 
[    0.830552] 13820000.serial: ttySAC2 at MMIO 0x13820000 (irq = 48, base_baud = 0) is a S3C6400/10
        . 
        . 
[    0.830873] 13830000.serial: ttySAC3 at MMIO 0x13830000 (irq = 49, base_baud = 0) is a S3C6400/10
        . 
        . 
[    0.843623] brd: module loaded
        . 
        . 
[    0.849065] loop: module loaded
        . 
        . 
[    0.849786] libphy: Fixed MDIO Bus: probed
        . 
        . 
[    0.849935] usbcore: registered new interface driver r8152
        . 
        . 
[    0.850220] usbcore: registered new interface driver asix
        . 
        . 
[    0.855651] usbcore: registered new interface driver ax88179_178a
        . 
        . 
[    0.861672] usbcore: registered new interface driver cdc_ether
        . 
        . 
[    0.867537] usbcore: registered new interface driver smsc75xx
        . 
        . 
[    0.873251] usbcore: registered new interface driver smsc95xx
        . 
        . 
[    0.878945] usbcore: registered new interface driver net1080
        . 
        . 
[    0.884616] usbcore: registered new interface driver cdc_subset
        . 
        . 
[    0.890489] usbcore: registered new interface driver zaurus
        . 
        . 
[    0.896075] usbcore: registered new interface driver cdc_ncm
        . 
        . 
[    0.902206] ehci_hcd: USB 2.0 'Enhanced' Host Controller (EHCI) Driver
        . 
        . 
[    0.908186] ehci-exynos: EHCI EXYNOS driver
        . 
        . 
[    0.912460] ohci_hcd: USB 1.1 'Open' Host Controller (OHCI) Driver
        . 
        . 
[    0.918515] ohci-exynos: OHCI EXYNOS driver
        . 
        . 
[    0.923005] usbcore: registered new interface driver usb-storage
        . 
        . 
[    0.929353] s3c-rtc 10070000.rtc: failed to find rtc source clock
        . 
        . 
[    0.934774] s3c-rtc: probe of 10070000.rtc failed with error -2
        . 
        . 
[    0.940797] i2c /dev entries driver
        . 
        . 
[    0.945780] s3c2410-wdt 10060000.watchdog: watchdog inactive, reset disabled, irq disabled
        . 
        . 
[    0.952757] device-mapper: ioctl: 4.35.0-ioctl (2016-06-23) initialised: dm-devel@redhat.com
        . 
        . 
[    0.962328] sdhci: Secure Digital Host Controller Interface driver
        . 
        . 
[    0.966941] sdhci: Copyright(c) Pierre Ossman
        . 
        . 
[    0.971543] s3c-sdhci 12530000.sdhci: clock source 2: mmc_busclk.2 (20000000 Hz)
        . 
        . 
[    1.032128] mmc0: SDHCI controller on samsung-hsmmc [12530000.sdhci] using ADMA
        . 
        . 
[    1.032293] Synopsys Designware Multimedia Card Interface Driver
        . 
        . 
[    1.036846] s5p-secss 10830000.sss: s5p-sss driver registered
        . 
        . 
[    1.037301] usbcore: registered new interface driver usbhid
        . 
        . 
[    1.037358] usbhid: USB HID core driver
        . 
        . 
[    1.043633] NET: Registered protocol family 10
        . 
        . 
[    1.044269] Segment Routing with IPv6
        . 
        . 
[    1.046316] sit: IPv6, IPv4 and MPLS over IPv4 tunneling driver
        . 
        . 
[    1.052550] NET: Registered protocol family 17
        . 
        . 
[    1.056623] NET: Registered protocol family 15
        . 
        . 
[    1.061078] Key type dns_resolver registered
        . 
        . 
[    1.065487] Registering SWP/SWPB emulation handler
        . 
        . 
[    1.081124] mmc0: new high speed SDHC card at address 0007
        . 
        . 
[    1.081518] mmcblk0: mmc0:0007 SD16G 14.5 GiB 
        . 
        . 
[    1.082121] hctosys: unable to open rtc device (rtc0)
        . 
        . 
[    1.085838] ALSA device list:
        . 
        . 
[    1.087948]   No soundcards found.
        . 
        . 
[    1.093015] Freeing unused kernel memory: 2048K
        . 
        . 
 
        . 
        . 
Processing /etc/profile... Done
        . 
        . 
 
        . 
        . 
/ # 
        . 
        . 
/ # 
        . 
        . 
/ # 
        . 
        ◦ 1
        ◦ 2
        ◦ 3
        ◦ 4
        ◦ 5
        ◦ 6
        ◦ 7
        ◦ 8
        ◦ 9
        ◦ 10
        ◦ 11
        ◦ 12
        ◦ 13
        ◦ 14
        ◦ 15
        ◦ 16
        ◦ 17
        ◦ 18
        ◦ 19
        ◦ 20
        ◦ 21
        ◦ 22
        ◦ 23
        ◦ 24
        ◦ 25
        ◦ 26
        ◦ 27
        ◦ 28
        ◦ 29
        ◦ 30
        ◦ 31
        ◦ 32
        ◦ 33
        ◦ 34
        ◦ 35
        ◦ 36
        ◦ 37
        ◦ 38
        ◦ 39
        ◦ 40
        ◦ 41
        ◦ 42
        ◦ 43
        ◦ 44
        ◦ 45
        ◦ 46
        ◦ 47
        ◦ 48
        ◦ 49
        ◦ 50
        ◦ 51
        ◦ 52
        ◦ 53
        ◦ 54
        ◦ 55
        ◦ 56
        ◦ 57
        ◦ 58
        ◦ 59
        ◦ 60
        ◦ 61
        ◦ 62
        ◦ 63
        ◦ 64
        ◦ 65
        ◦ 66
        ◦ 67
        ◦ 68
        ◦ 69
        ◦ 70
        ◦ 71
        ◦ 72
        ◦ 73
        ◦ 74
        ◦ 75
        ◦ 76
        ◦ 77
        ◦ 78
        ◦ 79
        ◦ 80
        ◦ 81
        ◦ 82
        ◦ 83
        ◦ 84
        ◦ 85
        ◦ 86
        ◦ 87
        ◦ 88
        ◦ 89
        ◦ 90
        ◦ 91
        ◦ 92
        ◦ 93
        ◦ 94
        ◦ 95
        ◦ 96
        ◦ 97
        ◦ 98
        ◦ 99
        ◦ 100
        ◦ 101
        ◦ 102
        ◦ 103
        ◦ 104
        ◦ 105
        ◦ 106
        ◦ 107
        ◦ 108
        ◦ 109
        ◦ 110
        ◦ 111
        ◦ 112
        ◦ 113
        ◦ 114
        ◦ 115
        ◦ 116
        ◦ 117
        ◦ 118
        ◦ 119
        ◦ 120
        ◦ 121
        ◦ 122
        ◦ 123
        ◦ 124
        ◦ 125
        ◦ 126
        ◦ 127
        ◦ 128
        ◦ 129
        ◦ 130
        ◦ 131
        ◦ 132
        ◦ 133
        ◦ 134
        ◦ 135
        ◦ 136
        ◦ 137
        ◦ 138
        ◦ 139
        ◦ 140
        ◦ 141
        ◦ 142
        ◦ 143
        ◦ 144
        ◦ 145
        ◦ 146
        ◦ 147
        ◦ 148
        ◦ 149
        ◦ 150
        ◦ 151
        ◦ 152
        ◦ 153
        ◦ 154
        ◦ 155
        ◦ 156
        ◦ 157
        ◦ 158
        ◦ 159
        ◦ 160
        ◦ 161
        ◦ 162
        ◦ 163
        ◦ 164
        ◦ 165
        ◦ 166
        ◦ 167
        ◦ 168
        ◦ 169
        ◦ 170
        ◦ 171
        ◦ 172
        ◦ 173
        ◦ 174
        ◦ 175
        ◦ 176
        ◦ 177
        ◦ 178
        ◦ 179
        ◦ 180
        ◦ 181
        ◦ 182
        ◦ 183
        ◦ 184
        ◦ 185
        ◦ 186
        ◦ 187
        ◦ 1
        ◦ 2
        ◦ 3
        ◦ 4
        ◦ 5
        ◦ 6
        ◦ 7
        ◦ 8
        ◦ 9
        ◦ 10
        ◦ 11
        ◦ 12
        ◦ 13
        ◦ 14
        ◦ 15
        ◦ 16
        ◦ 17
        ◦ 18
        ◦ 19
        ◦ 20
        ◦ 21
        ◦ 22
        ◦ 23
        ◦ 24
        ◦ 25
        ◦ 26
        ◦ 27
        ◦ 28
        ◦ 29
        ◦ 30
        ◦ 31
        ◦ 32
        ◦ 33
        ◦ 34
        ◦ 35
        ◦ 36
        ◦ 37
        ◦ 38
        ◦ 39
        ◦ 40
        ◦ 41
        ◦ 42
        ◦ 43
        ◦ 44
        ◦ 45
        ◦ 46
        ◦ 47
        ◦ 48
        ◦ 49
        ◦ 50
        ◦ 51
        ◦ 52
        ◦ 53
        ◦ 54
        ◦ 55
        ◦ 56
        ◦ 57
        ◦ 58
        ◦ 59
        ◦ 60
        ◦ 61
        ◦ 62
        ◦ 63
        ◦ 64
        ◦ 65
        ◦ 66
        ◦ 67
        ◦ 68
        ◦ 69
        ◦ 70
        ◦ 71
        ◦ 72
        ◦ 73
        ◦ 74
        ◦ 75
        ◦ 76
        ◦ 77
        ◦ 78
        ◦ 79
        ◦ 80
        ◦ 81
        ◦ 82
        ◦ 83
        ◦ 84
        ◦ 85
        ◦ 86
        ◦ 87
        ◦ 88
        ◦ 89
        ◦ 90
        ◦ 91
        ◦ 92
        ◦ 93
        ◦ 94
        ◦ 95
        ◦ 96
        ◦ 97
        ◦ 98
        ◦ 99
        ◦ 100
        ◦ 101
        ◦ 102
        ◦ 103
        ◦ 104
        ◦ 105
        ◦ 106
        ◦ 107
        ◦ 108
        ◦ 109
        ◦ 110
        ◦ 111
        ◦ 112
        ◦ 113
        ◦ 114
        ◦ 115
        ◦ 116
        ◦ 117
        ◦ 118
        ◦ 119
        ◦ 120
        ◦ 121
        ◦ 122
        ◦ 123
        ◦ 124
        ◦ 125
        ◦ 126
        ◦ 127
        ◦ 128
        ◦ 129
        ◦ 130
        ◦ 131
        ◦ 132
        ◦ 133
        ◦ 134
        ◦ 135
        ◦ 136
        ◦ 137
        ◦ 138
        ◦ 139
        ◦ 140
        ◦ 141
        ◦ 142
        ◦ 143
        ◦ 144
        ◦ 145
        ◦ 146
        ◦ 147
        ◦ 148
        ◦ 149
        ◦ 150
        ◦ 151
        ◦ 152
        ◦ 153
        ◦ 154
        ◦ 155
        ◦ 156
        ◦ 157
        ◦ 158
        ◦ 159
        ◦ 160
        ◦ 161
        ◦ 162
        ◦ 163
        ◦ 164
        ◦ 165
        ◦ 166
        ◦ 167
        ◦ 168
        ◦ 169
        ◦ 170
        ◦ 171
        ◦ 172
        ◦ 173
        ◦ 174
        ◦ 175
        ◦ 176
        ◦ 177
        ◦ 178
        ◦ 179
        ◦ 180
        ◦ 181
        ◦ 182
        ◦ 183
        ◦ 184
        ◦ 185
        ◦ 186
        ◦ 187
        ◦ 进入我们的最小文件系统了，喜大普奔！！！ : -)
总结
    • 
使用initramfs文件系统，只需要使用Busybox制作最小文件系统，重命名为initramfs
    • 
    • 
将根目录的linuxrc改为init
    • 
    • 
在内核的menuconfig中，配置Initramfs source file(s)为initramfs所在的路径
    • 
    • 
配置好U-BOOT的环境变量setenv bootargs root=/dev/ram0 rw rootfstype=ramfs console=ttySAC0,115200 init=/linuxrc earlyprintk
    • 
    • 
我们传递给内核文件系统的目录/dev/initramfs ，内核会在适当的时候调用脚本/scripts/gen_initramfs_list.sh ，生成文件usr/initramfs_data.cpio.gz，这个就是压缩后的文件系统
    • 
