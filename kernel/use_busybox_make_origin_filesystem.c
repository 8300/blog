1、busybox源码下载及配置

https://busybox.net/downloads/

1.1、修改Makefile

(1) ARCH = arm
(2) CROSS_COMPILE = /usr/local/arm/arm-2009q3/bin//arm-none-linux-gnueabi-

1.2、make menuconfig

复制代码
Busybox Settings--->
    Build Options--->
        [*]Build BusyBox as a static binary(no shared libs)
        
Busybox Library Tuning--->
    [*]vi-style line editing commands
    [*]Fancy shell prompts    
    
Linux Module Utilities--->
    [ ]Simplified modutils
    [*]insmod
    [*]rmmod
    [*]lsmod
    [*]modprobe
    [*]depmod
    
Linux System Utilities--->[*]mdev
    [*]Support /etc/mdev.conf
    [*]Support subdirs/symlinks
    [*]Support regular expressions substitutions when renaming dev
    [*]Support command execution at device addition/removal
    [*]Support loading of firmwares
复制代码
1.3、make 然后 make install

(1)make编译，如果有错误解决之
(2)make install执行的时候其实是在执行busybox顶层目录下的一个目标install。
(3)make install在所有的linux下的软件中作用都是安装软件。在传统的linux系统中安装软件时都是选择源代码方式安装的。我们下载要安装的软件源代码，然后配置、编译、安装。make install的目的就是将编译生成的可执行程序及其依赖的库文件、配置文件、头文件安装到当前系统中指定（一般都可以自己指定安装到哪个目录下，如果不指定一般都有个默认目录）的目录下

实验结果：

生成文件：bin  linuxrc sbin usr

挂载成功，执行/linuxrc（也就是busybox）成功，但是因为找不到/etc/init.d/rcS和/dev/tty2等文件所以一直在打印错误提示信息，但是其实有进入命令行。

2、移植inittab

inittab为linux初始化文件系统时init初始化程序用到的配置文件。

2.1、添加一个典型的inittab

(1)将我提供的典型的inittab文件复制到我们制作的rootfs的根目录下的/etc/目录下
(2)再次启动内核挂载这个rootfs看效果
(3)实验现象是成功启动并且挂载rootfs进入了控制台命令行。当前制作的最小rootfs成功了

复制代码
#first:run the system script file
::sysinit:/etc/init.d/rcS     #执行rcS
::askfirst:-/bin/sh           #执行命令行
::ctrlaltdel:-/sbin/reboot
#umount all filesystem
::shutdown:/bin/umount -a -r
#restart init process
::restart:/sbin/init
复制代码
2.2、inittab格式解析

(1)inittab的工作原理就是被/linuxrc（也就是busybox）执行时所调用起作用。
(2)inittab在/etc目录下，所以属于一个运行时配置文件，是文本格式的（内容是由一系列的遵照一个格式组织的字符组成的），实际工作的时候busybox会（按照一定的格式）解析这个inittab文本文件，然后根据解析的内容来决定要怎么工作。
(3)busybox究竟如何完成解析并且解析结果如何去工作（busybox中实现/etc/inittab的原理）并不是我们的目标，我们的重点是inittab的格式究竟怎样的？我们看到一个inittab后怎么去分析这个inittab对启动的影响。
(4)inittab的格式在busybox中定义的，网上可以搜索到详细的格式说明，具体去参考即可：
第一个：#开始的行是注释
第二个：冒号在里面是分隔符，分隔开各个部分。
第三个：inittab内容是以行为单位的，行与行之间没有关联，每行都是一个独立的配置项，每一个配置项表示一个具体的含义。
第四个：每一行的配置项都是由3个冒号分隔开的4个配置值共同确定的。这四个配置值就是id:runlevels:action:process。值得注意得是有些配置值可以空缺，空缺后冒号不能空缺，所以有时候会看到连续2个冒号。
第五个：每一行的配置项中4个配置值中最重要的是action和process，action是一个条件/状态，process是一个可被执行的程序的pathname。合起来的意思就是：当满足action的条件时就会执行process这个程序。

注意：理解inittab的关键就是明白“当满足action的条件时就会执行process这个程序。”

 3、rcS文件移植
