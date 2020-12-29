//---------------------make.c--------------------------
//---------------------makefile
//当make的目标为all时
-C $(KDIR) 指明跳转到源码目录下读取那里的Makefile；
M=$(PWD) 表明然后返回到当前目录继续读入、执行当前的Makefile。
//parameter
在makefile中可以预先使用一个未定义的变量， 这个变量可以在make执行时传递给它
比如makefile中有这么一行
include $(M)/$(COMPAT_CONFIG)
这个M可以通过make传递过来
make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
其中的M就是$(PWD)
甚至可以进行更复杂的操作， 就像在Makefile中一样
make CFLAG=-DDEBUG
//---------------------error
//error trying to exec 'cc1': execvp: No such file or directory
You cannot change the relative position of the cross-compile tool chain bin file. Cc1 is called
//---------------------make

//The origin Function
$(origin variable)//it tells you where it came from
//
//regular:
	TARGET...:PREEREQUISITES
		COMMAND
		...
		…
//make	会将【tab】开头的行交给shell程序去解释执行	
		将第一个目标作为终极目标
		默认规则：使用默认命令：cc -c 编译目标的.c文件
		单目标多依赖：
			kbd.o command.o files.o : command.h 
			display.o insert.o search.o files.o : buffer.h
		注释：#
		多行链接：\
//make查找文件顺序：GNUmakefile(只有GNU make可以识别)、makefile、Makefile
	  指定读取文件：“-f NAME”/”--file=NAME”
//@	close the echo
//u-boot-2019.04
  $(Q)$(MAKE) -C $(KBUILD_OUTPUT) KBUILD_SRC=$(CURDIR) \
   -f $(CURDIR)/Makefile $(filter-out _all sub-make,$(MAKECMDGOALS))
@make -C 
//---------------------kernel/uboot
makefile详解（kernel编译及uboot编译）
1.编译管理
1.1原地编译
污染源文件；一套源代码只能按照一种配置和编译的方式进行处理，无法同时维护2个或者2个以上的配置编译。
1.2异地编译（单独输出文件夹）
$ make O=输出目录
$ export BUILD_DIR=输出目录，然后再make
如果两个都指定了，那么O=xx具有更高的优先级
2.指定编译工具
ARCH=arm	//ARM架构
CROSS_COMPILE=arm-linux-	//编译工具前缀
6、UBOOT根目录下面的config.mk文件的作用
这一行非常的重要，在这里添加了一个Makefile
这200多行是我们导入进来的这个文件是为了确定交叉编译工具链的和自动推导原则的。

前面的是定义交叉编译工具链：
(1)$(TOPDIR)/config.mk(主Makefile的185行)
(2)编译工具定义config.mk

config.mk的详解：

定义了一大堆的交叉编译工具链

AS    = $(CROSS_COMPILE)as
LD    = $(CROSS_COMPILE)ld
CC    = $(CROSS_COMPILE)gcc
CPP    = $(CC) -E
AR    = $(CROSS_COMPILE)ar
NM    = $(CROSS_COMPILE)nm
LDR    = $(CROSS_COMPILE)ldr
STRIP    = $(CROSS_COMPILE)strip
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump
RANLIB    = $(CROSS_COMPILE)RANLIB
定义了一大堆的自动推导原则：

如果没有定义远程编译的话。那么就使用下面的自动的推导的原则
1、如果有.S的文件的话，让其编译成.s的文件
2、如果有.S的文件的话，让其编译成.o的文件
3、如果有.c的文件的话，让其编译成.o的文件

ifndef REMOTE_BUILD
 
%.s:    %.S
    $(CPP) $(AFLAGS) -o $@ $<
%.o:    %.S
    $(CC) $(AFLAGS) -c -o $@ $<
%.o:    %.c
    $(CC) $(CFLAGS) -c -o $@ $<
 
else
 
$(obj)%.s:    %.S
    $(CPP) $(AFLAGS) -o $@ $<
$(obj)%.o:    %.S
    $(CC) $(AFLAGS) -c -o $@ $<
$(obj)%.o:    %.c
    $(CC) $(CFLAGS) -c -o $@ $<
 
endif


7、UBOOT根目录下面的autoconfig.mk文件的作用

包含开发板的工具：

autoconfig.mk文件不是源码提供的，是配置过程中自动生成的。
必然有一个原始的原材料：

这个文件的作用就是用来指导整个UBOOT的编译过程的，这个文件的内容其实就是有很多CONFIG_开头的很多的宏（可以理解为变量）。

这些宏或者变量会影响我们UBOOT的编译过程的走向，原理就是条件编译。

