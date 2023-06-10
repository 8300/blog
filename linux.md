
## system install

### debian
grub install position is /dev/sda	
su	
chmod +w /etc/sudoers	
nano /etc/sudoers	
ts	ALL=(ALL:ALL) ALL	
ctrl +x ->y ->enter	
sudo nano /etc/apt/sources.list	
deb https://mirrors.aliyun.com/debian  stable main contrib non-free	
deb https://mirrors.aliyun.com/debian  stable-updates main contrib non-free	
ctrl +x ->y ->enter		
sudo apt update	
sudo apt install fonts-arphic-gbsn00lp fcitx-pinyin//chinese	
sudo apt install gcc make vim	

virtualbox top menu:right ctrl+c 

## VBoxWindowsAddition

sudo apt-get install linux-headers-$(uname -r)	
sh ./autorun.sh	

## fcitx-pinyin

sudo apt install fcitx fcitx-pinyin	
fcitx configuration +	
panel + keyboard layouts	

## shortcut

shell//applications->setting->Keyboard->add->"exo-open --launch TerminalEmulator"	

## startup disk

make ubuntu18.04 U-disk startup disk:	
sudo fdisk -l	
sudo umount /dev/sdb1	
sudo mkfs.vfat /dev/sdb1 -I//format the U-disk	
sudo dd if=~/Downloads/ubuntu-18.04-desktop-amd64.iso of=/dev/sdb status=progress	


## recover U-disk:

sudo fdisk /dev/sdb	
n	
w	
sudo mkfs.vfat -F 32 /dev/sdb1//format to fat32	


## mount

sudo fdisk /dev/sdb	
sudo /sbin/mkfs -t ext4 /dev/sdb1	
sudo blkid	
sudo vi /etc/fstab	
UUID=8560e36a-3dfc-4727-87b9-20fa3168fd2d /home/ts/project      ext4    defaults            0       2	
end
## screenrecord
	open/close	:alt + shift + ctrl + r
## env

vi .bashrc
++	export PATH=$PATH:~/S3C2440A/toolschain	

## panel recover to default

xfce4-panel --quit
pkill xfconfd
rm -rf ~/.config/xfce4/panel
rm -rf ~/.config/xfce4/xfconf/xfce-perchannel-xml/xfce4-panel.xml
xfce4-panel

## audio control

alsamixer

## end
kill -9:
	kill -HUP [PID]来杀掉这个僵尸进程
ping:
	计算机在与非本网段的地址通信时，计算机首先查找网关的MAC，如果网关MAC得不到回应，是不会对PING作出响应的；

0.reptyr	:Tool for moving running programs between ptys
1.reptyr &ltd&gtor moving running programs between ptys
1.error:
2.Unable to attach to pid 7191: Invalid argument
sudo reptyr &ltd&gtT
0.resolution
1.add
cvt 1920 1080
	"1920x1080_60.00"  173.00  1920 2048 2248 2576  1080 1083 1088 1120 -hsync +vsync
xrandr --newmode "1920x1080_60.00"  173.00  1920 2048 2248 2576  1080 1083 1088 1120 -hsync +vsync
xrandr --addmode Virtual1 "1920x1080_60.00"
0.error
1.menu is gone
rm ~/.cache
reboot
0.end


/home/ts/projects/linux_kernel/linux-5.3/scripts/checkpatch.pl --file &lt&gtog
如果共享库文件安装到了/usr/local/lib(很多开源的共享库都会安装到该目录下)或其它"非/lib或/usr/lib"目录下, 那么在执行ldconfig命令前, 还要把新共享库目录加入到共享库配置文件/etc/ld.so.conf中, 如下:

# cat /etc/ld.so.conf
include ld.so.conf.d/\*.conf
# echo "/usr/local/lib" &gt/etc/ld.so.conf
# ldconfig

ldconfig
netstat -tunlp
//--------------------- interface management
sudo update-alternatives --config x-session-manager
whereis gnome-terminal
reboot-&gtvence debian
sudo apt-get dist-upgrade
ctrl+alt+f1或f2或f3
sudo apt-get update --fix-missing
sudo apt-get dist-upgrade
sudo apt-get update --fix-missing

