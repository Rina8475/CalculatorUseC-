/** 此文件进行词法分析，将字符串流转换为 token 流
 * Author:  Rina
 * Date:    2022/11/20
 */
#include <iostream>
#include <string>
#include <cctype>
#include "flags.h"

/* 一个 name 的缓冲区的大小 */
#define BUFSIZE     256

using std::string;
using std::cin;
using std::isalpha;
using std::isdigit;

double tokenval;
string name;

/** 对输入字符流进行词法分析
 * 输入流中有 4 种 token 分别为 
 * DIGIT(float) 即浮点数 
 * SYMBOL       即 name 包括函数名和变量名
 * INSTRUCT     即 关键字 包括各种指令
 * 其余即为各种单字节字符 eg '+' '-' '(' ')' */
int lexan(void) {
    int c, i, token;
    double pow = 1.0, value = 0.0;
    char symbol[BUFSIZE];

    while((c = cin.get()) == ' ' || c == '\t');     /* 跳过空白符 */
    if (c == '#') {
    /* 如果是 INSTRUCT */
        for (i = 0; i < BUFSIZE-1 && isalpha(c = cin.get()); i += 1) {
            symbol[i] = c;
        }
        symbol[i] = '\0';
        name = symbol;
        token = INSTRUCT;
    } else if (isalpha(c)) {
    /* 如果是 SYMBOL */
        for (i = 0; i < BUFSIZE-1 && isalpha(c); i += 1) {
            symbol[i] = c;
            c = cin.get();
        }
        symbol[i] = '\0';
        name = symbol;
        token = SYMBOL;
    } else if (isdigit(c)) {
    /* 如果是 DIGIT */
        for (; isdigit(c); c = cin.get()) {
            value = value * 10 + (double) (c - '0');
        }
        if (c == '.') {
            c = cin.get();
        }
        for (; isdigit(c); c = cin.get()) {
            value = value * 10 + (double) (c - '0');
            pow *= 10.0;
        }
        tokenval = value / pow;
        token = DIGIT;
    } else {
    /* 否则其为单字符 */
        return c;
    }
    cin.putback(c);
    return token;
}