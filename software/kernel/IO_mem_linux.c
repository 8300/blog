Linux系统对IO端口和IO内存的管理
一、I/O端口
      端口（port）是接口电路中能被CPU直接访问的寄存器的地址。几乎每一种外设都是通过读写设备上的寄存器来进行的。CPU通过这些地址即端口向接口电路中的寄存器发送命令，读取状态和传送数据。外设寄存器也称为“I/O端口”，通常包括：控制寄存器、状态寄存器和数据寄存器三大类，而且一个外设的寄存器通常被连续地编址。

二、IO内存
       例如，在PC上可以插上一块图形卡，有2MB的存储空间，甚至可能还带有ROM,其中装有可执行代码。



三、IO端口和IO内存的区分及联系
         这两者如何区分就涉及到硬件知识，X86体系中，具有两个地址空间：IO空间和内存空间，而RISC指令系统的CPU（如ARM、PowerPC等）通常只实现一个物理地址空间，即内存空间。
内存空间：内存地址寻址范围，32位操作系统内存空间为2的32次幂，即4G。
IO空间：X86特有的一个空间，与内存空间彼此独立的地址空间，32位X86有64K的IO空间。

IO端口：当寄存器或内存位于IO空间时，称为IO端口。一般寄存器也俗称I/O端口,或者说I/O ports,这个I/O端口可以被映射在Memory Space,也可以被映射在I/O Space。

IO内存：当寄存器或内存位于内存空间时，称为IO内存。

 

四、外设IO端口物理地址的编址方式
        CPU对外设IO端口物理地址的编址方式有两种：一种是I/O映射方式（I/O－mapped），另一种是内存映射方式（Memory－mapped）。而具体采用哪一种则取决于CPU的体系结构。

1、统一编址
　　RISC指令系统的CPU（如，PowerPC、m68k、ARM等）通常只实现一个物理地址空间（RAM）。在这种情况下，外设I/O端口的物理地址就被映射到CPU的单一物理地址空间中，而成为内存的一部分。此时，CPU可以象访问一个内存单元那样访问外设I/O端口，而不需要设立专门的外设I/O指令。

       统一编址也称为“I/O内存”方式，外设寄存器位于“内存空间”（很多外设有自己的内存、缓冲区，外设的寄存器和内存统称“I/O空间”）。

2、独立编址
        而另外一些体系结构的CPU（典型地如X86）则为外设专门实现了一个单独地地址空间，称为“I/O地址空间”或者“I/O端口空间”。这是一个与CPU地RAM物理地址空间不同的地址空间，所有外设的I/O端口均在这一空间中进行编址。CPU通过设立专门的I/O指令（如X86的IN和OUT指令）来访问这一空间中的地址单元（也即I/O端口）。与RAM物理地址空间相比，I/O地址空间通常都比较小，如x86 CPU的I/O空间就只有64KB（0－0xffff）。这是“I/O映射方式”的一个主要缺点。

        独立编址也称为“I/O端口”方式，外设寄存器位于“I/O（地址）空间”。

3、优缺点
独立编址主要优点是：
1）、I/O端口地址不占用存储器空间；使用专门的I／O指令对端口进行操作，I/O指令短，执行速度快。
2）、并且由于专门I/O指令与存储器访问指令有明显的区别，使程序中I/O操作和存储器操作层次清晰，程序的可读性强。
3）、同时，由于使用专门的I/O指令访问端口，并且I/O端口地址和存储器地址是分开的，故I/O端口地址和存储器地址可以重叠，而不会相互混淆。 
4）、译码电路比较简单(因为I/0端口的地址空间一般较小，所用地址线也就较少)。
其缺点是：只能用专门的I/0指令，访问端口的方法不如访问存储器的方法多。

