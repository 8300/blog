//----------------------gcc.c---------------------------
gcc: fatal error: no input files
挂载Windows文件不能修改为可执行属性
0.//-------------------- tool
readelf	: -a xxx 目标文件分析工具
objdump	: -s 显示目标信息
nm	://List symbols in [file(s)] (a.out by default).
strip	://Removes symbols and sections from files
0.//--------------------- pointer
unsigned char uu[100];
{&uu[49]}
(char *)(&uu[49])
char *p;
char a;
a=*p;
(int *)p;
(unsigned int *)(&uu[49]);
unsigned int b;
b=*(unsigned int *)(&uu[49]);
unsigned int c;
*(unsigned int *)(&uu[49])=c
0.//------------------- macro

问题三：#与##的作用？
答：#是把宏参数转化为字符串的运算符，##是把两个宏参数连接的运算符。
例如：
#define STR(arg) #arg 则宏STR(hello)展开时为”hello”
#define NAME(y) name_y 则宏NAME(1)展开时仍为name_y
#define NAME(y) name_##y 则宏NAME(1)展开为name_1
#define DECLARE(name, type) typename##_##type##_type，
则宏DECLARE(val, int)展开为int val_int_type
0.//-------------------- so
error:gnueabihf/bin/ld: cannot find -lhcnetsdk
solution:file lib/libhcnetsdk.so
lib/libhcnetsdk.so: ELF 32-bit LSB pie executable, Intel 80386, version 1 (SYSV), dynamically linked, stripped
target:arm != intel80386
0.//------------------- static
Static的用途主要有两个，一是用于修饰存储类型使之成为静态存储类型，二是用于修饰链接属性使之成为内部链接属性。
1.静态存储类型：
在函数内定义的静态局部变量，该变量存在内存的静态区，所以即使该函数运行结束，静态变量的值不会被销毁，函数下次运行时能仍用到这个值。
在函数外定义的静态变量——静态全局变量，该变量的作用域只能在定义该变量的文件中，不能被其他文件通过extern引用。
1. 内部链接属性
静态函数只能在声明它的源文件中使用。
0.//------------------- const
1.声明常变量，使得指定的变量不能被修改。
const int a = 5;/*a的值一直为5，不能被改变*/
const int b; b = 10;/*b的值被赋值为10后，不能被改变*/
const int *ptr; /*ptr为指向整型常量的指针，ptr的值可以修改，但不能修改其所指向的值*/
int *const ptr;/*ptr为指向整型的常量指针，ptr的值不能修改，但可以修改其所指向的值*/
const int *const ptr;/*ptr为指向整型常量的常量指针，ptr及其指向的值都不能修改*/
1.修饰函数形参，使得形参在函数内不能被修改，表示输入参数。
如int fun(const int a);或int fun(const char *str);
1.修饰函数返回值，使得函数的返回值不能被修改。
const char *getstr(void);使用：const *str= getstr();
const int getint(void); 使用：const int a =getint();
0.//------------------- volatile
volatile指定的关键字可能被系统、硬件、进程/线程改变，强制编译器每次从内存中取得该变量的值，而不是从被优化后的寄存器中读取。例子:硬件时钟;多线程中被多个任共享的变量等。
0.//------------------- extern
1.用于修饰变量或函数，表明该变量或函数都是在别的文件中定义的，提示编译器在其他文件中寻找定义。
extern int a;
extern int *p;
extern int array[];
extern void fun(void);
其中，在函数的声明带有关键字extern，仅仅是暗示这个函数可能在别的源文件中定义，没有其他作用。如：
头文件A：A_MODULE.h中包含
extern int func(int a, int b);
源文件A: A_MODULE.c中
#include “A_MODULE.h”
int func(int a, int b)
{
returna+b;
}
此时，展开头文件A_MODULE.h后，为
extern int func(int a, int b);/*虽然暗示可能在别的源文件中定义，但又在本文件中定义，所以extern并没有起到什么作用，但也不会产生错误*/
int func(int a, int b)
{
returna+b;
}
而源文件B：B_MODULE.c中，
#include “A_MODULE.h”
int ret = func(10,5);/
展开头文件A_MODULE.h后，为
extern int func(int a, int b);/*暗示在别的源文件中定义，所以在下面使用func(5,10)时，在链接的时候到别的目标文件中寻找定义*/
int ret = func(10,5);
c语言经典面试题目知识点详解（经典，非常详细）
1 .用于extern “c
extern “c”的作用就是为了能够正确实现C++代码调用其他C语言代码。加上extern "C"后，会指示编译器这部分代码按C语言的编译方式进行编译，而不是C++的。
C++作为一种与C兼容的语言，保留了一部分面向过程语言的特点，如可以定义不属于任何类的全局变量和函数，但C++毕竟是一种面向对象的语言，为了支持函数的重载，对函数的编译方式与C的不同。例如，在C++中，对函数void fun(int,int)编译后的名称可能是_fun_int_int，而C中没有重载机制，一般直接利用函数名来指定编译后函数的名称，如上面的函数编译后的名称可能是_fun。
这样问题就来了，如果在C++中调用的函数如上例中的fun(1,2)是用C语言在源文件a_module.c中实现和编译的，那么函数fun在目标文件a_module.obj中的函数名为_fun，而C++在源文件b_module.cpp通过调用其对外提供的头文件a_module.h引用后，调用fun，则直接以C++的编译方式来编译，使得fun编译后在目标文件b_module.obj的名称为_fun_int_int，这样在链接的时候，因为_fun_int_int的函数在目标文件a_module.obj中不存在，导致了链接错误。
解决方法是让b_module.cpp知道函数fun是用C语言实现和编译了，在调用的时候，采用与C语言一样的方式来编译。该方法可以通过extern “C”来实现（具体用法见下面）。一般，在用C语言实现函数的时候，要考虑到这个函数可能会被C++程序调用，所以在设计头文件时，应该这样声明头文件：
/*头文件a_module.h*/
/*头文件被CPP文件include时，CPP文件中都含有该自定义的宏__cplusplus*/
/*这样通过extern “C”告诉C++编译器，extern “C”{}里包含的函数都用C的方式来编译*/
#ifdef __cplusplus
extern “C”
{
#endif
extern void fun(int a, int b);
#ifdef __cplusplus
}
#endif
extern "C"的使用方式
1. 可以是单一语句
extern "C" doublesqrt(double);
1. 可以是复合语句, 相当于复合语句中的声明都加了extern "C"
extern "C"
{
double sqrt(double);
int min(int, int);
}
1.可以包含头文件，相当于头文件中的声明都加了extern"C"
extern "C"
{
#include <cmath>
}
1. 不可以将extern"C" 添加在函数内部
1. 如果函数有多个声明，可以都加extern"C", 也可以只出现在第一次声明中，后面的声明会接受第一个链接指示符的规则。
1. 除extern"C", 还有extern "FORTRAN" 等。
0.//------------------- struct
C语言中对结构体变量的赋值或者在初始化或者在定义后按字段赋值。
1.//初始化
2.
struct tag
{
chara;
int b;
}x = {‘A’, 1};/*初始化*/
2.
struct tag
{
char a;
int b;
};
struct tag x = {‘A’,1};/*在定义变量时初始化*/
2.
GNU C中可使用另外一种方式：
struct tag
{
char a;
int b;
}x =
{
.a = ‘A’,
.b =1;
};
2.
struct tag
{
char a;
int b;
};
struct tag x =
{
.a= ‘A’,
.b=1,
};
2.
struct tag
{
char a;
int b;
};
struct tag x;/*定义变量*/
x.a = ‘A’;/*按字段赋值*/
x.b = 1; /*按字段赋值*/
而当你使用初始化的方式来赋值时，如x = {‘A’,1};则出错。
2.
struct tag
{
chara;
int b;
};
struct tag x,y;
x.a=’A’;
x.b=1;
y = x;/*结构变量间直接赋值*/
1.//结构体变量如何比较？
答：虽然结构体变量之间可以通过=直接赋值，但不同通过比较符如==来比较，因为比较符只作用于基本数据类型。这个时候，只能通过int memcmp(const void *s1, const void *s2, size_t n);来进行内存上的比较。
1.//结构体位域
位域是一个或多个位的字段，不同长度的字段（如声明为unsigned int类型）存储于一个或多个其所声明类型的变量中（如整型变量中）。
位域的类型：可以是char、short、int，多数使用int，使用时最好带上signed或unsigned
位域的特点：字段可以不命名，如unsignedint :1;可用来填充；unsigned int :0; 0宽度用来强制在下一个整型（因此处是unsigned int类型）边界上对齐。
位域的定义：
struct st1
{
unsigned chara:7;/*字段a占用了一个字节的7个bit*/
unsigned charb:2;/*字段b占用了2个bit*/
unsigned charc:7;/*字段c占用了7个bit*/
}s1;
sizeof(s1)等于3。因为一个位域字段必须存储在其位域类型的一个单元所占空间中,不能横跨两个该位域类型的单元。也就是说，当某个位域字段正处于两个该位域类型的单元中间时，只使用第二个单元，第一个单元剩余的bit位置补（pad）0。
于是可知Sizeof(s2)等于3*sizeof(int)即12
struct st2
{
unsigned inta:31;
unsigned intb:2;/*前一个整型变量只剩下1个bit，容不下2个bit，所以只能存放在下一个整型变量*/
unsigned int c:31;
}s2;
位域的好处：
1.有些信息在存储时，并不需要占用一个完整的字节， 而只需占几个或一个二进制位。例如在存放一个开关量时，只有0和1 两种状态，用一位二进位即可。这样节省存储空间，而且处理简便。这样就可以把几个不同的对象用一个字节的二进制位域来表示。
2.可以很方便的利用位域把一个变量给按位分解。比如只需要4个大小在0到3的随即数，就可以只rand()一次，然后每个位域取2个二进制位即可，省时省空间。
位域的缺点：
不同系统对位域的处理可能有不同的结果，如位段成员在内存中是从左向右分配的还是从右向左分配的，所以位域的使用不利于程序的可移植性。
问题四：结构体成员数组大小为0
构体数组成员的大小为0是GNU C的一个特性。好处是可以在结构体中分配不定长的大小。如
typedef struct st

