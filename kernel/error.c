全志A33-修改linux kernel启动参数
开发板：A33，运行linux-3.4.39

1. 修改kernel启动参数的几种方式

1）make menuconfig里修改“General Setup”子菜单中的“Default kernel command string” 选项，通过修改这个选项来修改autoconf.h文件中的CONFIG_CMDLINE宏。

2）直接修改arch/arm/kernel/setup.c文件default_command_line，CONFIG_CMDLINE宏的值最终是赋值给这个变量的。

3）一般情况下还可以修改uboot的环境变量中的启动参数bootargs，这个启动参数会由uboot传递给内核。

2. 内核配置中有如下选项

make menuconfig-> Boot options -> Kernel command line type (***) (X ) Use bootloader kernel arguments if available ( ) Extend bootloader kernel arguments () Always use the default kernel command string


解释：

第一项：Use bootloader kernel arguments if available

 Uses the command-line options passed by the boot loader. If the boot loader doesn't provide any, the default kernel command string provided in CMDLINE will be used.

第二项：Extend bootloader kernel arguments

The command-line arguments provided by the boot loader will be appended to the default kernel command string.

BootLoader环境变量bootargs被追加到kernel command string后面

第三项：Always use the default kernel command string

Always use the default kernel command string, even if the boot loader passes other arguments to the kernel. This is useful if you cannot or don't want to change the command-line options your boot loader passes to the kernel.

如果选择第三项，则内核只使用default kernel command string作为kernel启动参数（也在Boot options下设置）。
A33默认为：earlyprintk=ttyS0,115200 loglevel=9 initcall_debug=1 console=ttyS0,115200 ramfs

3. 测试

将Kernel command line type 配置为第二项：Extend bootloader kernel arguments，编译内核，然后重新打包烧录到A33。



内核启动时可以发现，uboot的环境变量bootargs被添加到kernel command string后面，如下：

Kernel command line: earlyprintk=ttyS0,115200 console=ttyS0,115200 root=/dev/mmcblk0p7 init=/init ......

注：kernel command string已修改为：earlyprintk=ttyS0,115200

 建议选择第一项：Use bootloader kernel arguments if available，因为修改uboot环境变量bootargs要比修改内核再重新编译烧录简单高效的多。

 

4. uboot给kernel传参：bootargs

（1）linux内核启动时可以接收uboot给他传递的启动参数，这些启动参数是uboot和内核约定好的形式、内容，linux内核在这些启动参数的指导下完成启动过程。这样的设计是为了灵活，为了内核在不重新编译的情况下可以用不同的方式启动。

（2）我们要做的事情就是：在uboot的环境变量中设置bootargs，然后bootm命令启动内核时会自动将bootargs传给内核。

（3）bootargs=console=ttySAC2,115200 root=/dev/mmcblk0p2 rw init=/linuxrc rootfstype=ext3

意义解释：

console=ttySAC2,115200 控制台使用串口2，波特率115200.

root=/dev/mmcblk0p2 rw 根文件系统在SD卡端口0设备（iNand）第2分区，根文件系统是可读可写的

init=/linuxrc linux的进程1（init进程）的路径

rootfstype=ext3 根文件系统的类型是ext3

（4）A33开发板bootargs 参数设置如下

setargs_mmc=setenv bootargs console=${console} root=${mmc_root} init=${init} ion_cma_512m=120m ion_cma_1g=176m ion_carveout_512m=96m ion_carveout_1g=150m coherent_pool=4m loglevel=${loglevel} partitions=${partitions}

 

setargs_nand=setenv bootargs console=${console} root=${nand_root} init=${init} ion_cma_512m=120m ion_cma_1g=176m ion_carveout_512m=96m ion_carveout_1g=150m coherent_pool=4m loglevel=${loglevel} partitions=${partitions}

 

5. 环境变量

（1）第一个命令：printenv/print

print命令不用带参数，作用是打印出系统中所有的环境变量。

环境变量就好像程序的全局变量一样。程序中任何地方都可以根据需要去调用或者更改环境变量（一般都是调用），环境变量和全局变量不同之处在于：全局变量的生命周期是在程序的一次运行当中，开始运行时诞生程序结束时死亡，下次运行程序时从头开始；但是环境变量被存储在Flash的另一块专门区域（Flash上有一个环境变量分区），一旦我们在程序中保存了该环境变量，那么下次开机时该环境变量的值将维持上一次更改保存后的值。

（2）设置（添加/更改）环境变量：setenv/set

用法：set name value

（3）保存环境变量的更改：saveenv/save

saveenv/save命令不带参数，直接执行，作用是将内存中的环境变量的值同步保存到Flash中环境变量的分区。注意：环境变量的保存是整体的覆盖保存，也就是说内存中所有的环境变量都会整体的将Flash中环境变量分区中原来的内容整体覆盖。

 

总结：彻底更改一个环境变量的值，需要2步：第一步set命令来更改内存中的环境变量，第二步用save命令将其同步到Flash中环境变量的分区。

有时候我们只是想测试下这个环境变量，不希望影响到下一次开机，那就只set不save，这样set后当前本次运行的uboot已经起效果了，只不过没save下一次开机还是会恢复到原来的状况。

 

注：A33 uboot环境变量：

boot_fastboot=fastboot

boot_normal=sunxi_flash read 40007800 boot;boota 40007800

boot_recovery=sunxi_flash read 40007800 recovery;boota 40007800

bootcmd=run setargs_mmc boot_normal

bootdelay=0

console=ttyS0,115200

fastboot_key_value_max=0x8

fastboot_key_value_min=0x2

filesize=6B1B6

init=/init

loglevel=7

mmc_root=/dev/mmcblk0p7

nand_root=/dev/nandd

partitions=boot-res@mmcblk0p2:env@mmcblk0p5:boot@mmcblk0p6:rootfs@mmcblk0p7:data@mmcblk0p8:UDISK@mmcblk0p1

recovery_key_value_max=0x13

recovery_key_value_min=0x10

setargs_mmc=setenv bootargs console=ttyS0,115200 root=/dev/mmcblk0p7 init=/init ion_cma_512m=120m ion_cma_1g=176m ion_carveout_512m=96m ion_carveout_1g=150m coherent_pool=4m loglevel=7 partitions=boot-res@mmcblk0p2:env@mmcblk0p5:boot@mmcblk0p6:rootfs@mmcblk0p7:data@mmcblk0p8:UDISK@mmcblk0p1

setargs_nand=setenv bootargs console=${console} root=${nand_root} init=${init} ion_cma_512m=120m ion_cma_1g=176m ion_carveout_512m=96m ion_carveout_1g=150m coherent_pool=4m loglevel=${loglevel} partitions=${partitions}

stderr=serial

stdin=serial

stdout=serial
--------------------- 
作者：crazy_baoli 
来源：CSDN 
原文：https://blog.csdn.net/u012247418/article/details/80941718 
版权声明：本文为博主原创文章，转载请附上博文链接！
关于flash分区打印信息jffs2: jffs2_scan_eraseblock(): Magic bitmask 0x1985
2017年02月09日 15:58:07 shuijingshizhe 阅读数 3634
 版权声明：本文为博主原创文章，未经博主允许不得转载。 https://blog.csdn.net/shuijingshizhe/article/details/54948230
uboot中的bootargs设置如下：
bootargs=noinitrd console=ttyS0,115200n8 maxcpus=1mem=240Mmtdparts=S29GL:768K(boot),128K(env),15M(system),13M(rootfs),-(custfs);spi1.0:640k(uboot),320k(env),64k(mac),6M(system),23M(rootfs),-(config)root=/dev/mtdblock9 rw rootfstype=jffs2 init=/linuxrc
可以看到给custfs分配了3M大小的空间。
系统启动后，挂载文件系统后出现如下现象：
只有一片串行flash的设备上不会有此打印信息，同时又串行和并行flash的设备上有此打印信息（此打印信息时有时无，有的设备上有，有的设备上没有）
jffs2: jffs2_scan_eraseblock(): Magic bitmask0x1985 not found at 0x00000000: 0x4255 instead
jffs2: jffs2_scan_eraseblock(): Magic bitmask0x1985 not found at 0x00000004: 0x0001 instead
jffs2: jffs2_scan_eraseblock(): Magic bitmask0x1985 not found at 0x00000018: 0x0fcc instead
jffs2: jffs2_scan_eraseblock(): Magic bitmask0x1985 not found at 0x0000003c: 0x9221 instead
一直到：
jffs2: jffs2_scan_eraseblock(): Magic bitmask0x1985 not found at 0x00ce01d4: 0x16f1 instead
jffs2: Further such events for this erase blockwill not be printed
jffs2: Cowardly refusing to erase blocks onfilesystem with no valid JFFS2 nodes
jffs2: empty_blocks 0, bad_blocks 0, c->nr_blocks104
   
       可以发现地址从0x00000000 ---0x00ce01d4刚好3M大小的内容。然后根据网上提供的两种解决方案：
    1. 
jffs2制作的过程中没有指定生成的文件文件系统的大小，在从内存地址中下载文件系统后写入到flash中的时候，写入的大小和生成的大小不一样，可能是由于内存中的脏数据下载到flash中去了，所以在制作jffs2文件系统的时候指定了生成文件的大小：--pad指定生成文件的大小
    2. 
/home/project/XLDK/buildroot/output/host/usr/sbin/mkfs.jffs2-e 0x10000 --pad=0x600000 -l -s 0x1000 -d/home/project/XLDK/buildroot/output/target -o/home/project/XLDK/buildroot/output/images/rootfs.jffs2
    3. 
    4. 
flash没有擦除干净
    5. 
把整片flash都擦除后重新烧写还是有此打印信息出现。
    6. 
经过这两种方法都没有解决此问题，说明问题不是出在此处，由于这块的操作一直都是针对第二片串行的flash进行操作的，发现打印出来的地址大小却是第一片并行flash的mtdblock4的大小，而且是给他分配多大空间，打印信息就报出多少空间的信息。所以定位在第一片并行flash对串行flash有影响。具体影响暂时没法定位，由于没有并行flash的座子，无法读出并行flash中的内容，无法判断flash是否被使用。
 
进入系统后df查看内存使用情况：（两片flash）
 
第一片flash的mtdblock4使用了388k的内容，当拆掉第一片flash后进入系统查看到：
      
根据df查看到内存使用情况后，突然发现文件系统下多出了一个mtdblock4，挂载在mnt目录下，对比其他设备都没有这一栏，所以这肯定是系统启动的时候系统自动创建目录并且把mtdblock4挂载到/mnt/custfs下。经过分析，后来查看到在/etc/inittab下发现了如下：
  
在文件系统制作过程中，会默认挂载mtdblock4到目录custfs下，那么之前的现象就得到解释了，这里才是出现此打印信息的真正原因所在，因为如果我们设备上有两片flash的话，mtd分区第一片flash分区如下：
mtdparts=S29GL:768K(boot),128K(env),15M(system),13M(rootfs),-(custfs);custfs刚好是mtdblock4，而系统启动过程中把mtdblock4挂载到/mnt/custfs下，并且文件类型是jffs2，这时候系统会去扫描该空间，出现打印信息，并且空间大小刚好是mtdblock4的大小，这就可以解释文档最开始的现象了。当我们拆掉并行flash后，mtd分区：
mtdparts=spi1.0:640k(uboot),320k(env),64k(mac),6M(system),23M(rootfs),-(config)其中mtdblock4刚好是rootfs，所以才会出现下面的有两个挂载的问题了
     