统一编址优点：
1）、由于对I/O设备的访问是使用访问存储器的指令，所以指令类型多，功能齐全，这不仅使访问I/O端口可实现输入/输出操作，而且还可对端口内容进行算术逻辑运算，移位等等；
2）、另外，能给端口有较大的编址空间，这对大型控制系统和数据通信系统是很有意义的。
这种方式的缺点是端口占用了存储器的地址空间，使存储器容量减小，另外指令长度比专门I/O指令要长，因而执行速度较慢。
        究竟采用哪一种取决于系统的总体设计。在一个系统中也可以同时使用两种方式，前提是首先要支持I/O独立编址。Intel的x86微处理器都支持I/O 独立编址，因为它们的指令系统中都有I/O指令，并设置了可以区分I/O访问和存储器访问的控制信号引脚。而一些微处理器或单片机，为了减少引脚，从而减 少芯片占用面积，不支持I/O独立编址，只能采用存储器统一编址。

五、Linux下访问IO端口
           对于某一既定的系统，它要么是独立编址、要么是统一编址，具体采用哪一种则取决于CPU的体系结构。 如，PowerPC、m68k等采用统一编址，而X86等则采用独立编址，存在IO空间的概念。目前，大多数嵌入式微控制器如ARM、PowerPC等并不提供I/O空间，仅有内存空间，可直接用地址、指针访问。但对于Linux内核而言，它可能用于不同的CPU，所以它必须都要考虑这两种方式，于是它采用一种新的方法，将基于I/O映射方式的或内存映射方式的I/O端口通称为“I/O区域”（I/O region），不论你采用哪种方式，都要先申请IO区域：request_resource()，结束时释放它：release_resource()。

IO region是一种IO资源，因此它可以用resource结构类型来描述。

         访问IO端口有2种途径：I/O映射方式（I/O－mapped）、内存映射方式（Memory－mapped）。前一种途径不映射到内存空间，直接使用 intb()/outb()之类的函数来读写IO端口；后一种MMIO是先把IO端口映射到IO内存（“内存空间”），再使用访问IO内存的函数来访问 IO端口。

1、I/O映射方式
       直接使用IO端口操作函数：在设备打开或驱动模块被加载时申请IO端口区域，之后使用inb(),outb()等进行端口访问，最后在设备关闭或驱动被卸载时释放IO端口范围。

 in、out、ins和outs汇编语言指令都可以访问I/O端口。内核中包含了以下辅助函数来简化这种访问：

inb( )、inw( )、inl( )
分别从I/O端口读取1、2或4个连续字节。后缀“b”、“w”、“l”分别代表一个字节（8位）、一个字（16位）以及一个长整型（32位）。

inb_p( )、inw_p( )、inl_p( )
分别从I/O端口读取1、2或4个连续字节，然后执行一条“哑元（dummy，即空指令）”指令使CPU暂停。

outb( )、outw( )、outl( )
分别向一个I/O端口写入1、2或4个连续字节。

outb_p( )、outw_p( )、outl_p( )
分别向一个I/O端口写入1、2或4个连续字节，然后执行一条“哑元”指令使CPU暂停。

insb( )、insw( )、insl( )
分别从I/O端口读入以1、2或4个字节为一组的连续字节序列。字节序列的长度由该函数的参数给出。

outsb( )、outsw( )、outsl( )
分别向I/O端口写入以1、2或4个字节为一组的连续字节序列。

流程如下：



      虽然访问I/O端口非常简单，但是检测哪些I/O端口已经分配给I/O设备可能就不这么简单了，对基于ISA总线的系统来说更是如此。通常，I/O设备驱动程序为了探测硬件设备，需要盲目地向某一I/O端口写入数据；但是，如果其他硬件设备已经使用这个端口，那么系统就会崩溃。为了防止这种情况的发生，内核必须使用“资源”来记录分配给每个硬件设备的I/O端口。资源表示某个实体的一部分，这部分被互斥地分配给设备驱动程序。在这里，资源表示I/O端口地址的一个范围。每个资源对应的信息存放在resource数据结构中:

struct resource {
         resource_size_t start;// 资源范围的开始
         resource_size_t end;// 资源范围的结束
         const char *name; //资源拥有者的名字
         unsigned long flags;// 各种标志
         struct resource *parent, *sibling, *child;// 指向资源树中父亲，兄弟和孩子的指针
};

          所有的同种资源都插入到一个树型数据结构（父亲、兄弟和孩子）中；例如，表示I/O端口地址范围的所有资源都包括在一个根节点为ioport_resource的树中。节点的孩子被收集在一个链表中，其第一个元素由child指向。sibling字段指向链表中的下一个节点。

        为什么使用树？例如，考虑一下IDE硬盘接口所使用的I/O端口地址－比如说从0xf000 到 0xf00f。那么，start字段为0xf000 且end 字段为0xf00f的这样一个资源包含在树中，控制器的常规名字存放在name字段中。但是，IDE设备驱动程序需要记住另外的信息，也就是IDE链主盘使用0xf000 到0xf007的子范围，从盘使用0xf008 到0xf00f的子范围。为了做到这点，设备驱动程序把两个子范围对应的孩子插入到从0xf000 到0xf00f的整个范围对应的资源下。一般来说，树中的每个节点肯定相当于父节点对应范围的一个子范围。I/O端口资源树(ioport_resource)的根节点跨越了整个I/O地址空间（从端口0到65535）。

任何设备驱动程序都可以使用下面三个函数，传递给它们的参数为资源树的根节点和要插入的新资源数据结构的地址：

request_resource( )        //把一个给定范围分配给一个I/O设备。

allocate_resource( )        //在资源树中寻找一个给定大小和排列方式的可用范围；若存在，将这个范围分配给一个I/O设备（主要由PCI设备驱动程序使用，可以使用任意的端口号和主板上的内存地址对其进行配置）。

release_resource( )      //释放以前分配给I/O设备的给定范围。

内核也为以上函数定义了一些应用于I/O端口的快捷函数：request_region( )分配I/O端口的给定范围，release_region( )释放以前分配给I/O端口的范围。当前分配给I/O设备的所有I/O地址的树都可以从/proc/ioports文件中获得。

2、内存映射方式
         将IO端口映射为内存进行访问，在设备打开或驱动模块被加载时，申请IO端口区域并使用ioport_map()映射到内存，之后使用IO内存的函数进行端口访问，最后，在设备关闭或驱动模块被卸载时释放IO端口并释放映射。

映射函数的原型为：
void *ioport_map(unsigned long port, unsigned int count);
通过这个函数，可以把port开始的count个连续的I/O端口重映射为一段“内存空间”。然后就可以在其返回的地址上像访问I/O内存一样访问这些I/O端口。但请注意，在进行映射前，还必须通过request_region( )分配I/O端口。

当不再需要这种映射时，需要调用下面的函数来撤消：
void ioport_unmap(void *addr);

在设备的物理地址被映射到虚拟地址之后，尽管可以直接通过指针访问这些地址，但是宜使用Linux内核的如下一组函数来完成访问I/O内存：·读I/O内存
unsigned int ioread8(void *addr);
unsigned int ioread16(void *addr);
unsigned int ioread32(void *addr);
与上述函数对应的较早版本的函数为（这些函数在Linux 2.6中仍然被支持）：
unsigned readb(address);
unsigned readw(address);
unsigned readl(address);
·写I/O内存
void iowrite8(u8 value, void *addr);
void iowrite16(u16 value, void *addr);
void iowrite32(u32 value, void *addr);
与上述函数对应的较早版本的函数为（这些函数在Linux 2.6中仍然被支持）：
void writeb(unsigned value, address);
void writew(unsigned value, address);
void writel(unsigned value, address);

流程如下：



六、Linux下访问IO内存
         IO内存的访问方法是：首先调用request_mem_region()申请资源，接着将寄存器地址通过ioremap()映射到内核空间的虚拟地址，之后就可以Linux设备访问编程接口访问这些寄存器了，访问完成后，使用ioremap()对申请的虚拟地址进行释放，并释放release_mem_region()申请的IO内存资源。