int a;
int b;
char c[0];
}st_t;
sizeof(st_t)等于8，即char c[0]的大小为0.
#define SIZE 100
st_t *s = (st_t *)malloc(sizeof(st_t) + SIZE);
0.//------------------- function
1.//函数参数入栈顺序
答：
C语言函数参数入栈顺序是从右向左的，这是由编译器决定的，更具体的说是函数调用约定决定了参数的入栈顺序。C语言采用是函数调用约定是__cdecl的，所以对于函数的声明，完整的形式是：int __cdecl func(int a, int b);
1.//inline内联函数
答：
inline关键字仅仅是建议编译器做内联展开处理，即是将函数直接嵌入调用程序的主体，省去了调用/返回指令。
c语言经典面试题目知识点详解（经典，非常详细）
0.//------------------- memory
1.//malloc/free与new/delete的区别
2. malloc与free是C/C++语言的标准库函数，new/delete是C++的运算符。它们都可用于申请动态内存和释放内存。
2. 对于非内部数据类型的对象而言，光用maloc/free无法满足动态对象的要求。对象在创建的同时要自动执行构造函数，对象在消亡之前要自动执行析构函数。由于malloc/free是库函数而不是运算符，不在编译器控制权限之内，不能够把执行构造函数和析构函数的任务强加于malloc/free。因此C++语言需要一个能完成动态内存分配和初始化工作的运算符new，以及一个能完成清理与释放内存工作的运算符delete。注意new/delete不是库函数。
我们不要企图用malloc/free来完成动态对象的内存管理，应该用new/delete。由于内部数据类型的“对象”没有构造与析构的过程，对它们而言malloc/free和new/delete是等价的。
2. 既然new/delete的功能完全覆盖了malloc/free，为什么C++不把malloc/free淘汰出局呢？这是因为C++程序经常要调用C函数，而C程序只能用malloc/free管动态内存。
如果用free释放“new创建的动态对象”，那么该对象因无法执行析构函数而可能导致程序出错。如果用delete释放“malloc申请的动态内存”，结果也会导致程序出错，但是该程序的可读性很差。所以new/delete必须配对使用，malloc/free也一样。
1.//malloc(0)返回值
答：如果请求的长度为0，则标准C语言函数malloc返回一个null指针或不能用于访问对象的非null指针，该指针能被free安全使用。
0.//------------------- parameter
可变参数列表是通过宏来实现的，这些宏定义在stdarg.h头文件，它是标准库的一部分。这个头文件声明了一个类型va_list和三个宏：va_start、va_arg和va_end。
typedef char *va_list;
#define va_start(ap, A) (void)((ap) = (char *)&(A) + _Bnd(A, _AUPBND))
#define va_arg(ap, T) (*(T )((ap) += _Bnd(T, _AUPBND)) - _Bnd(T, _ADNBND)))
#define va_end(ap) (void)0
int print(char *format, …)
宏va_start的第一个参数是va_list类型的变量，第二个参数是省略号前最后一个有名字的参数，功能是初始化va_list类型的变量，将其值设置为可变参数的第一个变量。
宏va_arg的第一个参数是va_list类型的变量，第二个参数是参数列表的下一个参数的类型。va_arg返回va_list变量的值，并使该变量指向下一个可变参数。
宏va_end是在va_arg访问完最后一个可变参数之后调用的。
问题一：实现printf函数
* A simple printf function. Only support the following format:
* Code Format
* %c character
* %d signed integers
* %i signed integers
* %s a string of characters
* %o octal
* %x unsigned hexadecimal
int my_printf( const char* format, ...)
{
	va_list arg;
	int done = 0;
	va_start (arg, format);
	while( *format != '') {
		if( *format == '%') {
			if( *(format+1) == 'c' ) {
				char c = (char)va_arg(arg, int);
				putc(c, stdout);
			} else if( *(format+1) == 'd' || *(format+1) == 'i') {
				char store[20];
				int i = va_arg(arg, int);
				char* str = store;
				itoa(i, store, 10);
				while( *str != '') putc(*str++, stdout);
			} else if( *(format+1) == 'o') {
				char store[20];
				int i = va_arg(arg, int);
				char* str = store;
				itoa(i, store, 8);
				while( *str != '') putc(*str++, stdout);
			} else if( *(format+1) == 'x') {
				char store[20];
				int i = va_arg(arg, int);
				char* str = store;
				itoa(i, store, 16);
				while( *str != '') putc(*str++, stdout);
			} else if( *(format+1) == 's' )	{
				char* str = va_arg(arg, char*);
				while( *str != '') putc(*str++, stdout);
			}
			// Skip this two characters.
			format += 2;
		} else {
			putc(*format++, stdout);
		}
	}
	va_end (arg);
	return done;
}
0.//------------------- ASSERT
答:ASSERT()是一个调试程序时经常使用的宏，在程序运行时它计算括号内的表达式，如果表达式为FALSE (0), 程序将报告错误，并终止执行。如果表达式不为0，则继续行后面的语句。这个宏通常原来判断程序中是否出现了明显非法的数据，如果出现了终止程序以免导致严重后果，同时也便于查找错误。例如，变量n在程序中不应该为0，如果为0可能导致错误，你可以这样写程序：
.....
ASSERT( n != 0);
k = 10/ n;
.....
ASSERT只有在Debug版本中才有效，如果编译为Release版本则被忽略。
assert()的功能类似，它是ANSI C标准中规定的函数，它与ASSERT的一个重要区别是可以用在Release版本中。
0.//------------------- system("pause");
答:系统的暂停程序，按任意键继续，屏幕会打印，"按任意键继续。。。。。"省去了使用getchar（）；

#error ----------------
1.symbol 'E2idE': can't resolve symbol
lib is error
1.malloc
必须在运算之前申请内存
eg:
error:
char *index = malloc();
strcat(index, "d")
char *buf = malloc
correct:
char *index= malloc
char *buf =malloc
strcat();
1.speed_t 数据丢失
一些数据类型不能直接赋值
1.float printf 
must %f
%x is error and change
//错题
试题一：
void test1()
{
char string[10];
char* str1 = "0123456789";
strcpy(string, str1);
}
答：字符串str1有11个字节（包括末尾的结束符''），而string只有10个字节，故而strcpy会导致数组string越界。
试题二：
void test2()
{
char string[10], str1[10];
int i;
for(i=0; i<10; i++)
{
str1= 'a';
}
strcpy(string, str1);
}
解答：因为str1没有结束符''，故而strcpy复制的字符数不确定。strcpy源码如下：
cpp] view plain copy
    #include <string.h>
    char *strcpy(char *s1, cosnt char *s2)
    {
    char *s = s1;
    for (s = s1; (*s++ = *s2++) != '';)/*最后的结束符''也会被复制*/
    ;
    return s1;
    }
试题三：
void test3(char* str1)
{
char string[10];
if(strlen(str1) <= 10 )
{
strcpy(string, str1);
}
}
解答：应修改为if (strlen(str1) < 10)，因为strlen的结果未统计最后的结束符''。strlen的源码如下：
[cpp] view plain copy
    #include <string.h>
   size_t strlen(const char *s)
    {
    const char *sc;
    for (sc = s; *sc != ''; ++sc)/*不包含最后的结束符''*/
    ;
    return (sc - s);
    }
试题四：
void GetMemory(char *p)
{
p = (char *)malloc( 100 );
}
void Test( void )
{
char *str = NULL;
GetMemory(str);
strcpy(str,"hello world");
printf(str);
}
解答：C语言中的函数参数为传值参数，在函数内对形参的修改并不能改变对应实参的值。故而调用GetMemory后，str仍为NULL。
试题五：
char *GetMemory( void )
{
char p[] = "hello world";
return p;
}
void Test( void )
{
char *str = NULL;
str = GetMemory();
printf(str);
}
解答：GetMemory中，p为局部变量，在函数返回后，该局部变量被回收。故而str仍为NULL
试题六：
oid GetMemory( char **p, int num )
{
*p = (char *)malloc(num);
}
void Test( void )
{
char *str = NULL;
GetMemory(&str, 100);
strcpy(str, "hello");
printf(str);
}
解答：试题6避免了试题4的问题，但在GetMemory内，未对*p为NULL情况的判断。当*p不为NULL时，在printf后，也未对malloc的空间进行free
试题七：
void Test( void )
{
r *str = (char *)malloc( 100 );
strcpy(str, "hello" );
free(str);
... //省略的其它语句
}
解答：未对str为NULL的情况的判断，在free(str)后，str未设置为NULL，可能变成一个野指针（后面对str的操作可能会导致踩内存）。

swap(int* p1,int* p2)
{
int *p;
*p = *p1;
*p1 = *p2;
*p2 = *p;
}
解答：在swap函数中，p是个野指针，可能指向系统区，导致程序运行的崩溃。故而，程序应改为：
swap(int* p1,int* p2)
{
int p;
p = *p1;
*p1 = *p2;
*p2 = p;
}
语言经典面试题目知识点详解（经典，非常详细）
.
读取INt型变量地址四个字节1&4交换，2与3交换

include "stdafx.h"
#include<stdio.h>
#include<string.h>
 
void swap(char **src,char**dst)
{
      char *temp = NULL;
      temp=*src;
      *src=*dst;
      *dst=temp;
    //  src++;dst++;    
}
 
int main(int argc, char* argv[])
{
    int i=55;
    // char a =*(char *)&i;
    char *p  = (char *)&i;
   char *p1 = (char *)&i+1;
    char *p2 = (char *)&i+2;
    char *p3 = (char *)&i+3;
    swap(&p,&p3);
    printf("%s\n",p);
    printf("%s\n",p3);
    return 0;
}

几种排序算法

include<IOSTREAM>
#include<string>
using namespace std;
 
void MaoPaoSort(int arr[],int n);
void QuickSort(int arr[], int first, int end);
void ChoiseSort(int arr[], int n);
void BinSearchSort(int arr[], int n, int nFind);
void BinSearchSort_DiGui(int arr[], int left, int right, int nFind);
void InsertSort(int arr[], int n);
 
void Swap(int &n1, int &n2);
void PrintData(int arr[],int n);

nt main(int, char **, char **)
{
int a[] ={1,3,5,7,8,4,9,2,6,0};
int n = sizeof(a)/sizeof(int);
// MaoPaoSort(a,n);
// InsertSort(a,n);
// ChoiseSort(a,n);
QuickSort(a,0,n-1);
PrintData(a,n);
cout<<endl;
// BinSearchSort(a, n, 7);
/ BinSearchSort_DiGui(a,0,n,7);
return 0;
}
// 快速排序，递归思想，和二分法一样
void QuickSort(int arr[], int first, int end)
{
   int i=first,j=end,temp=arr[first];
while(i<j)
{
while(i<j && arr[j]>=temp)
j--;
swap(arr[i],arr[j]);
while(i<j && arr[i]<=temp)
i++;
swap(arr[j],arr[i]);
}
 
if(first<i-1)
QuickSort(arr,first,i-1);
if(end>i+1)
QuickSort(arr,i+1,end);
}
 
// 选择排序 每次循环找到一个最大或者最小 和冒泡相似
void ChoiseSort(int arr[], int n)
{
for(int i=0; i< n-1; i++)

{

for(int j=i+1; j<n; j++)

{

if(arr[j] < arr[i])

swap(arr[i],arr[j]);

}

}

}

 

// 二分法查找

void BinSearchSort(int arr[], int n, int nFind)

{

    int Min = 0;

    int Max = n;

 

    while(Min <=Max)

    {

        int mid = (Min+Max)/2;

        if(arr[mid] < nFind)

            Min =mid +1;

        else if(arr[mid] >nFind)

            Max = mid -1;

         else

        {

            cout<<"Elemet At:" <<mid <<endl;

            return ;

          }

    }

}

 

// 二分法递归

void BinSearchSort_DiGui(int arr[], int left, int right, int nFind)

{

    int mid = (left+right)/2;


    if(arr[mid] <nFind)

         BinSearchSort_DiGui(arr, mid+1, right, nFind);

    else if(arr[mid] >nFind)

         BinSearchSort_DiGui(arr, left, mid-1, nFind);

    else

        cout<<"Elemet At:" <<mid <<endl;

}

 

// 直接插入排序

void InsertSort(int arr[], int n)

{

    for(int i =1; i<n; i++)

    {

        for(int j =0; j<i ; j++)

        {

            if(arr[i] < arr[j])

                swap(arr[i],arr[j]);

        }

    }

}


// 冒泡排序

void MaoPaoSort(int arr[],int n)

{

    for(int i=0; i<n; i++)

    {

        for(int j =0; j<n-1-i ; j++)

        {

            if(arr[j+1] > arr[j])

            swap(arr[j+1],arr[j]);

        }

    }

}

 

// 交换

void Swap(int &n1, int &n2)

{

    int temp;

    temp = n1;

    n1 = n2;

    n2 = temp;

}

 

// 打印

void PrintData(int arr[],int n)

{

    for(int i =0; i<n; i++)

    {

        cout<<arr[i]<<" ";

    }

}


3.

字符串颠倒：指出错误


int main()

{

    char* src = "hello,world";

    int len = strlen(src);

    char* dest = (char*)malloc(len+1);//要为\0分配一个空间

    char* d = dest;

    char* s = &src[len-1];//指向最后一个字符

    while( len-- != 0 )

    *d++=*s--;

    *d = 0;//尾部要加\0

    printf("%s\n",dest);

    free(dest);// 使用完，应当释放空间，以免造成内存汇泄露

    return 0;

}


4.

输入一个正整数，要求判断这个数是否是对称数（比如121，123321，1456541）。要求不能调用字符串库函数

#include <stdio.h>

#include <stdlib.h>

#include <memory.h>

 

int main(int argc, char* argv[])

{

     while(1)

     {

          unsigned int nInt = 0;

          scanf("%d",&nInt);


          if(0 == nInt)

               break;

 

          char szDigit[32];

          memset(szDigit,0,sizeof(szDigit));

          unsigned int nIntT = nInt;

  

          int nCount = 0;

          while(nIntT > 0 && nCount < sizeof(szDigit) - 1)

          {

               szDigit[nCount] = nIntT % 10;

               nIntT /= 10;

               nCount++;

          }

  

              bool bRel = true;

              for(int i = 0; i<nCount/2;i++)

              {

                   if(szDigit[i] != szDigit[nCount - i - 1])

                        bRel = false;

              }

  

              if(bRel)

                   printf("%d is Y\n",nInt);

              else

                   printf("%d is N\n",nInt);

     }

 

     system("pause");

     return 0;

}


5.

 atoi函数实现 


int   atoi(const   char   *s)

