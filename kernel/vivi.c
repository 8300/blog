VIVI常用命令 

　　vivi 是韩国Mizi 公司开发的BootLoader，适用于ARM9 处理器。vivi 有两种工作模式，启动加载模式可以在一段时间后（这个时间可更改）自行启动Linux 内核，这是vivi的默认模式。在下载模式下，vivi 为用户提供一个命令行接口，通过该接口可以使用vivi提供的一些命令。启动vivi时,在超级终端界面中键入任意键（除enter外）进入vivi命令界面,字符提示为“vivi>”，我是在启动vivi前按住ESC不放，因为vivi启动比较快，按其它键会有字符产生。 
　　---------------------------------------------------------------------- 
　　reset 命令 复位Arm9系统 
　　---------------------------------------------------------------------- 
　　help 命令 
　　help 显示开发板上vivi支持的所有命令 
　　param help 显示param命令的用法 
　　---------------------------------------------------------------------- 
　　part 命令 
　　part 命令用于对分区进行操作。通过part help 可以显示系统对part 
　　系列命令的帮助提示。 
　　part show 显示分区信息 
　　part add partname part_start_addr part_leng flag 添加分区,参数flag为分区类型 
　　part del partname 删除分区 
　　part save 保存part 分区信息 
　　part reset 恢复为系统默认part分区 
　　---------------------------------------------------------------------- 
　　load 命令 
　　load 命令下载程序到存储器中（Flash 或者RAM 中）。通过load help可以显示系统对load 系列命令的帮助提示。 
　　load flash partname x 使用xmodom 协议通过串口下载文件并且烧写带partname 分区 
　　例： 
　　－load flash vivi x //注意,这里的vivi是分区名 
　　－load flash kernel x 
　　－load flash root x 
　　load ram partname or addr x 使用xmodom 协议通过串口下载文件到内存中 
　　---------------------------------------------------------------------- 
　　param命令 
　　param 命令用于对bootloader 的参数进行操作。通过param help 可以 
　　显示系统对param 系列命令的帮助提示。 
　　param show 命令用于显示bootloader 的当前参数值。 
　　param reset 将bootloader 参数值复位成系统默认值。 
　　param set paramname value 设置参数值 
　　param set linux_cmd_line ”linux bootparam” 设置linux 启动参数，参数linux bootparam 表示要设置的linux kernel 命令行参数。 
　　param save 保存参数设置 
　　---------------------------------------------------------------------- 
　　boot命令 
　　boot命令用于引导linux kernel 启动。通过boot help 可以显示系统对boot命令的帮助提示。 
　　boot 默认方式启动 
　　boot ram ramaddr lenth 启动sdram 中ramaddr处长度为lenth的linux 内核。 
　　---------------------------------------------------------------------- 
　　bon 命令 
　　bon 命令用于对bon分区进行操作。通过bon help 可以显示系统对bon 
　　系列命令的帮助提示。bon分区是nand flash 设备的一种简单的分区管理方式。 
　　bon part info命令用于显示系统中bon分区的信息。 
　　bon part命令用于建立系统的bon分区表。bon分区表被保存到nand flash 
　　的最后0x4000 个字节中，即在nand flash的0x03FFC000 ～0x33FFFFFF 范围内，分区表起始于0x03FFC000。 
　　例如分为3个区：0～192k，192k～1M，1M～ 
　　vivi> bon part 0 192k 1M 
　　doing partition 
　　size = 0 
　　size = 196608 
　　size = 1048576 
　　check bad block 
　　part = 0 end = 196608 
　　---------------------------------------------------------------------- 
　　go 命令 
　　go 命令用于跳转到指定地址处执行该地址处的代码。 
　　go addr 跳转到指定地址运行该处程序。 
　　 
　　以上是整理的一些常用的vivi命令，具体语法可通过相应的help命令查看。

(1) 在主机上打开minicom常按空格键(其实键入任意键 -- 除Enter外，就可以) 
(2) 给目标板(开发板)加电/重启 
即可进入vivi命令界面 


