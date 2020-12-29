0.//-----Linux对IO端口资源的管理
　　几乎每一种外设都是通过读写设备上的寄存器来进行的。外设寄存器也称为“I/O端口”，通常包括：控制寄存器、状态寄存器和数据寄存器三大类，而且一个外设的寄存器通常被连续地编址。CPU对外设IO端口物理地址的编址方式有两种：一种是I/O映射方式（I/O－mapped），另一种是内存映射方式（Memory－mapped）。而具体采用哪一种则取决于CPU的体系结构。

　　有些体系结构的CPU（如，PowerPC、m68k等）通常只实现一个物理地址空间（RAM）。在这种情况下，外设I/O端口的物理地址就被映射到CPU的单一物理地址空间中，而成为内存的一部分。此时，CPU可以象访问一个内存单元那样访问外设I/O端口，而不需要设立专门的外设I/O指令。这就是所谓的“内存映射方式”（Memory－mapped）。

　　而另外一些体系结构的CPU（典型地如X86）则为外设专门实现了一个单独地地址空间，称为“I/O地址空间”或者“I/O端口空间”。这是一个与CPU地RAM物理地址空间不同的地址空间，所有外设的I/O端口均在这一空间中进行编址。CPU通过设立专门的I/O指令（如X86的IN和OUT指令）来访问这一空间中的地址单元（也即I/O端口）。这就是所谓的“I/O映射方式”（I/O－mapped）。与RAM物理地址空间相比，I/O地址空间通常都比较小，如x86 CPU的I/O空间就只有64KB（0－0xffff）。这是“I/O映射方式”的一个主要缺点。

　　linux将基于I/O映射方式的或内存映射方式的I/O端口通称为“I/O区域”（I/O region）。在讨论对I/O区域的管理之前，我们首先来分析一下Linux是如何实现“I/O资源”这一抽象概念的。

3．1 linux对I/O资源的描述

　　linux设计了一个通用的数据结构resource来描述各种I/O资源（如：I/O端口、外设内存、DMA和IRQ等）。该结构定义在include/linux/ioport.h头文件中：


　　struct resource {
const char *name;
unsigned long start, end;
unsigned long flags;
struct resource *parent, *sibling, *child;
　　};

　　各成员的含义如下：

　　1. name指针：指向此资源的名称。
　　2. start和end：表示资源的起始物理地址和终止物理地址。它们确定了资源的范围，也即是一个闭区间[start,end]。
　　3. flags：描述此资源属性的标志（见下面）。
　　4. 指针parent、sibling和child：分别为指向父亲、兄弟和子资源的指针。

　　属性flags是一个unsigned long类型的32位标志值，用以描述资源的属性。比如：资源的类型、是否只读、是否可缓存，以及是否已被占用等。下面是一部分常用属性标志位的定义（ioport.h）：


/*
* IO resources have these defined flags.
*/
#define IORESOURCE_BITS 0x000000ff /* Bus-specific bits */

#define IORESOURCE_IO 0x00000100 /* Resource type */
#define IORESOURCE_MEM 0x00000200
#define IORESOURCE_IRQ 0x00000400
#define IORESOURCE_DMA 0x00000800

#define IORESOURCE_PREFETCH 0x00001000 /* No side effects */
#define IORESOURCE_READONLY 0x00002000
#define IORESOURCE_CACHEABLE 0x00004000
#define IORESOURCE_RANGELENGTH 0x00008000
#define IORESOURCE_SHADOWABLE 0x00010000
#define IORESOURCE_BUS_HAS_VGA 0x00080000

#define IORESOURCE_UNSET 0x20000000
#define IORESOURCE_AUTO 0x40000000
#define IORESOURCE_BUSY 0x80000000
/* Driver has marked this resource busy */

　　指针parent、sibling和child的设置是为了以一种树的形式来管理各种I/O资源。
3．2 linux对I/O资源的管理

　　linux是以一种倒置的树形结构来管理每一类I/O资源（如：I/O端口、外设内存、DMA和IRQ）的。每一类I/O资源都对应有一颗倒置的资源树，树中的每一个节点都是一个resource结构，而树的根结点root则描述了该类资源的整个资源空间。

　　基于上述这个思想，linux在kernel/Resource.c文件中实现了对资源的申请、释放及查找等操作。

　　3．2．1 I/O资源的申请

　　假设某类资源有如下这样一颗资源树：