struct resource *requset_mem_region(unsigned long start, unsigned long len,char *name);
   这个函数从内核申请len个内存地址（在3G~4G之间的虚地址），而这里的start为I/O物理地址，name为设备的名称。注意，。如果分配成功，则返回非NULL，否则，返回NULL。
另外，可以通过/proc/iomem查看系统给各种设备的内存范围。

要释放所申请的I/O内存，应当使用release_mem_region（）函数：
void release_mem_region(unsigned long start, unsigned long len)

申请一组I/O内存后， 调用ioremap()函数：
void * ioremap(unsigned long phys_addr, unsigned long size, unsigned long flags);
其中三个参数的含义为：
phys_addr：与requset_mem_region函数中参数start相同的I/O物理地址；
size：要映射的空间的大小；
flags：要映射的IO空间的和权限有关的标志；

功能：将一个I/O地址空间映射到内核的虚拟地址空间上(通过release_mem_region（）申请到的)

流程如下：



七、ioremap和ioport_map
下面具体看一下ioport_map和ioport_umap的源码：

void __iomem *ioport_map(unsigned long port, unsigned int nr)
{
    if (port > PIO_MASK)
        return NULL;
    return (void __iomem *) (unsigned long) (port + PIO_OFFSET);
}

void ioport_unmap(void __iomem *addr)
{
    /* Nothing to do */
}
          ioport_map仅仅是将port加上PIO_OFFSET(64k)，而ioport_unmap则什么都不做。这样portio的64k空间就被映射到虚拟地址的64k~128k之间，而ioremap返回的虚拟地址则肯定在3G之上。ioport_map函数的目的是试图提供与ioremap一致的虚拟地址空间。分析ioport_map()的源代码可发现，所谓的映射到内存空间行为实际上是给开发人员制造的一个“假象”，并没有映射到内核虚拟地址，仅仅是为了让工程师可使用统一的I/O内存访问接口ioread8/iowrite8(......)访问I/O端口。
          最后来看一下ioread8的源码，其实现也就是对虚拟地址进行了判断，以区分IO端口和IO内存，然后分别使用inb/outb和readb/writeb来读写。

unsigned int fastcall ioread8(void __iomem *addr)
{
    IO_COND(addr, return inb(port), return readb(addr));
}

#define VERIFY_PIO(port) BUG_ON((port & ~PIO_MASK) != PIO_OFFSET)
#define IO_COND(addr, is_pio, is_mmio) do { \
    unsigned long port = (unsigned long __force)addr; \
        if (port < PIO_RESERVED) { \
            VERIFY_PIO(port); \
            port &= PIO_MASK; \
            is_pio; \
        } else { \
            is_mmio; \
        } \
} while (0)

展开：
unsigned int fastcall ioread8(void __iomem *addr)
{
    unsigned long port = (unsigned long __force)addr;
    if( port < 0x40000UL ) {
        BUG_ON( (port & ~PIO_MASK) != PIO_OFFSET );
        port &= PIO_MASK;
        return inb(port);
    }else{
        return readb(addr);
    }
}
八、总结
         外设IO寄存器地址独立编址的CPU，这时应该称外设IO寄存器为IO端口，访问IO寄存器可通过ioport_map将其映射到虚拟地址空间，但实际上这是给开发人员制造的一个“假象”，并没有映射到内核虚拟地址，仅仅是为了可以使用和IO内存一样的接口访问IO寄存器；也可以直接使用in/out指令访问IO寄存器。

          例如：Intel x86平台普通使用了名为内存映射（MMIO）的技术，该技术是PCI规范的一部分，IO设备端口被映射到内存空间，映射后，CPU访问IO端口就如同访 问内存一样。

          外设IO寄存器地址统一编址的CPU，这时应该称外设IO寄存器为IO内存，访问IO寄存器可通过ioremap将其映射到虚拟地址空间，然后再使用read/write接口访问。
--------------------- 