{

      char   *p   =   s;

      char   c;

      int   i   =   0;

      while(c=*p++)

      {

            if(c>='0'   &&   c<='9')

            {

                  i   =   i*10   +   (c-'0');

            }

            else

                  return   -1;                     //Invalid   string

      }

      return   i;

}

 

6.

itoa函数实现

 int main(int, char **, char **)

 {

     char temp[20]={0};

     int n =12345;

     int i =0;

     do

     {

         temp[i] = n%10+'0';

         n = n/10;

         i++;

     } while (n>0);

     cout<<temp<<endl;   // 这里输出反的为了输出正的，反向输出吧。。

     int n1 = strlen(temp);

     for(i=n1-1; i>=0; i--)

     {

         cout<<temp[i];

     }

     cout<<endl;

     return 0;

}

7.

strcpy函数实现


char *strcpy(char *strDest, const char *strSrc)

{

    assert((strDest!=NULL) && (strSrc !=NULL));  // 2分

    if(strDest ==strSrc)

         return strDest;


    char *address = strDest;                   // 2分

    while( (*strDest++ = * strSrc++) != ‘\0’ )    // 2分

       NULL ;

    return address ;                          // 2分

}

8.

strcmp函数实现


int strcmp (char *str1, char *str2)

{

    while ( *str1 && *str2 && *str1++ == *str2++ )

     ;

    if (*str1 > *str2) return 1;  /* string1 > string2 */

    else if (*str1 == *str2) return 0;  /* string1 == string2 */

    else return -1;   /* string1 < string 2 */

}

 

9.

strcat函数实现


char * strcat(char * dest, const char * src)

{

        char *tmp = dest;

 

        while (*dest)

                dest++;

        while ((*dest++ = *src++) != '\0')

                ;

 

        return tmp;

}

--------------------- 

版权声明：本文为CSDN博主「HarkerYX」的原创文章，遵循CC 4.0 by-sa版权协议，转载请附上原文出处链接及本声明。

原文链接：https://blog.csdn.net/yexiangCSDN/article/details/79723877


1、请填写bool , float,  指针变量 与“零值”比较的if  
语句。
提示：这里“零值”可以是0, 0.0 , FALSE  或者“空指针”
。例如int 变量n  与“零值”比较的if  语句为：
if ( n == 0 ) 
if ( n != 0 ) 
以此类推。

（1）请写出bool flag  与“零值”比较的if  语句：
【标准答案】if ( flag )    if ( !flag )
100 条经典 C语言笔试题目
（2）请写出float  x   与“零值”比较的if  语句：
【标准答案】 const float EPSINON = 0.00001;                    
if ((x >= - EPSINON) && (x <= EPSINON)                  
不可将浮点变量用“==” 或“！=” 与数字比较，应该设法
转化成“>=” 或“<=” 此类形式。
100 条经典 C语言笔试题目
（3）请写出char  *p   与“零值”比较的if  语句
【标准答案】 if (p == NULL)    if (p != NULL) 

 

 

2、以下为Linux下的32 位C 程序，请计算sizeof 的
值。
char  str[] = “Hello” ;                               
char   *p = str ;                                      
i nt    n = 10;                                                    
请计算
（1）sizeof (str ) =                  （2）s i zeof ( p ) =                 
（3）sizeof ( n ) =

【标准答案】

 

（1）6、（2）4 、（3 ）4

（4）void Func ( char str[100])                           
{                                          
…… ;                                                            
}                                          
请计算sizeof( str ) =                                       
（5）void * p = malloc( 100 );                                                
请计算sizeof ( p ) = 
【标准答案】（4）4、（5）4

 

4、用变量a 给出下面的定义
e) 一个有10个指针的数组，该指针是指向一个整型数
的;
f)  一个指向有10个整型数数组的指针;
g) 一个指向函数的指针，该函数有一个整型参数并返
回一个整型数;
h) 一个有10个指针的数组，该指针指向一个函数，该
函数有一个整型参数并返回一个整型数;
【标准答案】e)int *  a[10];      f)int (*a)[10]                                 
g)int (*a)(int);   h) int (*a[10])(int)

 

 

5、设有以下说明和定义：
typedef u nion {long i; int k[5]; char c;} DATE;
struct data { int cat; DATE cow; double dog;} too;
DATE max;
则语句printf("%d",sizeof(struct date)+sizeof(max)); 的
执行结果是：_____
【标准答案】DATE是一个union, 变量公用空间.  里面最
大的变量类型是int[5],  占用20个字节.  所以它的大小是
20
data 是一个struct,  每个变量分开占用空间.  依次为int4 + 
DATE20 + double8 = 32.
所以结果是20 + 32 = 52.
当然... 在某些16位编辑器下, int 可能是2字节,那么结果
是int2 + DATE10 + double8 = 20

 

 

6、请问以下代码有什么问题：
int main()
{
char a;
char *str=&a;
strcpy(str,"hello");
printf(str);
return 0;
}
【标准答案】没有为str分配内存空间，将会发生异常
问题出在将一个字符串复制进一个字符变量指针所指
地址。虽然可以正确输出结果，但因为越界进行内在
读写而导致程序崩溃。

 

 

 

 

7、请问以下代码有什么问题：
char* s="AAA";
printf("%s",s);
s[0]='B';
printf("%s",s);
有什么错？
【标准答案】"AAA" 是字符串常量。s是指针，指向这个
字符串常量，所以声明s的时候就有问题。
cosnt char* s="AAA";
然后又因为是常量，所以对是s[0] 的赋值操作是不合法
的。

 

 

 

 

8、int (*s[10])(int)  表示的是什么啊
【标准答案】int (*s[10])(int)  函数指针数组，每个指针
指向一个int func(intp aram) 的函数。

 

 

 

 

9、c和c++ 中的struct有什么不同？
【标准答案】c和c++ 中struct的主要区别是c中的struct
不可以含有成员函数，而c++ 中的struct可以。

c++ 中struct和class的主要区别在于默认的存取权限不同，
struct默认为public ，而class默认为private

 

 

 

 

10、void getmemory(char *p)
{
p=(char *) malloc(100);
strcpy(p,“hello world”);
}
int main( )
{
char *str=NULL;
getmemory(str);
printf(“%s/n”,str);
free(str);
return 0;
} 会出现什么问题？
【标准答案】程序崩溃，getmemory中的malloc 不能返回
动态内存，free （）对str操作很危险。

 

 

11、char szstr[10];
strcpy(szstr,"0123456789");
产生什么结果？为什么？
【标准答案】长度不一样，出现段错误。
100 条经典 C语言笔试题目
12、数组和链表的区别？
【标准答案】数组：数据顺序存储，固定大小；

链表：数据可以随机存储，大小可动态改变
 

 

13、void main()
{
char aa[10];
printf(“%d”,strlen(aa));
}                                          
会出现什么问题？打印结果是是多少？
【标准答案】sizeof()和初不初始化，没有关系，
strlen()和初始化有关，打印结果值未知。

 

 

 

 

14、给定结构struct A
{
char t:4;
char k:4;
unsigned short i:8;
unsigned long m;
}; 问sizeof(A) = ?
【标准答案】8

 

 

 

 

15、struct name1{
char str;
short x;
int num;
} ；求sizeof(name1)?
【标准答案】8

 

 

 

 

16、struct name2{
char str;
int num;
short x;
}; 求sizeof(name2)？
【标准答案】12

 

 

 

 

17、程序哪里有错误
wap( int* p1,int* p2 )
{
int * p;  //(int)malloc(4); is ok
*p = *p1;
*p1 = *p2;
*p2 = *p;
}

 

【标准答案】p 为野指针（指向一个已删除的对象或未申请访问受限内存区域的指针）

 

 

 

19、(void *)ptr 和(*(void**))ptr 的结果是否相同？其
中ptr为同一个指针。
【标准答案】(void *)ptr 和(*(void**))ptr 值是相同的

 

 

 

 

20、要对绝对地址0x100000赋值，我们可以用
(unsigned int*)0x100000 = 1234;
那么要是想让程序跳转到绝对地址是0x100000去执行
，应该怎么做？
【标准答案】*((void (*)( ))0x100000 ) ( );
首先要将0x100000强制转换成函数指针,即:
(void (*)())0x100000
然后再调用它:
*((void (*)())0x100000)();

 

 

 

 

22、关于内存的思考题（1）你能看出有什么问题？

https://blog.csdn.net/qq_17242957/article/details/50628309

void GetMemory(char *p) 
{ 
p = (char *)malloc(100); 
} 
void Test(void) 
{ 
char *str = NULL; 
GetMemory(str); 
strcpy(str, "hello world"); 
printf(str); 
} 
请问运行Test 函数会有什么样的结果？ 
答：程序崩溃。 
因为GetMemory 并不能传递动态内存， 
Test 函数中的 str 一直都是 NULL。 
strcpy(str, "hello world");将使程序崩 溃。---因为str没有空间

 

 

23、关于内存的思考题（2）你能看出有什么问题？

char *GetMemory(void) 
{ 
char p[] = "hello world";  //char *p="hello world"可以，p是局部变量，但是*p不是常量区的数据。
return p; 
} 
void Test(void) 
{ 
char *str = NULL; 
str = GetMemory(); 
printf(str); 
} 

 

字符串是不可直接返回的，因此没用所谓的返回"hello world"之类的说法，你只能返回字符串的首地址。
当你用char p[]方式定义时，系统在堆栈上创建一个临时数组，然后把hello world内容拷贝进去，因此当你返回p时，实际是返回那个临时数组的首地址。
改成指针以后，p实际指向的是一个常量字符串"hello world"，而这个字符串是在常量区永远存在的，不是临时变量。可以这么说，p是局部变量，但是*p不是

 


请问运行Test 函数会有什么样的结果？ 
答：可能是乱码。 
因为GetMemory 返回的是指向“栈内存” 
的指针，该指针的地址不是 NULL，但其原 
现的内容已经被清除，新内容不可知。 

 

 

 

25、关于内存的思考题（3）你能看出有什么问题？

void GetMemory2(char **p, int num) 
{ 
*p = (char *)malloc(num); 

printf("*p=%p\n", *p);//------------------地址1
} 
void Test(void) 
{ 
char *str = NULL; 
GetMemory(&str, 100); 

printf("str=%p\n", str); //------------------地址2 == 地址1
strcpy(str, "hello"); 
printf(str); 
} 
请问运行Test 函数会有什么样的结果？ 
答： 
（1）能够输出hello 
（2）内存泄漏       //没有释放

 

 

26、关于内存的思考题（4）你能看出有什么问题？

 

void Test(void) 
{ 
char *str = (char *) malloc(100); 
strcpy(str, “hello”); 
free(str); 
if(str != NULL) 
{ 
strcpy(str, “world”); 
printf(str); 
} 
} 
请问运行Test 函数会有什么样的结果？ 
答：篡改动态内存区的内容，后果难以预 
料，非常危险。 
因为free(str);之后，str 成为野指针， （free(str);之后要加上 str=NULL）

if(str != NULL)语句不起作用

 

1.为什么指针变量定义时一定要初始化？ 

    答:因为你首先要理解一点.内存空间不是你分配了才可以使用
    只是你分配了之后使用才安全,为什么要进行对他初始化呢
    因为,如果你没对他初始化,而引用这个指针并却其指向的内存进行修改
    因为指针未被初始化,所以指针所指向的也是随机的,他是个野指针,如果你引用指针,并修改这个指针所指向的内容,而如果这个指针所指向的内容恰好是另外一个程序的数据的话,你将其进行修改了,就会导致另外一个程序可能不能正常运行了.所以使用前一定要进行初始化

    2.指针变量初始化为NULL是什么意思？
    答:意思是说,强指针变量置空,初始化为NULL,使它不指向任何内容,这样引用她也不会出现上面的问题
     
    总之一点,记住在使用指针之前要对它进行初始化操作就可以了

27、关键字volatile有什么含意? 并给出三个不同的例
子。
【参考答案】一个定义为volatile的变量是说这变量可
能会被意想不到地改变，这样，编译器就不会去假设
这个变量的值了。精确地说就是，优化器在用到这个
变量时必须每次都小心地重新读取这个变量的值，而
不是使用保存在寄存器里的备份。下面是volatile变量
的几个例子：
1). 并行设备的硬件寄存器（如：状态寄存器）
2). 一个中断服务子程序中会访问到的非自动变量
(Non-automatic variables)
3). 多线程应用中被几个任务共享的变量

 

 

 

 