　　节点root、r1、r2和r3实际上都是一个resource结构类型。子资源r1、r2和r3通过sibling指针链接成一条单向非循环链表，其表头由root节点中的child指针定义，因此也称为父资源的子资源链表。r1、r2和r3的parent指针均指向他们的父资源节点，在这里也就是图中的root节点。

　　假设想在root节点中分配一段I/O资源（由图中的阴影区域表示）。函数request_resource()实现这一功能。它有两个参数：①root指针，表示要在哪个资源根节点中进行分配；②new指针，指向描述所要分配的资源（即图中的阴影区域）的resource结构。该函数的源代码如下（kernel/resource.c）:


　　int request_resource(struct resource *root, struct resource *new)
　　{
struct resource *conflict;

write_lock(&resource_lock);
conflict = __request_resource(root, new);
write_unlock(&resource_lock);
return conflict ? -EBUSY : 0;
　　}

　　对上述函数的NOTE如下：

　　①资源锁resource_lock对所有资源树进行读写保护，任何代码段在访问某一颗资源树之前都必须先持有该锁。其定义如下（kernel/Resource.c）：


　　static rwlock_t resource_lock = RW_LOCK_UNLOCKED;

　　②可以看出，函数实际上是通过调用内部静态函数__request_resource()来完成实际的资源分配工作。如果该函数返回非空指针，则表示有资源冲突；否则，返回NULL就表示分配成功。

　　③最后，如果conflict指针为NULL，则request_resource()函数返回返回值0，表示成功；否则返回－EBUSY表示想要分配的资源已被占用。

　　函数__request_resource()完成实际的资源分配工作。如果参数new所描述的资源中的一部分或全部已经被其它节点所占用，则函数返回与new相冲突的resource结构的指针。否则就返回NULL。该函数的源代码如下


（kernel/Resource.c）：
/* Return the conflict entry if you can't request it */
static struct resource * __request_resource
　　(struct resource *root, struct resource *new)
{
unsigned long start = new->start;
unsigned long end = new->end;
struct resource *tmp, **p;

if (end < start)
return root;
if (start < root->start)
return root;
if (end > root->end)
return root;
p = &root->child;
for (;;) {
tmp = *p;
if (!tmp || tmp->start > end) {
new->sibling = tmp;
*p = new;
new->parent = root;
return NULL;
}
p = &tmp->sibling;
if (tmp->end < start)
continue;
return tmp;
}
}

　　对函数的NOTE：

　　①前三个if语句判断new所描述的资源范围是否被包含在root内，以及是否是一段有效的资源（因为end必须大于start）。否则就返回root指针，表示与根结点相冲突。

　　②接下来用一个for循环遍历根节点root的child链表，以便检查是否有资源冲突，并将new插入到child链表中的合适位置（child链表是以I/O资源物理地址从低到高的顺序排列的）。为此，它用tmp指针指向当前正被扫描的resource结构，用指针p指向前一个resource结构的sibling指针成员变量，p的初始值为指向root－>sibling。For循环体的执行步骤如下：

　　l 让tmp指向当前正被扫描的resource结构（tmp＝*p）。

　　l 判断tmp指针是否为空（tmp指针为空说明已经遍历完整个child链表），或者当前被扫描节点的起始位置start是否比new的结束位置end还要大。只要这两个条件之一成立的话，就说明没有资源冲突，于是就可以把new链入child链表中：①设置new的sibling指针指向当前正被扫描的节点tmp（new->sibling=tmp）；②当前节点tmp的前一个兄弟节点的sibling指针被修改为指向new这个节点（*p=new）；③将new的parent指针设置为指向root。然后函数就可以返回了（返回值NULL表示没有资源冲突）。

　　l 如果上述两个条件都不成立，这说明当前被扫描节点的资源域有可能与new相冲突（实际上就是两个闭区间有交集），因此需要进一步判断。为此它首先修改指针p，让它指向tmp->sibling，以便于继续扫描child链表。然后，判断tmp->end是否小于new->start，如果小于，则说明当前节点tmp和new没有资源冲突，因此执行continue语句，继续向下扫描child链表。否则，如果tmp->end大于或等于new->start，则说明tmp->[start,end]和new->[start,end]之间有交集。所以返回当前节点的指针tmp，表示发生资源冲突。

　　3．2．2 资源的释放