移植完inittab后进入命令行时提示找不到rcS文件。
/etc/init.d/rcS文件是linux的运行时配置文件中最重要的一个，其他的一些配置都是由这个文件引出来的。这个文件可以很复杂也可以很简单，里面可以有很多的配置项。
复制代码
#!/bin/sh
PATH=/sbin:/bin:/usr/sbin:/usr/bin
runlevel=S
prevlevel=N
umask 022
export PATH runlevel prevlevel
mount -a
echo /sbin/mdev > /proc/sys/kernel/hotplug
mdev -s
/bin/hostname -F /etc/sysconfig/HOSTNAME
ifconfig eth0 192.168.1.88
复制代码
PATH=xxx

(1)首先从shell脚本的语法角度分析，这一行定义了一个变量PATH，值等于后面的字符串

(2)后面用export导出了这个PATH，那么PATH就变成了一个环境变量。
(3)PATH这个环境变量是linux系统内部定义的一个环境变量，含义是操作系统去执行程序时会默认到PATH指定的各个目录下去寻找。如果找不到就认定这个程序不存在，如果找到了就去执行它。将一个可执行程序的目录导出到PATH，可以让我们不带路径来执行这个程序。
(4)rcS中为什么要先导出PATH？就是因为我们希望一旦进入命令行下时，PATH环境变量中就有默认的/bin /sbin /usr/bin /usr/sbin 这几个常见的可执行程序的路径，这样我们进入命令行后就可以ls、cd等直接使用了。
(5)为什么我们的rcS文件还没添加，系统启动就有了PATH中的值？原因在于busybox自己用代码硬编码为我们导出了一些环境变量，其中就有PATH。

runlevel=

(1)runlevel也是一个shell变量，并且被导出为环境变量。

(2)runlevel这个环境变量到底有什么用？

(3)runlevel=S表示将系统设置为单用户模式

umask=

(1)umask是linux的一个命令，作用是设置linux系统的umask值。

(2)umask值决定当前用户在创建文件时的默认权限。

mount -a

(1)mount命令是用来挂载文件系统的

(2)mount -a是挂载所有的应该被挂载的文件系统，在busybox中mount -a时busybox会去查找一个文件/etc/fstab文件，这个文件按照一定的格式列出来所有应该被挂载的文件系统（包括了虚拟文件系统）

mdev

(1)mdev是udev的嵌入式简化版本，udev/mdev是用来配合linux驱动工作的一个应用层的软件，udev/mdev的工作就是配合linux驱动生成相应的/dev目录下的设备文件。
(2)因为这个问题涉及到驱动，因此详细讲解要等到驱动部分。这里我们只是通过一些直观的现象来初步理解udev/mdev的工作效果。
(3)在rcS文件中没有启动mdev的时候，/dev目录下启动后是空的；在rcS文件中添加上mdev有关的2行配置项后，再次启动系统后发现/dev目录下生成了很多的设备驱动文件。
(4)/dev目录下的设备驱动文件就是mdev生成的，这就是mdev的效果和意义。

hostname

(1)hostname是linux中的一个shell命令。命令（hostname xxx）执行后可以用来设置当前系统的主机名为xxx，直接hostname不加参数可以显示当前系统的主机名。
(2)/bin/hostname -F /etc/sysconfig/HOSTNAME -F来指定了一个主机名配置文件（这个文件一般文件名叫hostname或者HOSTNAME）

ifconfig

(1)有时候我们希望开机后进入命令行时ip地址就是一个指定的ip地址（譬如192.168.1.30），这时候就可以在rcS文件中ifconfig eth0 192.168.1.30

3.1、初步一直rcS

1、PATH&runlevel

(1)我们实战发现rcS文件明明存在但是却提示不存在，问题原因就是rcS文件在windows下创建的，行尾换行符为'\r\n'，多了点东西。但是因为ubuntu中的vi对行尾做了优化，所以在ubuntu中是看不出来多了东西的。但是在securecrt下一看就发现每一行末尾多出来了一个^M。
(2)这个故事告诉我们：shell脚本文件如果格式不对，运行时可能会被提示文件不存在。
(3)扩展讲一个：有时候一个应用程序执行时也会提示文件不存在，问题可能是这个程序所调用的一个动态链接库找不到。
(4)测试结果：PATH本来在busybox中就已经用代码导出过了，所以rcS中再次导出没有任何明显的现象，因此看不出什么差别；runlevel实际执行结果一直是unknown，问题在于busybox并不支持runlevel这个特性。