28、嵌入式系统经常具有要求程序员去访问某特定的
内存位置的特点。在某工程中，要求设置一绝对地址
为0x67a9的整型变量的值为0xaa66。编译器是一个纯
粹的ANSI编译器。写代码去完成这一任务。
【参考答案】这一问题测试你是否知道为了访问一绝对
地址把一个整型数强制转换（typecast ）为一指针是合
法的。这一问题的实现方式随着个人风格不同而不同
。典型的类似代码如下：
int * ptr;
ptr=  (int *)0x67a9;
*ptr = 0xaa55;

 

 

 

 

29、头文件中的ifndef/define/endif 干什么用？
【标准答案】防止该头文件被重复引用。

 

 

 

 

30、#include  <filename.h>    和#include  
“filename.h” 有什么区别？
【标准答案】对于#include  <filename.h>   ，编译器从
标准库路径开始搜索filename.h ;                                
对于#include  “filename.h” ，编译器从用户的工作路
径开始搜索filename.h 。

 

 

 

 

31、const   有什么用途？（请至少说明两种）
【标准答案】： （1）可以定义const   常量
（2）const  可以修饰函数的参数、返回值，甚至函数
的定义体。

被const  修饰的东西都受到强制保护，可
以预防意外的变动，能提高程序的健壮性。

 

 

 

 

32、static有什么用途？（请至少说明两种）
【标准答案】
1. 限制变量的作用域（static全局变量）；
2. 设置变量的存储域（static局部变量）。

 

 

 

 

33、堆栈溢出一般是由什么原因导致的？
【标准答案】没有回收垃圾资源。

 

 

 

 

34、如何引用一个已经定义过的全局变量？
【标准答案】可以用引用头文件的方式，也可以用
extern 关键字，如果用引用头文件方式来引用某个在
头文件中声明的全局变理，假定你将那个变量写错了
，那么在编译期间会报错，如果你用extern 方式引用
时，假定你犯了同样的错误，那么在编译期间不会报
错，而在连接期间报错。

 

 

 

 

35、全局变量可不可以定义在可被多个.C 文件包含的
头文件中？为什么？
【标准答案】可以，在不同的C 文件中以static形式来声
明同名全局变量。可以在不同的C文件中声明同名的全
局变量，前提是其中只能有一个C文件中对此变量赋初
值，此时连接不会出错。

https://blog.csdn.net/basstal/article/details/52275191

从技术上来说，声明不会有链接属性，因为声明不会在可执行映像中分配存储空间；因此，不存在链接器是否容许交叉引用那些存储空间的问题。

当把（所谓的）全局变量global定义为static时，由于static使定义的变量称为内部链接，所以在各个.c文件中，存在多个同名global但不同等的定义，每个翻译单元中的global维持自己的内存区域，此时链接器不会报告“符号被多重定义”错误。

 

此时，（所谓的）全局变量并没有达到一般意义上全局变量的效果，相当于每个翻译单元的局部变量。

36、队列和栈有什么区别？
【标准答案】队列先进先出，栈后进先出。

37、Heap与stack的差别。
【标准答案】Heap是堆，stack是栈。
Stack的空间由操作系统自动分配/释放，Heap上的空
间手动分配/释放。
Stack空间有限，Heap是很大的自由存储区
C 中的malloc 函数分配的内存空间即在堆上,C++中对
应的是new 操作符。
程序在编译期对变量和函数分配内存都在栈上进行,且
程序运行过程中函数调用时参数的传递也在栈上进行。

38、用宏定义写出swap（x，y），即交换两数。
【标准答案】
#define swap(x, y) (x)=(x)+(y);(y)=(x)–(y);(x)=(x)–(y);

39、写一个“标准”宏，这个宏输入两个参数并返回较
小的一个。
【标准答案】#define Min(X, Y) ((X)>(Y)?(Y):(X))// 结尾没有 ;

40、带参宏与带参函数的区别(至少说出5点)？
【标准答案】
                     带参宏                带参函数
处理时间             编译时                运行时
参数类型                 无               需定义
程序长度                变长               不变
占用存储空间           否                    是
运行时间           不占运行时间      调用和返回时占

 

实参如果是表达式容易出问题

#define S(r) r*r

area=S(a+b);第一步换为area=r*r;,第二步被换为area=a+b*a+b;

正确的宏定义是#define S(r) ((r)*(r))

（2）宏名和参数的括号间不能有空格

（3）宏替换只作替换，不做计算，不做表达式求解

（4）函数调用在编译后程序运行时进行，并且分配内存。宏替换在编译前进行，不分配内存

（5）宏的哑实结合不存在类型，也没有类型转换。

（6）宏展开使源程序变长，函数调用不会

（7）宏展开不占运行时间，只占编译时间，函数调用占运行时间（分配内存、保留现场、值传递、返回值。

 

41、关键字volatile有什么含意？
【标准答案】提示编译器对象的值可能在编译器未监测
到的情况下改变。

42、int main()
{
int x=3;
printf("%d",x);
return 1;
}
问函数既然不会被其它函数调用，为什么要返回1？
【标准答案】mian中，c标准认为0表示成功，非0表示
错误。具体的值是某中具体出错信息。

https://blog.csdn.net/menoy/article/details/54141585

https://blog.csdn.net/pangyemeng/article/details/78853671

 

 

43、已知一个数组tabl e ，用一个宏定义，求出数据的
元素个数。
【标准答案】
#define NTBL(table) (sizeof(table)/sizeof(table[0]))

 

44、A.c 和B.c两个c文件中使用了两个相同名字的
static变量,编译的时候会不会有问题?这两个static变量
会保存到哪里（栈还是堆或者其他的）?
【标准答案】static的全局变量，表明这个变量仅在本
模块中有意义，不会影响其他模块。
他们都放在静态数据区，但是编译器对他们的命名是
不同的。
如果要使变量在其他模块也有意义的话，需要使用
extern 关键字。

https://www.cnblogs.com/SarielTang/p/4501456.html  

关于Static、全局变量、局部变量、Extern之间的种种

 

45、static全局变量与普通的全局变量有什么区别？
【标准答案】 static全局变量只初使化一次，防止在其
他文件单元中被引用;

全局变量(外部变量)的说明之前再冠以static 就构成了静态的全局变量。

　　全局变量本身就是静态存储方式， 静态全局变量当然也是静态存储方式。 这两者在存储方式上并无不同。

　　这两者的区别在于非静态全局变量的作用域是整个源程序， 当一个源程序由多个源文件组成时，非静态的全局变量在各个源文件中都是有效的。 而静态全局变量则限制了其作用域， 即只在定义该变量的源文件内有效， 在同一源程序的其它源文件中不能使用它。由于静态全局变量的作用域局限于一个源文件内，只能为该源文件内的函数公用，因此可以避免在其它源文件中引起错误。 

static全局变量只初使化一次，防止在其他文件单元中被引用; 　 

46、static局部变量和普通局部变量有什么区别
【标准答案】static局部变量只被初始化一次，下一次
依据上一次结果值；

　把局部变量改变为静态变量后是改变了它的存储方式即改变了它的生存期。把全局变量改变为静态变量后是改变了它的作用域，限制了它的使用范围。  

　　static局部变量只被初始化一次，下一次依据上一次结果值

47、static函数与普通函数有什么区别？
【标准答案】static函数在内存中只有一份，普通函数在
每个被调用中维持一份拷贝

static函数与普通函数作用域不同,仅在本文件。只在当前源文件中使用的函数应该说明为内部函数(static修饰的函数)，内部函数应该在当前源文件中说明和定义。对于可在当前源文件以外使用的函数，应该在一个头文件中说明，要使用这些函数的源文件要包含这个头文件.

static函数在内存中只有一份，普通函数在每个被调用中维持一份拷贝

 

关于45-47 的参考文章

 

在C语言中，static的字面意思很容易把我们导入歧途，其实它的作用有三条。

（1）先来介绍它的第一条也是最重要的一条：隐藏。

当我们同时编译多个文件时，所有未加static前缀的全局变量和函数都具有全局可见性。为理解这句话，我举例来说明。我们要同时编译两个源文件，一个是a.c，另一个是main.c。

下面是a.c的内容

char a = 'A'; // global variable
void msg() 
{
    printf("Hello\n"); 
}

 下面是main.c的内容

int main(void)
{    
    extern char a;    // extern variable must be declared before use
    printf("%c ", a);
    (void)msg();
    return 0;
}

程序的运行结果是：

A Hello

你可能会问：为什么在a.c中定义的全局变量a和函数msg能在main.c中使用？前面说过，所有未加static前缀的全局变量和函数都具有全局可见性，其它的源文件也能访问。此例中，a是全局变量，msg是函数，并且都没有加static前缀，因此对于另外的源文件main.c是可见的。

如果加了static，就会对其它源文件隐藏。例如在a和msg的定义前加上static，main.c就看不到它们了。利用这一特性可以在不同的文件中定义同名函数和同名变量，而不必担心命名冲突。Static可以用作函数和变量的前缀，对于函数来讲，static的作用仅限于隐藏，而对于变量，static还有下面两个作用。

（2）static的第二个作用是保持变量内容的持久。存储在静态数据区的变量会在程序刚开始运行时就完成初始化，也是唯一的一次初始化。共有两种变量存储在静态存储区：全局变量和static变量，只不过和全局变量比起来，static可以控制变量的可见范围，说到底static还是用来隐藏的。虽然这种用法不常见，但我还是举一个例子。

#include <stdio.h>

int fun(void){
    static int count = 10;    // 事实上此赋值语句从来没有执行过
    return count--;
}

int count = 1;

int main(void)
{    
    printf("global\t\tlocal static\n");
    for(; count <= 10; ++count)
        printf("%d\t\t%d\n", count, fun());    
    
    return 0;
}

 程序的运行结果是：

global          local static

1               10

2               9

3               8

4               7

5               6

6               5

7               4

8               3

9               2

10              1

（3）static的第三个作用是默认初始化为0。其实全局变量也具备这一属性，因为全局变量也存储在静态数据区。在静态数据区，内存中所有的字节默认值都是0x00，某些时候这一特点可以减少程序员的工作量。比如初始化一个稀疏矩阵，我们可以一个一个地把所有元素都置0，然后把不是0的几个元素赋值。如果定义成静态的，就省去了一开始置0的操作。再比如要把一个字符数组当字符串来用，但又觉得每次在字符数组末尾加’\0’太麻烦。如果把字符串定义成静态的，就省去了这个麻烦，因为那里本来就是’\0’。不妨做个小实验验证一下。

#include <stdio.h>

int a;

int main(void)
{
    int i;
    static char str[10];

    printf("integer: %d;  string: (begin)%s(end)", a, str);

    return 0;
}

程序的运行结果如下

integer: 0; string: (begin)(end)

最后对static的三条作用做一句话总结。首先static的最主要功能是隐藏，其次因为static变量存放在静态存储区，所以它具备持久性和默认值0。

以上内容出自博客园Mr. Write之手，写的相当清晰易懂，存档方便复习。原文地址：http://www.cnblogs.com/dc10101/archive/2007/08/22/865556.html


全局变量(外部变量)的说明之前再冠以static 就构成了静态的全局变量
。全局变量本身就是静态存储方式，静态全局变量当然也是静态存储方
式。这两者在存储方式上并无不同。这两者的区别虽在于非静态全局变
量的作用域是整个源程序，当一个源程序由多个源文件组成时，非静态
的全局变量在各个源文件中都是有效的。而静态全局变量则限制了其作
用域，即只在定义该变量的源文件内有效，在同一源程序的其它源文
件中不能使用它。由于静态全局变量的作用域局限于一个源文件内，只
能为该源文件内的函数公用，因此可以避免在其它源文件中引起错误。
从以上分析可以看出，把局部变量改变为静态变量后是改变了它的存储
方式即改变了它的生存期。把全局变量改变为静态变量后是改变了它的
作用域，限制了它的使用范围。
static函数与普通函数作用域不同。仅在本文件。只在当前源文件中使
用的函数应该说明为内部函数(static)，内部函数应该在当前源文件中
说明和定义。对于可在当前源文件以外使用的函数，应该在一个头文件
中说明，要使用这些函数的源文件要包含这个头文件。

 

