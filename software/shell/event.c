//---------------------event.c-------------------------
//---------------------system install
//debian
sudo nano /etc/apt/sources.list
deb https://mirrors.aliyun.com/debian  stable main contrib non-free
deb https://mirrors.aliyun.com/debian  stable-updates main contrib non-free
ctrl +x ->y

sudo apt install fonts-arphic-gbsn00lp fcitx-pinyin//chinese

sudo apt install gcc make vim
window:right ctrl+c 
//#sudoers
su
chmod +w /etc/sudoers
vi /etc/sudoers
ts	ALL=(ALL:ALL) ALL
//#VBoxWindowsAddition
sh ./autorun.sh
apt-get install linux-image*
apt-get install linux-headers-$(uname -r)
//#apt sources
sudo nano /etc/apt/sources.list
deb http://ftp.debian.org/debian buster main contrib
deb http://ftp.debian.org/debian buster-updates main contrib
ctrl +x ->y
//#chinese 
dpkg-reconfigure locales
apt install fonts-arphic-gbsn00lp
//#fcitx
sudo apt install fcitx fcitx-pinyin
fcitx configuration +
panel + keyboard layouts
shortcut:
shell//applications->setting->Keyboard->add->"exo-open --launch TerminalEmulator"
//---------------------startup disk
make ubuntu18.04 U-disk startup disk:
sudo fdisk -l
sudo umount /dev/sdb1
sudo mkfs.vfat /dev/sdb1 -I//format the U-disk
sudo dd if=~/Downloads/ubuntu-18.04-desktop-amd64.iso of=/dev/sdb status=progress



grub install position is /dev/sda
//---------------------recover U-disk:
sudo fdisk /dev/sdb
n
w
sudo mkfs.vfat -F 32 /dev/sdb1//format to fat32
//---------------------use tool
sudo vi /etc/profile	//设置环境变量
++	export PATH=$PATH:~/S3C2440A/toolschain
source /etc/profile	//使环境变量生效
//-------------------gcc
sudo apt install lib32ncurses-dev lib32z1
sudo vi ~/.bashrc
PATH=$PATH:/usr/local/bin:/usr/bin:/bin:/sbin:/home/ts/project/ltlt/gcc/bin
//--------------mount
sudo fdisk /dev/sdb
sudo /sbin/mkfs -t ext4 /dev/sdb1
sudo blkid

sudo vi /etc/fstab
UUID=8560e36a-3dfc-4727-87b9-20fa3168fd2d /home/ts/project      ext4    defaults            0       2
//----------------------end-----------------------------
