//---------README_kicad.c------------
void section1()//#module
{
	//SOT25=SOT-23-5
}
void shortkey()
{
	//1.m			move
	//2.r			rotate
	//3.shift(hold down)	copy
	//4.d			delete	
	//5.V  			via hold
}
void gerbv_jointed_board()
{
	install();//sudo apt install gerbv
	open_gerbv_files();//ctrl+o
	output();//file->export
}
void pcb()
{
	/*
#1.元器件封装的制作
1.点击菜单栏中的PCB封装编辑器
2.点击新建封装按钮，在弹出的对话框中输入元件名称，不习惯英制的朋友单位可以选择mm
3.选择放焊盘的工具，进行焊盘的放置
--特别提醒，这里发生的一个小插曲，刚开始这个选择的视图模式是默认的而不是Cairo绘图模式。
4.新建一个库并保存,文件夹的名字会在自己起的mypcblib后加上.pretty
5.点击设置，选择封装库管理，在出现的对话框中选择添加库按钮，然后进行设置。那么${KIPRJMOD}/mypcblib.pretty是为何物？${KIPRJMOD}类似Makefile中的变量，指向的是工程目录。
*/
}
void polygon()
{
//2.敷铜选择:点击敷铜边缘
}
//------------pur--------