Extern与Static：

    首先，我要告诉你static与extern是一对“水火不容”的家伙，也就是说extern和static不能同时修饰一个变量；其次，static修 饰的全局变量声明与定义同时进行，也就是说当你在头文件中使用static声明了全局变量后，它也同时被定义了；最后，static修饰全局变量的作用域 只能是本身的编译单元，也就是说它的“全局”只对本编译单元有效，其他编译单元则看不到它,如:
    test1.h:
    #ifndef TEST1H
    #define TEST1H
    static char g_str[] = "123456"; 
    void fun1();
    #endif

    test1.cpp:
    #include "test1.h"
    
    void fun1()
    {
        cout << g_str << endl;
    }
    
    test2.cpp
    #include "test1.h"
    
    void fun2()
    {
        cout << g_str << endl;
    }
    
    以上两个编译单元可以连接成功, 当你打开test1.obj时，你可以在它里面找到字符串"123456", 同时你也可以在test2.obj中找到它们，它们之所以可以连接成功而没有报重复定义的错误是因为虽然它们有相同的内容，但是存储的物理地址并不一样， 就像是两个不同变量赋了相同的值一样，而这两个变量分别作用于它们各自的编译单元。
    也许你比较较真，自己偷偷的跟踪调试上面的代码,结果你发现两个编译单元（test1, test2）的g_str的内存地址相同，于是你下结论static修饰的变量也可以作用于其他模块，但是我要告诉你，那是你的编译器在欺骗你，大多数编 译器都对代码都有优化功能，以达到生成的目标程序更节省内存，执行效率更高，当编译器在连接各个编译单元的时候，它会把相同内容的内存只拷贝一份，比如上 面的"123456", 位于两个编译单元中的变量都是同样的内容，那么在连接的时候它在内存中就只会存在一份了， 如果你把上面的代码改成下面的样子，你马上就可以拆穿编译器的谎言:
    test1.cpp:
    #include "test1.h"
    
    void fun1()
    {
        g_str[0] = 'a';
        cout << g_str << endl;
    }

    test2.cpp
    #include "test1.h"
    
    void fun2()
    {
        cout << g_str << endl;
    }
    
    void main()
    {
        fun1(); // a23456
        fun2(); // 123456
    }
    
    这个时候你在跟踪代码时，就会发现两个编译单元中的g_str地址并不相同，因为你在一处修改了它，所以编译器被强行的恢复内存的原貌，在内存中存在了两份拷贝给两个模块中的变量使用。

    正是因为static有以上的特性，所以一般定义static全局变量时，都把它放在原文件中而不是头文件，这样就不会给其他模块造成不必要的信息污染，同样记住这个原则吧！

 

 

48、程序的局部变量存在于___ 中，全局变量存在于
____中，动态申请数据存在于___ 中。
【标准答案】程序的局部变量存在于栈(stack) 中，全局
变量存在于静态数据区中，动态申请数据存在于堆（
heap）中。

 

49、什么是预编译，何时需要预编译：
【标准答案】１、总是使用不经常改动的大型代码体
。
２、程序由多个模块组成，所有模块都使用一组标准
的包含文件和相同的编译选项。在这种情况下，可以
将所有包含文件预编译为一个预编译头。

https://blog.csdn.net/u011146511/article/details/69568830  C语言中的三大预编译功能

这三种预处理包括：宏定义、文件包含、条件编译。

 

50、用两个栈实现一个队列的功能？要求给出算法和
思路！
【参考答案】设2个栈为A,B, 一开始均为空.
入队:
将新元素push入栈A;
出队:
(1)判断栈B 是否为空；
(2)如果不为空，则将栈A中所有元素依次pop 出并
push到栈B；
(3)将栈B 的栈顶元素pop 出；

 

51、对于一个频繁使用的短小函数,在C 语言中应用什
么实现,在C++ 中应用什么实现?
【标准答案】c用宏定义，c++ 用inline

 

52、1. 用预处理指令#define  声明一个常数，用以表
明1年中有多少秒（忽略闰年问题）
【参考答案】#define SECONDS_PER_YEAR (60 * 60 
* 24 * 365)UL

 

 #define SECONDS_PER_YEAR (60 * 60 * 24 * 365)

printf("SECONDS_PER_YEAR=%ul\n", SECONDS_PER_YEAR);

 

 

53、Typedef 在C 语言中频繁用以声明一个已经存在的
数据类型的同义字。也可以用预处理器做类似的事。例
如，思考一下下面的例子：
#define dPS struct s *
typedef struct s * tPS;
以上两种情况的意图都是要定义dPS 和tPS 作为一个
指向结构s指针。哪种方法更好呢？（如果有的话）为
什么？
【参考答案】这是一个非常微妙的问题，任何人答对这个问题（正当的原
因）是应当被恭喜的。答案是：typedef 更好。思考下面的例子：
dPS p 1,p2;
tPS p3,p4;
第一个扩展为
struct s * p1, p2;
上面的代码定义p1为一个指向结构的指，p2为一个实际的结构，这也许
不是你想要的。第二个例子正确地定义了p3 和p4 两个指针。

 

 

 

 

54、在C++  程序中调用被C 编译器编译后的函数，
为什么要加extern “C”？
【标准答案】C++ 语言支持函数重载，C 语言不支持函
数重载。函数被C++ 编译后在库中的名字与 C 语言的
不同。假设某个函数的原型为：void foo(int x, int y); 
该函数被C 编译器编译后在库中的名字为_foo ，而
C++ 编译器则会产生像_foo_int_int之类的名字。 C++
提供了C 连接交换指定符号extern“C”来解决名字匹配
问题。

 

 

 

 

55、请简述以下两个for  循环的优缺点。

    for (i=0; i<N; i++)
    {
    	if (condition)
    		DoSomething();
    	else
    		DoOtherthing();
    }
     
     
    if (condition)
    {
    	for (i=0; i<N; i++)
    		DoSomething();
    }
    else
    {
    	for (i=0; i<N; i++)
    		DoOtherthing();
    }

 

【标准答案】

优点：程序简洁

缺点：多执行了N-1次逻辑判断，并且打断了循环“流水线”作业，使得编译器不能对循环进行优化处理，降低了效率。

优点：循环的效率高

缺点：程序不简洁

 

 

56、语句for(  ；1  ；) 有什么问题？它是什么意思？
【标准答案】死循环，和while(1)相同。

57、do……while和while……do有什么区别？
【标准答案】前一个循环一遍再判断，后一个判断以后
再循环。

 

 

 

 

58、请写出下列代码的输出内容
#include <stdio.h>
int main()
{
int a,b,c,d;
a=10;
b=a++; //b=10, a=11
c=++a; //c=12, a=12
d=10*a++;//d=120, a=13
printf("b，c ，d：%d，%d，%d"，b，c，d ）;
return 0;
}
【标准答案】10，12，120

 

 

 

 

59、unsigned char *p1;
unsigned long *p2;
p1=(unsigned char *)0x801000;
p2=(unsigned long *)0x810000;
请问p1+5= ;
p2+5= ;
【标准答案】0x801005、0x810020

unsigned char *p1;
unsigned long *p2;
p1=(unsigned char *)0x801000;
p2=(unsigned long *)0x810000;
printf("p1=%x, p1+5=%x\n", p1, p1+5);
printf("p2=%x, p2+5=%x\n", p1, p2+5);

p1=801000, p1+5=801005
p2=801000, p2+5=810014

 

 

 

 

60、main()
{
int a[5]={1,2,3,4,5};
int * ptr=(int*)(&a+1);
printf(“%d，%d”,*(a+1),*(ptr-1));
}
请问输出：
【标准答案】2,5

int a[5]={1,2,3,4,5};
int * ptr=(int*)(&a+1);  //此时a作为数组的首地址，+1的跨度为整个数组的长度，ptr指向数组a后面的空间地址。
printf(“%d，%d”,*(a+1),*(ptr-1)); //a+1代表a[1], *a[1]指向2,   (ptr-1)代码a数组的最后一个元素的地址。

a[0], a[1], a[2], a[3], a[4], ptr

 

printf("&a=%p\n", &a);
printf("&a[0]=%p\n", &a[0]);
printf("&a[1]=%p\n", &a[1]);
printf("&a[2]=%p\n", &a[2]);
printf("&a[3]=%p\n", &a[3]);
printf("&a[4]=%p\n", &a[4]);
printf("ptr=%p\n", ptr);


&a=0028FF10
&a[0]=0028FF10
&a[1]=0028FF14
&a[2]=0028FF18
&a[3]=0028FF1C
&a[4]=0028FF20
ptr=0028FF24

 

61、请问下面程序有什么错误?
 int a[60][250][1000],i,j,k;

  for(k=0;k<1000;k++)

   for(j=0;j<250;j++)

     for(i=0;i<60;i++)

      a[i][j][k]=0;


【标准答案】把循环语句内外换一下。

（编译的时候没错，运行的时候出错，但这个数组太大，如果放在栈中，还是会溢出，要作为全局变量）

 

https://www.cnblogs.com/ios8/p/ios-c-test.html  iOS求职之C语言面试题

 

 

62、以下是求一个数的平方的程序,请找出错误:
#define SQUARE(a)((a)*(a))
int a=5;
int b;
b=SQUARE(a++);
【标准答案】宏在预编译时会以替换的形式展开，仅仅
会替换。涉及到宏的地方，不要用++-- ，标准中对此
没有规定，因此最终结果将会依赖于不同的编译器。
执行程序的答案可能是25、也有可能是36。

dev-c++，windows下为b=25，a=7

 

 

 

 

63、#define Max_CB 500
void LmiQueryCSmd(StructMSgCB * pmsg)
{
unsigned char ucCmdNum;
......  
for(ucCmdNum=0;ucCmdNum<Max_CB;ucCmdN
um++)
{
......;
}                                          
} 这段代码执行有什么问题？
【标准答案】死循环
unsigned char //无符号字符型表示范围0~255
char // 有符号字符型 表示范围-128~127

 

 

 

 

64、嵌入式系统中经常要用到无限循环，你怎么用C
编写死循环。
【标准答案】while(1){}或者for(;;)
100 条经典 C语言笔试题目
65、程序输出结果是？
【标准答案】8 ，10，12，14，16

 

 

 

 

66、
【标准答案】16

 

 

 

 

67、int modifyvalue()
{ 
return(x+=10);
}
int changevalue(int x )
{
return(x+=1);
}
void m ain()
{
int x =10;
x++; //x=11
changevalue(x);
x++; //x=12
modifyvalue();
printf("First output:%d\n",x);
x++;//x=13
changevalue(x);
printf("Second output:%dn",x);
modifyvalue();
printf("Thirdoutput:%dn",x);
}输出?
【标准答案】12、13、13

 

 

 

 

68、不能做switch()的参数类型是：
【标准答案】switch 的参数不能为实型。

得是整型

 

 

 

 

69、请写出下列代码的输出内容
＃i nclude<stdio.h>
main()
{
int a,b,c,d;
a=10;
b=a++; //b=10, a=11
c=++a; //c=12, a=12
d=10*a++;//d=120, a = 13
printf("b，c ，d：%d，%d，%d"，b，c，d ）;
return 0;
}
【标准答案】10，12，120

 

 

 

 

70、找出程序的错误。
【标准答案】

 

 

 

 

71、一语句实现x是否为2 的若干次幂的判断。
【参考答案】 void mai n()                                                
{                                          
i nt a;                                                         
scanf(“%d”,&a);                                    
printf(“%c”,(a)&(a-1)?’n’:’y’); //   若是打印y，否则n          
}

 

 

 

 

72、中断是嵌入式系统中重要的组成部分，这导致了很多编译开发商提
供一种扩展—让标准C 支持中断。具代表事实是，产生了一个新的关键字
__interrupt 。下面的代码就使用了__interrupt 关键字去定义了一个中断服
务子程序(ISR)，请评论一下这段代码的。
__interrupt double compute_area (double radius)
{
double area = PI * radius * radius;
printf(" Area = %f", area);
return area;
}
【参考答案】这个函数有太多的错误了，以至让人不知从何说起了：
1). ISR 不能返回一个值。如果你不懂这个，那么你不会被雇用的。
2). ISR 不能传递参数。如果你没有看到这一点，你被雇用的机会等同第
一项。
3). 在许多的处理器/编译器中，浮点一般都是不可重入的。有些处理器/编
译器需要让额处的寄存器入栈，有些处理器/编译器就是不允许在ISR 中做
浮点运算。此外，ISR 应该是短而有效率的，在ISR 中做浮点运算是不明
智的。
4). 与第三点一脉相承，printf() 经常有重入和性能上的问题。如果你丢掉
了第三和第四点，我不会太为难你的。不用说，如果你能得到后两点，那
么你的被雇用前景越来越光明了。

 

 

 

 