挂载的问题是在制作文件系统的时候添加到/etc/inittab下的，所以可以更改制作文件系统时候的文件可解决该问题，该文件目录如下：
     
修改inittab后，重新烧写rootfs.jffs2即可解决问题。
Bon分区、part分区以及mtd分区
2009年10月13日 19:21:00 robertmorris 阅读数 1004

  从角度来分：bon分区是在物理上进行擦除分区，part以及mtd是在逻辑上进行定位。
   bon命令用于对bon分区进行操作。bon分区是nand flash设备的一种简单的分区管理方式。vivi中使用bon分区时，会将相应的分区信息放在NAND Flash的最后0x4000（16k）个字节中。linux就是通过读取此地址区域的nand获得的bon分区表信息。linux启动时也会检测到vivi使用bon分区时的mtd分区，并将参数 root = /dev/mtdblock/1（或者是/dev/bon/1，根据情况而定）传递给内核。
   Part和mtd则是从逻辑上对flash的定位，不过part是从vivi角度对flash进行分区。而执行完vivi后，vivi将权利交给操作系统，此时vivi已死，操作系统无法从part角度查看地址，只能通过mtd来启动根文件系统。根文件系统是由vivi下载到vivi的part分区中的root段，os想要找到根文件系统，mtd的根文件系统定位就必须与part的定位相一致，系统才能正常工作。如图1


图1
当然mtd的分区可以有多个，但至少存有root区与vivi的root区相对应。
 
Ubuntu 下J-Link配置及使用
 
分类： 嵌入式
2016-06-02 16:05:35
一.配置Jlink
1.jlink for Linux包下载地址
https://www.segger.com/jlink-software-beta-version.html
当前最新版本为V5.41，以此为例

2.将其放在用户目录中解压
tar zxvf JLink_Linux_V541e_i386.tgz

3.修改文件夹名
mv JLink_Linux_V541e_i386 Jlink

4.配置(读README.txt文件)
cd Jlink
cp 99-jlink.rules /etc/udev/rules.d

5.重启系统，然后在虚拟机中，点击连接Jlink

6.启动Jlink for linux
./JLinkExe
点击(此处)折叠或打开
    1. user@instant-contiki:~/Jlink$ ./JLinkExe 
    2. SEGGER J-Link Commander V5.41e (Compiled May 30 2016 17:33:48)
    3. DLL version V5.41e, compiled May 30 2016 17:33:43
    4. 
    5. Connecting to J-Link via USB...O.K.
    6. Firmware: J-Link ARM V8 compiled Jan 31 2018 18:34:52
    7. Hardware version: V8.00
    8. S/N: 20080643
    9. License(s): RDI,FlashDL,FlashBP,JFlash,GDBFull
    10. VTref = 1.831V
    11. 
    12. 
    13. Type "connect" to establish a target connection, '?' for help
    14. J-Link>
二.使用Jlink
1.设置目标板
J-Link>exec device=STM32F103VE

2.擦除目标板
J-Link>erase

3.下载bin文件
J-Link>loadbin /home/user/file/led.bin,0x08000000

4.退出
J-Link>q

5.重启开发板
u-boot移植--2、EN29LV160AB Nor Flash移植
2017年11月24日 20:27:44 a827415225 阅读数 327
 版权声明：本文为博主原创文章，未经博主允许不得转载。 https://blog.csdn.net/a827415225/article/details/77389176
常常说自己是老年人记忆，因为看过的东西很快就忘。秉承着看自己写的东西肯定要比看别人写东西要好理解的原则。写下这个系列的u-boot移植，同时也加深自己的理解。其实到现在网上大神很多，给出了很多的解决方案。我也以他们为参考。与之不同我会加上一些我自己对某些地方的理解来进行处理。希望对大家有帮助。
开发板:天嵌TQ2440
u-boot:u-boot-1.1.6
手里的开发板是天嵌的TQ2440，看的书是韦东山老师的《嵌入式linux开发完全手册》，有些许不同的地方，增加了移植困得，当应该也更加深自己的理解。

这一讲是u-boot移植的Nor Flash移植。
我使用的开发板Nor Flash型号为 EN29LV160AB
首先看一波电路
 
通过net label可以知道Nor flash接的是S3C2440的bank0,Nor flash数据手册的第47位为BYTE#，当其接高电平时，表示16位模式，即word模式。如图中所示我们所用的就是16为模式，所以图中数据接口D0-D15接的是S3C2440数据线LDATA0-LDATA15。地址接口A0-A19接S3C2440地址线的LADDR1-LADDR20。这里解释一下，为什么是从LADDR1开始接，原因就是我们使用了16位数据线模式，一次地址写两个字节。将地址线错一位接，使通过S3C2440访问读写的地址与实际的地址相符。举例来说就是你读0x00地址的数据与0x01地址的数据得到的是相同的结果。真正的差异是从LADDR1位开始的。（我也不知道自己解释的清不清楚。。）看电路图中接了LADDR21和LADDR22到NC管脚，据说这是做芯片扩展用，这里我们不用理会。实际上S3C2440可访问的存储空间为2^20*16bit=2Mbyte，这与芯片容量相符。
分析完芯片，我们来看看如何修改u-boot能够与之相匹配呢？
1、修改./linclude/configs/TQ2440.h
在TQ2440.h文件中有关于flash芯片的定义与支持，读取里面的代码我们发现之前2410支持两种片子即AMD_LV400与AMD_LV800。我们需要修改将对上面两个片子的支持屏蔽掉，并添加自己的代码。具体修改后的样子如下图所示。
    . 
/*-----------------------------------------------------------------------
    . 
    . 
 * FLASH and environment organization
    . 
    . 
 */
    . 
    . 
 
    . 
    . 
#if 0
    . 
    . 
#define CONFIG_AMD_LV400	1	/* uncomment this if you have a LV400 flash */
    . 
    . 
#define CONFIG_AMD_LV800	1	/* uncomment this if you have a LV800 flash */
    . 
    . 
#endif
    . 
    . 
 
    . 
    . 
#define CONFIG_EON_LV160 	1	/* uncomment this if you have a LV160 flash */
    . 
    . 
 
    . 
    . 
#define CFG_MAX_FLASH_BANKS	1	/* max number of memory banks */
    . 
    . 
 
    . 
    . 
#ifdef CONFIG_EON_LV160
    . 
    . 
#define PHYS_FLASH_SIZE		0x00200000 /* 2MB */
    . 
    . 
#define CFG_MAX_FLASH_SECT	(35)	/* max number of sectors on one chip 35 is define in datasheet */
    . 
    . 
#define CFG_ENV_ADDR		(CFG_FLASH_BASE + 0x1F0000) /* addr of environment the CFG_FLASH_BASE is define in front 0x00000000 */
    . 
    . 
#endif
    . 
    . 
 
    . 
    . 
#ifdef CONFIG_AMD_LV800
    . 
    . 
#define PHYS_FLASH_SIZE		0x00100000 /* 1MB */
    . 
    . 
#define CFG_MAX_FLASH_SECT	(19)	/* max number of sectors on one chip */
    . 
    . 
#define CFG_ENV_ADDR		(CFG_FLASH_BASE + 0x0F0000) /* addr of environment */
    . 
    . 
#endif
    . 
    . 
 
    . 
    . 
#ifdef CONFIG_AMD_LV400
    . 
    . 
#define PHYS_FLASH_SIZE		0x00080000 /* 512KB */
    . 
    . 
#define CFG_MAX_FLASH_SECT	(11)	/* max number of sectors on one chip */
    . 
    . 
#define CFG_ENV_ADDR		(CFG_FLASH_BASE + 0x070000) /* addr of environment */
    . 
    . 
#endif
    . 

上面说到2410支持两种片子即AMD_LV400与AMD_LV800，实际上官方也为其编写了完善的驱动代码（其路径为./board/TQ2440/flash.c，这个驱动支持AMD_LV400与AMD_LV800）。但是我们使用的EN29LV160AB显然不在其支持之列。但是其本质是大同小异的（LV400支持512KB，LV800支持1MB，EN29LV160AB支持2MB）。所以我们需要做相应的修改。首先需要明确的是我们需要在flash.c所对应的头文件中添加对EN29LV160AB的支持。其对应头文件为./include/flash.h。我们需要添加芯片制造商ID和芯片型号ID。添加的代码如下所示（这一部分的添加就是宏定义，添加位置没有要求，符合C语言规范就好，有人说要放到181行，大概是想和#define AMD_ID_LV160B放一起吧）
    . 
#define EON_ID_LV160AB   0x22492249    
    . 
    . 
#define EON_MANUFACT     0x001c001c
    . 
至于这两个参数怎么来的，我们可以看下面的EN29LV160AB的命令定义
  
首先我们配置的是word模式因此上表中我们只看word行，另外我们的Nor Flash型号为EN29LV160AB，它是Bottom Boot类型。通过以上两点我们可以知道manufacturer ID为0x001C，Device ID为0x2249。至于为什么要在定义的时候写两遍？是因为方便处理，将manufacturer ID(高16位)与Device ID(低16位)组合成一个32位的信息赋值给flash_id（这部分操作在./board/TQ2440/flash.c中完成）。
在flash.h中完成了对于flash芯片的厂商和型号ID的定义，接下来我们要修改对应的源文件来适配我们的EN29LV160AB。在修改之前我们来深入的了解一下EN29LV160AB结构。查看datasheet，我们找到了如下对我们有用的信息。
 
  
我们从中挖掘两条有用的信息
1、芯片存储空间的结构为1个8Kword，2个4Kword，1个16Kword，31个32Kword扇区，共35个扇区（word模式）
2、35个扇区的排列顺序为第0扇区大小为8K，第1、2为4K，第3为16K，后面31扇区为32K。前面4个扇区加起来刚好是32K
根据这些信息我们去处理./board/TQ2440/flash.c源文件。在u-boot中对Nor Flash的操作分别有初始化、擦除和写入，所以我们主要修改与硬件密切相关的三个函数flash_init、flash_erase、write_hword。
首先检查主扇区的大小，如下所示(即大多数扇区的大小，我们的为64KB)
 #define MAIN_SECT_SIZE  0x8000  /*32K */
接下来这两句话我们做一下解释（并没有修改）
    . 
 44 #define MEM_FLASH_ADDR1         (*(volatile u16 *)(CFG_FLASH_BASE + (0x00000555 << 1)))
    . 
    . 
 45 #define MEM_FLASH_ADDR2         (*(volatile u16 *)(CFG_FLASH_BASE + (0x000002AA << 1)))
    . 
根据上面的Table9中的命令我们可以看到对于word模式下给的地址是555和2AA，即flash芯片希望从A0-A19收到的是555和2AA。当我们通过S3C2440写地址时，我们需要注意前面我们是将LADDR1接到了flash的A0，（为了保持存储空间访问的一致性）因此为了传输正确地址，我们需要在S3C2440写地址之前把地址左移一下。
好接下来我们先先修改flash_init函数。修改后的函数如下所示。
    . 