在UBOOT的代码中有很多地方使用条件编译就是进行编译的，这个条件编译就是用来使用可移植性的。

使用条件变量匹配不同的开发板。可以说UBOOT的源代码在很大程度是拼凑

这个文件的原材料就是源代码目录的include/configs/xxx.h文件。这个.h头文件里面全部都是宏定义。这些宏定义就是我们对当前开发板的移植。

这个.h头文件里面全部都是宏定义，这些宏定义就是我们对当前开发板的移植。每个开发板的移植都对应这个目录的一个头文件。每一个宏定义都很重要。

这些配置的宏定义就是我们移植UBOOT的关键所在。

CONFIG_开头的东西

arm_config.mk ：预处理的属性。

如果没有定义LDSCRIPT 就是一个CONFIG_NAND_UBOOT

这个变量就是我们所说的链接脚本。
如果我们定义了CONFIG_NAND_U_BOOT宏，则链接脚本叫做u-boot-nand.lds
如果没有定义这个宏则链接脚本叫做u-boot.lds
(2)我们从字面意思分析，即可知道

-TEXT_BASE：在shell里面一个>表示创建一个文件

(1)Makefile中在配置我们的x210开发板时，在board/samsung/x210目录下面生成了一个config.mk，其中的内容就是
TEXT_BASE = 0xc3e00000 相当于定义了一个变量。

(2)TEXT_BASE是将来我们链接时的地址，
-Ttext 0x0 ：因为我们的UBOOT中启用了虚拟地址映射：
因此我们这个C3E00000地址就等于我们0x23E00000也可能是33E00000
这个具体的地址要取决于UBOOT中做的虚拟地址映射关系。

(3)回顾一下裸机中讲的链接地址的问题，再想一下UBOOT的dnw方式，先下载x210_usb.bin,然后再下载uboot.bin时为什么第二个地址是23E00000

-Ttext 相当于我们的链接地址一样的。

$@:指的是我们的目标
$<:指的是我们的依赖
//--------------------- examples
//1
ifeq ($(CONFIG_IEC_COMMON),y)
filename		:= common
current_dir		:= $(current_dir)$(filename)/

sub_inc_path	+= $(current_dir)inc/

obj-$(CONFIG_LINKED_LIST_IEC)			+= linked_list.o

current_dir		:=$(patsubst %/,%,$(dir $(patsubst %/,%,$(current_dir))))/

endif #HAL
//2 Kbuild

menuconfig IEC_COMMON
	tristate "iec_common"
if IEC_COMMON
config LINKED_LIST_IEC
	tristate "linked_list"
endif #IEC_COMMON
//3 make Kbuild
#auto make Kconfig depend on *.c
srcs = $(basename $(wildcard *.c))
show: 
	$(foreach src,$(srcs),$(call write_Kconfig, $(src)))
define write_Kconfig
	echo "config $(shell echo $(1) | tr [a-z] [A-Z])" >> Kconfig
	echo "	tristate "$(1)"" >> Kconfig

endef
//3
defconfig:
	./scripts/conf Kconfig
	./scripts/conf -s --syncconfig Kconfig
menuconfig:
	./scripts/mconf Kconfig
	./scripts/conf -s --syncconfig Kconfig
//4
#ts
ifeq ($(CONFIG_TS),y)
sub	:= ts
sub_inc_path 	:=
sub_src_path 	:=
include $(sub)/Makefile
inc_path		+= $(sub_inc_path)
src_path		+= $(sub_src_path)
current_dir		:=
endif #CONFIG_TS

#net
ifeq ($(CONFIG_EXAMPLES),y)
sub	:= examples
sub_inc_path 	:=
sub_src_path 	:=
include $(sub)/Makefile
inc_path		+= $(sub_inc_path)
src_path		+= $(sub_src_path)
current_dir		:=
endif #CONFIG_NET
$(obj)/hello: $(targets)
	$(CC) $^ $(ld_flags) -o $@
$(obj)/%.o: %.s config
	$(CC) -c $(c_flags) -o $@ $<
$(obj)/%.s: %.i config
	$(CC) -S $(c_flags) -o $@ $<
$(obj)/%.i: %.c config
	$(CC) -E $(c_flags) -o $@ $<
$(obj)/%.a: $(obj)/%.o config FORCE
	$(LD) $(KBUILD_LDFLAGS) -r -o $(@D)/.tmp_$(@F) $@
$(obj)/%.o: $(src)/%.S $(objtool_dep) config FORCE
	$(CC) $(c_flags) -c -o $@ $<
#----------------------end-----------------------------
//---------------------end------------------------------
