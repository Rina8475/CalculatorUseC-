/** 输入流中有 4 种 token 分别为 
 * DIGIT(float) 即浮点数 
 * SYMBOL       即 name 包括函数名和变量名
 * INSTRUCT     即 关键字 包括各种指令
 * 其余即为各种单字节字符 eg '+' '-' '(' ')' */
#define INSTRUCT    256
#define SYMBOL      257
#define DIGIT       258


/**
 * 语法树中标记每个节点中所存储的指针的类型
 */
#define PTREE_PAIR          1
#define PTREE_NUMBER        2
#define PTREE_STRING        4
/* 语法树中并不含有下列类型 只是方便于后续计算 */
#define PTREE_BUILDIN       8
#define PTREE_LAMBDA        16
#define PTREE_FUNCTION      PTREE_BUILDIN | PTREE_LAMBDA      


/**
 * 符号表中 symbol 所存储的值的类型
 */
#define ENV_ILLEGAL     0       /* 查无此值 */
#define ENV_DIGIT       1
#define ENV_BUILDIN     PTREE_BUILDIN
#define ENV_LAMBDA      PTREE_LAMBDA
#define ENV_FUNCTION    ENV_BUILDIN | ENV_LAMBDA