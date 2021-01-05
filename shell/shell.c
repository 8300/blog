//---------------------shell.c-------------------------
kill -9:
	kill -HUP [PID]来杀掉这个僵尸进程
ping:
	计算机在与非本网段的地址通信时，计算机首先查找网关的MAC，如果网关MAC得不到回应，是不会对PING作出响应的；

0.reptyr	:Tool for moving running programs between ptys
1.reptyr <pid>
1.error:
2.Unable to attach to pid 7191: Invalid argument
sudo reptyr <pid> -T
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


/home/ts/projects/linux_kernel/linux-5.3/scripts/checkpatch.pl --file <> > log
如果共享库文件安装到了/usr/local/lib(很多开源的共享库都会安装到该目录下)或其它"非/lib或/usr/lib"目录下, 那么在执行ldconfig命令前, 还要把新共享库目录加入到共享库配置文件/etc/ld.so.conf中, 如下:

# cat /etc/ld.so.conf
include ld.so.conf.d/\*.conf
# echo "/usr/local/lib" >> /etc/ld.so.conf
# ldconfig

ldconfig
netstat -tunlp
//--------------------- interface management
sudo update-alternatives --config x-session-manager
whereis gnome-terminal
reboot->advence debian
sudo apt-get dist-upgrade
ctrl+alt+f1或f2或f3
sudo apt-get update --fix-missing
sudo apt-get dist-upgrade
sudo apt-get update --fix-missing

//---------------------self
ctrl + z	//suspend the process and go into background
ctrl + s	//freeze terminal
ctrl + q	//thaw terminal
jobs		//display the currently suspended process
bg %N		//make the N task run in the background
fg %N		//make the N task run in the foreground
2>/dev/null	//redirect standard error to an empty device;no output error
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
scp <src-file> username_remote@ip_remote:remote_folder
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
ipcrm <shmid>
//---------------------editor
font  : arial
font size : 8
ecoding :UTF-8
//---------------------file <file_name>//check the file type
//---------------------cat
cat -n <file_name>//add line numbers
cat -b <file_name>//only add line numbers to text
cat -T <file_name>//^I replace all tabs
//---------------------more	//paging tool
more <filename>
find -name <filename> | more
//---------------------less	//more's updated version
less <filename>
find -name <filename> | less
//---------------------tail	//a few lines at the end of display text
tail -n <number> <file_name>
tail -<number> <file_name>
#### head //a few lines at the beginning of display text
head -<number> <file_name>
#### cp -i <source> <destination>
#### touch <dir>//create directory or file
#### ps -ef	//show all process
#### top	//Real-time display process information
#### kill <PID>
killall <process_name>
//---------------------mount
mount <source> <directory>
umount <directory>
//访问Windows共享文件
smbclient -L //192.168.0.135 -U ts
sudo mount -o username=ts,password=44321 //192.168.0.135/share /mnt/share
sudo mount -t vboxsf <windows name> <linux_path>-----挂载
mount: can't find /dev/sdb1 in /etc/fstab
cause://if the path is empty, the mount will check the rules in /etc/fstab
//development board
mkdir ~/nfs
sudo apt install nfs-kernel-server
sudo vi etc/exports
/home/ts/nfs <board_ip>(rw,sync,no_root_squash,no_subtree_check)
sudo /etc/init.d/rpcbind restart
sudo /etc/init.d/nfs-kernel-server restart
board cmd:mount -t nfs -o nolock <server_ip>:/home/ts/projects/tmp /tmp
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
e2label <old_dirname> <new_dirname>	//change disk name
eg.sudo e2label /dev/sdb1 sdb
//---------------------module
insmod *.ko
rmmod *.ko
lsmod	//list all the module
modprobe
depmod
//---------------------tar
//compress
tar -cvf <name>.tar <file>
tar -czf <name>.tar.gz <file>
tar -cjf <name>.tar.bz2 <file>
tar -cZf <name>.tar.Z <file>
//unpack
	tar -xf <name>.tar
tar -xzf <name>.tar.gz
tar -xjf <name>.tar.bz2
tar -xZf <name>.Z 
tar -xJf <name>.tar.xz
//---------------------ln 
ln -s <source_file> <target_file>
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
	echo xx xx xx > /dev/ttyUSB0	//send data
	echo -en '\x50\x42\x09'> /dev/ttyUSB0	//send 16 进制数
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
sudo apt --purge remove <software_name>
#error
//apt install can't use
cause: force exit from apt
	sudo cp /etc/apt/sources.list /etc/apt/sources.list.backup //backup the sources
	sudo apt update
	sudo apt -f dist-upgrade
//---------------------open
xdg-open <file_name>
#error
//Could not get lock /var/lib/dpkg/lock - open (11: Resource temporarily unavailable)
ps afs|grep apt
sudo kill -9 <pid>
//error:Failed to fetch http://cn.archive.ubu
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