vivi> help 
------------------------------------------------------ 
Usage: 
cpu [{cmds}] -- Manage cpu clocks 
bon [{cmds}] -- Manage the bon file system 
reset -- Reset the system 
param [set|show|save|reset] -- set/get parameter 
part [add|del|show|reset] -- Manage MTD partitions 
mem [{cmds}] -- Manage Memory 
loadyaffs {...} -- Load a yaffs image to Flash 
eboot -- Run Wince Ethernet Bootloader(eboot) 
wince -- Run Wince 
load {...} -- Load a file to RAM/Flash 
go <addr> <a0> <a1> <a2> <a3> -- jump to <addr> 
dump <addr> <length> -- Display (hex dump) a range of memory. 
call <addr> <a0> <a1> <a2> <a3> -- jump_with_return to <addr> 
boot [{cmds}] -- Booting linux kernel 
help [{cmds}] -- Help about help? 


mem系列命令用于对系统的内存进行操作 
------------------------------------------------------ 
vivi> mem 
invalid 'mem' command: wrong argumets 
Usage: 
compare <dst> <src> <length> -- compare 
mem copy <dst> <src> <length> 
mem info 
mem reset -- reset memory control register 
mem serach <start_addr> <end_addr> <value> -- serach memory address that contain value 

vivi>mem info 
RAM Information: 
Default ram size: 64M 
Real ram size : 64M 
Free memory : 63M 

RAM mapped to : 0x30000000 - 0x34000000 (SDRAM映射的地址范围 -- 64M) 
Flash memory mapped to -: 0x10000000 - 0x12000000 (Flash映射的地址范围 -- 32M) 
Available memory region : 0x30000000 - 0x33f80000 (用户可以使用的有效的内存区域地址范围) 
Stack base address : 0x33fafffc (栈的基地址) 
Current stack pointer : 0x33fafc7c (当前栈指针的值) 

Memory control register vlaues (S3C2410的内存控制寄存器的当前值) 
BWSCON = 0x2211d120 
BANKCON0 = 0x00000700 
BANKCON1 = 0x00000700 
BANKCON2 = 0x00000700 
BANKCON3 = 0x00000700 
BANKCON4 = 0x00000700 
BANKCON5 = 0x00000700 
BANKCON6 = 0x00018005 
BANKCON7 = 0x00018005 
REFRESH = 0x008e0459 
BANKSIZE = 0x000000b2 
MRSRB6 = 0x00000030 
MRSRB7 = 0x00000030 



load命令下载程序到存储器中(Flash或者RAM中) 
------------------------------------------------------ 
vivi> load help 
Usage: 
load <flash|ram> [ <partname> | <addr> <size> ] <x|y|z|t> 

<flash|ram> 
关键字参数flash和ram用于选择目标介质是Flash还是RAM。 
如果选择下载到Flash中，其实还是先要下载到RAM中(临时下载到SDRAM的起始地址处0x30000000保存一下，然后再转写入 FLASH)，然后再通过Flash驱动程序提供的写操作，将数据写入到Flash中。

如果选择了flash参数，那么到底是将"数据"写入NOR Flash还是NAND Flash，取决于boot loader编译的过程中，所进行的配置，这就要看配置的时候将MTD设备配置成NOR Flash还是NAND Flash。

[ <partname> | <addr> <size> ] 
partname是vivi的MTD分区表中的分区名,MTD分区的起始地址；addr和size是让用户自己选择下载的目标存储区域，而不是使用 vivi的MTD分区。addr表示下载的目标地址，size表示下载的文件大小，单位字节，size参数不一定非要指定得和待下载的文件大小一样大，但是一定要大于等于待下载的文件的字节数。

<x|y|z|t> 
关键字参数x y和z分别表示从 PC主机上下载文件到ARM9系统中，采用哪种串行文件传送协议，x表示采用xmodem协议，y表示采用ymodem协议，z表示采用zmodem协议 
请注意目前该bootloader -- vivi还没有实现zmodem协议，所以该参数只能选择x和y 
关键字参数t应该是开发板vivi增加的，是tftp下载！很好用的！速度比jtag要快多了！


开发板的vivi eboot烧写都要通过load命令 
如：要烧写eboot.nb0到flash的eboot分区 
vivi> load flash eboot t 
使用交叉网线连好PC与开发板，把eboot.nb0拷贝到于mtftp.exe同一目录下，在windows命令行输入 
mytftp -i 192.168.0.15 PUT eboot.nb0 
等待烧写完成即可


