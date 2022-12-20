#include <string>
#include <functional>

using std::string;
using std::function;

/*********************
 * 语法树数据结构声明
 *********************/

/* 构造一个语法树数据结构 */
class Pair
{
private:
    
public:
    void *val;
    int  flag;
    Pair *next;
    Pair(void *val, Pair *next, int flag);
    ~Pair();
    /* 返回以此节点为起点的链表的长度 */
    int len(void);
    /* 获取链表中序号为 x 的元素 此函数不检查 x  */
    Pair *get(int x);
    /* 将从此结点开始的链表全部释放 */
    void free(void);
    /** 对链表内的每个节点都调用函数 func 
     * 假定 func 返回节点的 next 域与原 next 域不同 */
    Pair *map(function<Pair *(Pair *)>);
    /** 将此语法树复制一遍 */
    Pair *copy(void);
    /** 将此节点的值格式化输出 */
    string self_str(void);
    /** 将此语法树打印出来 */
    string to_str(void);
};


/****************************
 * symbol table 数据结构声明
 ****************************/

/** 节点数据类 */
class Node
{
private:
    
public:
    string name;
    void *val;
    char flag;
    Node *next;
    Node(string name, void *value, char flag);
    ~Node();
};


/* 哈希表数据结构 */
class Table
{
private:
    unsigned size;
    Node **table;
    unsigned hash(string name);
public:
    Table(unsigned size);
    ~Table();
    void insert(string name, void *value, char flag);
    Node *lookup(string name);
};


/* Symbol table 数据结构 */
class Frame
{
private:
    
public:
    Frame *parent;
    Table *bindings;
    Frame(Frame *prev, unsigned tablesize);
    Frame(Frame *prev);
    ~Frame();
    void define(string name, void *val, char flag);
    Node *get_symbol(string name);
    Frame *make_child_frame(Pair *formals, Pair *vals);
};