ulong flash_init (void)
    . 
    . 
{
    . 
    . 
	int i, j;
    . 
    . 
	ulong size = 0;
    . 
    . 
 
    . 
    . 
	for (i = 0; i < CFG_MAX_FLASH_BANKS; i++) {
    . 
    . 
		ulong flashbase = 0;
    . 
    . 
 
    . 
    . 
		flash_info[i].flash_id =
    . 
    . 
#if defined(CONFIG_AMD_LV400)
    . 
    . 
			(AMD_MANUFACT & FLASH_VENDMASK) |
    . 
    . 
			(AMD_ID_LV400B & FLASH_TYPEMASK);
    . 
    . 
#elif defined(CONFIG_AMD_LV800)
    . 
    . 
			(AMD_MANUFACT & FLASH_VENDMASK) |
    . 
    . 
			(AMD_ID_LV800B & FLASH_TYPEMASK);
    . 
    . 
#elif defined(CONFIG_EON_LV160)
    . 
    . 
			(EON_MANUFACT & FLASH_VENDMASK) |
    . 
    . 
			(EON_ID_LV160AB & FLASH_TYPEMASK);
    . 
    . 
#else
    . 
    . 
#error "Unknown flash configured"
    . 
    . 
#endif
    . 
    . 
			flash_info[i].size = FLASH_BANK_SIZE;
    . 
    . 
		flash_info[i].sector_count = CFG_MAX_FLASH_SECT;
    . 
    . 
		memset (flash_info[i].protect, 0, CFG_MAX_FLASH_SECT);
    . 
    . 
		if (i == 0)
    . 
    . 
			flashbase = PHYS_FLASH_1;
    . 
    . 
		else
    . 
    . 
			panic ("configured too many flash banks!\n");
    . 
    . 
		for (j = 0; j < flash_info[i].sector_count; j++) {
    . 
    . 
			if (j <= 3) {
    . 
    . 
				/* 1st one is 16 KB */
    . 
    . 
				if (j == 0) {
    . 
    . 
					flash_info[i].start[j] =
    . 
    . 
						flashbase + 0;
    . 
    . 
				}
    . 
    . 
 
    . 
    . 
				/* 2nd and 3rd are both 8 KB */
    . 
    . 
				if ((j == 1) || (j == 2)) {
    . 
    . 
					flash_info[i].start[j] =
    . 
    . 
						flashbase + 0x4000 + (j -
    . 
    . 
								      1) *
    . 
    . 
						0x2000;
    . 
    . 
				}
    . 
    . 
 
    . 
    . 
				/* 4th 32 KB */
    . 
    . 
				if (j == 3) {
    . 
    . 
					flash_info[i].start[j] =
    . 
    . 
						flashbase + 0x8000;
    . 
    . 
				}
    . 
    . 
			} else {
    . 
    . 
				flash_info[i].start[j] =
    . 
    . 
					flashbase + (j - 3) * MAIN_SECT_SIZE;
    . 
    . 
			}
    . 
    . 
		}
    . 
    . 
		size += flash_info[i].size;
    . 
    . 
	}
    . 
    . 
 
    . 
    . 
	flash_protect (FLAG_PROTECT_SET,
    . 
    . 
		       CFG_FLASH_BASE,
    . 
    . 
		       CFG_FLASH_BASE + monitor_flash_len - 1,
    . 
    . 
		       &flash_info[0]);
    . 
    . 
 
    . 
    . 
	flash_protect (FLAG_PROTECT_SET,
    . 
    . 
		       CFG_ENV_ADDR,
    . 
    . 
		       CFG_ENV_ADDR + CFG_ENV_SIZE - 1, &flash_info[0]);
    . 
    . 
 
    . 
    . 
	return size;
    . 
    . 
}
    . 
flash_init函数主要的作用就是将我们在./board/TQ2440/TQ2440.h中定义的关于flash的内容添加flash_info结构体中，便于对flash空间进行规范的管理。首先我们需要修改要写入的flashid，如上面代码中第一处红色所示。接下来对于扇区起始地址的处理，虽然我们是以word形式访问存储单元（地址上表现为LADDR1接到了flash的A0），但是表示16kB还是用0x4000表示，8KB用0x2000表示，32KB用0x8000表示。我们在红色部分做了对应的修改。
接下我们修改flash_print_info函数，这个函数就是打印芯片信息用的。
    . 
void flash_print_info (flash_info_t * info)
    . 
    . 
{
    . 
    . 
	int i;
    . 
    . 
 
    . 
    . 
	switch (info->flash_id & FLASH_VENDMASK) {
    . 
    . 
	case (AMD_MANUFACT & FLASH_VENDMASK):
    . 
    . 
		printf ("AMD: ");
    . 
    . 
		break;
    . 
    . 
        case (EON_MANUFACT & FLASH_VENDMASK):
    . 
    . 
                printf ("EON: ");
    . 
    . 
                break;
    . 
    . 
	default:
    . 
    . 
		printf ("Unknown Vendor ");
    . 
    . 
		break;
    . 
    . 
	}
    . 
    . 
 
    . 
    . 
	switch (info->flash_id & FLASH_TYPEMASK) {
    . 
    . 
	case (AMD_ID_LV400B & FLASH_TYPEMASK):
    . 
    . 
		printf ("1x Amd29LV400BB (4Mbit)\n");
    . 
    . 
		break;
    . 
    . 
	case (AMD_ID_LV800B & FLASH_TYPEMASK):
    . 
    . 
		printf ("1x Amd29LV800BB (8Mbit)\n");
    . 
    . 
		break;
    . 
    . 
        case (EON_ID_LV160AB & FLASH_TYPEMASK):
    . 
    . 
                printf ("1x EN29LV160AB (16Mbit)\n");
    . 
    . 
                 break;
    . 
    . 
	default:
    . 
    . 
		printf ("Unknown Chip Type\n");
    . 
    . 
		goto Done;
    . 
    . 
		break;
    . 
    . 
	}
    . 
    . 
 
    . 
    . 
	printf ("  Size: %ld MB in %d Sectors\n",
    . 
    . 
		info->size >> 20, info->sector_count);
    . 
    . 
 
    . 
    . 
	printf ("  Sector Start Addresses:");
    . 
    . 
	for (i = 0; i < info->sector_count; i++) {
    . 
    . 
		if ((i % 5) == 0) {
    . 
    . 
			printf ("\n   ");
    . 
    . 
		}
    . 
    . 
		printf (" %08lX%s", info->start[i],
    . 
    . 
			info->protect[i] ? " (RO)" : "     ");
    . 
    . 
	}
    . 
    . 
	printf ("\n");
    . 
    . 
 
    . 
    . 
      Done:;
    . 
    . 
}
    . 
代码中两处红色代码，是分别添加的对于厂商和芯片型号的打印支持。
接下来修改flash_erase。
    . 
	if ((info->flash_id & FLASH_VENDMASK) !=
    . 
    . 
	    (AMD_MANUFACT & FLASH_VENDMASK)) {
    . 
    . 
		return ERR_UNKNOWN_FLASH_VENDOR;
    . 
    . 
	}
    . 
    . 
改为
    . 
    . 
	if ((info->flash_id & FLASH_VENDMASK) !=
    . 
    . 
	    (EON_MANUFACT & FLASH_VENDMASK)) {
    . 
    . 
		return ERR_UNKNOWN_FLASH_VENDOR;
    . 
    . 
	}
    . 
至此关于flash支持的修改已经修改完毕。

我们make all，下载然后查看串口的效果。
 
可以看到Flash处已经变成了2MB，通过flinfo命令，我们也看到了存储单元信息与扇区信息，说明我们的移植成功了。

最后说一下，我们的移植仅限于兼容性移植，没有对命令时序等做过多的关注，有兴趣的同学可以研究一下。


参考文献：1、《嵌入式linux开发完全手册》、韦东山
2、http://blog.csdn.net/zhaocj （这位博主下面的u-boot系列）
3、http://blog.163.com/cailing_07%40126/blog/static/3391508720116610246756/
4、http://blog.csdn.net/xsckernel/article/details/38868205
5、http://tanatseng.blog.163.com/blog/static/1749916292010102411193391/

bootcmd和bootargs参数
2011年09月02日 13:01:09 M_O_ 阅读数 9934
版权声明：本文为博主原创文章，未经博主允许不得转载。 https://blog.csdn.net/crazyman2010/article/details/6742687
u-boot的环境参数中有两个和内核启动相关的，它们是bootcmd和bootargs
1.u-boot中和环境参数有关的命令
       printenv：打印当前环境参数
       setenv param_name "value"：设置环境参数
       saveenv：保存环境参数到FLASH
2.bootcmd：
这个参数包含了一些命令，这些命令将在u-boot进入主循环后执行
示例：
       bootcmd=boot_logo;nand read 10000003c0000 300000;bootm 1000000
       意思是启动u-boot后，执行boot_logo显示logo信息，然后从nand flash中读内核映像到内存，然后启动内核。
3.bootargs
这个参数设置要传递给内核的信息，主要用来告诉内核分区信息和根文件系统所在的分区。
示例：
       root=/dev/mtdblock5 rootfstype=jffs2console=ttyS0,115200 mem=35M mtdparts=nand.0:3840k(u-boot),4096k(kernel),123136k(filesystem)
       其中：
       root=/dev/mtdblock5 表示根文件系统在第五分区
       rootfstype=jffs2 表示根文件系统的类型是jffs2
       console=ttyS0,115200 表示终端为ttyS0,串口波特率为115200
       mem=35M 表示内存大小为35M
       mtdparts告诉内核MTD分区情况，它在内核目录/drivers/mtd/cmdlinepart.c文件中有介绍：
* The format for the command line is as follows:
 *
 * mtdparts=<mtddef>[;<mtddef]
 * <mtddef>  := <mtd-id>:<partdef>[,<partdef>]
 *              where <mtd-id> is the name from the "cat /proc/mtd" command
 * <partdef> := <size>[@offset][<name>][ro][lk]
 * <mtd-id>  := unique name used in mapping driver/device (mtd->name)
 * <size>    := standard linux memsize OR "-" to denote all remaining space
 * <name>    := '(' NAME ')'
 *
 * Examples:
 *
 * 1 NOR Flash, with 1 single writable partition:
 * edb7312-nor:-
 *
 * 1 NOR Flash with 2 partitions, 1 NAND with one
 * edb7312-nor:256k(ARMboot)ro,-(root);edb7312-nand:-(home)
 */
关于mtd-id，每个FLASH都不一样，实际上我觉得和当前FLASH的驱动有关系，在内核启动后会打印相关的分区信息，里面会有相关FLASH的mtd-id。
解决引导内核遇到undefined instruction的错误
其实在上一篇随笔之前，就是在启动linux 内核的时候，出了点问题
 
刚Starting kernel ...就出现了undefined instrction，这是什么问题呢？
在网上也搜了不少资料，有两篇很有启发
http://www.latelee.org/porting-uboot/u-boot-porting-booting-kernel.html
http://blog.csdn.net/liangkaiming/article/details/5986680
就是这两篇。
 