//---------------------top
top show one line:	top -b -n 1 | grep save
//---------------------self
ctrl + z	//suspend the process and go into background
ctrl + s	//freeze terminal
ctrl + q	//thaw terminal
jobs		//display the currently suspended process
bg %N		//make the N task run in the background
fg %N		//make the N task run in the foreground
2&gtev/null	//redirect standard error to an empty device;no output error
getconf		//get system variables

#error
//sudo
1.sudo: unable to execute ./autorun.sh: Permission denied
sh ./autorun.sh	//OK
2.kernel:vbox_gas:Kernel headers not found for target kernel  4.19.0-6-amd64
sudo apt install linux-headers-amd64
//multi change filename
#!/bin/sh
for file in `ls | grep 'arm-linu*'`
do
 newfile=`echo $file | sed 's/-gnueabihf/1/g'`
 mv $file $newfile
done
//PATH
cd ~
vi ./bashrc
relogin
//--------------------- 485
#error
A+ B-
A- B+
//---------------------ssh
scp &ltc-file&gtsername_remote@ip_remote:remote_folder
scp ../tmp/main root@192.168.1.101:/root/ts
//---------------------sed
//multi-file string replacement
sudo sed -i "s/unsigned char/uint8_t/g" `grep "unsigned char" -rl *`
//---------------------minicom
sudo minicom
CTRL-A Z
O
Filenames and paths
A - Download directory : /opt/FriendlyARM/mini2440/download           |
B - Upload directory   : /opt/FriendlyARM/mini2440/upload             |
Serial port setup
A -    Serial Device      : /dev/ttyUSB0
E -    Bps/Par/Bits       : 115200 8N1
F - Hardware Flow Control : No
Save setup as dfl
Exit
//---------------------shm
//show all shm
ipcs -m
//delete shm
ipcrm &ltmid&gt>
//---------------------editor
font  : arial
font size : 8
ecoding :UTF-8
//---------------------file &ltle_name&gtcheck the file type
//---------------------cat
cat -n &ltle_name&gtadd line numbers
cat -b &ltle_name&gtonly add line numbers to text
cat -T &ltle_name&gt^I replace all tabs
//---------------------more	//paging tool
more &ltlename&gt------more	//paging tool
find -name &ltlename&gt more
//---------------------less	//more's updated version
less &ltlename&gt------less	//more's updated version
find -name &ltlename&gt less
//---------------------tail	//a few lines at the end of display text
tail -n &ltmber&gtfile_name>
tail -&ltmber&gtfile_name>
#### head //a few lines at the beginning of display text
head -&ltmber&gtfile_name>
#### cp -i &lturce&gtdestination>
#### touch &ltr&gtcreate directory or file
#### ps -ef	//show all process
#### top	//Real-time display process information
#### kill &ltD&gtime display process information
killall &ltocess_name&gtplay process information
//---------------------mount
mount &lturce&gtdirectory>
umount &ltrectory&gtctory>
//访问Windows共享文件
smbclient -L //192.168.0.135 -U ts
sudo mount -o username=ts,password=44321 //192.168.0.135/share /mnt/share
sudo mount -t vboxsf &ltndows name&gtlinux_path>-----挂载
mount: can't find /dev/sdb1 in /etc/fstab
cause://if the path is empty, the mount will check the rules in /etc/fstab
//development board
mkdir ~/nfs
sudo apt install nfs-kernel-server
sudo vi etc/exports
/home/ts/nfs &ltard_ip&gtw,sync,no_root_squash,no_subtree_check)
sudo /etc/init.d/rpcbind restart
sudo /etc/init.d/nfs-kernel-server restart
board cmd:mount -t nfs -o nolock &ltrver_ip&gthome/ts/projects/tmp /tmp
#error
1.mount.nfs mount system call failed
vers=3
eg.mount -t nfs -o vers=3,nolock,rw remote_ip:remote_dir local_dir
//---------------------df
//show mount information
df -h
//---------------------grep
grep -rn "keyword"
//---------------------e2label
e2label &ltd_dirname&gtnew_dirname>	//change disk name
eg.sudo e2label /dev/sdb1 sdb
//---------------------module
insmod *.ko
rmmod *.ko
lsmod	//list all the module
modprobe
depmod
//---------------------tar
//compress
tar -cvf &ltme&gtar <file>
tar -czf &ltme&gtar.gz <file>
tar -cjf &ltme&gtar.bz2 <file>
tar -cZf &ltme&gtar.Z <file>
//unpack
	tar -xf &ltme&gtar