73、下面的代码输出是什么，为什么？
void foo(void)
{
unsigned int a = 6;
int b = -20;
(a+b> 6)? puts("> 6") : puts("<= 6");
}
【参考答案】这个问题测试你是否懂得C 语言中的整数自动转换原则，
我发现有些开发者懂得极少这些东西。不管如何，这无符号整型问题的答
案是输出是“>6” 。原因是当表达式中存在有符号类型和无符号类型时所有
的数都自动转换为无符号类型。因此-20 变成了一个非常大的正整数，所
以该表达式计算出的结果大于6 。这一点对于应当频繁用到无符号数据类
型的嵌入式系统来说是丰常重要的。如果你答错了这个问题，你也就到了
得不到这份工作的边缘。

printf("b=%u\n", b);  //b=4294967276

a+b = 6+(-20) = 6 + 4294967276 = 4294967282

 

 

74、评价下面的代码片断：
unsigned int zero = 0;
unsigned int compzero =  0xFFFF;
/*1‘s complement of zero */
【参考答案】对于一个int型不是16位的处理器为说，上面的代码是不正
确的。应编写如下：
unsigned int compzero = ~0;
这一问题真正能揭露出应试者是否懂得处理器字长的重要性。在我的经
验里，好的嵌入式程序员非常准确地明白硬件的细节和它的局限，然而
PC机程序往往把硬件作为一个无法避免的烦恼。
到了这个阶段，应试者或者完全垂头丧气了或者信心满满志在必得。如
果显然应试者不是很好，那么这个测试就在这里结束了。但如果显然应
试者做得不错，那么我就扔出下面的追加问题，这些问题是比较难的，
我想仅仅非常优秀的应试者能做得不错。提出这些问题，我希望更多看
到应试者应付问题的方法，而不是答案。不管如何，你就当是这个娱乐
吧…

unsigned int zero = 0;
unsigned int compzero1 = 0xFFFF;
unsigned int compzero2 = ~0;
printf("compzero1=%u\n", compzero1); 
printf("compzero2=%u\n", compzero2); 

compzero1=65535
compzero2=4294967295

 

 

75、下面的代码片段的输出是什么，为什么？
char *ptr;
if ((ptr = (char *)malloc(0)) == NULL)
puts("Gota null pointer");
else
puts("Gota valid pointer");
【参考答案】这个你可以先大胆猜测下，然后再用你的
编译器尝试着编译下~~

char cc='a';
char *ptr;
ptr = &cc;
printf("ptr=%p, *ptr=%c\n", ptr,*ptr);

if ((ptr = (char *)malloc(0)) == NULL)
    puts("Gota null pointer");
else
    puts("Gota valid pointer");
    
printf("ptr=%p, *ptr=%c\n", ptr,*ptr);