我们先来了解下mkimage这个命令，重要关注loadaddress和entry address
./mkimage -A arch -O os -T type -C comp -a addr -e ep -n name -d data_file[:data_file...] image
-A ==> set architecture to 'arch'
-O ==> set operating system to 'os'
-T ==> set image type to 'type'
-C ==> set compression type 'comp'
-a ==> set load address to 'addr' (hex)
-e ==> set entry point to 'ep' (hex)
-n ==> set image name to 'name'
-d ==> use image data from 'datafile'
-x ==> set XIP (execute in place)
mkimage是将zImage之前加上64字节的镜像头
所以如果使用mkimage生成内核镜像文件的话，会在内核的前头加上了64byte的信息，供建立tag之用。bootm命令会首先判断bootm xxxx 这个指定的地址xxxx是否与-a指定的加载地址是否相同。
(1)如果不同的话会从这个地址开始提取出这个64byte的头部，对其进行分析，然后把去掉头部的内核复制到-a指定的load地址中去运行之
(2)如果相同的话那就让其原封不同的放在那，但-e指定的入口地址会推后64byte，以跳过这64byte的头部。
 
我们来看看这三个地址的不同情况：
1> mkimage -A arm -O linux -T kernel -C none -a 30008000 -e 
30008040 -n linux-2.6.18.8 -d zImage uImage2.6.18.8-8040 
这种情况 ，只能把 uImage download到 30008000的位置上，否则 从 30008040
是启动不了的。
原因：如果将uImage(加了头的镜像文件)下载到不同于指定加载地址的地方，则会进行上面的操作，将去掉头部的内核拷贝到指定的加载地址，此时加载地址和入口地址需要是相同的，因为已经没有镜像头了，所以此时入口地址也应该为30008000，而不应该再加上64个字节
所以在构建镜像头部中的加载地址和入口地址时千万要考虑下载的地址，否则将会启动不了。
2> mkimage -A arm -O linux -T kernel -C none -a 30008000 -e 
30008000 -n linux-2.6.18.8 -d zImage uImage2.6.18.8-8000 
这种情况download地址随便。 还是按上面说的，因为将加载地址和入口地址设置成同样的地址，在下载到任意地址时，将去掉头部的内核镜像拷贝到指定加载地址后，可以直接从加载地址开始启动。但是要是下载地址和指定加载地址相同呢？也就是下面的：

如果 tftp 下载地址==0x30008000 ， 此时因为下载地址和指定加载地址相同，所以就不会搬动，内核直接从指定加载地址自解压啦，但是因为指定的入口地址也是0x30008000,还是在镜像头处，可以看到上面的代码，如果相同没有做任何事，只是打印了提示信息，所以还得将入口地址往后推后64个字节还是从 0x30008040 启动就肯定OK 。
 
所以在制作镜像头以及下载地址就有两种情况：
1，mkimage -n 'linux-2.6.14' -A arm -O linux -T kernel -C none -a 0x30008000 -e 0x30008000 -d zImage zImage.img
加载地址和入口地址相同
tftp 0x31000000 zImage.img
bootm 0x31000000
下载地址可以任意放。
 
2,mkimage -n 'linux-2.6.14' -A arm -O linux -T kernel -C none -a 0x30008000 -e 0x30008040 -d zImage zImage.img
入口地址在加载地址后面64个字节
tftp 0x30008000 zImage.img
bootm 0x30008000
下载地址一定要在指定的加载地址上。
 
所以，我在制作镜像头的时候没有自己动手制作，而是make uImage ARCH=arm CROSS_COMPILE=arm-linux-直接一步到位，所以启动的时候应该在0x30008040的位置启动！
 
看到这个图片还是很高兴的！！！

本节总结:
uboot启动流程如下:
    • 1)设置CPU为管理模式
    • 2)关看门狗
    • 3)关中断
    • 4）设置时钟频率   
    • 5)关mmu,初始化各个bank
    • 6)进入board_init_f()函数 (初始化定时器,GPIO,串口等,划分内存区域)
    • 7)重定位     复制uboot,然后修改SDRAM上的uboot链接地址)
    • 8)清bss
    • 9)跳转到board_init_r()函数,启动流程结束

 
1.首先来安装arm-linux-gcc-4.3.2交叉编译器
mkdir  arm-linux-gcc-4.3.2                 //创建目录
tar -xjf  arm-linux-gcc-4.3.2.tar.bz2 -C arm-linux-gcc-4.3.2/  //解压到arm-linux-gcc-4.3.2目录下
然后添加环境变量:
有两种方法,第一种只是临时修改,重启虚拟机便会复位:
export PATH=/arm-linux-gcc-4.3.2/usr/local/arm/4.3.2/bin:/usr/sbin:/usr/bin... ...
             //将arm-linux-gcc-4.3.2添加到环境变量
第二种,重启不复位:
vi /etc/environment
添加:
PATH=/arm-linux-gcc-4.3.2/usr/local/arm/4.3.2/bin:/usr/sbin:/usr/bin... ...                   //将arm-linux-gcc-4.3.2添加到环境变量
2.然后进入ftp://ftp.denx.de/pub/u-boot/来下载u-boot-2012.04.01
2.1.创建source insight工程,来看代码
1)在board 目录下只添加:
board/samsung/smdk2410/               // (2410单板文件)
 2)在arch 目录下只添加:
 
arch/arm/cpu/arm920t/                //(只添加这个目录下的*.c,*.S公用文件)                
arch/arm/cpu/arm920t/s3c24x0/        //(24x0架构所有文件)
arch/arm/include/asm/                //(只添加这个目录下的*.h公用头文件)
arch/arm/include/asm/proc-armv/      //(arm架构的文件)
arch/arm/include/asm/arch-s3c24x0/   //(24x0架构头文件)
arch/arm/lib/                        //(与arm相关的库文件)
 
3)在include/configs目录下只添加:
include/configs/smdk2410.h              // (用来配置2410单板的头文件)
 2.2编译烧写:
 
tar xjf u-boot-2012.04.01.tar.bz2

cd u-boot-2012.04.01                 //进入解压后文件目录
make smdk2410_config                 //由于该uboot不支持2440板卡,所以只有配置2410板卡
make                                 //编译,生成u-boot.bin 
 
 
3.最后烧写u-boot.bin,发现无法启动,接下来便来分析uboot的启动流程
 4.首先查看arch/arm/cpu/u-boot.lds链接脚本
如下图所示,看到uboot最开始会进入_start:
  
 5. _start位于arch/arm/cpu/arm920t/start.S         
所以,我们从start.S开始分析uboot启动流程:
 
.globl _start                                //声明_start全局符号,这个符号会被lds链接脚本用到
_start:    
b     start_code                            //跳转到start_code符号处,0x00
       ldr   pc, _undefined_instruction                    //0x04
       ldr   pc, _software_interrupt                       //0x08
       ldr   pc, _prefetch_abort                           //0x0c
       ldr   pc, _data_abort                               //0x10
       ldr   pc, _not_used                                 //0x14
       ldr   pc, _irq                                      //0x18
       ldr   pc, _fiq                                      //0x20
_undefined_instruction:  .word undefined_instruction           //定义_undefined_instruction指向undefined_instruction(32位地址)
_software_interrupt:      .word software_interrupt
_prefetch_abort:    .word prefetch_abort
_data_abort:          .word data_abort
_not_used:             .word not_used
_irq:               .word irq
_fiq:               .word fiq

   .balignl 16,0xdeadbeef        //balignl使用，参考http://www.cnblogs.com/lifexy/p/7171507.html
 
 其中符号保存的地址都在顶层目录/system.map中列出来了
 
6. 从上面看到, _start会跳转到start_code处
 
start_code:

    /*设置CPSR寄存器,让CPU进入管理模式*/
       mrs  r0, cpsr                 //读出cpsr的值
       bic   r0, r0, #0x1f           //清位
       orr   r0, r0, #0xd3          //位或
       msr  cpsr, r0                 //写入cpsr
#if   defined(CONFIG_AT91RM9200DK) || defined(CONFIG_AT91RM9200EK)
       /*
        * relocate exception table
        */
       ldr   r0, =_start            
       ldr   r1, =0x0                //r1等于异常向量基地址
       mov r2, #16
copyex:
       subs       r2, r2, #1           //减16次,s表示每次减都要更新条件标志位
       ldr   r3, [r0], #4       
       str   r3, [r1], #4      //将_start标号后的16个符号存到异常向量基地址0x0~0x3c处
       bne  copyex             //直到r2减为0#endif

#ifdef CONFIG_S3C24X0

       /* 关看门狗*/
#  define pWTCON       0x53000000
#  define INTMSK 0x4A000008    /* Interrupt-Controller base addresses */
#  define INTSUBMSK  0x4A00001C
#  define CLKDIVN       0x4C000014    /* clock divisor register */

       ldr   r0, =pWTCON       
       mov r1, #0x0        
       str   r1, [r0]           //关看门狗,使WTCON寄存器=0

       /*关中断*/
       mov r1, #0xffffffff
       ldr   r0, =INTMSK
       str   r1, [r0]                  //关闭所有中断
# if defined(CONFIG_S3C2410)
       ldr   r1, =0x3ff
       ldr   r0, =INTSUBMSK
       str   r1, [r0]                  //关闭次级所有中断
# endif

    /* 设置时钟频率, FCLK:HCLK:PCLK = 1:2:4 ,而FCLK默认为120Mhz*/
       ldr   r0, =CLKDIVN
       mov r1, #3
       str   r1, [r0]

 #ifndef CONFIG_SKIP_LOWLEVEL_INIT
       bl    cpu_init_crit                         //关闭mmu,并初始化各个bank
#endif

call_board_init_f:
       ldr   sp, =(CONFIG_SYS_INIT_SP_ADDR) //CONFIG_SYS_INIT_SP_ADDR=0x30000f80
       bic   sp, sp, #7         //sp=0x30000f80
       ldr   r0,=0x00000000
       bl    board_init_f   
 
 上面的CONFIG_SYS_INIT_SP_ADDR =0x30000f80,是通过arm-linux-objdump -D u-boot>u-boot.dis生成反汇编,然后从u-boot.dis得到的,如下图所示:
  
 
 
7.然后进入第一个C函数:board_init_f()
该函数主要工作是：
清空gd指向的结构体、通过init_sequence函数数组,来初始化各个函数以及逐步填充gd结构体，最后划分内存区域,将数据保存在gd里,然后调用relocate_code()对uboot重定位
(gd是用来传递给内核的参数)
1)具体代码如下所示:
 
void board_init_f(ulong bootflag) // bootflag=0x00000000
{
       bd_t *bd;
       init_fnc_t **init_fnc_ptr;         //函数指针
       gd_t *id;
       ulong addr, addr_sp;
#ifdef CONFIG_PRAM
       ulong reg;#endif

       bootstage_mark_name(BOOTSTAGE_ID_START_UBOOT_F, "board_init_f");
       /* Pointer is writable since we allocated a register for it */
       gd = (gd_t *) ((CONFIG_SYS_INIT_SP_ADDR) & ~0x07);
 
 其中gd是一个全局变量,用来传递给内核的参数用的,如下图所示,在arch/arn/include/asm/global_data.h中定义,*gd指向r8寄存器,所以r8专门提供给gd使用
  
而CONFIG_SYS_INIT_SP_ADDR,在6节里得到=0x30000f80,所以gd=0x30000f80
 2)继续来看board_init_f():
 
      __asm__ __volatile__("": : :"memory");           //memory:让cpu重新读取内存的数据

      memset((void *)gd, 0, sizeof(gd_t));        //将0x30000f80地址上的gd_t结构体清0
      gd->mon_len = _bss_end_ofs;           // _bss_end_ofs =__bss_end__ - _start,在反汇编找到等于0xae4e0,所以mon_len等于uboot的数据长度
      gd->fdt_blob = (void *)getenv_ulong("fdtcontroladdr", 16, (uintptr_t)gd->fdt_blob);

       for (init_fnc_ptr = init_sequence; *init_fnc_ptr; ++init_fnc_ptr)            //调用init_sequence[]数组里的各个函数
      {
              if ((*init_fnc_ptr)() != 0)     //执行函数,若函数执行出错,则进入hang()
             {    
　　　　　　　　　　 hang ();   //打印错误信息,然后一直while
             }

       }
 
 上面的init_sequence[]数组里存了各个函数,比如有:
    • board_early_init_f():设置系统时钟,设置各个GPIO引脚
    • timer_init():初始化定时器
    • env_init():设置gd的成员变量
    • init_baudrate():设置波特率
    • dram_init():设置gd->ram_size= 0x04000000(64MB）
3)继续来看board_init_f():
 
