//---------------------bootloader与linux中位置无关代码的分析理解.c
首先，需要理解加载域与运行域的概念。加载域是代码存放的地址，运行域是代码运行时的地址。为什么会产生这2个概念？这2个概念的实质意义又是什么呢？

在一些场合，一些代码并不在储存这部分代码的地址上执行地址，比如说，放在norflash中的代码可能最终是放在RAM中运行，那么中norflash中的地址就是加载域，而在RAM中的地址就是运行域。

在汇编代码中我们常常会看到一些跳转指令，比如说b、bl等，这些指令后面是一个相对地址而不是绝对地址，比如说b main，这个指令应该怎么理解呢？main这里究竟是一个什么东西呢？这时候就需要涉及到链接地址的概念了，链接地址实际上就是链接器对代码中的变量名、函数名等东西进行一个地址的编排，赋予这些抽象的东西一个地址，然后在程序中访问这些变量名、函数名就是在访问一些地址。一般所说的链接地址都是指链接这些代码的起始地址，代码必须放在这个地址开始的地方才可以正常运行，否则的话当代码去访问、执行某个变量名、函数名对应地址上的代码时就会找不到，接着程序无疑就是跑飞。但是上面说的那个b main的情形有点特殊，b、bl等跳转指令并不是一个绝对跳转指令，而是一个相对跳转指令，什么意思呢？就是说，这个main标签最后得到的只并不是main被链接器编排后的绝对地址，而是main的绝对地址减去当前的这个指令的绝对地址所得到的值，也就是说b、bl访问到的是一个相对地址，不是绝对地址，因此，包括这个语句和main在内的代码段无论是否放在它的运行域这段代码都能正常运行。这就是所谓的位置无关代码。

由上面的论述可以得知，如果你的这段代码需要实现位置无关，那么你就不能使用绝对寻址指令，否则的话就是位置有关了。

 

接着，将结合uboot、vivi、linux中的PIC（position independent code）代码进行分析。

另外需要指出的是本文的分析基于mini2440的板子及配套代码，对于其他板子或者源代码代码或者会有差别。

 

Uboot部分：

 

在/u-boot-1.1.2/cpu/arm920t/start.S截取部分相关代码如下：

1 .globl _start

2 _start:   b       reset

3   ldr pc, _undefined_instruction

4   ldr pc, _software_interrupt

5   ldr pc, _irq

6   ldr pc, _fiq

7

8 reset:

9   mrs r0,cpsr

10  bic r0,r0,#0x1f

11  orr r0,r0,#0xd3

12  bl  cpu_init_crit

12 relocate:                /* relocate U-Boot to RAM       */

13  adr r0, _start      /* r0 <- current position of code   */

14  ldr r1, _TEXT_BASE      /* test if we run from flash or RAM */

15  cmp     r0, r1                  /* don't reloc during debug         */

16  beq     stack_setup

17  ldr r2, _armboot_start

18  ldr r3, _bss_start

19  sub r2, r3, r2      /* r2 <- size of armboot            */

20  add r2, r0, r2      /* r2 <- source end address         */

21

22 copy_loop:

23  ldmia   r0!, {r3-r10}       /* copy from source address [r0]    */

24  stmia   r1!, {r3-r10}       /* copy to   target address [r1]    */

25  cmp r0, r2          /* until source end addreee [r2]    */

26  ble copy_loop

27

28  /* Set up the stack                         */

29 stack_setup:

30  ldr r0, _TEXT_BASE      /* upper 128 KiB: relocated uboot   */

31  sub r0, r0, #CFG_MALLOC_LEN /* malloc area                      */

32  sub r0, r0, #CFG_GBL_DATA_SIZE /* bdinfo                        */

33 #ifdef CONFIG_USE_IRQ

34  sub r0, r0, #(CONFIG_STACKSIZE_IRQ+CONFIG_STACKSIZE_FIQ)

35 #endif

36  sub sp, r0, #12     /* leave 3 words for abort-stack    */

37  ldr pc, _start_armboot

 

