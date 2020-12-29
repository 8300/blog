//--------------------- gdb.c -------------------------
0.thread
1.command
info threads	:show all threads
thread <id>	:switch thread
b <filename>:<line> thread all	:set break on all threads
thread apply <id1> <id2> <command>	:executive command on one or many threads
thread apply all <command>	:executive command on all threads;
set follow-fork-mode <parent/child>	:这条命令可以用于在调试父进程或是子进程的模式之间进行切换。例如在fork函数执行之前执行set follow-fork-mode child, 当fork执行后，设定在子进程上的断点将会有效，而父进程上的断点此时无效；反之依然。缺省gdb是调试主进程的。
0.common
1.b <func>	:set break on function entry
1.x /nfu <addr>	:x /3uh 0x4322 //从内存地址0x4322读取内热，h表示以双字节为单位，3表示输出三个单位，u表示按16进制显示。
1.finish	:quit function
1.shell <command>	:executive shell command
1.set args	:set params of the runtime
1.show args	:show all params
1.info args	:print the name of the argument to the current function and its value;
1.info locals	:print all local variables to the current function and its value;
1,info catch	:print exception handing information to the current funtion
1.clear <line>	:clear line break
2.变量值进入函数后被改变了，跟闹鬼一样，莫名其妙；原因：自定义函数库没更新（交叉编译的库，和运行时的库不一样）
90      in ./sub_ts/mosq_ts.c
(gdb) p tmp_hard
$2 = (t_hard *) 0x40e00b30
(gdb) p tmp_hard->next
$3 = (struct T_hard *) 0x0
(gdb) s
./sub_ts/mosq_ts.c(743)-ts_mosq_get_config->waiting hard and order
hard_get_from_int (hard=0x40e00b30, hard_int=514) at param_ts.c:136
136     param_ts.c: No such file or directory.
(gdb) p hard
$4 = (t_hard *) 0x40e00b30
(gdb) p hard->next
$5 = (struct T_hard *) 0x26350
(gdb) p hard->id  
$6 = 1
(gdb) p hard->next->id
$7 = 0
(gdb) p hard->next->next
$8 = (struct T_hard *) 0x10c
(gdb) q                 
A debugging session is active.

//--------------------- end ---------------------------