addr = CONFIG_SYS_SDRAM_BASE + gd->ram_size;  // addr=0x34000000 // CONFIG_SYS_SDRAM_BASE:  SDRAM基地址,为0X30000000// gd->ram_size:          等于0x04000000 

#if !(defined(CONFIG_SYS_ICACHE_OFF) && defined(CONFIG_SYS_DCACHE_OFF))
       /* reserve TLB table */
       addr -= (4096 * 4);        //addr=33FFC000
       addr &= ~(0x10000 - 1);  // addr=33FF0000,   
       gd->tlb_addr = addr;   //将64kB分配给TLB,所以TLB地址为33FF0000~33FFFFFF#endif

       /* round down to next 4 kB limit */
       addr &= ~(4096 - 1);                    //4kb对齐, addr=33FF0000
       debug("Top of RAM usable for U-Boot at: %08lx\n", addr);
       /*
        * reserve memory for U-Boot code, data & bss
        * round down to next 4 kB limit
        */
       addr -= gd->mon_len; // 在前面分析过gd->mon_len=0xae4e0,                           //所以addr=33FF0000 -0xae4e0=33F41B20,

       addr &= ~(4096 - 1);  //4095=0xfff,4kb对齐, addr=33F41000                             //所以分配给uboot各个段的重定位地址为33F41000~33FFFFFF
       debug("Reserving %ldk for U-Boot at: %08lx\n", gd->mon_len >> 10, addr);

#ifndef CONFIG_SPL_BUILD
       addr_sp = addr - TOTAL_MALLOC_LEN; //分配一段malloc空间给addr_sp
                       //TOTAL_MALLOC_LEN=1024*1024*4,所以malloc空间为33BF1000~33F40FFF
       addr_sp -= sizeof (bd_t);            //分配一段bd_t结构体大的空间
　　　　bd = (bd_t *) addr_sp;               //bd指向刚刚分配出来的bd_t结构体
　　　　gd->bd = bd;                         // 0x30000f80处的gd变量的成员bd等于bd_t基地址
　　　　addr_sp -= sizeof (gd_t);              //分配一个gd_t结构体大的空间
　　　　id = (gd_t *) addr_sp;                 //id指向刚刚分配的gd_t结构体
　　　　gd->irq_sp = addr_sp;                 //0x30000f80处的gd变量的成员irq_sp等于gd_t基地址

　　　　addr_sp -= 12;
　　　　addr_sp &= ~0x07;
　　　　... ...

　　　　relocate_code(addr_sp, id, addr);  //进入relocate_code()函数,重定位代码,以及各个符号
　　　　// addr_sp: 栈顶,该栈顶向上的位置用来存放gd->irq_sp、id 、gd->bd、malloc、uboot、TLB(64kb),
　　　　//id:       存放 gd_t结构体的首地址
　　　　// addr:    等于存放uboot重定位地址33F41000
}
 
 执行完board_init_f()后,最终内存会划分如下图所示:
  
其实此时uboot还在flash中运行,然后会进入start.S的relocate_code()里进行uboot重定位 
8.接下来进入重定位
1)start.S的relocate_code()代码如下所示
 
relocate_code:
       mov r4, r0      /* save addr_sp */              // addr_sp栈顶值
       mov r5, r1      /* save addr of gd */           // id值
       mov r6, r2      /* save addr of destination */  // addr值:uboot重定位地址

       /* Set up the stack        */
stack_setup:
       mov sp, r4                //设置栈addr_sp
       adr  r0, _start           //在顶层目录下system.map符号文件中找到_start =0,所以r0=0
       cmp r0, r6                //判断_start(uboot重定位之前的地址)和addr(重定位地址)是否一样
       beq clear_bss             /* skip relocation */ 

       mov r1, r6             /* r1 <- scratch for copy_loop */ //r1= addr(重定位地址)
       ldr   r3, _bss_start_ofs               //_bss_start_ofs=__bss_start - _start(uboot代码大小)
       add r2, r0, r3         /* r2 <- source end address*/   //r2= uboot重定位之前的结束地址
copy_loop:
       ldmia      r0!, {r9-r10}  /* copy from source address [r0] */                              //将r0处的两个32位数据拷到r9-r10中,然后r0+=8
       stmia      r1!, {r9-r10}  /* copy to   target address [r1]*/                             //将拷出来的两个数据放入r1(重定位地址)处,然后r1+=8
       cmp r0, r2  /* until source end address [r2]*/   //判断拷贝的数据是否到结束地址
       blo  copy_loop
 
 上面只是把代码复制到SDRAM上,而链接地址内容却没有改变,比如异常向量0x04的代码内容还是0x1e0,
我们以异常向量0x04为例,来看它的反汇编:
  
如上图所示,即使uboot在SDRAM运行,由于代码没修改,PC也会跳到0x1e0(flash地址)上
和之前老的uboot有很大区别,以前老的uboot直接是使用的SDRAM链接地址,如下图所示:
  
所以,新的uboot采用了动态链接地址的方法,在链接脚本uboot.lds中,可以看到这两个段(.rel.dyn、.dynsym):
  
该两个段里,便是保存了各个文件的相对动态信息(.rel.dyn)、动态链接地址的符号(.dynsym)
以上图的.rel.dyn段为例来分析,找到__rel_dyn_start符号处:
  
如上图所示,其中0x17表示的是符号的结束标志位,我们以0x20为例来讲解:
在之前,我们讲过0x20里面保存的是异常向量0x04跳转的地址(0x1e0),如下图所示:
  
所以,接下来的代码,便会根据0x20里的值0x1e0(flash地址),将SDRAM的33F41000+0x20的内容改为33F41000+0x1e0(SDRAM地址),来改变uboot的链接地址
2)重定位的剩余代码,如下所示:
 
#ifndef CONFIG_SPL_BUILD
       /*
        * fix .rel.dyn relocations
        */
       ldr   r0, _TEXT_BASE             /* r0 <- Text base */  //r0=text段基地址=0
       sub  r9, r6, r0         /* r9 <- relocation offset */   //r9= 重定位后的偏移值=33F41000
       ldr   r10, _dynsym_start_ofs  /* r10 <- sym table ofs */ 
                                          //_dynsym_start_ofs =__dynsym_start - _start=0x73608
                                          //所以r10=动态符号表的起始偏移值=0x73608
       add r10, r10, r0            /* r10 <- sym table in FLASH */
                                       //r10=flash上的动态符号表基地址=0x73608
       ldr   r2, _rel_dyn_start_ofs     /* r2 <- rel dyn start ofs */
                                          //r2=__rel_dyn_start - _start=0x6b568
                                          //所以r2=相对动态信息的起始偏移值=0x6b568
       add r2, r2, r0         /* r2 <- rel dyn start in FLASH */
                                      //r2=flash上的相对动态信息基地址=0x6b568
       ldr   r3, _rel_dyn_end_ofs      /* r3 <- rel dyn end ofs */
                                          // _rel_dyn_end_ofs=__rel_dyn_end - _start=00073608
                                          //所以r3=相对动态信息的结束偏移值=00073608

       add r3, r3, r0         /* r3 <- rel dyn end in FLASH */
                                    //r3=flash上的相对动态信息结束地址=0x6b568