ptr=0028FF2F, *ptr=a
Gota valid pointer
ptr=00381720, *ptr=`

malloc(0)后，ptr地址发生变更

 

100 条经典 C语言笔试题目
欢迎进入C语言程序笔试面试，编写程序代码单元。

76、编写strcpy 函数
已知strcpy 函数的原型是 char *strcpy(char *strDest, 
const char *strSrc);其中strDest是目的字符串，
strSrc 是源字符串。
（1）不调用C++/C 的字符串库函数，请编写函数
strcpy 。
（2）strcpy 能把 strSrc 的内容复制到strDest，为什
么还要char *  类型的返回值？

https://blog.csdn.net/okawari_richi/article/details/57411796 strcpy()函数详解

    char * strcpy(char *dst,const char *src)   
    {
        if((dst==NULL)||(src==NULL))
             
               return NULL; 
     
        char *ret = dst; //[1]
     
        while ((*dst++=*src++)!='\0'); //[2]
     
        return ret;//[3]
    }

（1）const 修饰：源字符串参数用const修饰，防止修改源字符串；

（2）空指针检查：源指针和目的指针都有可能会出现空指针的情况，所以应该对其进行检查；

（3）为什么要设置ret 指针以及返回ret指针的位置[3]，由于目的指针dst已经在进行移动了，所以用辅助指针ret表明首指针；

（4）以上所示[2]处，为简单的字符串的复制过程，正好表明strcpy函数遇到'\0'将会停止；

 

 

77、写出二分查找的代码。
【参考答案】 

    int binary_search(int* arr, int key, int n)
     {
        int low=0;
        int mid;
        int high=n-1;
        while(low<=high)
        {
            mid = (low+high)/2;
            if(key < arr[mid])
            {
                high=mid-1;
            }
            else if(key>arr[mid])
            {
                low=mid+1;
            }
            else
            {
                return mid;
            }
        }
        return -1;
     }
     
    int main(int argc, char *argv[])
    {
        if(1)
        {
            int it[] = {0,1,2,3,4,5,6,7};
            int index = binary_search(it, 7, 8);
            printf("index=%d\n", index); //index=7
        }
     
        return 0;
    }

 

 

78、请编写一个C 函数，该函数给出一个字节中被置
1  的位的个数。
【参考答案】
 

    unsigned int TestAsOne1(char log)
    {
        int i;
        unsigned int num=0, val;
        for(i=0; i<8; i++)
        {
            val = log >> i; // 移位
            val &= 0x01; // 与1 相与
            if(val)
                num++;
        }
        return num;
    }

 

 

 

79、请编写一个C 函数，该函数将给定的一个字符串
转换成整数。
【参考答案】
int Invert(char* str)  //此实现不能处理负数和非数字的字符串
{ 
int num =0; 
while(*str!='\0') 
{ 
int d igital=*str-48; 
num=num*10+digital; 
str=str+1; 
} 
return num; 
} 

    int Invert2(char* str)
    {
        if(str==NULL)
            return -1;
            
        int num=0, negFlag=0;
        if(str[0] == '-')
        {
            negFlag=1;
            str++;
        }
        while(*str!='\0')
        {
            if(*str>='0' && *str<='9')
            {
                num = num*10 + (*str-'0');
            }
            else
            {
                num=-1;
                break;
            }
            str++;
        }
        
        if(negFlag)
            num *= -1;
            
        return num;
    }
    int main(int argc, char *argv[])
    {
        if(1)
        {
            char *str1="1234";
            int str_i1= Invert2(str1);
            printf("str1=%s, str_i1=%d\n", str1, str_i1);
            char *str2="-1234";
            int str_i2= Invert2(str2);
            printf("str2=%s, str_i2=%d\n", str2, str_i2);
            char *str3="-12s4";
            int str_i3= Invert2(str3);
            printf("str3=%s, str_i3=%d\n", str3, str_i3);
        }
        return 0;
    }
    /*
    str1=1234, str_i1=1234
    str2=-1234, str_i2=-1234
    str3=-12s4, str_i3=1
    */

 

 

 

80、请编写一个C 函数，该函数将给定的一个整数转
换成字符串。
【参考答案】
void IntToCharChange(int num,  char* pval)  //此实现，转换负数有问题
{ 
char strval[100]; 
int i , j; 
int val0 = 0; 
int val1 = 0; 
val0 = num; 
for(i=0; i<100; i++) 
{ 
val1 = val0 % 10; //取余
val0 = val0 / 10; // 取整
strval[i] = val1 + 48;  // 数字—字符
if(val0 < 10) 
{ 
i++; 
strval[i] = val0 + 48; 
break; 
} 
} 
for(j=0; j<=i; j++)  // 倒置
pval[j] = strval[i-j]; 
pval[j] = '\0'; 
}

    void IntToCharChange1(int num, char* pval)
    {
        //判断正负号 
        char negFlag=0;
        if(num<0)
        {
            negFlag=1;
        }
        
        //计算字符串长度 
        int len=0;
        int tmp_num=num;
        if(negFlag)
        { 
            tmp_num *= -1;
            len++;
        } 
        while(tmp_num>0)
        {
            len++;
            tmp_num /= 10;
        }  
        cout << "len=" << len << endl;
        
        //转成字符串 
        tmp_num = num; 
        if(negFlag)
        { 
            tmp_num *= -1;
            pval[0] = '-';
        } 
        
        int val;
        int i=0;
        while(tmp_num>0)
        {
            val = tmp_num%10;
            pval[len-i-1]= val+'0';
            tmp_num=tmp_num/10; 
            i++;
        }
        pval[len] = '\0';
    } 
     
    int main(int argc, char *argv[])
    {
        if(1)
        {
            int num1 = 1234;
            char *str1;
            IntToCharChange1(num1, str1);
            cout << str1 << endl;
            
            int num2 = -1234;
            char *str2;
            IntToCharChange1(num2, str2);
            cout << str2 << endl;
        }
        return 0;
    }
     
    /*
    len=4
    1234
    len=5
    -1234
    */

 

 

 

81、实现strcmp 函数。
【参考答案】
int m ystrcmp(const c har* str1, const char* str2)
{
assert((str1 != NULL) && (str2 != NULL));
int r et = 0;
while (!(ret = *(unsigned char*)str1 - * (unsigned char*)str2) && *str2)
{
str1++;
str2++;
}
if (ret > 0)
ret = 1;
else if (ret < 0)
ret = -1;
return ret;
}

    //str1和str2可以是字符串常量或者字符串变量，返回值为整形。返回结果如下规定：
    //①str1小于str2，返回负值或者-1（VC返回-1）；②str1等于str2，返回0；
    //③str1大于str2，返回正值或者1（VC返回1）；
    int mystrcmp(const char* str1, const char* str2)
    {
        assert((str1 != NULL) && (str2 != NULL));
        int ret=0;
        while((ret=*(unsigned char*)str1-*(unsigned char*)str2)==0 && *str2)
        {
            str1++;
            str2++;
        } 
        
        if (ret > 0)
            ret = 1;
        else if (ret < 0)
            ret = -1;
            
        return 0;
    } 
    int main(int argc, char *argv[])
    {
        if(1)
        {
            char *str1="hellod";
            char *str2="hello";
            int res = mystrcmp1(str1, str2);
            cout << res << endl;
        }
        return 0;
    }

 

 

 

82、请编写一个C 函数，该函数将一个字符串逆序。
【参考答案】
void AntitoneValue(cha r* father, char* child) 
{ 
int i ; 
char source[100]; 
int j = 0; 
while(father[j]) //放入source ，[j] 为长度
{ 
source[j] = father[j]; 
j++; 
if(j > 99) 
return; 
} 
source[j] = '\0'; 
for(i=0; i<j; i++) 
child[i] = source[j-i-1];  // 反序
child[i] = '\0'; 
} 

    void AntitoneValue(char* father, char* child)
    {
        if(father == NULL)
            return;
     
        int len = strlen(father);
        int i;
        for(i=0; i<len; i++)
        {
            child[i]=father[len-i-1];
        } 
        child[len]='\0';
    }
    int main(int argc, char *argv[])
    {
        if(1)
        {
            char *str1="dongj1223";
            char str2[100];
            AntitoneValue(str1, str2);
            cout << str2 << endl;
        }
        return 0;
    }
    //3221jgnod

 

 

 

83、请编写一个C 函数，该函数在给定的内存区域搜
索给定的字符，并返回该字符所在位置索引值。
【参考答案】
int s earch(char* cpSource, intn , char ch)  // 起始地址，搜索长度，目标字符
{
int i;
for(i=0; i<n && *(cpSource+i) != ch; ++i);
return i;
}

    int search(char* cpSource, int n , char ch) // 起始地址，搜索长度，目标字符
    {
        int res; 
        int i;
        for(i=0; i<n && *(cpSource+i)!=ch; i++)
        {   
            ;   //不相等，则继续；相等则退出；超出范围也退出；
        } 
        
        if(i==n)
            res = -1; //考虑找不到的情况，返回-1
        else
            res = i;  //找到，则返回索引值
        
        return res;
    }

 

 

 

84、请编写一个C 函数，该函数在一个字符串中找到
可能的最长的子字符串，该字符串是由同一字符组成
的。
【参考答案】
int C hildString(char*p)     // 自己写
{   
char *q =p;
int s tringlen=0, i=0,j=1,len=0,maxlen=1;   
while(*q!=’\0’)          //不能用strlen, 求得长度stringlen
{
Stringlen++;
q++;
}
while( i<  String len )    
{   
if(*(p+i)==*(p+j)& & j< St ri ngle n )   
{   
len++;                    // 统计子串长度
i++;
j++;   
}   
else   
{   
if(len>maxlen)           // 统计最大子串长度
{   
maxlen=len+1;   
len=0;
}  
else
len=0;
i++;
j++;
}   
}  
retu rn   ma xlen;    
}

    int ChildString(char*p)   // 自己写
    { 
    	char *q = p;
    	int strlen = 0; 
    	while(*q != '\0')     //不能用strlen, 求得长度stringlen
    	{
    		strlen++;
    		q++;
    	}
    	
    	int i, sublen=1, maxlen=0; 
    	for(i=0; i<strlen; i++)  
    	{ 
    		if(*(p+i)==*(p+i+1) && (i+1<strlen)) 
    		{ 
    			sublen++;   // 统计子串长度
    		} 
    		else 
    		{ 
    			if(sublen > maxlen)      // 统计最大子串长度
    			{ 
    				maxlen = sublen;   
    			}            
    			sublen = 1;
    		} 
    	} 
    	return maxlen;  
    }
    int main(int argc, char *argv[])
    {
        if(1)
        {
            char *str = "hellllo";
            int len = ChildString(str);
            cout << len << endl;
        }
        return 0;
    }

 

 

 

85、华为面试题：怎么判断链表中是否有环？
【参考答案】答：用两个指针来遍历这个单向链表，第
一个指针p1，每次走一步；第二个指针p2，每次走两
步；当p2 指针追上p1的时候，就表明链表当中有环
路了。
int testLinkRing(Link *head)
{
Link *t1=head,*t2=head;
while( t1->next && t2->next)
{
t1 = t1->next;
if (NULL == (t2 = t2->next->next))
return 0; // 无环
if (t1 == t2)
return 1;
}
return 0;
}

    int testLinkRing1(Link *head)
    {
        Link *t1=head,*t2=head;
        while( t1->next && t2->next)
        {
            t1 = t1->next;
            if (NULL == (t2 = t2->next->next))
                return 0; // 无环
            if (t1 == t2)
                return 1;
        }
        return 0;
    }
    int testLinkRing2(Link *head)
    {
        Link *slow=head, *fast=head;
        while(fast->next && fast->next->next)
        {
            slow=slow->next;
            fast=fast->next->next;
            if(slow==fast) //相遇则有环 
                return 1;
        }
        return 0;
    } 
     
    Link *getLinkRingNode(Link *head)
    {
        Link *slow=head, *fast=head;
        while(fast->next && fast->next->next)
        {
            slow=slow->next;
            fast=fast->next->next;
            if(slow==fast) //相遇则有环 
            {
                printf("slow's val = %d\n", slow->val);
                break;
            }
        }
        
        if(fast->next==NULL || fast->next->next==NULL)
            return NULL;
        
        slow=head;
        while(slow != fast)
        {
            slow=slow->next;
            fast=fast->next;
        }
        
        
        return slow;
    } 
     
    int main(int argc, char *argv[])
    {
        if(1)
        {
            Link *node1 = (Link*)malloc(sizeof(Link));
            node1->val=1;
            node1->next=NULL;
            
            Link *node2 = (Link*)malloc(sizeof(Link));
            node2->val=2;
            node2->next=NULL;
            
            Link *node3 = (Link*)malloc(sizeof(Link));
            node3->val=3;
            node3->next=NULL;
            
            Link *node4 = (Link*)malloc(sizeof(Link));
            node4->val=4;
            node4->next=NULL;
            
            node1->next=node2;
            node2->next=node3;
            node3->next=node4;
            node4->next=node2;
            
            int res1 = testLinkRing1(node1); 
            int res2 = testLinkRing2(node1); 
            printf("testLinkRing1=%d, testLinkRing2=%d\n", res1, res2); 
     
            Link *ring_node = getLinkRingNode(node1); 
            if(ring_node) 
               printf("ring_node->val=%d\n", ring_node->val); 
            else
               printf("ring_node is null\n");  
            
        }
        return 0;
    }
     
    /*
    testLinkRing1=1, testLinkRing2=1
    slow's val = 4
    ring_node->val=2
    */

 

 

 

86、有一浮点型数组A, 用C 语言写一函数实现对浮点
数组A进行降序排序,并输出结果,要求要以数组A 作为
函数的入口.( 建议用冒泡排序法)
【参考答案】
void BubbleSort(double a rr[], int n) 
{ 
int i，j ；
int e xchange = 1 ；// 交换标志
for(i=1;i<n;i++)
{ // 最多做n-1趟排序
exchange=0 ；// 本趟排序开始前，交换标志应为假
for(j=n-1;j>=i ；j--) //对当前无序区R[i..n]自下向上扫描
if(arr[j+1] > arr [ j])
{// 交换记录
arr[0]=arr[j+1]；//R[0]不是哨兵，仅做暂存单元
arr[j+1]=arr[j]；
arr[j]=arr[0]；
exchange=1 ；// 发生了交换，故将交换标志置为真
} 
if(!exchange) // 本趟排序未发生交换，提前终止算法
return ；
} //endfor(外循环) 
}

    void BubbleSort(double arr[], int n)
    {
        int i,j;
        int swapFlag=0;
        for(i=0; i<n-1; i++)
        {
            swapFlag=0;
            for(j=0; j<n-i-1; j++)
            {
                if(arr[j]< arr[j+1])
                {
                    double temp=arr[j];
                    arr[j]=arr[j+1];
                    arr[j+1]=temp;
                    swapFlag=1;
                }
            }
            if(swapFlag==0)
                break;
        }
    } 
     
    int main(int argc, char *argv[])
    {
        if(1)
        {
            double arr[5]={3.1,2.3,5.6,4.1,1.2};
            BubbleSort(arr, 5);
            int i=0;
            for(i=0; i<5; i++)
            {
                cout << arr[i] << "  ";
            }
            cout << endl; 
        }
        return 0;
    }

 

 

    void merge(double arr[], int start, int center, int end, int n)
    {
        int mid=center+1;
        double tmp_arr[n];
        printf("start=%d, end=%d, length=%d\n", start, end, n);
        //double *tmp_arr=(double*)malloc(5 * sizeof(double)); 
        int start_backup=start;
        int tmp_index=start;
        while(start<=center && mid<=end)
        {
            if(arr[start] >= arr[mid])
            {
                tmp_arr[tmp_index++] = arr[start++];
            }
            else
            {
                tmp_arr[tmp_index++] = arr[mid++];
            }
        }
        while(start<=center)
        {
            tmp_arr[tmp_index++] = arr[start++];
        }
        while(mid<=end)
        {
            tmp_arr[tmp_index++] = arr[mid++];
        }
        
        for(tmp_index=start_backup; tmp_index<=end; tmp_index++)
        {
            arr[tmp_index] =  tmp_arr[tmp_index]; 
        }
       //while(start_backup<=end)
       //{
       //arr[start_backup] =  tmp_arr[start_backup++];
       //}
    }
    void sort(double arr[], int start, int end, int n)
    {
        if(start<end)
        {
            int center = (start+end)/2;
            sort(arr, start, center, n);
            sort(arr, center+1, end, n);
            merge(arr, start, center, end, n);
        }
    }
    void mergeSort(double arr[], int n)
    {
        int start=0;
        int end=n-1;
        sort(arr, start, end, n);    
    }
     
    int main(int argc, char *argv[])
    {
        if(1)
        {
            double arr[5]={3.1,2.3,5.6,4.1,1.2};
            //BubbleSort(arr, 5);
            mergeSort(arr, 5);
            int i=0;
            for(i=0; i<5; i++)
            {
                cout << arr[i] << "  ";
            }
            cout << endl; 
        }
        return 0;
    }
    /*
    start=0, end=1, length=5
    start=0, end=2, length=5
    start=3, end=4, length=5
    start=0, end=4, length=5
    5.6  4.1  3.1  2.3  1.2
    */

87、实现双向链表删除一个节点P，在节点P 后插入一
个节点，写出这两个函数。
【参考答案】
// 删除操作
Status ListDelete_DuL(DuLinkList &L,inti,Ele mType &e)
{
if(!(p=G e tElemP_DuL(L,i))) return ERROR;
e=p->data;
p->prior->next=p->next;
p->next->prior=p->pror;
free(p);
return OK;
}
// 插入操作
Status ListInsert_DuL(DuLinkList &L,inti,ElemType &e)
{
if(!(p=G e tElemP_DuL(L,i)))
return ERROR;
if(!(s=(DuLinkList)malloc(sizeof(DuLNode))))
return ERROR;
s->data=e;
s->pri or =p;
p-> next -> prior =s;
p->next=s;
s->next=p->next->next;
return OK;
}

 

 

 

 

88、把一个链表反向。
【参考答案】从第一个元素开始，ps指向他，将他（
ps）指向头节点(ps->next = head) ，将ps设为头节点
（head = ps; ）操作下一个元素（ps= pe->next;）等
于是依次将每个元素翻到原头节点前面。
void reverse(test* head)
{
test* pe = head;
test* ps = head->next;
while(ps)
{ 
pe->next = ps->next;
ps->next = head;
head = ps;
ps =  pe->next;
}
}

    typedef struct Node{
        int val;
        struct Node *next;
    }Link;
     
    Link* reverse(Link *head)
    {
        if(head==NULL || head->next==NULL)
            return head;
        
        Link *pcur=head, *pnew=NULL, *prev=NULL, *pnext=NULL;
        while(pcur)
        {
            pnext=pcur->next;
        
            if(pnext==NULL)
            {
                pnew=pcur;
                printf("pnew's head=%d\n", pnew->val); 
            }
            
            pcur->next=prev;
            prev=pcur;
            pcur=pnext;
        }
     
        return pnew;
    }
     
    Link* reverse2(Link* head)
    {
        if(head==NULL || head->next==NULL)
            return head;
            
        Link* prev = head;
        Link* pnext = head->next;
        while(pnext)
        {
            prev->next = pnext->next;
            pnext->next = head;
            head = pnext;
            pnext = prev->next;
        }
        return head;
    }
    int main(int argc, char *argv[])
    {
        if(1)
        {
            Link *node1 = (Link*)malloc(sizeof(Link));
            node1->val=1;
            node1->next=NULL;
            
            Link *node2 = (Link*)malloc(sizeof(Link));
            node2->val=2;
            node2->next=NULL;
            
            Link *node3 = (Link*)malloc(sizeof(Link));
            node3->val=3;
            node3->next=NULL;
            
            Link *node4 = (Link*)malloc(sizeof(Link));
            node4->val=4;
            node4->next=NULL;
            
            node1->next=node2;
            node2->next=node3;
            node3->next=node4;
            //node4->next=node2;
            
            Link *head=node1; 
            printf("原始链表：\n"); 
            while(head)
            {
                printf("%d ", head->val);
                head=head->next;
            }
            printf("\n");
            
            head=node1; 
            head = reverse(head);
            //head = reverse2(head);
            printf("翻转链表：\n"); 
            while(head)
            {
                printf("%d ", head->val);
                head=head->next;
            }
            printf("\n");
        }
        return 0;
    }
     
    /*
    原始链表：
    1 2 3 4
    pnew's head=4
    翻转链表：
    4 3 2 1
    */

 

 

89、将二维数组行列元素互换，存到另一个数组中。
【参考答案】
#include <stdio.h>
main()
{   int a [2][3]={{1,2,3},{4,5,6}};
int b[3][2],i,j;
printf("array a :\n");
for(i=0;i<=1;i++)
{   for(j=0;j<=2;j++)
{   printf("%5d",a[i][j]);
b[j][i]=a[i][j];
}
printf("\n");
}    
printf("array b :\n");
for(i=0;i<=2;i++)
{   for(j=0;j<=1;j++)
printf("%5d",b[i][j]);
printf("\n");
}
}

    //二维数组行列元素互换，存到另一个数组中
    void convertArry()
    {
        int a[2][3]={{1,2,3},{4,5,6}};
        int b[3][2];
        int i,j;
        printf("a:\n");
        for(i=0; i<2; i++)
        {
            for(j=0; j<3; j++)
            {
                printf("%d ", a[i][j]);
                b[j][i]=a[i][j];
            }
            printf("\n");
        }
        printf("b:\n");
        for(i=0; i<3; i++)
        {
            for(j=0; j<2; j++)
            {
                printf("%d ", b[i][j]);
            }
            printf("\n");
        }
    }

 

 

 

90、输入一行字符，统计其中有多少个单词。
【参考答案】
#include <stdio.h>
main()
{   
char str i ng[81];
int i,num=0,word=0;
char c;
gets(string);
for(i=0;(c=string[i])!='\0';i++)
if(c==' ')  
word=0;
else if(word==0)
{   
word=1;  num++;   }
printf("There are %d word s in the line\n",num);
}

    //输入一行字符，统计其中有多少个单词。
    int words(char *arr)
    {
        int words = 0;
        int prev_is_char=0;
        int pcur_is_char=0;
        while(*arr != '\0')
        {
            if((*arr>='a' && *arr <='z') || (*arr>='A' && *arr<='Z'))
            {
                pcur_is_char=1;
            }
            
            if(prev_is_char==0 && pcur_is_char==1)
            {
                words++;
            }
            prev_is_char = pcur_is_char;
            pcur_is_char = 0;
            arr++;
        }    
        
        return words;
    }
    int words2(char *arr)
    {
        int i,num=0,word=0;
        char c;
        for(i=0;(c=arr[i])!='\0';i++)
        {
            if(c==' ') 
                word=0;
            else if(word==0)
            { 
                word=1; 
                num++; 
            }
        }
        return num;
    }
     
    int main(int argc, char *argv[])
    {
        if(1)
        {
            char *str="hello, world, china** i love you!!";
            int count = words(str);   //保证了单词必须是字母 
            printf("count=%d\n", count);
            int count2 = words2(str); //含有"1234"的错认为是单词 
            printf("count2=%d\n", count2);
            
            char *str1="hello, 1234, world, china** i love you!!";
            count = words(str1);   //保证了单词必须是字母 
            printf("count=%d\n", count);
            count2 = words2(str1); //含有"1234"的错认为是单词 
            printf("count2=%d\n", count2);
        }
        return 0;
    }
     
    /*
    count=6
    count2=6
    count=6
    count2=7
    */

 

 

 

91、写一个内存拷贝函数,不用任何库函数.就是前些时
候本版讨论的那个问题。
【参考答案】
void* m e mcpy(void* pvTo, const void* pvFrom, size_t size)
{
assert((pvTo != NULL) && (pvFrom ! = NULL));
byte* pbTo= pvTo;
byte* pbFrom = pbFrom;
while (size-- >  0)
{
*pbTo++ = *pbFrom++;
}
return pvTo;
}

 

https://blog.csdn.net/Wilsonboliu/article/details/7919773  

    void* my_memcpy(void* dest, void* src,size_t n)
    {
        if(dest==NULL || src==NULL)
            return NULL;
            
    	void* ret = dest;
    	unsigned char* str1 = (unsigned char*)dest;
    	unsigned char* str2 = (unsigned char*)src;
    	
    	while (n--)
    	{  
    		*str1++ = *str2++;
    		
    	}
    	return ret;
    }
    void* my_memmove(void* dest, void* src, size_t n)
    {
        if(dest==NULL || src==NULL)
            return NULL;
            
        void* ret = dest;
        unsigned char* str1 = (unsigned char*)dest;
    	unsigned char* str2 = (unsigned char*)src;
    	
        if(str1 > str2)
        {
            while(n--)
            {
                *(str1+n)=*(str2+n);
            }
        }
        else
        {
            while(n--)
            {
                *str1++ = *str2++;
            }
        }
        return ret;
    }
     
    char *strcat1(char *dest, const char *src) //将源字符串加const，表明其为输入参数
    {
        assert((dest!=NULL) && (src!=NULL));
        char *ret = dest;
        while(*dest)
        {
            dest++;
        }
        while(*dest++=*src++);
        return ret;
    }
    char *strcpy2(char *dest, const char *src)
    {
        assert((dest!=NULL)&&(src!=NULL));
        char *ret = dest;
        while(*dest++=*src++);
        return ret;
    }
    char *strcpy1(char *strDest,const char *strSrc)
    {
    	if((strDest==NULL)||(strSrc==NULL))  
               return NULL; 
     
        char *ret = strDest; //[1]
        while ((*strDest++=*strSrc++)!='\0'); //[2]
        return ret;//[3]
    }
    int strcmp1 (const char *str1,const char *str2)
    {
        assert((str1!=NULL) && (str2!=NULL));
        //assert((str1 != '\0') && (str2 != '\0'));
        while(*str1 && *str2 && (*str1==*str2))
        {
            str1++;
            str2++;
        }
        return *str1-*str2;
    }
     
    int strlen1(const char *str)
    {
        if(str==NULL)
            return 0;
        int len=0;
        while(*str++ != '\0')
        {
            len++;
        }
        return len;
    }
    int main(int argc, char *argv[])
    {
        if(1)
        {
           char str1[100]={"i love"};
           char str2[50]={"China"};
           //printf("%s\n",strcat1(str1,str2));
           //printf("%s\n",strcpy1(str1,str2));
           //printf("%d\n",strcmp1(str1,""));
           printf("%d\n",strlen1(str2));
        }
        if(0)
        {
            int arr1[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
            int arr2[10] = { 0 };
            my_memcpy(arr2, arr1, sizeof(arr1));
            int i;
            for (i = 0; i < 10; i++)
            {
                printf("%d ", arr2[i]);
            }
            printf("\n");
            
            int arr3[10] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
            my_memmove(arr3 + 5, arr3 + 4, 3 * sizeof(int));
            for (i = 0; i < 10; i++)
            {
                printf("%d ", arr3[i]);
            }
            printf("\n");
        }
        return 0;
    }

 

92、有1、2、3 、4个数字，能组成多少个互不相同且
无重复数字的三位数？都是多少？
【参考答案】
#inc lude  "stdio.h"
#include "conio.h"
main()
{
int i,j,k;
printf("\n");
for(i=1;i<5;i++) /* 以下为三重循环*/
for(j=1;j<5;j++)
for (k=1;k<5;k++)
{
if (i!=k&&i!=j&&j!=k) /* 确保i、j、k三位互不相同*/
pr intf("%d,%d,%d\n",i,j,k);
}
getch();
}

 

 

 

 

93、取一个整数a从右端开始的4～7位。
【参考答案】
main()
{
unsigned a,b,c,d;
scanf("%o",&a);
b=a>>4;
c=~(~0<<4);
d=b&c;
printf("%o\n%o\n",a,d);

}
 

    //取一个整数a从右端开始的4～7位
    void getNumsRight()
    {
        char str[50]={0};
        int len=0;
        unsigned int num=12345678;
        char val;
        while(num>0)
        {
            val = num%10;
            num = num/10;
            str[len]=val+'0';
            len++;
        }
        printf("num str = %s\n", str);
        
        int i;
        for(i=3; i<7; i++)
        {
            printf("%c", str[i]);
        }
        printf("\n");
    }
    int main(int argc, char *argv[])
    {
        if(1)
        {
            getNumsRight();
        }
        return 0;
    }
     
    /*
    num str = 87654321
    5432
    */

 

 

94、打印出杨辉三角形（要求打印出10行如下图）。
【参考答案】
main()
{
int i,j;
int a [10][10];
printf("\n");
for(i=0;i<10;i++)
{
a[i][0]=1;
a[i][i]=1;
}
for(i=2;i<10;i++)
for(j=1;j<i;j++)
a[i][j]=a[i-1][j-1]+a[i-1][j];
for(i=0;i<10;i++)
{
for(j=0;j<=i;j++)
printf("%5d",a[i][j]);
printf("\n");
}
getch(); 
}

    void yanghuiTriangle(int M)
    #define PYRAMID // 金字塔，会额外填充空格
    // #define REVERSE // 反向再来一次，得到菱形
    {
      int a [M][M], i, j;   // 二维数组和循环变量，a[行][列]
      for (i = 0; i<M; i++) // 每一行
      {
    #ifdef PYRAMID
        for (j = 0;j <= M-i; j++) 
            printf ("  ");
    #endif // 填充结束
     
        for (j = 0; j <= i; j++) // 赋值打印
        {
            if(i == j || j == 0)
            {
                a[i][j] = 1;
            }
            else
            {
                a[i][j] = a[i-1][j] + a[i-1][j-1]; // 使用上一行计算
            }
            printf("%4d", a[i][j]);
        }
        printf("\n");
      }
     
     
    void yanghui(int M)
    {
        int i,j;
        int a [M][M];
        
        printf("\n");
        
        //for(i=0;i<10;i++)
        //{
            //a[i][0]=1;
           // a[i][i]=1;
        //}
     
        for(i=0;i<M;i++)
        {
            for(j=0;j<=i;j++)
            {
                if(i==j || j==0)
                    a[i][j] = 1;
            }
        }
        
        for(i=2; i<M; i++)
            for(j=1; j<i; j++)
                a[i][j] = a[i-1][j-1] + a[i-1][j];
            
        for(i=0; i<M; i++)
        {
            //for(j = 0;j <= M-i; j++)   //添加此行为金字塔，不添加为直角三角形
                //printf ("  ");
            for(j=0; j<=i; j++)
                printf("%5d", a[i][j]);
            printf("\n");
        }
        
    }
     
    int main(int argc, char *argv[])
    {
        if(1)
        {
            yanghui(10);
        }
        return 0;
    }

 

 

95、实现strcmp 函数。
100 条经典 C语言笔试题目
96、写一个函数，求一个字符串的长度，在main函数
中输入字符串，并输出其长度。
【参考答案】
main()
{
intlen;
char *s tr[20];
printf("please input a string:\n");
scanf("%s",str);
len=length(str);
printf("the s tring has %d characters.",len);
getch();
}
length(p)
char *p;
{
int n ;
n=0;
while(*p!='\0')
{
n++;
p++;
}
return n;
}

 

 

 

 

97、809*??=800*??+9*??+1  其中??代表的两位数
,8*?? 的结果为两位数，9*??的结果为3位数。求??代
表的两位数，及809*??后的结果。
【参考答案】output(long b,long i)
{
printf("\n%ld/%ld=809*%ld+%ld",b,i,i,b%i);
}
main()
{
long int a ,b,i;
a=809;
for(i=10;i<100;i++)
{
b=i*a+1;
if(b>=1000&&b<=10000&&8*i<100&&9*i>=100)
output(b,i);
}
getch();
}

 

 

 

 

98、某个公司采用公用电话传递数据，数据是四位的整数，在
传递过程中是加密的，加密规则如下：每位数字都加上5, 然后
用和除以10的余数代替该数字，再将第一位和第四位交换，第
二位和第三位交换。
【参考答案】
main()
{
int a ,i,aa[4],t;
scanf( " % d " ,&a) ;
aa[0]=a %10;
aa[1]=a %100/10;
aa[2]=a %1000/100;
aa[3]=a /1000;
for(i=0 ; i<=3 ;i++)
{
aa [i]+=5;
aa[i] %=10;
}
for(i=0;i<=3/2;i++)
{
t=aa[i];
aa[i]= aa[3- i];
aa[3- i]= t ;
}
for(i=3;i>=0;i--)
prin tf("% d ", a [i]);
getch();
}

 

 

 

 

99、计算字符串中子串出现的次数。
【参考答案】
main()
{
char str1[20],str2[20],*p1,*p2;
int sum=0;
printf("please input two strings\n");
scanf("%s%s",str1,str2);
p1=str1;p2=str2;
while(*p1!='\0')
{
if(*p1==*p2)
{
while(*p1==*p2&&*p2!='\0')
{
p1++;
p2++;
}
}
else
p1++;
if(*p2=='\0')
sum++;
p2=str2;
}
printf("%d",sum);
getch();
} 

 

 

 

 

100 、有两个磁盘文件A和B, 各存放一行字母，要求把
这两个文件中的信息合并（按字母顺序排列），输出
到一个新文件C 中。
【参考答案】
100 条经典 C语言笔试题目
main()
{
FILE *fp;
in t i,j,n,ni;
char c[160],t,ch;
if((fp=fopen("A","r "))==NULL)
{
printf("file A cannot  be opened\n");exit(0);
}
printf("\nA contents are :\n");
for(i=0;(ch=fgetc(fp))!=EOF ; i++)
{
c[i]=ch;pu tchar(c[i]);
}
fclose(fp);
ni=i;
if((fp=fopen("B","r "))==NULL)
{
printf("file B cannot  be opened\n");exit(0);
}
printf("\nB contents are :\n");
for(i=0;(ch=fgetc(fp))!=EOF ; i++)
{
c[i]=ch;pu tchar(c[i]);
}
fclose(fp);
n=i;
for(i=0;i<n;i++)
for(j=i+1;j<n;j++)
if(c[i]>c[j]){t=c[i ];c[i]=c[j];c[j]=t;}
printf("\nC file is:\n");
fp =f open("C","w");
for(i=0;i<n;i++){putc(c[i ],fp);putchar(c[i]);}
fclose(fp);
getch();
}

//---------------------end-----------------------------