　　函数release_resource()用于实现I/O资源的释放。该函数只有一个参数——即指针old，它指向所要释放的资源。起源代码如下：


int release_resource(struct resource *old)
{
int retval;

write_lock(&resource_lock);
retval = __release_resource(old);
write_unlock(&resource_lock);
return retval;
}

　　可以看出，它实际上通过调用__release_resource()这个内部静态函数来完成实际的资源释放工作。函数__release_resource()的主要任务就是将资源区域old（如果已经存在的话）从其父资源的child链表重摘除，它的源代码如下：


static int __release_resource(struct resource *old)
{
struct resource *tmp, **p;

p = &old->parent->child;
for (;;) {
tmp = *p;
if (!tmp)
break;
if (tmp == old) {
*p = tmp->sibling;
old->parent = NULL;
return 0;
}
p = &tmp->sibling;
}
return -EINVAL;
}

　　对上述函数代码的NOTE如下：

　　同函数__request_resource()相类似，该函数也是通过一个for循环来遍历父资源的child链表。为此，它让tmp指针指向当前被扫描的资源，而指针p则指向当前节点的前一个节点的sibling成员（p的初始值为指向父资源的child指针）。循环体的步骤如下：

　　①首先，让tmp指针指向当前被扫描的节点（tmp＝*p）。

　　②如果tmp指针为空，说明已经遍历完整个child链表，因此执行break语句推出for循环。由于在遍历过程中没有在child链表中找到参数old所指定的资源节点，因此最后返回错误值－EINVAL，表示参数old是一个无效的值。

　　③接下来，判断当前被扫描节点是否就是参数old所指定的资源节点。如果是，那就将old从child链表中去除，也即让当前结点tmp的前一个兄弟节点的sibling指针指向tmp的下一个节点，然后将old－>parent指针设置为NULL。最后返回0值表示执行成功。

　　④如果当前被扫描节点不是资源old，那就继续扫描child链表中的下一个元素。因此将指针p指向tmp－>sibling成员。

　　3．2．3 检查资源是否已被占用，

　　函数check_resource()用于实现检查某一段I/O资源是否已被占用。其源代码如下：


int check_resource(struct resource *root, unsigned long start, unsigned long len)
{
struct resource *conflict, tmp;

tmp.start = start;
tmp.end = start + len - 1;
write_lock(&resource_lock);
conflict = __request_resource(root, &tmp);
if (!conflict)
__release_resource(&tmp);
write_unlock(&resource_lock);
return conflict ? -EBUSY : 0;
}

　　对该函数的NOTE如下：

　　①构造一个临时资源tmp，表示所要检查的资源[start,start+end-1]。

　　②调用__request_resource()函数在根节点root申请tmp所表示的资源。如果tmp所描述的资源还被人使用，则该函数返回NULL，否则返回非空指针。因此接下来在conflict为NULL的情况下，调用__release_resource()将刚刚申请的资源释放掉。

　　③最后根据conflict是否为NULL，返回－EBUSY或0值。
3．2．4 寻找可用资源

　　函数find_resource()用于在一颗资源树中寻找未被使用的、且满足给定条件的（也即资源长度大小为size，且在[min,max]区间内）的资源。其函数源代码如下：


/*
* Find empty slot in the resource tree given range and alignment.
*/
static int find_resource(struct resource *root, struct resource *new,
unsigned long size,
unsigned long min, unsigned long max,
unsigned long align,
void (*alignf)(void *, struct resource *, unsigned long),
void *alignf_data)
{
struct resource *this = root->child;

new->start = root->start;
for(;;) {
if (this)
new->end = this->start;
else
new->end = root->end;
if (new->start < min)
new->start = min;
if (new->end > max)
new->end = max;
new->start = (new->start + align - 1) & ~(align - 1);
if (alignf)
alignf(alignf_data, new, size);
if (new->start < new->end && new->end - new->start + 1 >= size)
{
new->end = new->start + size - 1;
return 0;
}
if (!this)
break;
new->start = this->end + 1;
this = this->sibling;
}
return -EBUSY;
}

　　对该函数的NOTE如下：

　　同样，该函数也要遍历root的child链表，以寻找未被使用的资源空洞。为此，它让this指针表示当前正被扫描的子资源节点，其初始值等于root->child，即指向child链表中的第一个节点，并让new->start的初始值等于root->start，然后用一个for循环开始扫描child链表，对于每一个被扫描的节点，循环体执行如下操作：

