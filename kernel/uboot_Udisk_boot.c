分类： U-BOOT usb2011-01-04 22:41 1574人阅读 评论(15) 收藏 举报
AT91SAM9260

U-BOOT

OHCI

 

对于 U 盘启动 kernel, 先通过了解整个框架，在细说 USB 枚举（包括 HUB ） ,OHCI 等内容

 

一．            总体流程

在 U-BOOT 中， USB HOST 是可以不支持的，如果需要 U 盘启动内核时，会使用此功能。

在 U_BOOT_CMD 中有 usb host 的命令结构，具体调用的是 do_usb ，若使用 USB storage （ U 盘），则同时会有 do_usbboot

使用 U 盘启动内核的过程一般是，（举例）

在 do_usb 函数中添加如下代码，

if (strcmp(argv[1], "boot") == 0) {

                            char *s;

              s=getenv("boot");

              run_command(s,0);

              return 0;

       }

插上 U 盘之后，进入命令行模式，先写命令 usb start 启动 HUB 和 U 盘设备，然后配置环境变量，如 boot=usbboot ，接着写命令 usb boot ，如上代码所示，取得环境变量之后，就会调用 do_usbboot 完成 U 盘启动 kernel image

首先分析 do_usb ， do_usbboot 在 USB storage 部分分析

do_usb--------usb_init,usb_init 主要调用 usb_lowlevel_init 和 usb_scan_devices



识别设备之后，如果配置了 USB storage ，则会调用 usb_stor_scan 配置 U 盘，接着输入命令 usb boot 就会调用 do_usbboot

 

二．            Usb_low_init

usb_low_init 主要流程如下，

a)      配置外部时钟，使能时钟，寄存器为 PCER 和 SCER

b)      配置 host controller 相关变量，包括

gohci,ghcca[0],phcca,ohci_dev,gtd,ptd,urb_priv 等

c)      hc_reset

写命令到 host controller 的 control 寄存器， reset ，等待 reset 结束 ( 不懂为啥用两次 reset)

d)     hc_start

 

三．            Usb device 的识别

1 ． Usb_scan_devices

usb_scan_devices 主要调用了两个函数， usb_alloc_new_device 和 usb_new_device

usb_alloc_new_device 初始化 usb_device *dev, 并将初始化成功的 dev 存放在数组 usb_dev 中，便于以后的查找

usb_new_device 先设置 Dev 的地址为 0 ，然后通过枚举过程得到新的 USB 设备的地址，设备地址一般从 1 开始分配， roothub 的地址就为 1 ， u 盘的地址为 2

USB 设备枚举过程，

get device descriptor

set address

get device descriptor

get configuration descriptor

set configuration

get string

由于 root hub 是直接与 host controller 相连的，所以枚举过程一定存在 ( 至少有一个 USB device)

最后调用 usb_hub_probe 识别连接上的设备是否为 hub

 

2. 枚举过程

具体的， scan devices 枚举包括两个部分，一个是 ROOT HUB 的枚举过程，一个 u 盘的枚举过程

a) ROOT HUB 的枚举过程： 上述描述的过程识别处 ROOT HUB 这一 USB 设备后，还未确定是否是 HUB 设备，则调用 usb_hub_probe 识别，识别出是HUB 设备后，调用 usb_hub_configure 配置 ROOT HUB ，由于是 HUB ，会重新进行 HUB 的枚举过程

如同前面所说的数组 usb_dev 一样，对于所有的 HUB 设备也有一个数组 hub_dev , 在 usb_hub_configure 中，先分配一个 usb_hub_device *hub, 然后对hub 进行枚举，过程包括，（更详细的枚举见 uboot 之 USB 枚举）

usb_get_hub_descriptor 得到 hub descriptor length

usb_get_hub_descriptor 用新得到的 length 获取对应长的描述符

usb_get_hub_status     获取 port 数等