tar -xzf &ltme&gtar.gz
tar -xjf &ltme&gtar.bz2
tar -xZf &ltme&gt 
tar -xJf &ltme&gtar.xz
//---------------------ln 
ln -s &lturce_file&gttarget_file>
必须使用绝对路径//否则提示符号连接的层次过多
//---------------------shortcuts
ctrl shift j	//Longitudinal split screen 
ctrl shift h	//Horizontal split screen
alt k/j/h/l	//select up/down/left/right screen
alt f7	//move windows
//---------------------type
	#define U32 unsigned int 
	#define U16 unsigned short 
	#define S32 int 
	#define S16 short int 
	#define U8  unsigned char 
	#define S8  char
//---------------------wget
wget -r -p -np -k http://ar
//---------------------serial
	su
	stty -F /dev/ttyUSB0 speed 115200  cs8 -parenb -cstopb//115200波特率 8数据位 1停止位 无校验位
	cat -A /dev/ttyUSB0	//receive data
	echo xx xx xx &gtdev/ttyUSB0	//send data
	echo -en '\x50\x42\x09'&gtdev/ttyUSB0	//send 16 进制数
	//用串口调试助手一定要输入回车键
//---------------------tftp
install:
sudo apt install tftp-hpa	//client
sudo apt install tftpd-hpa	//server
set_path:
sudo mkdir ~/tftp_boot
sudo chmod 777 ~/tftp_boot -R
seting:
sudo vi /etc/default/tftpd-hpa
{
//“=”前面不能加“tab”
TFTP_USERNAME="pluto"//必须是登录账号
TFTP_DIRECTORY="/home/pluto/tftp_boot"//必须是绝对路径
TFTP_ADDRESS="0.0.0.0:69"
TFTP_OPTIONS="--secure"
}
sudo service tftpd-hpa restart
netstat -a |grep tftp//check to see whether the service is on
//---------------------apt
//uinstall
sudo apt --purge remove &ltftware_name&gter the service is on
#error
//apt install can't use
cause: force exit from apt
	sudo cp /etc/apt/sources.list /etc/apt/sources.list.backup //backup the sources
	sudo apt update
	sudo apt -f dist-upgrade
//---------------------open
xdg-open &ltle_name&gt-open
#error
//Could not get lock /var/lib/dpkg/lock - open (11: Resource temporarily unavailable)
ps afs|grep apt
sudo kill -9 &ltd&gt//error:Failed to fetch http://cn.archive.ubu
	//cause:other windows runing
//sf_read_super_aux err=-22 sudo mount -t vboxsf baidu /mnt/shared
	//install VBoxGuestAdditions
//---------------------dpkg
dpkg -l | grep "ncurses*"
//---------------------hexdump

open file in hexadecimal format

//---------------------strings
#!/bin/bash
while true
do
cat /proc/5318/smaps |grep heap
sleep 1
done
//---------------------end-----------------------------
Bash Shell 快捷键
字体大小：Ctrl + Shift + ’+‘ //Ctrl + Shift '-'
l         <CTRL k>：删除从光标到行尾的部分
l         <CTRL u>：删除从光标到行首的部分
l         <ALT d>：删除从光标到当前单词结尾的部分
l         <CTRL w>：删除从光标到当前单词开头的部分
l         <CTRL a>：将光标移到行首
l         <CTRL e>：将光标移到行尾
l         <ALT a>：将光标移到当前单词头部
l         <ALT e>：将光标移到当前单词尾部
l         <CTRL y>：插入最近删除的单词
l         <!$>：重复前一个命令最后的参数。