　　①首先，判断this指针是否为NULL。如果不为空，就让new->end等于this->start，也即让资源new表示当前资源节点this前面那一段未使用的资源区间。

　　②如果this指针为空，那就让new->end等于root->end。这有两层意思：第一种情况就是根结点的child指针为NULL（即根节点没有任何子资源）。因此此时先暂时将new->end放到最大。第二种情况就是已经遍历完整个child链表，所以此时就让new表示最后一个子资源后面那一段未使用的资源区间。

　　③根据参数min和max修正new->[start,end]的值，以使资源new被包含在[min,max]区域内。

　　④接下来进行对齐操作。

　　⑤然后，判断经过上述这些步骤所形成的资源区域new是否是一段有效的资源（end必须大于或等于start），而且资源区域的长度满足size参数的要求（end－start＋1>=size）。如果这两个条件均满足，则说明我们已经找到了一段满足条件的资源空洞。因此在对new->end的值进行修正后，然后就可以返回了（返回值0表示成功）。

　　⑥如果上述两条件不能同时满足，则说明还没有找到，因此要继续扫描链表。在继续扫描之前，我们还是要判断一下this指针是否为空。如果为空，说明已经扫描完整个child链表，因此就可以推出for循环了。否则就将new->start的值修改为this->end+1，并让this指向下一个兄弟资源节点，从而继续扫描链表中的下一个子资源节点。

　　3．2．5 分配接口allocate_resource()

　　在find_resource()函数的基础上，函数allocate_resource()实现：在一颗资源树中分配一条指定大小的、且包含在指定区域[min,max]中的、未使用资源区域。其源代码如下：


/*
* Allocate empty slot in the resource tree given range and alignment.
*/
int allocate_resource(struct resource *root, struct resource *new,
unsigned long size,
unsigned long min, unsigned long max,
unsigned long align,
void (*alignf)(void *, struct resource *, unsigned long),
void *alignf_data)
{
int err;

write_lock(&resource_lock);
err = find_resource(root, new, size, min, max, align, alignf, alignf_data);
if (err >= 0 && __request_resource(root, new))
err = -EBUSY;
write_unlock(&resource_lock);
return err;
}

　　3．2．6 获取资源的名称列表

　　函数get_resource_list()用于获取根节点root的子资源名字列表。该函数主要用来支持/proc/文件系统（比如实现proc/ioports文件和/proc/iomem文件）。其源代码如下：


int get_resource_list(struct resource *root, char *buf, int size)
{
char *fmt;
int retval;

fmt = " %08lx-%08lx : %s
";
if (root->end < 0x10000)
fmt = " %04lx-%04lx : %s
";
read_lock(&resource_lock);
retval = do_resource_list(root->child, fmt, 8, buf, buf + size) - buf;
read_unlock(&resource_lock);
return retval;
}

　　可以看出，该函数主要通过调用内部静态函数do_resource_list()来实现其功能，其源代码如下：


/*
* This generates reports for /proc/ioports and /proc/iomem
*/
static char * do_resource_list(struct resource *entry, const char *fmt,
　　int offset, char *buf, char *end)
{
if (offset < 0)
offset = 0;

while (entry) {
const char *name = entry->name;
unsigned long from, to;

if ((int) (end-buf) < 80)
return buf;

from = entry->start;
to = entry->end;
if (!name)
name = "";

buf += sprintf(buf, fmt + offset, from, to, name);
if (entry->child)
buf = do_resource_list(entry->child, fmt, offset-2, buf, end);
entry = entry->sibling;
}

return buf;
}

　　函数do_resource_list()主要通过一个while{}循环以及递归嵌套调用来实现，较为简单，这里就不在详细解释了。

3．3 管理I/O Region资源

　　linux将基于I/O映射方式的I/O端口和基于内存映射方式的I/O端口资源统称为“I/O区域”（I/O Region）。I/O Region仍然是一种I/O资源，因此它仍然可以用resource结构类型来描述。下面我们就来看看Linux是如何管理I/O Region的。

　　3．3．1 I/O Region的分配

　　在函数__request_resource()的基础上，linux实现了用于分配I/O区域的函数__request_region()，如下:


