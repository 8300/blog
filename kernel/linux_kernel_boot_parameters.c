一 传递方式

主要通过两种方式：

1. 将启动参数存放在内核页中，然后内核启动的时候直接从该内存页读取启动参数即可，这种传递启动参数的方法需要boot和kernel约定好，存放的内存页的地址（一般为内核启动紧挨着的前一页）和存放的启动参数的格式；

2.kernel通过调用boot的函数去获取启动参数；

由于第一种方式较为简单，所以这里只对第二种参数传递方法进行说明。



二 调用boot函数获取启动参数

这种方式首要的问题是： 内核如何能够调用到boot中的函数?

很明显不能直接调用，要不然内核链接的时候就要链接boot的代码了，所以只能采用另外一种方法即函数指针，在内核中定义函数指针变量，通过函数指针调用获取启动参数，所以现在的关键问题就变成了这个函数指针是如何初始化的问题，即如何设置这个函数指针的值。首先我想到的一个很傻的方法就是这个指针变量在内核编译时就指定，不过这种方法实在是太不灵活，因为boot的地址空间改变的话这个值也需要相应的跟在改变。既然这种编译指定不好，那就只能运行时去设置了，那么这个值从哪里来呢？很明显只有两种来源：寄存器或者内存，从内存取的话要指定内存地址，不够灵活也不够方便，因为这个内存地址要固定，所以从寄存器去取就是唯一正确的方法：boot在跳转到内核之前设置好该寄存器的值，跳转到内核之后首先取出寄存器的值设置定义好的函数指针，然后后面就可以调用函数指针获取到启动参数了。

下面分析一个实例的实现：

1. boot跳转之前设置好寄存器

LEAF(RunProgram)

..........

move t0,a0 # 
move a1,zero # A1 = 0
move a0,gp # A0 = handle
li a3,CFE_EPTSEAL  # A3 = entrypoint signature
LR t0,0(sp) # entry point


j t0 # go for it.

2.跳转到内核后首先执行 （arch/mips/kernel/head.s）

NESTED(kernel_entry, 16, sp) # kernel entry point


kernel_entry_setup # cpu specific setup

其中kernel_entry_setup定义如下： （arch/mips/include/asm/mach-brcmstb/kernel-entry-init.h）

.macro kernel_entry_setup


# save arguments for CFE callback
sw a0, cfe_handle
sw a2, cfe_entry
sw a3, cfe_seal


jal bmips_enable_xks01


.endm

3.通过函数指针获取启动参数

cfe_init(cfe_handle, cfe_entry);



int cfe_init(u64 handle, u64 ept)
{
cfe_dispfunc = NATIVE_FROM_XPTR(ept);
cfe_handle = handle;
return 0;
}

