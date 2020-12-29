#jffs2
制作根文件系统命令
mkfs.jffs2 -n -s 512 -e 16KiB -d fs -o fs.jffs2 --pad=0xa00000
//mkfs.jffs2 -n -s 2048 -e 128KiB -d fs -o fs-jffs2.bin --pad=0xa00000
//mkfs.jffs2 -d fs -l -e 0x10000 -o fs.jffs2 
//          -d ??  -l  ???? , -e ??????? . -o ???????
#error
//Empty flash at 0x012dbffc ends at 0x012dc000
page size
各参数的意义：
(1)-r ：指定根文件系统目录.
(2)-o : 指定输出的根文件系统名.
(3)-s : 指定flash每一页的大小;
(3)-e : 指定flash的檫除块的大小，預設是64KB.
要注意，不同的flash, 其page size和block size會不一樣.，如果是K9F2G08U0M,在它的datasheet上有说明：
1 Page = (2K+64)Bytes
1 Block = (2K+64)Bytes = (128K+4K) Bytes
1 Device = (2K+64)B * 64Pages * 2048 Blocks = 2112 Mbits
所以上面指定:"-s 2048 -e 128KiB"。
如果是K9F1208U0C，在它的datasheet上有说明：
1 Page = 528 Bytes
1 Block = 528 Bytes * 32 Page = (16K+512) Bytes
1 Device = 528 Bytes * 32 Pages * 4096 Blocks = 528 Mbits
则上面指定: "-s 512 -e 16KiB"
//Cowardly refusing to erase blocks on filesystem with no valid JFFS2 nodes
bootloader 内核启动参数设置
supervivi:
part del root
part add root 0x00560000 0xa00000 0
part save