struct resource * __request_region(struct resource *parent,
　　unsigned long start, unsigned long n, const char *name)
{
struct resource *res = kmalloc(sizeof(*res), GFP_KERNEL);

if (res) {
memset(res, 0, sizeof(*res));
res->name = name;
res->start = start;
res->end = start + n - 1;
res->flags = IORESOURCE_BUSY;

write_lock(&resource_lock);

for (;;) {
struct resource *conflict;

conflict = __request_resource(parent, res);
if (!conflict)
break;
if (conflict != parent) {
parent = conflict;
if (!(conflict->flags & IORESOURCE_BUSY))
continue;
}

/* Uhhuh, that didn't work out.. */
kfree(res);
res = NULL;
break;
}
write_unlock(&resource_lock);
}
return res;
}

NOTE：

　　①首先，调用kmalloc（）函数在SLAB分配器缓存中分配一个resource结构。

　　②然后，相应的根据参数值初始化所分配的resource结构。注意！flags成员被初始化为IORESOURCE_BUSY。

　　③接下来，用一个for循环开始进行资源分配，循环体的步骤如下：

　　l 首先，调用__request_resource()函数进行资源分配。如果返回NULL，说明分配成功，因此就执行break语句推出for循环，返回所分配的resource结构的指针，函数成功地结束。

　　l 如果__request_resource()函数分配不成功，则进一步判断所返回的冲突资源节点是否就是父资源节点parent。如果不是，则将分配行为下降一个层次，即试图在当前冲突的资源节点中进行分配（只有在冲突的资源节点没有设置IORESOURCE_BUSY的情况下才可以），于是让parent指针等于conflict，并在conflict->flags&IORESOURCE_BUSY为0的情况下执行continue语句继续for循环。

　　l 否则如果相冲突的资源节点就是父节点parent，或者相冲突资源节点设置了IORESOURCE_BUSY标志位，则宣告分配失败。于是调用kfree（）函数释放所分配的resource结构，并将res指针置为NULL，最后用break语句推出for循环。

　　④最后，返回所分配的resource结构的指针。

　　3．3．2 I/O Region的释放

　　函数__release_region()实现在一个父资源节点parent中释放给定范围的I/O Region。实际上该函数的实现思想与__release_resource()相类似。其源代码如下：