fixloop:
       ldr   r0, [r2]           /* r0 <- location to fix up, IN FLASH! */                               //以0x20为例,r0=0x6b568地址处的内容= 0x20
       add r0, r0, r9         /* r0 <- location to fix up in RAM */
                                     //r0=33F41000+0x20=33F41020

       ldr   r1, [r2, #4]             //r1= 33F41024地址处的内容=0x17
       and  r7, r1, #0xff       
       cmp r7, #23                  /* relative fixup? */  //0x17=23,所以相等
       beq fixrel                                       //跳到:fixerl
       cmp r7, #2                    /* absolute fixup? */
       beq fixabs
       /* ignore unknown type of fixup */
       b     fixnext
fixabs:
       /* absolute fix: set location to (offset) symbol value */
       mov r1, r1, LSR #4         /* r1 <- symbol index in .dynsym */
       add r1, r10, r1              /* r1 <- address of symbol in table */
       ldr   r1, [r1, #4]             /* r1 <- symbol value */
       add r1, r1, r9         /* r1 <- relocated sym addr */
       b     fixnext

fixrel:
       /* relative fix: increase location by offset */
       ldr   r1, [r0]                  //r1=33F41020地址处的内容=0x1e0
       add r1, r1, r9                //r1=0x1e0+33F41000= 33F411e0
fixnext:
       str   r1, [r0]             //改变链接地址里的内容, 33F41020=33F411e0  (之前为0x1e0)   
       add r2, r2, #8             //r2等于下一个相对动态信息(0x24)的地址
       cmp r2, r3                //若没到尾部__rel_dyn_end,便继续执行: fixloop
       blo  fixloop                 #endif
 
 9.清除bss段
 
/*重定位完成后,清除bss段*/
clear_bss:
 #ifndef CONFIG_SPL_BUILD
       ldr   r0, _bss_start_ofs                        //获取flash上的bss段起始位置
       ldr   r1, _bss_end_ofs                          //获取flash上的bss段结束位置
       mov r4, r6                    /* reloc addr */     //获取r6(SDRAM上的uboot基地址)
       add r0, r0, r4                                  //加上重定位偏移值,得到SDRAM上的bss段起始位置
       add r1, r1, r4                                     //得到SDRAM上的bss段结束位置
       mov r2, #0x00000000           /* clear*/

clbss_l:
    str    r2, [r0]           /* clear loop...       */                 //开始清除SDRAM上的bss段
       add r0, r0, #4
       cmp r0, r1
       bne  clbss_l
       bl coloured_LED_init
       bl red_led_on#endif
 
9.1继续往下分析
 
#ifdef CONFIG_NAND_SPL                   //未定义,所以不执行
  ... ...                          #else                                   //执行else

       ldr   r0, _board_init_r_ofs         //r0=flash上的board_init_r()函数地址偏移值
       adr  r1, _start                    //0
       add lr, r0, r1                     //返回地址lr=flash上的board_init_r()函数
       add lr, lr, r9                     //加上重定位偏移值(r9)后,lr=SDRAM上的board_init_r()函数

       /* setup parameters for board_init_r */
       mov r0, r5             /* gd_t */              //r0=id值
       mov r1, r6             /* dest_addr */         //r1=uboot重定位地址
       /* jump to it ... */
       mov pc, lr              //跳转:  board_init_r()函数
_board_init_r_ofs:
       .word board_init_r - _start        //获取在flash上的board_init_r()函数地址偏移值
#endif
 
从上面代码看出, 接下来便会进入uboot的board_init_r()函数,该函数会对各个外设初始化、环境变量初始化等.
 
uboot的启动过程到此便结束了.
下一章便来修改uboot,添加2440板卡,实现nor启动,nand启动
[uboot] （第五章）uboot流程——uboot启动流程
 
http://blog.csdn.net/ooonebook/article/details/53070065
以下例子都以project X项目tiny210（s5pv210平台,armv7架构）为例
[uboot] uboot流程系列： 
[project X] tiny210(s5pv210)上电启动流程（BL0-BL2） 
[project X] tiny210(s5pv210)从存储设备加载代码到DDR 
[uboot] （第一章）uboot流程——概述 
[uboot] （第二章）uboot流程——uboot-spl编译流程 
[uboot] （第三章）uboot流程——uboot-spl代码流程 
[uboot] （第四章）uboot流程——uboot编译流程 
[uboot] （番外篇）global_data介绍 
[uboot] （番外篇）uboot relocation介绍
建议先看《[project X] tiny210(s5pv210)上电启动流程（BL0-BL2）》，根据例子了解一下上电之后的BL0\BL1\BL2阶段，以及各个阶段的运行位置，功能。 
建议可以和《[uboot] （番外篇）global_data介绍》和《[uboot] （番外篇）uboot relocation介绍》结合起来看。
=================================================================================
一、uboot说明
1、uboot要做的事情
CPU初始刚上电的状态。需要小心的设置好很多状态，包括cpu状态、中断状态、MMU状态等等。其次，就是要根据硬件资源进行板级的初始化，代码重定向等等。最后，就是进入命令行状态，等待处理命令。 
在armv7架构的uboot，主要需要做如下事情
    • 
arch级的初始化
    • 
        ◦ 关闭中断，设置svc模式
        ◦ 禁用MMU、TLB
        ◦ 关键寄存器的设置，包括时钟、看门狗的寄存器
    • 
板级的初始化
    • 
        ◦ 堆栈环境的设置
        ◦ 代码重定向之前的板级初始化，包括串口、定时器、环境变量、I2C\SPI等等的初始化
        ◦ 进行代码重定向
        ◦ 代码重定向之后的板级初始化，包括板级代码中定义的初始化操作、emmc、nand flash、网络、中断等等的初始化。
        ◦ 进入命令行状态，等待终端输入命令以及对命令进行处理
上述工作，也就是uboot流程的核心。
2、疑问
    • 
在前面的文章中虽然已经说明了，在spl的阶段中已经对arch级进行了初始化了，为什么uboot里面还要对arch再初始化一遍？ 
回答：spl对于启动uboot来说并不是必须的，在某些情况下，上电之后uboot可能在ROM上或者flash上开始执行而并没有使用spl。这些都是取决于平台的启动机制。因此uboot并不会考虑spl是否已经对arch进行了初始化操作，uboot会完整的做一遍初始化动作，以保证cpu处于所要求的状态下。
    • 
    • 
和spl在启动过程的差异在哪里？ 
回答：以tiny210而言，前期arch的初始化流程基本上是一致的，出现本质区别的是在board_init_f开始的。
    • 
        ◦ spl的board_init_f是由board自己实现相应的功能，例如tiny210则是在board/samsung/tiny210/board.c中。其主要实现了复制uboot到ddr中，并且跳转到uboot的对应位置上。一般spl在这里就可以完成自己的工作了。
        ◦ uboot的board_init_f是在common下实现的，其主要实现uboot relocate前的板级初始化以及relocate的区域规划，其还需要往下走其他初始化流程。
3、代码入口
project-X/u-boot/arch/arm/cpu/u-boot.lds
ENTRY(_start)
    • 1
    • 1
所以uboot-spl的代码入口函数是_start 
对应于路径project-X/u-boot/arch/arm/lib/vector.S的_start，后续就是从这个函数开始分析。
二、代码整体流程
1、首先看一下主枝干的流程（包含了arch级的初始化）
在arch级初始化是和spl完全一致的 
_start———–>reset————–>关闭中断 
………………………………| 
………………………………———->cpu_init_cp15———–>关闭MMU,TLB 
………………………………| 
………………………………———->cpu_init_crit————->lowlevel_init————->关键寄存器的配置和初始化 
………………………………| 
………………………………———->_main————–>进入板级初始化，具体看下面
2、板级初始化的流程
_main————–>board_init_f_alloc_reserve —————>堆栈、GD、early malloc空间的分配 
…………| 
…………————->board_init_f_init_reserve —————>堆栈、GD、early malloc空间的初始化 
…………| 
…………————->board_init_f —————>uboot relocate前的板级初始化以及relocate的区域规划 
…………| 
…………————->relocate_code、relocate_vectors —————>进行uboot和异常中断向量表的重定向 
…………| 
…………————->旧堆栈的清空 
…………| 
…………————->board_init_r —————>uboot relocate后的板级初始化 
…………| 
…………————->run_main_loop —————>进入命令行状态，等待终端输入命令以及对命令进行处理
三、arch级初始化代码分析
1、_start
上述已经说明了_start是整个uboot的入口，其代码如下： 
arch/arm/lib/vector.S
_start:#ifdef CONFIG_SYS_DV_NOR_BOOT_CFG
    .word   CONFIG_SYS_DV_NOR_BOOT_CFG#endif
    b   reset
会跳转到reset中。
2、reset
建议先参考[kernel 启动流程] （第二章）第一阶段之——设置SVC、关闭中断，了解一下为什么要设置SVC、关闭中断以及如何操作。
代码如下： 
arch/arm/cpu/armv7/start.S
    .globl  reset
    .globl  save_boot_params_ret

reset:
    /* Allow the board to save important registers */
    b   save_boot_params
save_boot_params_ret:
    /*
     * disable interrupts (FIQ and IRQ), also set the cpu to SVC32 mode,
     * except if in HYP mode already
     */
    mrs r0, cpsr
    and r1, r0, #0x1f       @ mask mode bits
    teq r1, #0x1a       @ test for HYP mode
    bicne   r0, r0, #0x1f       @ clear all mode bits
    orrne   r0, r0, #0x13       @ set SVC mode
    orr r0, r0, #0xc0       @ disable FIQ and IRQ
    msr cpsr,r0
@@ 以上通过设置CPSR寄存器里设置CPU为SVC模式，禁止中断
@@ 具体操作可以参考《[kernel 启动流程] （第二章）第一阶段之——设置SVC、关闭中断》的分析

    /* the mask ROM code should have PLL and others stable */#ifndef CONFIG_SKIP_LOWLEVEL_INIT
    bl  cpu_init_cp15
@@ 调用cpu_init_cp15，初始化协处理器CP15,从而禁用MMU和TLB。
@@ 后面会有一小节进行分析

    bl  cpu_init_crit
@@ 调用cpu_init_crit，进行一些关键的初始化动作，也就是平台级和板级的初始化
@@ 后面会有一小节进行分析#endif

    bl  _main
@@ 跳转到主函数，也就是板级初始化函数
@@ 下一节中进行说明。
3、cpu_init_cp15
建议先参考[kernel 启动流程] （第六章）第一阶段之——打开MMU两篇文章的分析。 
cpu_init_cp15主要用于对cp15协处理器进行初始化，其主要目的就是关闭其MMU和TLB。 
代码如下(去掉无关部分的代码)： 
arch/arm/cpu/armv7/start.S
ENTRY(cpu_init_cp15)
    /*
     * Invalidate L1 I/D
     */
    mov r0, #0          @ set up for MCR
    mcr p15, 0, r0, c8, c7, 0   @ invalidate TLBs
    mcr p15, 0, r0, c7, c5, 0   @ invalidate icache
    mcr p15, 0, r0, c7, c5, 6   @ invalidate BP array
    mcr     p15, 0, r0, c7, c10, 4  @ DSB
    mcr     p15, 0, r0, c7, c5, 4   @ ISB
@@ 这里只需要知道是对CP15处理器的部分寄存器清零即可。
@@ 将协处理器的c7\c8清零等等，各个寄存器的含义请参考《ARM的CP15协处理器的寄存器》

    /*
     * disable MMU stuff and caches
     */
    mrc p15, 0, r0, c1, c0, 0
    bic r0, r0, #0x00002000 @ clear bits 13 (--V-)
    bic r0, r0, #0x00000007 @ clear bits 2:0 (-CAM)
    orr r0, r0, #0x00000002 @ set bit 1 (--A-) Align
    orr r0, r0, #0x00000800 @ set bit 11 (Z---) BTB#ifdef CONFIG_SYS_ICACHE_OFF
    bic r0, r0, #0x00001000 @ clear bit 12 (I) I-cache#else
    orr r0, r0, #0x00001000 @ set bit 12 (I) I-cache#endif
    mcr p15, 0, r0, c1, c0, 0
@@ 通过上述的文章的介绍，我们可以知道cp15的c1寄存器就是MMU控制器
@@ 上述对MMU的一些位进行清零和置位，达到关闭MMU和cache的目的，具体的话去看一下上述文章吧。

ENDPROC(cpu_init_cp15)
4、cpu_init_crit
cpu_init_crit，进行一些关键寄存器的初始化动。其代码核心就是lowlevel_init，如下 
arch/arm/cpu/armv7/start.S
ENTRY(cpu_init_crit)
    /*
     * Jump to board specific initialization...
     * The Mask ROM will have already initialized
     * basic memory. Go here to bump up clock rate and handle
     * wake up conditions.
     */
    b   lowlevel_init       @ go setup pll,mux,memoryENDPROC(cpu_init_crit)
所以说lowlevel_init就是这个函数的核心。 
lowlevel_init一般是由板级代码自己实现的。但是对于某些平台来说，也可以使用通用的lowlevel_init，其定义在arch/arm/cpu/lowlevel_init.S中 
以tiny210为例，在移植tiny210的过程中，就需要在board/samsung/tiny210下，也就是板级目录下面创建lowlevel_init.S，在内部实现lowlevel_init。（其实只要实现了lowlevel_init了就好，没必要说在哪里是实现，但是通常规范都是创建了lowlevel_init.S来专门实现lowlevel_init函数）。
在lowlevel_init中，我们要实现如下：
    • 检查一些复位状态
    • 关闭看门狗
    • 系统时钟的初始化
    • 内存、DDR的初始化
    • 串口初始化（可选）
    • Nand flash的初始化
下面以tiny210的lowlevel_init为例（这里说明一下，当时移植tiny210的时候，是直接把kangear的这个lowlevel_init.S文件拿过来用的） 
这部分代码和平台相关性很强，简单介绍一下即可 
board/samsung/tiny210/lowlevel_init.S
lowlevel_init:
    push    {lr}

    /* check reset status  */

    ldr r0, =(ELFIN_CLOCK_POWER_BASE+RST_STAT_OFFSET)
    ldr r1, [r0]
    bic r1, r1, #0xfff6ffff
    cmp r1, #0x10000
    beq wakeup_reset_pre
    cmp r1, #0x80000
    beq wakeup_reset_from_didle
@@ 读取复位状态寄存器0xE010_a000的值，判断复位状态。

    /* IO Retention release */
    ldr r0, =(ELFIN_CLOCK_POWER_BASE + OTHERS_OFFSET)
    ldr r1, [r0]
    ldr r2, =IO_RET_REL
    orr r1, r1, r2
    str r1, [r0]
@@ 读取混合状态寄存器E010_e000的值，对其中的某些位进行置位，复位后需要对某些wakeup位置1，具体我也没搞懂。

    /* Disable Watchdog */
    ldr r0, =ELFIN_WATCHDOG_BASE    /* 0xE2700000 */
    mov r1, #0
    str r1, [r0]
@@ 关闭看门狗

@@ 这里忽略掉一部分对外部SROM操作的代码

    /* when we already run in ram, we don't need to relocate U-Boot.
     * and actually, memory controller must be configured before U-Boot
     * is running in ram.
     */
    ldr r0, =0x00ffffff
    bic r1, pc, r0      /* r0 <- current base addr of code */
    ldr r2, _TEXT_BASE      /* r1 <- original base addr in ram */
    bic r2, r2, r0      /* r0 <- current base addr of code */
    cmp     r1, r2                  /* compare r0, r1                  */
    beq     1f          /* r0 == r1 then skip sdram init   */
@@ 判断是否已经在SDRAM上运行了，如果是的话，就跳过以下两个对ddr初始化的步骤
@@ 判断方法如下：
@@ 1、获取当前pc指针的地址，屏蔽其低24bit，存放与r1中
@@ 2、获取_TEXT_BASE（CONFIG_SYS_TEXT_BASE）地址，也就是uboot代码段的链接地址，后续在uboot篇的时候会说明，并屏蔽其低24bit
@@ 3、如果相等的话，就跳过DDR初始化的部分

    /* init system clock */
    bl system_clock_init
@@ 初始化系统时钟，后续有时间再研究一下具体怎么配置的

    /* Memory initialize */
    bl mem_ctrl_asm_init
@@ 重点注意：在这里初始化DDR的！！！后续会写一篇文章说明一下s5pv210平台如何初始化DDR.
@@ 其实，在tiny210的项目中，已经在spl里面对ddr初始化了一遍，这里还是又重新初始化了一遍，从实际测试结果来看，并不影响正常的使用。
1:
    /* for UART */
    bl uart_asm_init
@@ 串口初始化，到这里串口会打印出一个'O'字符，后续通过写字符到UTXH_OFFSET寄存器中，就可以在串口上输出相应的字符。

    bl tzpc_init
#if defined(CONFIG_NAND)
    /* simple init for NAND */
    bl nand_asm_init
@@ 简单地初始化一下NAND flash，有可能BL2的镜像是在nand  flash上面的。#endif

    /* Print 'K' */
    ldr r0, =ELFIN_UART_CONSOLE_BASE
    ldr r1, =0x4b4b4b4b
    str r1, [r0, #UTXH_OFFSET]
@@ 再串口上打印‘K’字符，表示lowlevel_init已经完成

    pop {pc}
@@ 弹出PC指针，即返回。
当串口中打印出‘OK’的字符的时候，说明lowlevel_init已经执行完成。
三、板级初始化代码分析
1、_main
板级初始化代码的入口就是_main。从这里开始分析。 
建议可以和《[uboot] （番外篇）global_data介绍》和《[uboot] （番外篇）uboot relocation介绍》结合起来看。 
代码如下，去除无关代码部分 
arch/arm/lib/crt0.S
ENTRY(_main)
/*
 * Set up initial C runtime environment and call board_init_f(0).
 */
    ldr sp, =(CONFIG_SYS_INIT_SP_ADDR)
    bic sp, sp, #7  /* 8-byte alignment for ABI compliance */

    mov r0, sp
    bl  board_init_f_alloc_reserve
    mov sp, r0
    /* set up gd here, outside any C code */
    mov r9, r0
    bl  board_init_f_init_reserve
@@ 以上是堆栈、GD、early malloc空间的分配，具体参考《[uboot] （番外篇）global_data介绍》

    mov r0, #0
    bl  board_init_f
@@ uboot relocate前的板级初始化以及relocate的区域规划，后续小节继续说明
@@ 其中relocate区域规划也可以参考一下《[uboot] （番外篇）uboot relocation介绍》
/*
 * Set up intermediate environment (new sp and gd) and call
 * relocate_code(addr_moni). Trick here is that we'll return
 * 'here' but relocated.
 */

    ldr sp, [r9, #GD_START_ADDR_SP] /* sp = gd->start_addr_sp */
    bic sp, sp, #7  /* 8-byte alignment for ABI compliance */
    ldr r9, [r9, #GD_BD]        /* r9 = gd->bd */
    sub r9, r9, #GD_SIZE        /* new GD is below bd */
    adr lr, here
    ldr r0, [r9, #GD_RELOC_OFF]     /* r0 = gd->reloc_off */
    add lr, lr, r0
    ldr r0, [r9, #GD_RELOCADDR]     /* r0 = gd->relocaddr */
    b   relocate_code
here:/*
 * now relocate vectors
 */
    bl  relocate_vectors
@@ GD、uboot、异常中断向量表的relocate，可以参考《[uboot] （番外篇）uboot relocation介绍》，这里不详细说明
/* Set up final (full) environment */
    bl  c_runtime_cpu_setup /* we still call old routine here */
@@ 通过操作协处理器的c7寄存器来关闭Icache

    ldr r0, =__bss_start    /* this is auto-relocated! */
    ldr r3, =__bss_end      /* this is auto-relocated! */
    mov r1, #0x00000000     /* prepare zero to clear BSS */
    subs    r2, r3, r0      /* r2 = memset len */
    bl  memset
@@ 因为堆栈段已经被relocate，所以这里需要清空原来的堆栈段的内容

    bl coloured_LED_init
    bl red_led_on
@@ LED灯的初始化，可以不实现，想要实现的话，可以在board里重新实现一个函数定义。

    /* call board_init_r(gd_t *id, ulong dest_addr) */
    mov     r0, r9                  /* gd_t */
    ldr r1, [r9, #GD_RELOCADDR] /* dest_addr */
    /* call board_init_r */
    ldr pc, =board_init_r   /* this is auto-relocated! */
    /* we should not return here. */
@@ uboot relocate后的板级初始化，注意，uboot必须在这里就完成工作，或者在里面实现死循环，不应该返回。
ENDPROC(_main)
通过上述，有两个很重要的初始化函数，board_init_f和board_init_r，后续继续说明。
2、board_init_f
代码如下： 
common/board_f.c
void board_init_f(ulong boot_flags){
    gd->flags = boot_flags;
    gd->have_console = 0;// 设置global_data里面的一些标志位

    if (initcall_run_list(init_sequence_f))
        hang();// 调用initcall_run_list依次执行init_sequence_f函数数组里面的函数，initcall_run_list这里不深究// 一旦init_sequence_f的函数出错，会导致initcall_run_list返回不为0，而从卡掉
}
打开DEBUG宏之后，可以通过log观察哪些init函数被调用，如下log：
uboot log中有如下log：initcall: 23e005a4
根据u-boot.map可以发现对应
 .text.print_cpuinfo
                0x23e005a4        0x8 arch/arm/cpu/armv7/built-in.o
                0x23e005a4                print_cpuinfo
也就是说print_cpuinfo被initcall调用了。
所以uboot relocate之前的板级初始化的核心就是init_sequence_f中定义的函数了。 
如下，这里只做简单的说明，需要的时候再具体分析：
static init_fnc_t init_sequence_f[] = {
    setup_mon_len,// 计算整个镜像的长度gd->mon_len
    initf_malloc,// early malloc的内存池的设定
    initf_console_record,// console的log的缓存
    arch_cpu_init,      /* basic arch cpu dependent setup */// cpu的一些特殊的初始化
    initf_dm,
    arch_cpu_init_dm,
    mark_bootstage,     /* need timer, go after init dm */
    /* TODO: can any of this go into arch_cpu_init()? */
    env_init,       /* initialize environment */// 环境变量的初始化，后续会专门研究一下关于环境变量的内容
    init_baud_rate,     /* initialze baudrate settings */// 波特率的初始化
    serial_init,        /* serial communications setup */// 串口的初始化
    console_init_f,     /* stage 1 init of console */// console的初始化
    print_cpuinfo,      /* display cpu info (and speed) */// 打印CPU的信息
    init_func_i2c,
    init_func_spi,// i2c和spi的初始化

    dram_init,      /* configure available RAM banks */// ddr的初始化，最重要的是ddr ram size的设置！！！！gd->ram_size// 如果说uboot是在ROM、flash中运行的话，那么这里就必须要对DDR进行初始化//========================================
    setup_dest_addr,
    reserve_round_4k,
    reserve_trace,
    setup_machine,
    reserve_global_data,
    reserve_fdt,
    reserve_arch,
    reserve_stacks,// ==以上部分是对relocate区域的规划，具体参考《[uboot] （番外篇）uboot relocation介绍》
    setup_dram_config,
    show_dram_config,
    display_new_sp,
    reloc_fdt,
    setup_reloc,// relocation之后gd一些成员的设置
    NULL,
};
注意，必须保证上述的函数都正确地返回0值，否则会导致hang。
3、board_init_r
代码如下： 
common/board_r.c
void board_init_r(gd_t *new_gd, ulong dest_addr){
    if (initcall_run_list(init_sequence_r))
        hang();// 调用initcall_run_list依次执行init_sequence_r函数数组里面的函数，initcall_run_list这里不深究// 一旦init_sequence_r的函数出错，会导致initcall_run_list返回不为0，而从卡掉

    /* NOTREACHED - run_main_loop() does not return */
    hang();// uboot要求在这个函数里面终止一切工作，或者进入死循环，一旦试图返回，则直接hang。
}
所以uboot relocate之前的板级初始化的核心就是init_sequence_r中定义的函数了。 
如下，这里只做简单的说明，需要的时候再具体分析： 
common/board_r.c
init_fnc_t init_sequence_r[] = {
    initr_trace,// trace相关的初始化
    initr_reloc,// gd中一些关于relocate的标识的设置
    initr_reloc_global_data,// relocate之后，gd中一些的成员的重新设置
    initr_malloc,// malloc内存池的设置
    initr_console_record,
    bootstage_relocate,
    initr_bootstage,#if defined(CONFIG_ARM) || defined(CONFIG_NDS32)
    board_init, /* Setup chipselects */// 板级自己需要的特殊的初始化函数，如board/samsung/tiny210/board.c中定义了board_init这个函数#endif
    stdio_init_tables,
    initr_serial,// 串口初始化
    initr_announce,// 打印uboot运行位置的log
    initr_logbuffer,// logbuffer的初始化
    power_init_board,#ifdef CONFIG_CMD_NAND
    initr_nand,// 如果使用nand flash，那么这里需要对nand进行初始化#endif#ifdef CONFIG_GENERIC_MMC
    initr_mmc,// 如果使用emmc，那么这里需要对nand进行初始化#endif
    initr_env,// 初始化环境变量
    initr_secondary_cpu,
    stdio_add_devices,
    initr_jumptable,
    console_init_r,     /* fully init console as a device */
    interrupt_init,// 初始化中断#if defined(CONFIG_ARM) || defined(CONFIG_AVR32)
    initr_enable_interrupts,// 使能中断#endif
    run_main_loop,// 进入一个死循环，在死循环里面处理终端命令。
};
最终，uboot运行到了run_main_loop，并且在run_main_loop进入命令行状态，等待终端输入命令以及对命令进行处理。 
到此，uboot流程也就完成了，后续会专门说明uboot的run_main_loop是怎么运行的。
linux内核中udevd的模块自动加载机制
2010年06月24日 21:35:00 amicablehj 阅读数 11213

提交者: Addylee 日期: 2008/10/8 22:50 阅读: 636

来源: www.osplay.org
摘要: 本文讨论了linux内核中udevd的模块自动加载机制。
标题
    1. 思 考
如果想让内核启动过程中自动加载某个模块 该怎么做呢？最容易想到的方法就是到/etc/init.d/中添加一个启动脚本，然后在/etc/rcN.d/目录下创建一个符号链接，这个链接的名字 以S开头，这内核启动时，就会自动运行这个脚本了，这样就可以在脚本中使用modprobe来实现自动加载。但是我们发现，内核中加载了许多硬件设备的驱 动，而搜索/etc目录，却没有发现任何脚本负责加载这些硬件设备驱动程序的模块。那么这些模块又是如何被加载的呢？
 
    • 
每一个设备都有Verdon ID, Device ID, SubVendor ID等信息。而每一个设备驱动程序，必须说明自己能够为哪些Verdon ID, Deviece
    • 
ID, SubVendor ID的设备提供服务。以PCI设备为例，它是通过一个pci_device_id的数据结构来实现这个功能的。例如：RTL8139的 pci_device_id定义为：
static struct pci_device_id rtl8139_pci_tbl[] = {

        {0x10ec, 0x8139, PCI_ANY_ID, PCI_ANY_ID, 0, 0, RTL8139 },

        {0x10ec, 0x8138, PCI_ANY_ID, PCI_ANY_ID, 0, 0, RTL8139 },

        ......

}

MODULE_DEVICE_TABLE (pci, rtl8139_pci_tbl);
上面的信息说明，凡是Verdon ID为0x10EC, Device ID为0x8139, 0x8138的PCI设备(SubVendor ID和SubDeviceID为PCI_ANY_ID，表示不限制。)，都可以使用这个驱动程序(8139too)。
    • 
在模块安装的时候，depmod会根据模块中的rtl8139_pci_tbl的信息，生成下面的信息，保存到/lib/modules/uname-r /modules.alias 文件中，其内容如下：
    • 
 
alias pci:v000010ECd00008138sv*sd*bc*sc*i* 8139too

alias pci:v000010ECd00008139sv*sd*bc*sc*i* 8139too

......
v后面的000010EC说明其Vendor ID为10EC，d后面的00008138说明Device ID为8139，而sv,和sd为SubVendor ID和SubDevice ID，后面的星号表示任意匹配。
另外在/lib/modules/uname-r /modules.dep文件中还保存这模块之间的依赖关系，其内容如下：
(这里省去了路径信息。)

8139too.ko:mii.ko
    • 
在内核启动过程中，总线驱动程序会会总线协议进行总线枚举(总线驱动程序总是集成在内核之中，不能够按模块方式加载，你可以通过make menuconfig进入Bus
    • 
options，这里面的各种总线，你只能够选择Y或N，而不能选择M.)，并且为每一个设备建立一个设备对象。每一个总线对象有一个kset对 象，每一个设备对象嵌入了一个kobject对象，kobject连接在kset对象上，这样总线和总线之间，总线和设备设备之间就组织成一颗树状结构。 当总线驱动程序为扫描到的设备建立设备对象时，会初始化kobject对象，并把它连接到设备树中，同时会调用kobject_uevent()把这个 (添加新设备的)事件，以及相关信息(包括设备的VendorID,DeviceID等信息。)通过netlink发送到用户态中。在用户态的udevd 检测到这个事件，就可以根据这些信息，打开/lib/modules/uname-r /modules.alias文件，根据
alias pci:v000010ECd00008138sv*sd*bc*sc*i* 8139too
得知这个新扫描到的设备驱动模块为8139too。于是modprobe就知道要加载8139too这个模块了，同时modprobe根据 modules.dep文件发现，8139too依赖于mii.ko，如果mii.ko没有加载，modprobe就先加载mii.ko，接着再加载 8139too.ko。
试验
在你的shell中，运行：
# ps aux | grep udevd


root     25063  ...... /sbin/udevd --daemon
我们得到udevd的进程ID为25063，现在结束这个进程：
# kill -9 25063
然后跟踪udevd，在shell中运行：
# strace -f /sbin/udevd --daemon
这时，我们看到udevd的输出如下：
......

close(8)                                = 0

munmap(0xb7f8c000, 4096)                = 0

select(7, [3 4 5 6], NULL, NULL, NULL
我们发现udevd在这里被阻塞在select()函数中。
select函数原型如下：
int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);

第一个参数：nfds表示最大的文件描述符号，这里为7(明明是6 ?)。

第二个参数：readfds为读文件描述符集合，这里为3,4,5,6.

第三个参数：writefds为写文件描述符集合，这里为NULL。

第四个参数：exceptfds为异常文件描述符集合，这里为NULL。

第五个参数：timeout指定超时时间，这里为NULL。
select函数的作用是：如果readfds中的任何一个文件有数据可读，或者witefds中的任何一个文件可以写入，或者exceptfds 中的任何一个文件出现异常时，就返回。否则阻塞当前进程，直到上诉条件满足，或者因阻塞时间超过了timeout指定的时间，当前进程被唤 醒，select返回。
所以，在这里udevd等待3,4,5,6这几个文件有数据可读，才会被唤醒。现在，到shell中运行：
# ps aux | grep udevd

root     27615  ...... strace -o /tmp/udevd.debug -f /sbin/udevd --daemon

root     27617  ...... /sbin/udevd --daemon
udevd的进程id为27617，现在我们来看看select等待的几个文件：
# cd /proc/27615/fd

# ls -l


udevd的标准输入，标准输出，标准错误全部为/dev/null.

0 -> /dev/null

1 -> /dev/null

2 -> /dev/null


udevd在下面这几个文件上等待。

3 -> /inotify

4 -> socket:[331468]

5 -> socket:[331469]

6 -> pipe:[331470]

7 -> pipe:[331470]
由于不方便在运行中插入一块8139的网卡，因此现在我们以一个U盘来做试验，当你插入一个U盘后，你将会看到strace的输出，从它的输出可以 看到udevd在select返回后，调用了modprobe加载驱动模块，并调用了sys_mknod，在dev目录下建立了相应的节点。
execve("/sbin/modprobe", ["/sbin/modprobe", "-Q", "usb:v05ACp1301d0100dc00dsc00dp00"...]

......

mknod("/dev/sdb", S_IFBLK|0660, makedev(8, 16)) = 0

......
这里modprobe的参数"usb:v05AC..."对应modules.alias中的某个模块。
可以通过udevmonitor来查看内核通过netlink发送给udevd的消息，在shell中运行：
# udevmonitor --env 
然后再插入U盘，就会看到相关的发送给udevd的消息。
== 内核处理过程 ==：
    • 
这里我们以PCI总线为例，来看看在这个过程中，内核是如何处理的。当PCI总线驱动程序扫描到一个新的设备时，会建立一个设备对象，然后 调用pci_bus_add_device()函数，这个函数最终会调用kobject_uevent()通过netlink向用户态的udevd发送消 息。
    • 
 
int pci_bus_add_device(struct pci_dev *dev)

{

        int retval;

        retval = device_add(&dev->dev);


        ......


        return 0;

}
device_add()代码如下：
int device_add(struct device *dev)

{

        struct device *parent = NULL;


        dev = get_device(dev);


        ......


        error = bus_add_device(dev);

        if (error)

                goto BusError;

        kobject_uevent(&dev->kobj, KOBJ_ADD);

        ......

}
device_add()在准备好相关数据结构后，会调用kobject_uevent()，把这个消息发送到用户空间的udevd。
int kobject_uevent(struct kobject *kobj, enum kobject_action action)

{

        return kobject_uevent_env(kobj, action, NULL);

}

int kobject_uevent_env(struct kobject *kobj, enum kobject_action action, char *envp_ext[])

{

        struct kobj_uevent_env *env;

        const char *action_string = kobject_actions[action];

        const char *devpath = NULL;

        const char *subsystem;

        struct kobject *top_kobj;

        struct kset *kset;

        struct kset_uevent_ops *uevent_ops;

        u64 seq;

        int i = 0;

        int retval = 0;


        ......


        /* default keys */

        retval = add_uevent_var(env, "ACTION=%s", action_string);

        if (retval)

                goto exit;

        retval = add_uevent_var(env, "DEVPATH=%s", devpath);

        if (retval)

                goto exit;

        retval = add_uevent_var(env, "SUBSYSTEM=%s", subsystem);

        if (retval)

                goto exit;


        /* keys passed in from the caller */

        if (envp_ext) {

                for (i = 0; envp_ext[i]; i++) {

                        retval = add_uevent_var(env, envp_ext[i]);

                        if (retval)

                                goto exit;

                }

        }


        ......


        /* 通过netlink发送消息，这样用户态的udevd进程就会从select()函数返回，并做相应的处理。 */

#if defined(CONFIG_NET)

        /* send netlink message */

        if (uevent_sock) {

                struct sk_buff *skb;

                size_t len;


                /* allocate message with the maximum possible size */

                len = strlen(action_string) + strlen(devpath) + 2;

                skb = alloc_skb(len + env->buflen, GFP_KERNEL);

                if (skb) {

                        char *scratch;


                        /* add header */

                        scratch = skb_put(skb, len);

                        sprintf(scratch, "%s@%s
", action_string, devpath);


                        /* copy keys to our continuous event payload buffer */

                        for (i = 0; i < env->envp_idx; i++) {

                                len = strlen(env->envp[i]) + 1;

                                scratch = skb_put(skb, len);

                                strcpy(scratch, env->envp[i]);

                        }


                        NETLINK_CB(skb).dst_group = 1;

                        netlink_broadcast(uevent_sock, skb, 0, 1, GFP_KERNEL);

                }

        }

#endif


        ......

        return retval;

}
思考
现在我们知道/dev目录下的设备文件是由udevd负责建立的，但是在内核启动过程中，需要mount一个根目录，通常我们的根目录是在硬盘上， 比如：/dev/sda1， 但是硬盘对应的驱动程序没有加载前，/dev/sda1是不存在的， 如果没有/dev/sda1，就不能通过mount /dev/sda1 /来挂载根目录。另一方面udevd是一个可执行文件，如果连硬盘驱动程序到没有加载，根目录都不存在，udevd就不能运行。如果udevd不能运行， 那么就不会自动加载磁盘驱动程序，也就不能自动创建/dev/sda1。这不是死锁了吗？那么你的Linux是怎么启动的呢？
参考资料： Essential Linux Device Drivers
应该是缺少驱动，将电脑硬件驱动在内核里都选上，如果不知道选哪些驱动，可以弄一个ubuntu（其它发行版也可以）的liveCD启动使用lspci -k查看都下载了哪些驱动，然后去lfs里配置