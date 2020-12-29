//---------------------driver.c------------------------
0.//--------------------- command 
lsmod 	: read and analysis /proc/modules's file
same to /sys/module
modprobe: stronger than insmod, it can load other modules that it depend on. the file /lib/modules/<kernel-version>/modules.dep include the symbiosis 
module be maked when you compile the whole kernel
modinfo	: get module information
unload	: rmmod
licence : "GPL" "GPL v2" "GPL and additional rights" " Dual BSD/GPL" "Dual MPL/GPL" "Proprietary"
0.//---------------------message
1.printk
//---------------------function
MKDEV是将主设备号和次设备号转换成dev_t类型的一个内核函数。
copy_from_user
request_mem_region要使用I/O内存首先要申请,然后才能映射,使用I/O端口首先要申请,或者叫请求
//---------------------sample
#Makefile文件
CROSS_COMPILE:= arm-hisiv600-linux-
ARCH:= arm 
CC    := $(CROSS_COMPILE)gcc
LD    := $(CROSS_COMPILE)ld 
obj-m := hello.o 

KERNELDIR = /media/pluto/sdb/datasheet/tmp/linux-3.18.y 

PWD := $(shell pwd) 

modules: 
      $(MAKE) -C $(KERNELDIR) M=$(PWD) modules 

modules_install: 
      $(MAKE) -C $(KERNELDIR) M=$(PWD) modules_install 

clean:
  rm -f *.o
  rm -f *.symvers
  rm -f *.order
  rm -f *.ko
  rm -f *.mod.c
//---------------------dmesg
sudo dmesg -c//清除之前的记录信息,
	//---------重新给板子上电
	dmesg//可以得知，开发板 idVendor=5345, idProduct=1234 ， 记住这两个数
	sudo vi secbulk.c
	/*---------secbulk.c-----修改参数值--------//
	 *	static struct usb_device_id secbulk_table[]= {  
    	 *	{ USB_DEVICE(0x5345, 0x1234)},  
    	 *	{ }  
	 *	}; 
	//---------secbulk.c----------------------*/
	make -C /lib/modules/`uname -r`/build M=`pwd` modules
	sudo insmod secbulk.ko
	lsmod | grep secbulk //来查看是否加载成功lsmod | grep secbulk
//---------------------Linux驱动编程的本质属于Linux 内核编程
2.驱动设计的硬件基础
2.1处理器
2.1.1通用处理器
中央处理器体系结构
按储存结构划分如下：
冯.诺依曼结构	：ARM7、MIPS//又称普林斯顿结构。
哈佛结构	：AVR、ARM9、ARM10、ARM11、Cortex A
按指令集划分如下：
RISC精简指令集	：ARM、MIPS、PowerPC
CISC复杂指令集	：
2.1.2数字信号处理器
DSP：定点、浮点
DSC=MCU+DSP
2.2存储器
非易失性NVM：ROM、Flash、光磁介质
掉电丢失RAM：SRAM、DRAM`
2.3接口
以太网接口：从 CPU 到 最 终 接 口 依 次 为 CPU、MAC、PHY、以太网隔离变压器、RJ45 插座。
3.Linux内核及内核编程
3.3.2Linux内核的组成部分：SCHED（进程调度）、MM（内存管理）、VFS（虚拟文件系统）、NET（网络接口）、IPC（进程间通信）
3.5Linux下的C编程特点
3.5.1linux编码风格
1）对于结构体、if/for/while/switch的语句，”{“不另起一行
2）如果if、for循环只有一行，不加“{}”
3）if和else混用的情况下，else不另起一行
	if（）{
		。。。。
	} else if（）{
	}else{
		。。。
	}
4）对于函数，“{”另起一行
switch case对齐
“ for(ٮiٮ=ٮ0; ٮiٮ<ٮ10; ٮi++){ٮ ”语句中的“ٮ"都是空格
4linux内核模块
4.2Linux内核模块程序结构
模块加载函数：__init标识声明，module_init的形式被指定
模块卸载函数：__exit标识声明，module_exit的形式来指定
模块许可证声明：
以下为可选：
模块参数：module_param(参数名，参数类型，参数读写权限)
	tail -n 2 /var/log/messages//查看内核输出
模块导出符号：EXPORT_SYMBOL(符号名);EXPORT_SYMBOL_GPL(符号名);
模块作者等信息声明：MODULE_DEVICE_TABLE声明该驱动模块所支持的设备
5Linux文件系统与设备文件
5.1.2C库文件操作
1.创建和打开fopen（path，mode）
2.读写fgetc；fputc；fgets；fputs；fprintf；fscanf；fread；fwrite；
3.关闭fclose；
5.2Linux文件系统
5.4udev用户空间设备管理
5.4.2sysfs文件系统与Linux设备模型
device_driver(驱动)和device（设备）都必须依附于一种总线
bus_type的match（）对设备和驱动进行配对
udev可以利用内核通过netlink发出的uevent信息动态创建设备文件节点
6字符设备驱动
//---------------------error
//Device or resource busy
i等会儿就好了/chmod 777 *
//selected processor does not support ARM mode `dsb'
不要使用readb等函数，自己定义即可！重新实现这些函数！

static inline void my_writeb(u8 b, volatile void __iomem *addr)
{
	*(volatile u8 __force *) addr = b;
}
 
static inline u8 my_readb(const volatile void __iomem *addr)
{
	return *(const volatile u8 __force *) addr;
}
//---------------------end-----------------------------