void __release_region(struct resource *parent,
　　　　unsigned long start, unsigned long n)
{
struct resource **p;
unsigned long end;

p = &parent->child;
end = start + n - 1;

for (;;) {
struct resource *res = *p;

if (!res)
break;
if (res->start <= start && res->end >= end) {
if (!(res->flags & IORESOURCE_BUSY)) {
p = &res->child;
continue;
}
if (res->start != start' 'res->end != end)
break;
*p = res->sibling;
kfree(res);
return;
}
p = &res->sibling;
}
printk("Trying to free nonexistent resource <%08lx-%08lx>
", start, end);
}

　　类似地，该函数也是通过一个for循环来遍历父资源parent的child链表。为此，它让指针res指向当前正被扫描的子资源节点，指针p指向前一个子资源节点的sibling成员变量，p的初始值为指向parent->child。For循环体的步骤如下：

　　①让res指针指向当前被扫描的子资源节点（res＝*p）。

　　②如果res指针为NULL，说明已经扫描完整个child链表，所以退出for循环。

　　③如果res指针不为NULL，则继续看看所指定的I/O区域范围是否完全包含在当前资源节点中，也即看看[start,start+n-1]是否包含在res->[start,end]中。如果不属于，则让p指向当前资源节点的sibling成员，然后继续for循环。如果属于，则执行下列步骤：

　　l 先看看当前资源节点是否设置了IORESOURCE_BUSY标志位。如果没有设置该标志位，则说明该资源节点下面可能还会有子节点，因此将扫描过程下降一个层次，于是修改p指针，使它指向res->child，然后执行continue语句继续for循环。

　　l 如果设置了IORESOURCE_BUSY标志位。则一定要确保当前资源节点就是所指定的I/O区域，然后将当前资源节点从其父资源的child链表中去除。这可以通过让前一个兄弟资源节点的sibling指针指向当前资源节点的下一个兄弟资源节点来实现（即让*p=res->sibling），最后调用kfree（）函数释放当前资源节点的resource结构。然后函数就可以成功返回了。

　　3．3．3 检查指定的I/O Region是否已被占用

　　函数__check_region()检查指定的I/O Region是否已被占用。其源代码如下：


int __check_region(struct resource *parent, unsigned long start, unsigned long n)
{
struct resource * res;

res = __request_region(parent, start, n, "check-region");
if (!res)
return -EBUSY;

release_resource(res);
kfree(res);
return 0;
}

　　该函数的实现与__check_resource()的实现思想类似。首先，它通过调用__request_region()函数试图在父资源parent中分配指定的I/O Region。如果分配不成功，将返回NULL，因此此时函数返回错误值－EBUSY表示所指定的I/O Region已被占用。如果res指针不为空则说明所指定的I/O Region没有被占用。于是调用__release_resource()函数将刚刚分配的资源释放掉（实际上是将res结构从parent的child链表去除），然后调用kfree（）函数释放res结构所占用的内存。最后，返回0值表示指定的I/O Region没有被占用。
3．4 管理I/O端口资源

　　我们都知道，采用I/O映射方式的X86处理器为外设实现了一个单独的地址空间，也即“I/O空间”（I/O Space）或称为“I/O端口空间”，其大小是64KB（0x0000－0xffff）。linux在其所支持的所有平台上都实现了“I/O端口空间”这一概念。

　　由于I/O空间非常小，因此即使外设总线有一个单独的I/O端口空间，却也不是所有的外设都将其I/O端口（指寄存器）映射到“I/O端口空间”中。比如，大多数PCI卡都通过内存映射方式来将其I/O端口或外设内存映射到CPU的RAM物理地址空间中。而老式的ISA卡通常将其I/O端口映射到I/O端口空间中。

　　linux是基于“I/O Region”这一概念来实现对I/O端口资源（I/O－mapped 或 Memory－mapped）的管理的。

　　3．4．1 资源根节点的定义

　　linux在kernel/Resource.c文件中定义了全局变量ioport_resource和iomem_resource，来分别描述基于I/O映射方式的整个I/O端口空间和基于内存映射方式的I/O内存资源空间（包括I/O端口和外设内存）。其定义如下：


struct resource ioport_resource =
　　　　{ "PCI IO", 0x0000, IO_SPACE_LIMIT, IORESOURCE_IO };
struct resource iomem_resource =
　　　　{ "PCI mem", 0x00000000, 0xffffffff, IORESOURCE_MEM };

　　其中，宏IO_SPACE_LIMIT表示整个I/O空间的大小，对于X86平台而言，它是0xffff（定义在include/asm-i386/io.h头文件中）。显然，I/O内存空间的大小是4GB。

　　3．4．2 对I/O端口空间的操作

　　基于I/O Region的操作函数__XXX_region()，linux在头文件include/linux/ioport.h中定义了三个对I/O端口空间进行操作的宏：①request_region()宏，请求在I/O端口空间中分配指定范围的I/O端口资源。②check_region()宏，检查I/O端口空间中的指定I/O端口资源是否已被占用。③release_region()宏，释放I/O端口空间中的指定I/O端口资源。这三个宏的定义如下：


#define request_region(start,n,name)
__request_region(&ioport_resource, (start), (n), (name))
#define check_region(start,n)
__check_region(&ioport_resource, (start), (n))
#define release_region(start,n)
__release_region(&ioport_resource, (start), (n))

　　其中，宏参数start指定I/O端口资源的起始物理地址（是I/O端口空间中的物理地址），宏参数n指定I/O端口资源的大小。

　　3．4．3 对I/O内存资源的操作

　　基于I/O Region的操作函数__XXX_region()，linux在头文件include/linux/ioport.h中定义了三个对I/O内存资源进行操作的宏：①request_mem_region()宏，请求分配指定的I/O内存资源。②check_ mem_region()宏，检查指定的I/O内存资源是否已被占用。③release_ mem_region()宏，释放指定的I/O内存资源。这三个宏的定义如下：


#define request_mem_region(start,n,name)
　　__request_region(&iomem_resource, (start), (n), (name))
#define check_mem_region(start,n)
__check_region(&iomem_resource, (start), (n))
#define release_mem_region(start,n)
__release_region(&iomem_resource, (start), (n))

　　其中，参数start是I/O内存资源的起始物理地址（是CPU的RAM物理地址空间中的物理地址），参数n指定I/O内存资源的大小。

　　3．4．4 对/proc/ioports和/proc/iomem的支持

　　linux在ioport.h头文件中定义了两个宏：

　　get_ioport_list()和get_iomem_list()，分别用来实现/proc/ioports文件和/proc/iomem文件。其定义如下：


#define get_ioport_list(buf) get_resource_list(&ioport_resource, buf, PAGE_SIZE)
#define get_mem_list(buf) get_resource_list(&iomem_resource, buf, PAGE_SIZE)

3．5 访问I/O端口空间

　　在驱动程序请求了I/O端口空间中的端口资源后，它就可以通过CPU的IO指定来读写这些I/O端口了。在读写I/O端口时要注意的一点就是，大多数平台都区分8位、16位和32位的端口，也即要注意I/O端口的宽度。

　　linux在include/asm/io.h头文件（对于i386平台就是include/asm-i386/io.h）中定义了一系列读写不同宽度I/O端口的宏函数。如下所示：

　　⑴读写8位宽的I/O端口


　　unsigned char inb（unsigned port）；
　　void outb（unsigned char value，unsigned port）；

　　其中，port参数指定I/O端口空间中的端口地址。在大多数平台上（如x86）它都是unsigned short类型的，其它的一些平台上则是unsigned int类型的。显然，端口地址的类型是由I/O端口空间的大小来决定的。

　　⑵读写16位宽的I/O端口


　　unsigned short inw（unsigned port）；
　　void outw（unsigned short value，unsigned port）；

　　⑶读写32位宽的I/O端口


　　unsigned int inl（unsigned port）；
　　void outl（unsigned int value，unsigned port）；

　　3．5．1 对I/O端口的字符串操作

　　除了上述这些“单发”（single－shot）的I/O操作外，某些CPU也支持对某个I/O端口进行连续的读写操作，也即对单个I/O端口读或写一系列字节、字或32位整数，这就是所谓的“字符串I/O指令”（String Instruction）。这种指令在速度上显然要比用循环来实现同样的功能要快得多。

　　linux同样在io.h文件中定义了字符串I/O读写函数：

　　⑴8位宽的字符串I/O操作


　　void insb（unsigned port，void * addr，unsigned long count）；
　　void outsb（unsigned port ，void * addr，unsigned long count）；

　　⑵16位宽的字符串I/O操作


　　void insw（unsigned port，void * addr，unsigned long count）；
　　void outsw（unsigned port ，void * addr，unsigned long count）；

　　⑶32位宽的字符串I/O操作


　　void insl（unsigned port，void * addr，unsigned long count）；
　　void outsl（unsigned port ，void * addr，unsigned long count）；

　　3．5．2 Pausing I/O

　　在一些平台上（典型地如X86），对于老式总线（如ISA）上的慢速外设来说，如果CPU读写其I/O端口的速度太快，那就可能会发生丢失数据的现象。对于这个问题的解决方法就是在两次连续的I/O操作之间插入一段微小的时延，以便等待慢速外设。这就是所谓的“Pausing I/O”。

　　对于Pausing I/O，linux也在io.h头文件中定义了它的I/O读写函数，而且都以XXX_p命名，比如：inb_p()、outb_p()等等。下面我们就以out_p()为例进行分析。

　　将io.h中的宏定义__OUT(b,”b”char)展开后可得如下定义：


extern inline void outb(unsigned char value, unsigned short port) {
__asm__ __volatile__ ("outb %" "b " "0,%" "w" "1"
: : "a" (value), "Nd" (port));
}

extern inline void outb_p(unsigned char value, unsigned short port) {
__asm__ __volatile__ ("outb %" "b " "0,%" "w" "1"
__FULL_SLOW_DOWN_IO
: : "a" (value), "Nd" (port));
}

　　可以看出，outb_p()函数的实现中被插入了宏__FULL_SLOWN_DOWN_IO，以实现微小的延时。宏__FULL_SLOWN_DOWN_IO在头文件io.h中一开始就被定义：


#ifdef SLOW_IO_BY_JUMPING
#define __SLOW_DOWN_IO "
jmp 1f
1: jmp 1f
1:"
#else
#define __SLOW_DOWN_IO "
outb %%al,$0x80"
#endif

#ifdef REALLY_SLOW_IO
#define __FULL_SLOW_DOWN_IO __SLOW_DOWN_IO
　　__SLOW_DOWN_IO __SLOW_DOWN_IO __SLOW_DOWN_IO
#else
#define __FULL_SLOW_DOWN_IO __SLOW_DOWN_IO
#endif

　　显然，__FULL_SLOW_DOWN_IO就是一个或四个__SLOW_DOWN_IO（根据是否定义了宏REALLY_SLOW_IO来决定），而宏__SLOW_DOWN_IO则被定义成毫无意义的跳转语句或写端口0x80的操作（根据是否定义了宏SLOW_IO_BY_JUMPING来决定）。
3．6 访问I/O内存资源

　　尽管I/O端口空间曾一度在x86平台上被广泛使用，但是由于它非常小，因此大多数现代总线的设备都以内存映射方式（Memory－mapped）来映射它的I/O端口（指I/O寄存器）和外设内存。基于内存映射方式的I/O端口（指I/O寄存器）和外设内存可以通称为“I/O内存”资源（I/O Memory）。因为这两者在硬件实现上的差异对于软件来说是完全透明的，所以驱动程序开发人员可以将内存映射方式的I/O端口和外设内存统一看作是“I/O内存”资源。

　　从前几节的阐述我们知道，I/O内存资源是在CPU的单一内存物理地址空间内进行编址的，也即它和系统RAM同处在一个物理地址空间内。因此通过CPU的访内指令就可以访问I/O内存资源。

　　一般来说，在系统运行时，外设的I/O内存资源的物理地址是已知的，这可以通过系统固件（如BIOS）在启动时分配得到，或者通过设备的硬连线（hardwired）得到。比如，PCI卡的I/O内存资源的物理地址就是在系统启动时由PCI BIOS分配并写到PCI卡的配置空间中的BAR中的。而ISA卡的I/O内存资源的物理地址则是通过设备硬连线映射到640KB－1MB范围之内的。但是CPU通常并没有为这些已知的外设I/O内存资源的物理地址预定义虚拟地址范围，因为它们是在系统启动后才已知的（某种意义上讲是动态的），所以驱动程序并不能直接通过物理地址访问I/O内存资源，而必须将它们映射到核心虚地址空间内（通过页表），然后才能根据映射所得到的核心虚地址范围，通过访内指令访问这些I/O内存资源。

　　3．6．1 映射I/O内存资源

　　linux在io.h头文件中声明了函数ioremap（），用来将I/O内存资源的物理地址映射到核心虚地址空间（3GB－4GB）中，如下：


void * ioremap(unsigned long phys_addr, unsigned long size, unsigned long flags);
void iounmap(void * addr);

　　函数用于取消ioremap（）所做的映射，参数addr是指向核心虚地址的指针。这两个函数都是实现在mm/ioremap.c文件中。具体实现可参考《情景分析》一书。

　　3．6．2 读写I/O内存资源

　　在将I/O内存资源的物理地址映射成核心虚地址后，理论上讲我们就可以象读写RAM那样直接读写I/O内存资源了。但是，由于在某些平台上，对I/O内存和系统内存有不同的访问处理，因此为了确保跨平台的兼容性，linux实现了一系列读写I/O内存资源的函数，这些函数在不同的平台上有不同的实现。但在x86平台上，读写I/O内存与读写RAM无任何差别。如下所示（include/asm-i386/io.h）：


#define readb(addr) (*(volatile unsigned char *) __io_virt(addr))
#define readw(addr) (*(volatile unsigned short *) __io_virt(addr))
#define readl(addr) (*(volatile unsigned int *) __io_virt(addr))

#define writeb(b,addr) (*(volatile unsigned char *) __io_virt(addr) = (b))
#define writew(b,addr) (*(volatile unsigned short *) __io_virt(addr) = (b))
#define writel(b,addr) (*(volatile unsigned int *) __io_virt(addr) = (b))

#define memset_io(a,b,c) memset(__io_virt(a),(b),(c))
#define memcpy_fromio(a,b,c) memcpy((a),__io_virt(b),(c))
#define memcpy_toio(a,b,c) memcpy(__io_virt(a),(b),(c))

　　上述定义中的宏__io_virt()仅仅检查虚地址addr是否是核心空间中的虚地址。该宏在内核2.4.0中的实现是临时性的。具体的实现函数在arch/i386/lib/Iodebug.c文件。

　　显然，在x86平台上访问I/O内存资源与访问系统主存RAM是无差别的。但是为了保证驱动程序的跨平台的可移植性，我们应该使用上面的函数来访问I/O内存资源，而不应该通过指向核心虚地址的指针来访问。
