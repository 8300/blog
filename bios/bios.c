//---------------------bootloader.c--------------------

0.//---------------------uboot 
1.//within
setenv serverip 192.168.1.110
setenv ipaddr 192.168.1.101
setenv ethaddr 00:00:7c:23:0b:c1
saveenv
tftp 0xc0700000 uImage
flinfo 查询Nor大小。
nand 用 Nand子系统命令，方法是 nand info
1.//compile
export BUILD_DIR=<dir>
make ARCH=arm CROSS_COMPILE=arm-linux- mini2440_config
make ARCH=arm CROSS_COMPILE=arm-linux- all
1.//board support
grep -rn mini2440
shell:  doc/README.scrapyard:265:mini2440         arm         arm920t        af5b9b1f    2014-01-13  #2014年一月停止维护mini2440
1.//cut
command liner  去掉u-boot 会减去很大空间

#error
1.//compiler-gcc6.h: No such file or directory 
reason:the compiler version is error
find -name compiler-gcc*
1.//cc1: error: bad value (‘armv8-a’) for ‘-march=’ switch
reason:compiler is empty
1.//Not enough room for program headers
修改顶层makefile
790行:LDFLAGS_u-boot + = $（call ld-option，--no-dynamic-linker）
修改u-boot-2016.11/fs/ubifs/lpt_commit.c
添加:
int dbg_chk_lpt_free_spc(struct ubifs_info *c);
int dbg_check_ltab(struct ubifs_info *c);
int dbg_chk_lpt_sz(struct ubifs_info *c, int action, int len);
1.//undefined reference to `board_nand_init'
发现是少了文件
/drivers/mtd/nand/s3c2410_nand.c 复制为 
/drivers/mtd/nand/s3c2440_nand.c 打开 里面的 所有 2410 换为 2440

drivers/mtd/nand/Makefile
添加一行
obj-$(CONFIG_NAND_S3C2440) += s3c2440_nand.o
1.//Error: end address not on sector boundary
vi include/configs/mini2440.h
#define CONFIG_ENV_SIZE  0x4000 -->
#define CONFIG_ENV_SIZE  0x10000
0.//---------------------study
1.//ldr
ldr r0, 0x12345678//就是把0x12345678这个地址中的值存放到r0中

//---------------------end-----------------------------

