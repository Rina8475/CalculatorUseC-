/** 此文件实现符号表 由哈希表实现
 * Author:  Rina
 * Date:    2022/11/21
 */
#include <iostream>
#include "table.h"
#include "flags.h"
#define TABLESIZE 10

/*************************
 * 实现 Symbol table 节点
 *************************/

Node::Node(string name, void *value, char flag)
{
    this->name = name;
    this->val = value;
    this->flag = flag;
    this->next = nullptr;
}

Node::~Node()
{
}


/************************
 *     实现哈希表
 ************************/

Table::Table(unsigned size)
{
    this->size = size;
    this->table = new Node*[size];
    for (unsigned i = 0; i < size; i += 1) {
        table[i] = nullptr;
    }
}

Table::~Table()
{
    delete [] table;
}

/** 哈希函数，参照 C 程序设计语言 */
unsigned Table::hash(string name) {
    unsigned hashval = 0;
    int size = name.size();
    for (int i = 0; i < size; i += 1) {
        hashval = name[i] + hashval * 31;
    }
    return hashval % size;
}

/** 在哈希表中查找一个值 
 *  若查找到，则返回该表项的指针，否则返回 NULL */
Node *Table::lookup(string name) {
    unsigned index = hash(name);
    Node *ptr = table[index];

    for (; ptr != nullptr && name != ptr->name; ptr = ptr->next);
    return ptr;
}

/** 将一个 key-value 对插入哈希表
 *  如果该 key 在该哈希表内，则更新其 value
 *  否则新建一项，将 key-value 对放入 */
void Table::insert(string name, void *value, char flag) {
    Node *ptr = lookup(name);
    if (ptr == NULL) {  
    /* 如果之前没有插入过 key */
        Node *elem = new Node(name, value, flag);
        unsigned index = hash(name);
        elem->next = table[index];
        table[index] = elem;
    } else {
    /* 如果之前插入过 key */
        ptr->val = value;
        ptr->flag = flag;
    }
}


/*********************
 *  实现 Symbol table
 *********************/

Frame::Frame(Frame *prev, unsigned tablesize)
{
    parent = prev;
    bindings = new Table(tablesize);
}

Frame::Frame(Frame *prev)
{
    parent = prev;
    bindings = new Table(TABLESIZE);
}

Frame::~Frame()
{
    delete bindings;
}

/** 将一对值插入环境 */
void Frame::define(string name, void *val, char flag) {
    bindings->insert(name, val, flag);
}

/** 从环境中查找一个值 返回值为该值对应的节点 
 * 如果表中未查到对应的值 则返回一个特殊节点 */
Node *Frame::get_symbol(string name) {
    Node *elem = bindings->lookup(name);
    if (elem != nullptr) {
        return elem;
    } else if (parent != nullptr) {
        return parent->get_symbol(name);
    }
    return new Node(name, NULL, ENV_ILLEGAL);       
    /* 此节点需要在 calculate 阶段处理后 delete */
}

/** 为当前环境添加一个子环境 且子环境内预定义一些值
 * 假定 formals 和 vals 有相等的长度 */
Frame *Frame::make_child_frame(Pair *formals, Pair *vals) {
    Frame *ptr = new Frame(this);
    for (; formals != NULL; formals = formals->next, vals = vals->next) {
        ptr->define(*(string *) formals->val, vals->val, ENV_DIGIT);
    }
    return ptr;
}