Ctrl + a - 跳到行首  
Ctrl + b - 左移一个字符  
Ctrl + c - 终端进程  
Ctrl + d - 从光标处向右删除  
Ctrl + e - 跳到行尾  
Ctrl + f - 右移一个字符  
Ctrl + k - 从光标处删除到行尾  
Ctrl + l - 清屏，类似 clear 命令  
Ctrl + r - 查找历史命令  
Ctrl + R - Search the history backwards with multi occurrence  
Ctrl + u - Delete backward from cursor // 密码输入错误的时候比较有用  
Ctrl + xx - Move between EOL and current cursor position  
Ctrl + x @ - Show possible hostname completions   
Ctrl + z - Suspend/ Stop the command  
补充:  
Ctrl + h - 删除当前字符  
Ctrl + w - 删除最后输入的单词  

终端快捷键:alt+f2然后打gnome-terminal就可以打开终端了

添加快捷键：系统——首选项——键盘快捷键

最常用
win+n切换背景颜色风格
alt+tab=不用说了。
win+tab若开3D效果了切换
ctrl+alt+backspace=相当于强制注销
ctrl+alt+del=调出关机菜单
ctrl+alt+l=锁定桌面

ctrl+alt+d=最小化gnome所有窗口
ctrl+alt+f2=linux终端用户（alt + f7返回xwindows，alt+ <- 或-> 进行终端切换）
ctrl+alt+ <- 或-> =切换桌面

终端快捷键
tab=补全
ctrl+a=开始位置
ctrl+e=最后位置
ctrl+k=删除此处至末尾所有内容
ctrl+u=删除此处至开始所有内容
ctrl+d=删除当前字母

ctrl+w=删除此处到左边的单词
ctrl+y=粘贴由ctrl+u，ctrl+d，ctrl+w删除的单词
ctrl+r=正在测试，还没弄明白，呵呵。
ctrl+l=相当于clear
ctrl+b=向回移动
ctrl+f=向前移动
ctrl+p=向上显示缓存命令
ctrl+n=向下显示缓存命令
ctrl+d=关闭终端
shift+上或下=终端上下滚动
shift+pgup或pgdown=终端上下翻页滚动
ctrl+shift+f=全屏（仅限于konsole）
ctrl+shift+n=新终端（terminal和konsole有所不同）
ctrl+c=终止

以下为桌面的快捷键列表

打开主菜单 = Alt + F1
运行 = Alt + F2
显示桌面 = Ctrl + Alt + d
最小化当前窗口 = Alt + F9
最大化当前窗口 = Alt + F10
关闭当前窗口 = Alt + F4
截取全屏 = Print Screen
截取窗口 = Alt + Print Screen

默认特殊快捷键
展示所有窗口程序 = F10
展示当前窗口最上层程序 = F11
展示当前窗口所有程序 = F12
切换窗口 = Alt + Tab
旋转3D桌面 = Ctrl + Alt + 左/右箭头（也可以把鼠标放在标题栏或桌面使用滚轮切换）
旋转3D桌面（活动窗口跟随） = Ctrl + Shift + Alt + 左/右箭头
手动旋转3D桌面 = Ctrl + Alt + 左键单击并拖拽桌面空白处
窗口透明/不透明 = possible with the “transset” utility or Alt + 滚轮
放大一次 = 超级键 + 右击
手动放大 = 超级键 + 滚轮向上
手动缩小 = 超级键 + 滚轮向下
移动窗口 = Alt + 左键单击
移动窗口时贴住边框 = 左键开始拖动后再 Ctrl + Alt
调整窗口大小 = Alt + 中击
Bring up the window below the top window = Alt + middle-click
动态效果减速 = Shift + F10
水纹 = 按住 Ctrl+超级键
雨点 = Shift-F9
桌面展开＝ Ctrl + Alt + 下箭头，然后按住 Ctrl + Alt 和左/右箭头选择桌面

## deb package

mkdir work
cd work/
mkdir -p ./usr/bin
mkdir DEBIAN
echo "Package: add-num
Version: 1.0
Section: custom
Priority: optional
Architecture: all
Essential: no
Installed-Size: 1024
Maintainer: linuxconfig.org
Description: Print linuxconfig.org on the screen" >DEBIAN/control


cp ../bin/debian/main usr/bin/chat
dpkg -b . chat_0.0.1.deb

## irc server

https://libera.chat/guides/clients