3.2、umask测试

(1)umask是022的时候，默认touch创建一个文件的权限是644

(2)umask是044的时候，默认touch创建一个文件的权限是622

(3)umask是444的时候，默认touch创建一个文件的权限是222

总结：umask的规律就是：umask值和默认创建文件的权限值加起来是666.

3.3、mount测试

(1)挂载时全部出错：

mount: mounting proc on /proc failed: No such file or directory

mount: mounting sysfs on /sys failed: No such file or directory

mount: mounting tmpfs on /var failed: No such file or directory

mount: mounting tmpfs on /tmp failed: No such file or directory

mount: mounting tmpfs on /dev failed: No such file or directory

(2)原因是因为根文件系统中找不到挂载点。所谓挂载点就是我们要将目标文件系统（当然这里都是虚拟文件系统）挂载到当前文件系统中的某一个目录中，这个目录就是挂载点。

(3)解决方案就是自己在制作的rootfs根目录下创建这些挂载点目录即可。

(4)验证是否挂载成功，可以看挂载时输出信息；还可以启动后去看proc和sys文件夹，如果有文件出现则证明挂载成功了，如果没东西就证明失败了。

4、profile文件与命令提示符

4.1、profile文件添加

(1)之前添加了/bin/hostname在/etc/sysconfig/HOSTNAME文件中定义了一个hostname（aff），实际效果是：命令行下hostname命令查到的host名字确实是aff。但是问题就是命令行的提示符是没有显示的。
(2)这个问题的解决就要靠profile文件。将提供的profile文件放入/etc/目录下即可。
(3)添加了之后的实验现象：命令行提示符前面显示：[@aff ]#
结论是：第一，profile文件起了作用，hostname显示出来了。第二，还有个问题，登录用户名没显示出来。原因就是我们直接进入了命令行而没有做登录。等我们添加了用户登录功能，并且成功登陆后这个问题就能解决。
(4)profile文件工作原理是：profile文件也是被busybox（init进程）自动调用的，所以是认名字的。

复制代码
# Ash profile
# vim: syntax=sh
# No core files by default
ulimit -S -c 0 > /dev/null 2>&1
USER="`id -un`"
LOGNAME=$USER
PS1='[\u@\h \W]\# '
PATH=$PATH
HOSTNAME=`/bin/hostname`
export USER LOGNAME PS1 PATH
复制代码
4.2、如何看到用户登录界面

(1)linux中有一个原则就是用一个小程序来完成一个功能。如果我们产品确实需要很复杂的综合型的功能，我们倾向于先使用很多个小程序完成其中的一个功能，然后再将这些小程序集成起来完成整个大功能的产品。
(2)这种集成很多个小程序来完成一个大的功能，有很多种技术实现。譬如shell脚本，还有一些别的技术，譬如linux启动中的inittab。
(3)因为我们之前intttab中有一个配置项 ::askfirst:-/bin/sh，这个配置项作用就是当系统启动后就去执行/bin/sh，执行这个就会出现命令行。因此我们这样的安排就会直接进入命令行而不会出现登录界面。
(4)我们要出现登录界面，就不能直接执行/bin/sh，而应该执行一个负责出现登录界面并且负责管理用户名和密码的一个程序，busybox中也集成了这个程序（就是/bin/login和/sbin/gettty），因此我们要在inittab中用/bin/login或者/sbin/getty去替代/bin/sh。

4.3、用户名和密码的设置

(1)用户名和密码的设置是和登录程序有关联的，但是/bin/login和/sbin/getty在用户名和密码的管理上是一样的。其实常见的所有的linux系统的用户名和密码的管理几乎都是一样的。
(2)密码一般都是用加密文字的，而不是用明文。意思就是系统中的密码肯定是在系统中的一个专门用来存密码的文件中存储的，用明文存密码有风险，因此linux系统都是用密文来存储密码的。关于密文密码的使用下节课实践时会详细讲。

5、用户登录实战与密码设置移植　　

5.1、添加/bin/login到sysinit

