//---------------------fs.c--------------------------
//---------------------jffs2
mkfs.jffs2 -n -l -s 0x1000 -e 0x40000 -p 0x400000 -d rootfs_jffs2/ -o jffs2.img
mkfs.jffs2 -n -l -s 0x0100 -e 0x10000 -p 0x400000 -d rootfs_jffs2/ -o jffs2.img		//页256B  块64K
mkfs.jffs2 -n -l -s 0x0100 -e 0x8000 -p 0x400000 -d rootfs_jffs2/ -o jffs2.img		//页256B  块32K
mkfs.jffs2 -n -l -s 0x1000 -e 0x10000 -p 0x400000 -d rootfs_jffs2/ -o jffs2.img		//页4KB   块64K
mkfs.jffs2 -n -l -s 0x1000 -e 0x8000 -p 0x400000 -d rootfs_jffs2/ -o jffs2.img		//页4KB   块32K
mkfs.jffs2 -n -l -s 0x1000 -e 0x1000 -p 0x400000 -d rootfs_jffs2/ -o jffs2.img		//页4KB   块4K
mkfs.jffs2 -n -l -s 0x1000 -e 0x4000 -p 0x400000 -d rootfs_jffs2/ -o jffs2.img		//页4KB   块16K

mkfs.jffs2 -l -e 0x10000 -p 0x500000 -d rootfs_jffs2/ -o jffs2.img					//页4KB   块64KB		//目前测试成功
mkfs.jffs2 -l -e 0x20000 -p 0x500000 -d rootfs_jffs2/ -o jffs2.img					//页4KB   块128KB
mkfs.jffs2 -l -s 0x100 -e 0x10000 -p 0x500000 -d rootfs_jffs2/ -o jffs2.img			//页256   块64KB
mkfs.jffs2 -l -e 0x10000 -p 0xb00000 -d rootfs_jffs2/ -o whyjffs2.img					//页4KB   块64KB      //spi flash 16M  这里rootfs给了11M， 测试成功
sumtool -e 0x10000 -p -i whyjffs2.img -o whyjffs2-sum.img	//略微使文件系统加速

说明：
页大小0x1000   4k
块大小0x40000  256k
jffs2分区总空间0x400000即4M
当然生成的jffs2.img并没有一下子分配4M,还是实际大小0xc0000
mkfs.jffs2使用方法
选项含义(man a mkfs.jffs2)
mkfs.jffs2: Usage: mkfs.jffs2 [OPTIONS]
Make a JFFS2 file system p_w_picpath from an existing directory tree
Options:
-p, --pad[=SIZE]       用16進制來表示所要輸出檔案的大小，也就是root.jffs2的size。
                                       很重要的是, 為了不浪費flash空間, 這個值最好符合flash driver所規劃的區塊大小。
                                       如果不足则使用0xff来填充补满。 
-r, -d, --root=DIR      指定要做成p_w_picpath的源資料夾.(默认：当前文件夹)
-s, --pagesize=SIZE     节点页大小(默认: 4KiB)
-e, --eraseblock=SIZE   设定擦除块的大小为(默认: 64KiB)
-c, --cleanmarker=SIZE Size of cleanmarker (default 12)
-m, --compr-mode=MODE   Select compression mode (default: priortiry)
-x, --disable-compressor=COMPRESSOR_NAME
                          Disable a compressor
-X, --enable-compressor=COMPRESSOR_NAME
                          Enable a compressor
-y, --compressor-priority=PRIORITY:COMPRESSOR_NAME
                          Set the priority of a compressor
-L, --list-compressors Show the list of the avaiable compressors
-t, --test-compression Call decompress and compare with the original (for test)
-n, --no-cleanmarkers   指明不添加清楚标记（nand flash 有自己的校检块，存放相关的信息。）
                          如果挂载后会出现类似：
                          CLEANMARKER node found at 0x0042c000 has totlen 0xc != normal 0x0
                          的警告，则加上-n 就会消失。
-o, --output=FILE       指定輸出p_w_picpath檔案的文件名.(default: stdout)
-l, --little-endian     指定使用小端格式
-b, --big-endian        指定使用大端格式
-D, --devtable=FILE     Use the named FILE as a device table file
-f, --faketime          Change all file times to '0' for regression testing
-q, --squash            Squash permissions and owners making all files be owned by root
-U, --squash-uids       将所有文件的拥有者设为root用户
-P, --squash-perms      Squash permissions on all files
      --with-xattr        stuff all xattr entries into p_w_picpath
      --with-selinux      stuff only SELinux Labels into jffs2 p_w_picpath
      --with-posix-acl    stuff only POSIX ACL entries into jffs2 p_w_picpath
-h, --help              显示这些文字
-v, --verbose           Verbose operation
-V, --version           显示版本
-i, --incremental=FILE Parse FILE and generate appendage output for it
#error
//Node at 0x0000f568 with length 0x00000b20 would run over theend of the erase block
Perhaps the file system was created with the wrong erasesize?
jffs2_scan_eraseblock(): Magic bitmask 0x1985 not found at0x0000f56c: 0x0b20 instead
jffs2_scan_eraseblock(): Magic bitmask 0x1985 not found at0x0000f570: 0x74a8 instead
仔细检查之后发现是制作文件系统时块大小设置错了，只要修改mkfs.jffs2后-e的参数即可，例如本例，现在spiflash的块大小只有64k，而我制作文件系统之时设置成256k即0x40000，修改如下：
#osdrv/pub/bin/pc/mkfs.jffs2 -d osdrv/pub/rootfs_uclibc -l-e 0x40000 -oosdrv/pub/rootfs_uclibc_256k.jffs2
osdrv/pub/bin/pc/mkfs.jffs2 -d osdrv/pub/rootfs_uclibc -l-e 0x10000 -oosdrv/pub/rootfs_uclibc_256k.jffs2

//---------------------

//---------------------end-----------------------------
