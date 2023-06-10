
<!DOCTYPE html>
<head>
    <meta charset="UTF-8">
</head>
<h1>glib_note.html</h1>
<p style="white-space:pre-wrap">
</p><h2>G_DECLARE_FINAL_TYPE</h2><p style="white-space:pre-wrap">

## G_DECLARE_FINAL_TYPE(ModlueObjName, module_obj_name, MODULE, OBJ_NAME, ParentName)
一个很方便的宏定义。用于在头文件中为不打算被子类化的类型发出通常的声明.
你可以在头文件中使用它，如下所示：

#ifndef _myapp_window_h_
#define _myapp_window_h_

#include <gtk/gtk.h>

#define MY_APP_TYPE_WINDOW my_app_window_get_type ()
G_DECLARE_FINAL_TYPE (MyAppWindow, my_app_window, MY_APP, WINDOW, GtkWindow)

MyAppWindow *    my_app_window_new    (void);

...

#endif

这导致发生以下事情：
	.通常的my_app_window_get_type()以GType为返回类型的函数被声明。
	.MyAppWindow类型被定义作为结构体_MyAppWindow的类型定义。这个结构本身没有被定义，应该在G_DEFINE_TYPE()使用前从.c文件中定义
	.MY_APP_WINDOW()转换作为静态内联函数与MY_APP_IS_WINDOW()类型检查函数一起发出。
	.MyAppWindowClass类型被定义为一个包含GtkWindowClass的结构。这样做是为了方便定义类型的人，不应该被视作ABI（application binary interface应用程序二进制接口）的一部分。特别的是，没有实例结构体的明确声明，子类化类型是不可能的。因此暴露类结构体的大小这一事实是不用关心的。它能在未来任何一个点自由改变。
	.根据你的父类的类型为你的类型添加g_autoptr()支持。
只有你父类也支持g_autoptr()时，你才能使用这个函数。
因为类型宏(上面示例中的MY_APP_TYPE_WINDOW)是不可调用的，所有你必须继续手动将其定义为自己的宏。
_get_type()函数的声明是宏发出的第一件事。这允许此宏以通常的方式与导出控制和api版本控制宏一起使用。
如果你想声明你自己的类结构体，使用G_DECLARE_DERIVABLE_TYPE().
如果你正在写一个库，请务必注意，在不破坏api或者abi情况下，将类型从使用G_DECLARE_FINAL_TYPE()转换为G_DECLARE_DERIVABLE_TYPE()是可能的。作为预防措施，你应该使用G_DECLARE_FINAL_TYPE(),除非你确信将你的类子类化是有意义的。一旦一个类结构被暴露，在不破坏api和abi的情况下改变它的大小删除或重新排序项目是不可能的。

参数：
	ModuleObjName	新类名称。陀峰命名方式（像GtkWidget）
	module_obj_name 新类名称。以'_'为分隔符的小写命名方式（像gtk_widget）
	MODULE		模型名称。全部大写模式(像GTK)
	OBJ_NAME	类的裸名称。全部大写模式（像WIDGET）
	ParentName	父类名称。陀峰命名方式（像GtkWidget）

</p><h2>G_DEFINE_TYPE</h2><p style="white-space:pre-wrap">
## G_DEFINE_TYPE(TN, t_n, T_P) G_DEFINE_TYPE_EXTENDED (TN, t_n, T_P, 0, {})

一个很方便的类型实现宏定义，声明了一个类初始化函数，一个实例初始化函数（有关这些信息，参阅GTypeInfo）和一个名为t_n_parent的指向父类的变量。
此外，它定义了一个*_get_type()函数。示例见G_DEFINE_TYPE_EXTENDED();


参数：
	TN	新类名称。陀峰命名方式
	t_n	新类名称。以'_'为分隔符的小写命名方式
	T_P	父类的GType
</p>