usb_set_port_feature 根据 Port 数调用相应次数，对每个 Port 进行 set feature

usb_get_port_status          根据 Port 数调用相应次数，获取 Port 是否有设备插入的信息等

usb_hub_port_connect_change 对于 Port 端口有设备插入的，调用此函数，对新设备进行检测枚举，此函数中又包括如下函数操作，

       aa)usb_clear_port_feature

       ab)disconnect any existing devices under this port    wai_ms(200)

       ac)hub_port_reset 这里需要 reset 的原因在于， usb 设备速度的识别，对于以前的 1.0 系列 USB ，只有全速和低速，所以无需 reset 也能识别（通过两根数据线的上拉电阻识别），对于高速和全速，插上之后先识别为全速设备， hub_port_reset 之后再通过电路时序识别是否是高速设备

       ad) 建立设备的树结构， dev->children[port] 和 usb->parent 实现

       ae) usb_new_device 这里继续调用此函数识别 USB 设备，如果连接到 root hub 上的设备仍然为 HUB 设备，则会一直延续着调用此函数，直至usb_hub_probe 识别出连接到 HUB Port 上的设备不为 HUB ，对于 U 盘启动，这里 usb_new_device 在调用一次之后就不再调用， usb_hub_probe 识别出 U盘设备不为 HUB 设备

 

b)U 盘的枚举过程（假设已经连接上）： 这里由于在 usb_hub_port_connect_change 中检测到 Port 上有 USB device ，所以会调用 usb_new_device 进行枚举，从而 USB device 得到识别，接着在 调用 usb_stor_scan 配置 U 盘

（具体见 uboot 之 usb 枚举）

 

四．            USB storage

u-boot 中如果配置 usb storage ，则会调用函数 usb_stor_scan

主要流程如下：

     （ 1 ） 初始化结构 usb_dev_desc[], 该结构为 block_dev_desc ，包括 target SCSI ID,type of the interface(usb),device number,part_type( 未知 ) ，以及初始化 block_read 方法

       （ 2 ）调用函数 usb_get_dev_index 找到 usb_dev[] 数组下标得到 u 盘设备的 usb_device * 结构指针

       （ 3 ）调用 usb_storage_probe ，取得 us_data usb_stor[]

该函数包括三个重要数据结构 usb_device ， usb_interface_descriptor ， us_data ，主要目的就是根据 usb_device 和 usb_interface_descriptor 来填充usb_data 结构，如下图所示，



 

     这里一些赋值操作主要还是枚举中获得的接口信息，所以熟悉枚举过程是很必要的。

       （ 4 ）最后调用 usb_stor_get_info 取得 block_dev_desc_t usb_dev_desc[]

       该函数主要是 U 盘的一些操作，如 INQUIRY,READ CAPACITY ，要来填充结构 static block_dev_desc_t usb_dev_desc[USB_MAX_STOR_DEV] ，便于后面引导 kernel 中所需的信息。（这里许多 UFI 命令可以参考 USB 枚举过程） , 这里涉及的 CCB 结构就不讲了，枚举中会细说

       流程大致如下，

a) 对于一些特定的 USB 设备，不需要 transport_reset, 否则调用 transport_reset 方法。

b) 发送 USB_INQUIRY 命令，获取 U 盘基本信息 ( 这里涉及的可以看 U-BOOT 之 USB 枚举 ) （调用 transport 方法）

c) 调用 usb_test_unit_ready 查看设备是否准备就绪（调用 transport 方法）

d) 调用 usb_read_capacity 读取 U 盘容量（返回信息包括块数目，块大小，设备类型，例如 DOS 盘等）（调用 transport 方法）

e) 调用 init_part 初始化分区，对于 DOS 盘，则调用 test_part_dos 读取分区头信息，检测是否正确（调用 block_read 方法） , 对于 DOS 盘，读取出的第一块的信息，要求 0x1fe 偏移处值为 0x55,0x1ff 为 0xaa

