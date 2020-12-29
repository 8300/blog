//---------------------libc.c--------------------------
0.//------------------- link
1.c/c++编译时，指定程序运行时查找的动态链接库路径
-Wl,-rpath,  用于指定程序运行时查找动态链接库的路径，多个路径是使用冒号隔开。这样就不用添加路径到 /etc/ld.so.conf 文件中了，在需要多个so版本共存时很有用
eg.g++ -Wl,-rpath,/usr/local/lib/ -oevh libevent_http.cpp -levent
1.查看程序链接的动态库
arm-linux-gnueabihf-readelf -d example.so
0.//---------------------function
//rindex:找出参数s字符串中最后一个出现的参数c地址
//strtok:分解字符串为一组字符串
//strchr:在一个字符串中查找给定字符的第一个匹配之处
//sscanf:读取格式化的字符串中的数据
//
//---------------------undefined
//undefined reference to 'pthread_create',
	在main.cpp同目录的CMakeLists.txt中添加两行：
	find_package(Threads)
	target_link_libraries(main ${CMAKE_THREAD_LIBS_INIT})

//---------------------other
//问题点：加printf运行可以通过，去掉就无法通过
//c语言里，表达一个string是n位字符+'\0'，如果是memcpy字串size应该是n+1位
//libcloog.so.0: cannot openshared object file: No such file or directory
	solve:
	sudo apt-get install libcloog-ppl-dev
//malloc:invalid conversion from ‘void*’ to ‘char*’
	char *foo = (char*)malloc(1);
//no type named ‘type’ in typedef typename result_of<_Callable(_Args...)>::type result_type;
	//thread first(tsrec,sock,oppoaddr,filename);
	//void tsrec(int sock,sockaddr_in oppoaddr,string filename)
//---------------------note
文件结束符：ctrl + D
char        8   u8
wchar_t     16  L
char16_t    16  u
char32_t    32  U
short       16
int         32
long        32  l/L
long long   64  ll/LL
unsigned    32  u/U
float       32  f/F 6位有效数字
double      64  10位有效数字
long double 可以打印 sizeof(long double)得知 10位有效数字 l/L
UL可以合在一起使用，会选择能匹配空间最小的作为数据类型
char在一些机器上是有符号一些是无符号
unsigned仅能表示大于0的数字
当数值不可能是负数时用无符号
bool b = 42;非零即为true
unsigned char c = -1;当超出范围时，结果位取模后的余数（不可维护程序）
signed char c2 = 256;未定义
算术表达式中有无符号时，会把其他类型转换为无符号进行运算
0开头表示八进制，0x开头表示16进制：023 0x32
字面值：
十进制字面值带符号int（前提是能容下，否则就是long/long long），其他进制可能带可能不带
指数用E或e
字符字面值：‘a’
字符串字面值：“sef”
bool：false true
指针：nullptr
eg：10uL

/n  换行      /t  横向制表符       /v  纵向制表符       /a  报警
/b  退格      /"  双引号         /'  单引号         /\反斜杠
/r  回车      /f进纸            /?  问号
/x接十六进制 /接八进制 如果/后面八进制数字超过三个，只有前三个数字构成转移序列（不可维护程序） 
2.2变量初始化：
int units_sold = 0;
int units_sold = {0};
int units_sold{0};
int units_sold(0);      
long double ld = 3.1415926536;
int a{ld}, b = {ld}; // error: narrowing conversion required
int c(ld), d = ld;   // ok: but value will be truncated
内衣在函数体之外的变量被初始化位0，函数体内部的内置类型变量将不被初始化
声明而非定义用：extern；如果extern包含了初始值就不再是声明而是定义了。
用户自定义的标识符不能连续出现两个下划线，也不能以下划线紧连大写字母开头，定义在函数体之外的标识符不能以下划线开头。
变量名用小写
用户自定义类名用大写开头
::<val_name>局部变量和全局变量冲突时使用此方法调用全局变量。
2.3引用
起另外的名字
int ival = 105;
int &refVal = ival;
int &fsdf;错，必须初始化
指针：
int *p1 = nullptr; // equivalent to int *p1 = 0;
int *p2 = 0;       // directly initializes p2 from the literal constant 0
// must #include cstdlib
int *p3 = NULL;    // equivalent to int *p3 = 0;
void *可以从存放任意对象的地址
int* p1,p2;p1是指针，p2是int
int *&r = p1;//r是指针p的别名/引用。
了解变量类型，从右往左，离变量名最近的符号
2.4限定符const 
创建后不能改变，而且必须初始化。
共享：对于const变量不管是定义还是声明都要添加extern
int i = 42;
int &r1 = i;          //
const int &r2 = i;    //
r1 = 0;               //
r2 = 0;               //
r1 bound to i
r2 also bound to i; but cannot be used to change i
r1 is not const; i is now 0
error: r2 is a reference to const
int i = 0;
int *const p1 = &i;  // we can't change the value of p1; const is top-level
const int ci = 42;   // we cannot change ci; const is top-level
const int *p2 = &ci; // we can change p2; const is low-level
const int *const p3 = p2; // right-most const is top-level, left-most is not
const int &r = ci;  // const in reference types is always low-level
2.5处理类型
typedef double wages;   // wages is a synonym for double
typedef wages base, *p; // base is a synonym for double, p for double*
auto 让编译器替我们分析表达式所属的类型
auto item = val1 + val2;
decltype类型指示符
decltype(f()) sum = x; // sum has whatever type f returns
3.2处理string中的字符
处理每个字符用for(auto c : str)
下标类型：string::seze_type
6
cc -c 分离式编译
一旦某个形参有了默认值，后面所有形参必须有默认值
__func__函数名
__FILE__文件名
__LINE__行号
__TIME__编译时间
__DATE__编译日期
#error
//strlen
char *里面没东西，不能使用strlen
//---------------------end-----------------------------