假设（其实不是假设，一般都是）这段代码当前是在起始地址为0的norflash中储存，上电复位后CPU首先跳转到2运行，这句使用的是一个相对跳转指令b，这个指令将跳转到本地储存的reset例程中运行。接下来的所有语句中都没有通过绝对寻址来寻找某个变量或者函数，因此即使目前的加载域与运行域不一致也没有问题，因为运行域主要是设计绝对寻址的正确性，如果没有进行绝对寻址，不一致又能奈他何？

这里其实还有个细节需要点出，就是3~6这几句异常处理都是使用ldr进行绝对跳转的，因此使用的undefined_instruction等都是这些名称对应的链接地址，但是由下面的分析可知，这几个中断处理函数可能根本没有被复制到RAM中（（uboot只是把第一个C语言开始的代码复制到RAM，但是这个结论尚未通过反汇编来验证），也就是这些函数没有被加载到运行域所以一旦发生reset以外的异常情况（比如说硬件中断）程序就会跑飞，由此也可以得到另一个结论：uboot不支持中断（至少当前分析的这个版本1.1.2是这样的，其他版本可能不一样）。

       下一个重点是12开始的这段重定位代码，13中使用了一个很特别的指令adr， adr r0, _start 作用是获得 _start 的实际运行所在的地址值，其中adr r0, _start翻译成 add r0,(PC+#offset)，offset 就是 adr r0, _start 指令到_start 的偏移量，值为负数，在链接时确定，这个偏移量是地址无关的，而pc为当前指令的地址的下一个地址值，由于CPU复位后pc的值从0开始增加，因此到这这里的值刚刚好能将offset抵消，所以最后r0的值就是0。

对于14，ldr r1, _TEXT_BASE 指令表示以程序相对偏移的方式加载数据，是索引偏移加载的另外一种形式，等同于ldr r1,[PC+#offset]，offset 是 ldr r1, _TEXT_BASE 到 _TEXT_BASE 的偏移量, 因此最后r1得到的值就是TEXT_BASE，这个值在其他文件中已经定义（比如对smdk2410平台而言就是在~/board/smdk2410/config.mk文件中）

15的就是比较上面的r0、r1是否相等，如果相等那么证明程序现在的位置正是想要去的运行域，因此不需要做重定位就可以通过16语句跳转stack_setup，否则就需要继续执行17开始的代码做重定位。17是将start_armboot这个地址给r2，start_armboot是谁？就是uboot的第一个C语言函数，这个函数名在链接的时候已经被安排为某个地址，也就是运行域。18是将bss_start这个值付给r3，那bss_start又是谁呢？说到这里先看看u-boot/board/smdk2410下的u-boot.lds链接文本的内容：

1 ENTRY(_start)

2 SECTIONS

3 {

4   . = 0x00000000;

5   . = ALIGN(4);

6   .text      :

7   {

8     cpu/arm920t/start.o   (.text)

9     *(.text)

10  }

11  . = ALIGN(4);

12  .rodata : { *(.rodata) }

13  . = ALIGN(4);

14  .data : { *(.data) }

15  . = ALIGN(4);

16  .got : { *(.got) }

17  __u_boot_cmd_start = .;

18  .u_boot_cmd : { *(.u_boot_cmd) }

19  __u_boot_cmd_end = .;

20  . = ALIGN(4);

21  __bss_start = .;

22  .bss : { *(.bss) }

23  _end = .;

24 }

链接文本是链接器用来链接可执行文件所使用的脚本文件，链接器根据这个来决定编译器编译出来的一大堆.o文件如何组织成为一个可执行文件。其中ENTRY(_start)指定了这个可执行文件的入口点（或者通俗点说第一个执行的函数）。SECTIONS描述了代码在内存中的布局情况，4指定了内存的分布从0开始，但是实际上在uboot链接过程中使用了一个-Ttext选项，这个选项最后就替代了0而将.text的链接地址修改为0x33f80000，这个链接命令如下：

arm-linux-ld –Tu-boot-1.1.4/board/smdk2410/u-boot.lds –Ttext 0x33f80000

因此这个链接脚本实际上并不是最终的内存布局。另外需要额外说明的是，这个链接地址是一个虚拟地址，如果mmu没有打开的话（对于uboot就是这种情况，mmu一直都是处于关闭状态）那么这个虚地址就是物理地址。在linux中，对于ARM平台内核一般从0xC0004000开始链接，这个地址明显就是虚拟地址，在2410中一般把内核解压在0x30008000这个物理地址上，所以在内核打开mmu之前的代码必须是位置无关的，否则没有办法启动内核；在打开mmu后，可以使用mmu将0x30000000开始的64M的物理空间映射为0xC0000000开始的64M虚拟空间。

说了这么多的题外话后回到之前的话题，bss_start就是在链接脚本中指出的一个符号，用以得到一个地址给外部的程序使用，这个地址实际上就是uboot链接后的最高地址，后面的bss虽然也还是uboot可执行文件的范畴，但是这个段在链接的时候并不分配任何内存空间，因此可以认为可执行文件到bss_start就结束了。由此可以理解/u-boot-1.1.2/cpu/arm920t/start.S的17、18实际上是分别得到了uboot的第一个C语言函数地址和这个可执行文件的最后一个链接地址，之前运行的那段位置无关的汇编代码直接被忽略。

22~26这段就是实现将uboot从norflash复制到RAM，地址为TEXT_BASE。

29~36是设置堆栈空间和sp指针的值，为执行C语言程序做准备。

37是重点，作用是跳转到C语言的入口点start_armboot函数。

ldr   pc, _start_armboot

这个语句使用的是ldr指令，这是一个绝对寻址指令，将start_armboot这个绝对地址加载到pc寄存器中，从而CPU转到start_armboot这个地址上开始运行，实现了从flash到RAM的跳转。

 

Uboot启动内核的过程：

1、  uboot对需要启动的映像文件有一定的格式要求，对于一个需要使用uboot引导的映像文件需要先使用uboot自带的mkimage工具对这个映像进行修改，添加结构为image_header头部。具体指令如下：

./mkimage -A arch -O os -T type -C comp -a addr -e ep -n name -d data_file[:data_file...] image

这样映像文件就增加了sizeof(image_header_t)个字节。为什么要增加一个这样的头部呢？关于这个文件头还是很有研究分量的，但是由于这里并不把这个作为重点，因此把这部分的分析写在了另一篇文章《uboot中关于mkimage指令的矛盾分析.doc》中。这里仅仅强调一下addr、ep这2个参数，前者是映像的下载地址，就是把这个映像复制到哪里（不包括上面说到的那个文件头，这部分在复制的时候被跳过），后者就是内核的入口，uboot就是从这点来找到并启动内核的。

2、按照文件头的参数将映像从flash或者RAM的其他位置复制到addr这里，其中文件头被直接忽略。如果是使用mkimage的时候使用了压缩选项那么就不是单纯的复制，而是解压到这个addr。

3、设置启动参数表。与vivi不一样，uboot对向内核传递参数时使用的是struct tag（标记列表，tagged list）方式。设置方式大概遵循如下步骤：

       1 setup_start_tag (bd);

       2 setup_memory_tags (bd);

       3 setup_commandline_tag (bd, commandline);

       4 setup_initrd_tag (bd, initrd_start, initrd_end);

       5 setup_end_tag (bd);

       其中，1是标记列表的开始，这个设置选项主要是指定这个启动参数表的存放位置，比如说一般会指定在SDRAM_BASE+0x100这个地方。2是设置内存标记，告诉内核这个硬件平台的RAM有多大、起始地址是多少等。3是设置命令行参数。4是设置initrd标记，如果并不使用initrd的话那么这个设置是不需要的。

4、关闭cache，禁止mmu，禁止中断，调用kernel函数跳转到内核，thekernel函数是如何定义的呢？这个过程是怎么实现的呢？

1 void (*theKernel)(int zero, int arch, uint params);

2 theKernel = (void (*)(int, int, uint))ntohl(hdr->ih_ep);

3 theKernel (0, bd->bi_arch_number, bd->bi_boot_params);

这是在armlinux.c文件截取do_bootm_linux函数（这是启动内核时调用的最后一个函数）的的几个相关语句，1是定义一个函数指针，2是把这个指针指向实际执行的函数地址，这个地址是谁呢？就是上面第一点说到的那个ep，就是内核的入口地址，或者说内核的第一个语句，这里还使用了(void (*)(int, int, uint))将这个地址强制转换为有3个参数的函数，为什么要这样做呢？因为在跳转到内核前需要设置好r0、r1、r2这3个寄存器的值，而在一般的C函数调用过程中，入口参数就是使用r0、r1、r2.。。。。。。。等来传递的，换言之，在3语句中调用theKernel函数时提供的3个入口参数最后将分别传给r0、r1、r2，这样就非常巧妙地通过C语言的特性来实现了r0、r1、r2的设置。

       其中值得一提的是，r2的值在vivi中是设置为内核在RAM中的地址，但是在uboot中是设置为了uboot传递给linux的启动参数表在RAM中的位置。究竟谁是对的呢？从逻辑上看，vivi向内核传递内核所在的地址没有意义，因为内核要这个没用，但是内核需要启动参数表，因此设置这个参数表的位置有意义，由此可以认定vivi那样做是不对的。再者，vivi如果要将参数正确传递给linux，必须事先知道linux默认会去哪里找启动参数表，然后在这个位置上填好这个参数表，否则如果随便找个地方放这个表linux是没有办法找到的，这样的话内核将启动失败。

 

另外需要再次强调的是，uboot没有打开mmu，因此一直使用的地址都是直接对应在物理地址上，但是vivi为了实现更好的性能打开了cache和mmu，因此情况就相当不一样了，下面将说明vivi的情况。

 

Vivi部分：

 

Vivi的入口点在vivi/arch/s3c2440/head.S的start，在这个文件中截取部分相关代码进行分析：

1 ENTRY(_start)

2   b   Reset

3   b   HandleUndef

4   b   HandleIRQ

5   b   HandleFIQ

6 Reset:

7   @ disable watch dog timer

8   mov r1, #0x53000000

9   mov r2, #0x0

10  str r2, [r1]

11  @ disable all interrupts

12  mov r1, #INT_CTL_BASE

13  mov r2, #0xffffffff

14  str r2, [r1, #oINTMSK]

15  ldr r2, =0x7ff

16  str r2, [r1, #oINTSUBMSK]  

17  @ initialise system clocks

18  mov r1, #CLK_CTL_BASE

19  bl  memsetup

20  bl  InitUART

21 # ifdef CONFIG_S3C2440_NAND_BOOT

22  bl  copy_myself

23  @ jump to ram

24  ldr r1, =on_the_ram

25  add pc, r1, #0

26  nop

27  nop

28 1:   b   1b      @ infinite loop

29 on_the_ram:

30 #endif

31  @ get read to call C functions

32  ldr sp, DW_STACK_START  @ setup stack pointer

33  mov fp, #0          @ no previous frame, so fp=0

34  mov a2, #0          @ set argv to NULL

35  bl  main            @ call main

36  mov pc, #FLASH_BASE     @ otherwise, reboot

 

2~5都是使用b进行跳转，就是说对异常情况vivi能正常处理，但是在vivi的中还没发现有哪个驱动程序使用硬件中断。而在11也看到了vivi将硬件中断屏蔽掉，至于有没有在后面重新打开我没有进一步去求证。

7~18都是位置无关的，因为都没有对某个变量名、函数名进行绝对寻址，要注意的是CLK_CTL_BASE受雇一个直接数而不是一个变量名。

19、20、22都使用了一个bl进行相对跳转，跳到储存在本地的函数名中运行，这些函数的实现都放在了head.S文件中。但是这应该不是必须的，因为在copy_myself函数中就调用了一个C文件中的函数。下面分析一下这个copy_myself函数中重要的几个语句。

    下面是从copy_myself截取出来的几个重要的语句：

1   @ get read to call C functions (for nand_read())

2   ldr sp, DW_STACK_START  @ setup stack pointer

3   mov fp, #0          @ no previous frame, so fp=0

4

5   @ copy vivi to RAM

6   ldr r0, =VIVI_RAM_BASE

7   mov     r1, #0x0

8   mov r2, #0x20000

9   bl  nand_read_ll

上面已经说到，copy_myself将调用一个C语言函数nand_read_ll，C函数在运行之前必须设置好堆栈指针sp，1~3就是这样来的。

6~8是9中nand_read_ll函数的入口参数，分别代表了目标地址、源地址、复制的size，执行这个nand_read_ll以后vivi就玩玩整整地被复制一个副本到了VIVI_RAM_BASE开始的地方。为跳转到RAM中运行提供了前提条件。

说完了copy_myself函数继续回到之前的reset函数进行分析。23~25意图很明显，先是通过24将on_the_ram这个链接地址给r1，接着就将r1的值赋给pc从而跳转到这个地址上运行，当然也可以直接把on_the_ram赋给pc。这里用的指令是ldr，是一个绝对寻址指令，on_the_ram对应的链接地址被直接送进了r1，这一步已经是位置相关了，由于已经将vivi复制到RAM中的运行域所以这时on_the_ram对应的链接地址上已经真的放着相应的代码，所以这样执行是不会出任何问题的。

之后，32语句被执行，但是这个语句已经是在RAM中，和上一条语句的位置已经有天壤之别。这句就是设置堆栈指针sp以便35调用C语言函数main，但是在copy_myself中不是已经设置了吗？我也觉得在这里应该不需要再设置一次也能使程序正常运行。

35使用bl来相对寻址到main，这时的main也是在RAM中的那个，当然这里也可以使用绝对跳转指令。到了main后就进入了C语言环境了。

再返回上面提到的VIVI_RAM_BASE这个数值来展开讨论。Mini2440提供的vivi中，这个数值在vivi/include/platform/smdk440.h中定义，VIVI_RAM_BASE=0x33f00000。这个地址按道理应该和vivi的链接地址是一致的，那么到底是不是呢？查看链接文本vivi/arch/vivi.lds.in文件找到TEXTADDR，这个值在vivi/arch/makefile中相应地也定义为0x33f00000，因此是符合之前的猜测的。

说到这里本来就可以结束了，但是由于在vivi的main函数里面使用了mmu，因此情况一下子变得复杂起来。首先为什么vivi要打开MMU呢？在uboot里面这个一直是关闭的。原因也是比较简单的，就是追求系统运行的高效。因为s3c2410的Icache不受MMU的影响，而Dcache和write buffer则必须开启了MMU功能之后，才能使用。而使用Dcache和write buffer后，对系统运行速度的提高是非常明显的，后面还将通过实验来验证这一点。也就是说，在nand flash启动时，vivi使用了MMU，主要是为了获得Dcache和write buffer的使用权，借此提高系统运行的性能。（在《vivi开发笔记: MMU分析》一文中有关于这点的说明）。

接下来需要讨论一下mmu被开启后的代码运行情况。

在vivi/init/main.c的main函数中截取以下语句：

    mem_map_init();

    mmu_init();

第一句实际上是建立一个内存映射表，对于nand启动的内存映射完全是线性映射，就是将虚拟地址的0~4G映射为物理地址的0~4G，虚拟地址等于物理地址，实现语句如下：

static inline void mem_mapping_linear(void)

{

    unsigned long pageoffset, sectionNumber;

    for (sectionNumber = 0; sectionNumber < 4096; sectionNumber++)

    {

        pageoffset = (sectionNumber << 20);

        *(mmu_tlb_base + (pageoffset >> 20)) = pageoffset | MMU_SECDESC;

    }

    for (pageoffset = DRAM_BASE; pageoffset < (DRAM_BASE+DRAM_SIZE); pageoffset += SZ_1M)

{

        *(mmu_tlb_base + (pageoffset >> 20)) = pageoffset | MMU_SECDESC | MMU_CACHEABLE;

}

}

    对于norflash则是先将norflash的物理地址映射到一个空闲的虚拟地址，把norflash原先占用的0开始的虚拟地址空间让出来；接着，将虚拟地址为0开始的1M空间映射到DRAM对应的物理地址上。这样的结果就是，虚拟地址为0开始的1M空间被映射到DRAM的物理地址，同时与DRAM物理地址相同的虚拟地址也映射到DRAM的物理地址上；物理地址为0开始的空间被映射到另一端空闲的虚拟地址空间；发生中断、寻址的时候CPU用的都是虚拟地址，CPU把虚拟地址发给MMU，由MMU通过映射表得到相应的虚拟地址并对物理地址进行寻址。在CPU地址总线上出现的就是物理地址。具体的实现语句这里不打算贴出来，可以看看源代码。

回到main函数中的mmu_init()，这个函数主要是使能MMU，并将上一步建立好的映射表地址告诉MMU，这样MMU就能使用已经建立好的映射表来进行内存映射。这个函数都是一些汇编指令，具体功能没有一条条语句去分许。

    由上面的分析还可以得到一些额外的结论。中2410/2440在nand方式启动下，物理地址0开始的4K空间被映射为内部的4K SRAM，接着nand开始的4K代码（实际上vivi的start）就被复制到这个4K SRAM中开始运行，并在4K代码结束前就把自身复制到DRAM中并跳转到DRAM运行，发生异常的时候就跳转到SRAM中的异常入口表中执行；而vivi的中断跳转命令都用的是b，因此都是跳转到SRAM本地的相应处理例程。如果是以nor启动，那么物理地址0开始的地方映射在norflash，发生异常的时候都跳转到norflash中运行。还有一种情况就是上面所说的把物理地址0映射到了DRAM，这样在发生异常的情况的时候实际就跳转到DRAM的地址中运行。前2种情况对与uboot也是一样的，但是由于uboot没有启用MMU因此没有第三种情况。

       

    Vivi启动内核的过程：

1、    把内核复制到合适的地址上。一般都是将linux内核从flash中复制到DRAM，具体将内核从哪个地址复制到哪个地址都是自定义的。

2、    设置传递给linux的启动参数。Bootloader在执行过程中必须设置和初始化 Linux 的内核启动参数。目前传递启动参数主要采用两种方式：即通过 struct param_struct 和struct tag（标记列表，tagged list）两种结构传递。struct param_struct 是一种比较老的参数传递方式，在 2.4 版本以前的内核中使用较多。从 2.4 版本以后 Linux 内核基本上采用标记列表的方式。但为了保持和以前版本的兼容性，它仍支持 struct param_struct 参数传递方式，只不过在内核启动过程中它将被转换成标记列表方式。 
标记列表方式是种比较新的参数传递方式，它必须以 ATAG_CORE 开始，并以ATAG_NONE 结尾。中间可以根据需要加入其他列表。Linux内核在启动过程中会根据该启动参数进行相应的初始化工作。（参考《ARM Linux启动过程分析》一文）。Vivi中使用的就是param_struct方式，先是创建一个param_struct结构并进行填充，主要是填充以下几个域：

    params->u1.s.page_size = LINUX_PAGE_SIZE;

    params->u1.s.nr_pages = (DRAM_SIZE >> LINUX_PAGE_SHIFT);

    memcpy(params->commandline, linux_cmd, strlen(linux_cmd) + 1);

第一个是指定linux内核的分页大小，第二个指定DRAM总共有多少个页。第三个就是指定命令行参数。填充完之后就将这个结构的内容复制到一个特定的地址上，这个地址在vivi中定义为param_base=DRAM+0x100。

    3、关闭cache，禁止mmu，设置r0＝0，r1＝处理器类型，r2＝内核在RAM中的地址，这个地址实际上就是bootloader复制内核时使用的那个地址。直接就使用mov   pc, r2或者类似的指令将r2的值赋给pc，CPU就转到了linux内核中运行。

 

Linux部分：

 

    Bootloader跳转到内核运行的入口点在linux/arch/arm/boot/compressed/Head.S文件中的start函数。怎么知道这里是内核的入口点呢？

首先需要了解linux内核的产生过程。首先是通过ld命令将编译好的各个.o文件链接成为linux/vmlinux可执行文件，同时产生system.map文件，这是一个非压缩的可执行文件，链接脚本为linux/arch/arm/kernel/vmlinux.lds.S,链接地址为TEXTADDR=0xC0008000，可以称之为真正的linux内核；接着通过objcopy工具将vmlinux复制为linux/arch/arm /boot/image，这时大小有变化，估计是格式已经发生了变化吧^_^；接下来在通过linux/arch/arm/boot/compressed/

makefile将image复制到本目录下并压缩成为piggy.gz，体积大大减少，这个是压缩后的内核；通过linux/arch/arm/boot/compressed/piggy.S文件将piggy.gz转换为数据段；通过linux/

arch/arm/boot/compressed下的makefile、vmlinux.lds.in将piggy.o、head.o、misc.o链接成vmlinux。Head.o、misc.o是什么呢？这是为了使用压缩内核而设计的一段解压缩代码，因为压缩内核没办法直接运行，只能解压后运行，所有在内核运转起来前必须先有一段代码将这个内核解压缩，这就是head.o、misc.o的作用了。在链接的时候piggy.o作为一个数据段链接到内核映像中，而不是作为一个程序的目标文件进行链接，head.o、misc.o则视为一般的目标文件进行链接，链接地址从0x00000000开始，这个链接后的vmlinux就是一个加上压缩代码后的压缩后内核；链接完vmlinux后就将这个文件使用objcopy转换为zImage等我们熟知的映像文件。

    从上面的分析可知bootloader转移到内核运行的时候第一个运行的实际上还不是真内核的内容，而只是内核前面的那段解压缩代码，由linux/arch/arm/boot/compressed/vmlinux.lds.in的内容可知head.S中的start函数就是bootloader转到内核映像时第一个要被执行的函数。这部分代码是链接在0x00000000这个地方的（注意解压缩代码运行期间一直没有打开mmu），但是实际上却被复制在DRAM上运行，因此这部分代码必须是位置无关的。事实上通过查看这部分代码可以发现的确是这样的，在调用函数的时候也是通过b、bl等指令而不是一些绝对寻址指令。那么到底到什么时候PIC代码才结束呢？答案是，压缩代码解压完毕并跳转到真正内核入口运行，并在执行完一些初始化的工作后打开mmu，这时候才是PIC代码结束的时候。那为什么是这样呢？上面已经说到，内核是从0xC0008000开始链接的，但是代码实际上是放在0x30008000的地方，0xC0008000这段地址在s3c2410中又是保留的地址，不是实际的RAM，因此没有办法使用像uboot、vivi那样的重定位方式来解决运行域与加载域不一致的矛盾，只能通过虚拟内存映射的方法，将物理上的0x30008000映射为虚拟的0xC0008000，这样运行域就与加载域一致了。因此在使用mmu之前的代码必须是位置无关的，否则代码将无法运行。

    这里还有一个细节需要提到一下，就是在查看system.map的时候会发现内核映像并不是从0xC0008000开始，而是从0xC0004000开始，那么是不是说链接地址应该是0xC0004000呢？不是的，0xC0004000开始的16K空间放在一个叫做初始化页表的东西，这个东西是在链接的时候静态添加到链接地址之前的16K开始的地方去的，实际上的链接地址就是0xC0008000。这个值可以在linux/arch/arm/mach-s3c2410/Makefile.boot中找到。

　　　对于linux部分并没有对代码一句句分析，客观上是因为还有很多事情做，只能写这么多了，主观上，linux的开始的那些汇编代码好难懂=_=|||.所以就到此为止了。

//---------------------theKernel
void (*theKernel)(int zero, int arch, uint params);
theKernel是一函数指针，它指向kernel的启动地址，对于uImage格式的内核，kernel=image_header_t结构体的ih_ep值

theKernel = (void (*)(int, int, uint))ntohl(hdr->ih_ep);
这里存在一个转换：ntohl(hdr->ih_ep)，原因在于下面两行代码：
mkimage.c
ep = addr;	// 通过mkimage工具传入的内核实际地址(不是uImage的首地址)
hdr->ih_ep = htonl(ep);

启动内核的代码为：
theKernel (0, bd->bi_arch_number, bd->bi_boot_params);
这里涉及三个参数，第二、第三个参数bd->bi_arch_number、bd->bi_boot_params含义由名字就可以清楚的知道，至于第一个参数，为什是0？有什么内涵？
这是规定。
或者这么说，你要启动内核，必须这么做，它来自于linux-2.6.30.4\Documentation\arm\Booting：

下面一段话说的很透彻，摘自《bootloader与linux中位置无关代码的分析》：
“为什么要这样做呢？因为在跳转到内核前需要设置好r0、r1、r2这3个寄存器的值，而在一般的C函数调用过程中，入口参数就是使用r0、r1、r2...等来传递的，换言之，在3语句中调用theKernel函数时提供的3个入口参数最后将分别传给r0、r1、r2，这样就非常巧妙地通过C语言的特性来实现了r0、r1、r2的设置。”
//---------------------end-----------------------------