(1)在inittab中修改，去掉/bin/sh，换上/bin/login，则系统启动后出现登录界面。可以输入用户名和密码。
(2)实验现象：成功出现用户登录界面，但是死活密码不对。

5.2、添加passwd和shadow文件

(1)为什么用户名和密码不对？因为我们根本没有为root用户设置密码。
(2)linux系统中用来描述用户名和密码的文件是passwd和shadow文件，这两个文件都在etc目录下。passwd文件中存储的是用户的密码设置，shadow文件中存储的是加密后的密码。
(3)我们直接复制ubuntu系统中的/etc/passwd和/etc/shadow文件到当前制作的rootfs目录下，然后再做修改即可。
(4)/etc/passwd和/etc/shadow修理好后，shadow中默认有一个加密的密码口令，这个口令和你拷贝的shadow本身有关，像我的ubuntu中root用户的密码就是root，因此复制过来后登陆时的密码还是root。

5.3、重置密码实践

(1)ubuntu刚装好的时候默认登录是用普通用户登录的，默认root用户是关闭的。普通用户的密码是在装系统的时候设置的，普通用户登陆后可以使用su passwd root给root用户设置密码，设置了密码后root用户才可以登录。
(2)其实这个原因就是root用户在/etc/shadow文件中加密口令是空白的。所以是不能登录的。
(3)busybox中因为没有普通用户，所以做法是：默认root用户如果加密口令是空的则默认无密码直接登录。等我们登陆了之后还是可以用passwd root给root用户设置密码。
(4)平时有时候我们忘记了自己的操作系统的密码，怎么办？有一种解决方法就是用其他系统（WindowsPE系统或者ubuntu的单用户模式等···）来引导启动，启动后挂载到我们的硬盘上，然后找到/etc/shadow文件，去掉密文密码后保存。然后再重启系统后密码就没了。

5.4、getty实战

(1)大家后面做项目会发现，inittab中最常见的用于登录的程序不是/bin/login，反而是/sbin/getty。
(2)这两个的差别不详，但是在busybox中这两个是一样的。这两个其实都是busybox的符号链接而已。因此不用严格区分这两个
(3)我们可以在inittab中用getty替换login程序来实现同样的效果。
s3c2410_serial2::sysinit:/bin/login
s3c2410_serial2::respawn:/sbin/getty -L s3c2410_serial2 115200 vt100

复制代码
#first:run the system script file
::sysinit:/etc/init.d/rcS
#::askfirst:-/bin/sh
#s3c2410_serial2::sysinit:/bin/login
s3c2410_serial2::respawn:/sbin/getty -L s3c2410_serial2 115200 vt100
::ctrlaltdel:-/sbin/reboot
#umount all filesystem
::shutdown:/bin/umount -a -r
#restart init process
::restart:/sbin/init
复制代码
到此为止一个比较完备的根文件系统就做好了

附加：动态链接库的拷贝

当前的系统由于缺乏必要的C语言库文件只能执行静态链接的文件。如要执行动态链接库的文件需要将常用的动态链接库复制过来。

将arm-linux-gcc的动态链接库文件复制到开发板rootfs的/lib目录下即可解决。

附近1：开机自启动

rcS会在开机时候运行，可以将需要开机自动执行的文件加入即可开机自启动。

附近2：前台运行与后台运行

(1)程序运行时占用了当前的控制台，因此这个程序不结束我们都无法使用控制台，这就叫前台运行。默认执行程序就是前台运行的。
(2)后台运行就是让这个程序运行，并且同时让出控制台。这时候运行的程序还能照常运行而且还能够不影响当前控制台的使用。
(3)让一个程序后台运行的方法就是 ./xxx &

附件3：制作ext2格式的镜像并烧录启动


(1)执行下述命令

dd if=/dev/zero of=rootfs.ext2 bs=1024 count=10240
losetup /dev/loop1 rootfs.ext2
mke2fs -m 0 /dev/loop1 10240
mount -t ext2 /dev/loop1 ./ext2_rootfs/
(2)向./rootfs中复制内容，用cp ../rootfs/* ./ -rf
(3)umount /dev/loop1
losetup -d /dev/loop1
(4)完成后得到的rootfs.ext2就是我们做好的rootfs镜像。拿去烧录即可。

