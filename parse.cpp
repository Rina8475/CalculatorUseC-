/** 此文件进行语法分析，将 token 流转换为语法树
 * Author:  Rina
 * Date:    2022/11/21
 */
#include <iostream>
#include <string>
#include "flags.h"
#include "table.h"

using std::string;
using std::to_string;


/** 语法树数据结构
 * 合法的语法树只有两种形式
 * 1. 单节点    节点类型为 PTREE_NUMBER 或 PTREE_STRING
 * 2. 树        节点类型为 PTREE_PAIR */

Pair::Pair(void *val, Pair *next, int flag)
{
    this->val = val;
    this->next = next;
    this->flag = flag;
}

Pair::~Pair()
{
}

int Pair::len(void) {
    int count = 0;
    Pair *ptr = this;

    while (ptr != nullptr) {
        count += 1;
        ptr = ptr->next;
    }
    return count;
}

Pair *Pair::get(int x) {
    Pair *elem = this;
    for (; x > 0; x -= 1) {
        elem = elem->next;
    }
    return elem;
}

void Pair::free(void) {
    Pair *ptr, *next;
    ptr = this;
    next = ptr->next;
    while (ptr != NULL) {
        next = ptr->next;
        delete ptr;
        ptr = next;
    }
}

Pair *Pair::map(function<Pair *(Pair *)> func) {
    Pair *mapped;
    if (this->next == NULL) {
        return func(this);
    }
    mapped = func(this);
    mapped->next = mapped->next->map(func);
    return mapped;
}

Pair *Pair::copy(void) {
    Pair *result = new Pair(val, next, flag);
    if (flag == PTREE_PAIR) {
        result->val = ((Pair *) val)->copy();
    }
    if (next != NULL) {
        result->next = next->copy();
    }
    return result;
}


/** 表达式文法如下  加 . 的位置表示 space 
 * stmt     -> instruct . list | expr
 * list     -> name . expr | name (args) . expr
 * args     -> expr , args | expr
 * expr     -> expr + term | expr - term | term
 * term     -> term * factor | term / factor | factor
 * factor   -> digit | (expr) | name (expr) | name
 * 消除左递归后可得
 * stmt     -> instruct . name list | expr                      first: instruct, digit, (, name 
 * list     -> . expr | (args) . expr                           first: ., (
 * args     -> expr args'                                       first: digit, (, name
 * args'    -> , expr args' | $                                 first: ,, $
 * expr     -> term expr'                                       first: digit, (, name
 * expr'    -> + term expr' | - term expr' | $                  first: +, -, $
 * term     -> factor term'                                     first: digit, (, name
 * term'    -> * factor term' | / factor term' | $              first: *, /, $
 * factor   -> digit | (expr) | name name'                      first: digit, (, name 
 * name'    -> (args) | $                                       first: (, $ 
 */

/** 变量和函数声明开始 */
extern double tokenval;
extern string name;
extern int lexan(void);

static int lookahead;

void match(int token);
Pair *expr(void);
Pair *term(void);
Pair *factor(void);
Pair *define(void);
Pair *args(void);
/** 变量和函数声明结束 */


Pair *parse(void) {
    Pair *result;

    lookahead = lexan();
    if (lookahead == INSTRUCT) {
        // ! 将指令存储起来
        match(INSTRUCT);
        // ! 比较指令 决定该调用的函数
        result = define();
        return result;
    } else {
        return expr();
    }
}

Pair *expr(void) {
    Pair *subleft, *subright;
    char operat;
    
    subleft = term();
    while (lookahead == '+' || lookahead == '-') {
        operat = lookahead;
        match(lookahead);
        subright = term();

        /* 构造语法树 从后向前构造 */
        subleft->next = subright;
        Pair *head = new Pair(new string(operat == '+' ? "+" : "-"),
            subleft, PTREE_STRING);
        subleft = new Pair(head, NULL, PTREE_PAIR);    /* 树的根节点 */
    }
    return subleft;
}

Pair *term(void) {
    Pair *subleft, *subright;
    char operat;

    subleft = factor();
    while (lookahead == '*' || lookahead == '/') {
        operat = lookahead;
        match(lookahead);
        subright = factor();

        /* 构造语法树 从后向前构造 */
        subleft->next = subright;
        Pair *head = new Pair(new string(operat == '*' ? "*" : "/"),
            subleft, PTREE_STRING);
        subleft = new Pair(head, NULL, PTREE_PAIR);    /* 树的根节点 */
    }
    return subleft;
}

Pair *factor(void) {
    Pair *result, *rest = NULL;
    if (lookahead == DIGIT) {
        match(DIGIT);       /* digit 后不可能再跟一个 digit */
        result = new Pair(new double(tokenval), NULL, PTREE_NUMBER);
    } else if (lookahead == '(') {
        match('(');
        result = expr();
        match(')');
    } else {
        string symbol = name;       /* 将变量名存储下来 */
        match(SYMBOL);
        if (lookahead == '(') {
        /* 如果是函数调用 */
            match('(');
            rest = args();
            match(')');
            result = new Pair(new string(symbol), rest, PTREE_STRING);
            result = new Pair(result, NULL, PTREE_PAIR);
        } else {
        /* 否则是一个变量 */
            result = new Pair(new string(symbol), NULL, PTREE_STRING);
        }
    }
    return result;
}

/* define 指令的语法树构造 */
Pair *define(void) {
    Pair *symbol, *body, *arg;
    char c = std::cin.peek();       /* 查看输入流中的下一个字符，但不是提取该字符 */
    string s = string(name);        /* 将字符串预先存储起来 */
    
    match(SYMBOL);
    if (c == '(') {
    /* 函数定义 */
        match('(');
        arg = args();
        match(')');
        symbol = new Pair(new string(s), arg, PTREE_STRING);
        symbol = new Pair(symbol, NULL, PTREE_PAIR);
    } else {
    /* 否则为变量定义 */
        symbol = new Pair(new string(s), NULL, PTREE_STRING);
    }
    body = expr();
    /* 构造语法树 从后向前 */
    symbol->next = body;
    Pair *node = new Pair(new string("define"), symbol, PTREE_STRING);
    return new Pair(node, NULL, PTREE_PAIR);
}

/* 识别函数参数，形参和实参均可 
 * P.S. 此函数返回的并非是语法树 而是 Symbol 的后续节点
 */
Pair *args(void) {
    Pair *result, *last, *next;
    result = last = expr();
    while (lookahead == ',') {
        match(',');
        next = expr();
        last->next = next;
        last = next;
    }
    return result;
}

/** 检查 lookahead 与 token 是否配对
 *  配对失败则抛出一个 string 异常 
 *  配对成功则获取下一个 token
 */
void match(int token) {
    if (token != lookahead) {
        throw (string) "Unexpected token " + to_string(lookahead);
    }
    lookahead = lexan();
}