param 命令用于对bootloader的参数进行操作 
------------------------------------------------------ 
vivi> param help 
Usage: 
param help -- Help aout 'param' command 
param reset -- Reset parameter table to default table 
param save -- Save parameter table to flash memeory 
param set <name> <value> -- Reset value of parameter 
param set linux_cmd_line "..." -- set boot parameter 
param set wince_part_name "..." -- set the name of partition wich wincewill be stored in 
param show -- Display parameter table


vivi> param show 
Number of parameters: 19 
name : hex integer 
------------------------------------------------------------- 
//(1)类型，193表示 S3C2410的开发系统 
//(2)媒介类型，即指示了bootloader从哪个媒介启动起来的 
//(3)引导 linux 内核启动的基地址映像将被从 Flash 中拷贝到boot_mem_base + 0x8000 的地址处，内核参数将被建立在boot_mem_base+0x100的地址处 
//(4)bootloader启动时，默认设置的串口波特率

mach_type : 000000c1 193 //(1) 
media_type : 00000003 3 //(2) 
boot_mem_base : 30000000 805306368 //(3) 
baudrate : 0001c200 115200 //(4)

//(5) 以下三个参数和xmodem文件传送协议相关: 
xmodem_one_nak 
表示接收端(即ARM9系统这端)发起第一个NAK信号给发送端(即PC主机这端)到启动； 
xmodem_initial_timeout 
表示接收端(即ARM9系统这端)启动xmodem协议后的初始超时时间，第一次接收超时按照这个参数的值来设置，但是超时一次后，后面的超时时间就不再是这个参数的值了，而是xmodem_timeout的值； 
xmodem_timeout 
表示在接收端(即ARM9系统这端)等待接受发送端(即PC主机这端)送来的数据字节过程中，如果发生了一次超时，那么后面的超时时间就设置成参数 xmodem_timeout的值了这三个参数不需要修改，系统默认的值就可以了，不建议用户去修改这几个参数值

xmodem_one_nak : 00000000 0 
xmodem_initial_timeout : 000493e0 300000 
xmodem_timeout : 000f4240 1000000


//(6) ymodem_initial_timeout 
表示接收端(即 ARM9 系统这端)在启动了ymodem协议后的初始超时时间，这个参数不需要修改，系统默认的值就可以了，不建议用户去修改这几个参数值 
//(7) boot_delay是bootloader自动引导linux kernel功能的延时时间

ymodem_initial_timeout : 0016e360 1500000 //(6) 
boot_delay : 00300000 3145728 //(7) 
os : WINCE 
display : VGA 640X480 
ip : 192.168.0.15 
host : 192.168.0.1 
gw : 192.168.0.1 
mask : 255.255.255.0 
wincesource : 00000001 1 
wincedeploy : 00000000 0 
mac : 00:00:c0:ff:ee:08 
wince part name : wince 
Linux command line : noinitrd root=/dev/mtdblock/3 init=/linuxrc console=ttyS0

//( Linux command line不是bootloader的参数，而是kernel启动的时候，kernel不能自动检测到的必要的参数些参数需要bootloader传递给 linux kernel, Linux command line就是设置linux kernel启动时，需要手工 
传给 kernel的参数


param reset 将bootloader 参数值复位成系统默认值。 
param set paramname value 设置参数值 
param save 保存参数设置 
param set linux_cmd_line ”linux bootparam” 
设置linux 启动参数，参数linux bootparam表示要设置的linux kernel命令行参数。

修改boot_delay(延长等待时间) 
vivi> param set boot_delay 0x05000000 
vivi> param save


part命令用于对MTD分区进行操作 
------------------------------------------------------------------------------- 
vivi> part show 
mtdpart info. (5 partitions) 
name offset size flag 
------------------------------------------------ 
vivi : 0x00000000 0x00020000 0 128k 
eboot : 0x00020000 0x00020000 0 128k 
param : 0x00040000 0x00010000 0 64k 
kernel : 0x00050000 0x00100000 0 1M 
root : 0x00150000 0x03eac000 0 62M+688k

MTD分区是针对Flash(NOR Flash或者NAND Flash)的分区，以便于对bootloader对Flash进行管理

