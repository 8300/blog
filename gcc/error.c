0.error
1.SIGSEGV
2.type：fuction undeclared
warning but the return value from void to unsigned int
2.type：calloc error 
char *buffer = (char *)calloc(1, sizeof(len));


3.进入函数之前成员变量有值，进入函数之后成员变量无值，是因为头文件定义的结构体变了，动态库需要重新编译
//--------------------- end ---------------------------