下面主要讲讲涉及到的三个方法， transport_reset ， transport ， block_read

    i)                    tranport_reset

    transport_reset 指向的函数为 usb_stor_BBB_reset ， reset 的步骤需要严格支持以下步骤，

  /*

    * Reset recovery (5.3.4 in Universal Serial Bus Mass Storage Class)

    *

    * For Reset Recovery the host shall issue in the following order:

    * a) a Bulk-Only Mass Storage Reset

    * b) a Clear Feature HALT to the Bulk-In endpoint

    * c) a Clear Feature HALT to the Bulk-Out endpoint

    *

    * This is done in 3 steps.

    *

    * If the reset doesn't succeed, the device should be port reset.

    *

    * This comment stolen from FreeBSD's /sys/dev/usb/umass.c.

    */

所以调用的分别是 usb_control_msg ，

usb_clear_halt, usb_clear_hatl

 

    ii)                  transport

transport 指向的函数 usb_stor_bbb_transport ，该函数主要调用函数 usb_stor_BBB_comdat 填充 CBW ，接着调用 usb_bulk_msg 发送 CBW,调用 usb_bulk_msg 返回数据，调用 usb_bulk_msg 接收 CSW ，进行结构 CSW 解析，查看是否发送成功，如果有误，则 reset ，即usb_stor_BBB_reset

    iii)                block_read

该部分实际调用的是 UFI 命令，实际上仍然是 transport 方法，读取指定块数据，略

 

五．            USB 引导 kernel

在最后一步，做 U 盘引导 kernel 时，最好格式化 U 盘，然后将 kernel.img 存入 U 盘进行引导。

这部分的原理与 UBOOT 最后检查 img 的合法性，然后调用 do_bootm 方法实现引导 kernel 原理一致（只不过多了一步分析 U 盘的文件系统的步骤）

当然这里所说的启动 kernel 的方式还使用命令行，实际上稍微修改下代码是可以实现自动检测 kernel ，如果存在且合法则启动，否则使用默认 kernel

该部分调用函数 do_usbboot, 首先通过环境变量 bootdevice 得到引导设备的下标，从而找到引导设备。

这里可以设置 bootdevice=0, 则为获取得的 block_dev_desc 的 * stor_dev 指针 .

具体的流程为，（这里主要调用的方法是 block_read ）

a)      get_partition_info 获取分区信息，分析分区信息

b)      读取 img 的 img_head ，分析合法性，如果合法，则调用 do_bootm 引导 kernel （这里设置 autostart=yes 这一环境变量则合法直接引导）

该函数的重点在于分析 U 盘的文件系统。事实上， uboot 中的对于 u 盘启动 kernel 的方法是不完善的（支持引导 DOS 盘），特别是对与普通的 U盘。（查看源代码可以发现这一不足，本人用 U 盘引导 kernel 时也出现一些问题，主要是文件系统方面的）

下面对 FAT32 文件系统简单分析（ FAT16 类似，如果需要完善 U 盘引导 kernel 这部分，需要对大多数的 U 盘支持，所以最好对大多数的文件系统熟悉，并且对需要用到的信息封装结构体，这样在便于实现的同时也实现对不同文件系统的支持）

这里只分析与引导 kernel 部分有关的，具体的可以参照《 4.5 万字透视 FAT32 文件系统》

由于 U 盘是经过格式化的，且一般 img 的大小不会超过第一分区的大小，所以通过计算 img 起始位置，然后读取 img 对应的块数数据即可

下图是 FAT 文件系统的部分结构图



 

一般而言只要分析出 DBR 中相关数据，即可计算起始位置

下图是 U 盘的 DBR 截图（用的是 WINHEX ）



可以看到该 U 盘使用的是 FAT32 文件系统

参照 FAT32 的 BPB 说明，根据下图信息即可找到对应的起始地址，具体的就分析略。