part add 命令用于添加一个MTD分区 
part add name offset size flag 
参数 name是要添加的分区的分区名 
参数 offset是要添加的分区的偏移(相对于整个MTD设备的起始地址的偏移，在 ARM9系统中不论配置的是NOR Flash，还是NAND Flash，都只注册了一个mtd_info结构，也就是说逻辑上只有一个MTD设备，这个MTD设备的起始地址为0x00000000)； 
参数 size是要添加的分区的大小，单位为字节； 
参数 flag是要添加的分区的标志，参数flag的取值只能为以下字符串(请注意必须为大写)或者通过连接符| 
这个标志表示了这个分区的用途 
“BONFS” 作为BONFS文件系统的分区； 
“JFFS2” 作为JFFS2文件系统的分区； 
“LOCK” 该分区被锁定了； 
“RAM” 该分区作为RAM使用

例如，添加新的 MTD分区mypart 
vivi> part add mypart 0x500000 0x100000 JFFS2 
mypart: offset = 0x00500000,

size = 0x00100000, flag = 8

part del 命令用于删除一个 MTD分区 
part del name 
参数name是要删除的MTD分区的分区名

part save 保存part分区信息 
part reset 恢复为系统默认part分区


boot命令用于引导linux kernel启动 
------------------------------------------------------ 
vivi>boot help 
Usage: 
boot <media_type> -- booting kernel 
value of media_type (location of kernel image) 
1 = RAM 
2 = NOR Flash Memory 
3 = SMC (On S3C2410) 
boot <media_type> <mtd_part> -- boot from specific mtd partition 
boot <media_type> <addr> <size> 
boot help -- help about 'boot' command

<media_type> 
boot关键字后面media_type必须指定媒介类型，因为boot命令对不同媒介的处理方式是不同的，例如如果kernel在 SDRAM中，那么boot执行的过程中就可以跳过拷贝kernel映像到SDRAM中这一步骤了 
Boot命令识别的媒介类型有以下三种： 
ram 表示从RAM(在ARM9系统中即为SDRAM)中启动linux kernel，linux kernel必须要放在RAM中 
nor 表示从NOR Flash中启动linux kernel，linux kernel必须已经被烧写到了NOR Flash中 
smc 表示从NAND Flash中启动linux kernel，linux kernel必须已经被烧写到了NAND Flash中

<mtd_part> 
参数mtd_part是MTD分区的名，MTD设备的一个分区中启动linux kernel，kernel映像必须被放到这个分区中；

<addr> <size> 
分别表示linux kernel起始地址和kernel的大小。为什么要指定kernel大小呢?因为kernel首先要被copy到boot_mem_base + 0x8000的地方，然后在boot_mem_base + 0x100开始的地方设置内核启动参数，要拷贝 kernel，当然需要知道kernel的大小啦，这个大小不一定非要和kernel实际大小一样，但是必须许大于等于kernel的大小，单位字节


bon命令 
------------------------------------------------------ 
bon命令用于对bon分区进行操作。通过bon help可以显示系统对bon系列命令的帮助提示。bon分区是nand flash设备的一种简单的分区管理方式。bootloader支持bon分区，同时Samsung提供的针对S3C2410移植的linux版本中也支持了bon分区，这样就可以利用bon分区来加载linux的根文件系统. 
MTD分区和BON分区，当ARM9系统配置了NAND Flash作为MTD设备，那么MTD分区和BON分区都在同一片NAND Flash上

vivi> bon part info 命令用于显示系统中bon分区的信息

BON info. (3 partitions) 
No: offset size flags bad 
--------------------------------------------- 
0: 0x00000000 0x00030000 00000000 0 192k 
1: 0x00030000 0x00100000 00000000 0 1M 
2: 0x00130000 0x03ec8000 00000000 1 62M+800k 
bon分区表被保存到nand flash的最后0x4000个字节中，即在nand flash的0x03FFC000~0x33FFFFFF范围内，分区表起始于0x03FFC000。

vivi> bon part 命令用于建立系统的bon分区表 
vivi> bon part 0 192k 1M 
doing partition 
size = 0 
size = 196608 
size = 1048576 
check bad block 
part = 0 end = 196608


go命令 
------------------------------------------------------ 
go命令用于跳转到指定地址处执行该地址处的代码。 
go addr跳转到指定地址运行该处程序。


reset命令 
------------------------------------------------------ 
复位Arm9系统
