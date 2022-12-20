/** 此文件实现计算器内的内置函数
 * Author:  Rina
 * Date:    2022/11/22
*/
#include <cmath>

double add(double a, double b) {
    return a + b;
}

double sub(double a, double b) {
    return a - b;
}

double mul(double a, double b) {
    return a * b;
}

double mdiv(double a, double b) {
    return a / b;
}

double msin(double x) {
    return sin(x);
}

double mcos(double x) {
    return cos(x);
}

double mtan(double x) {
    return tan(x);
}

double mpow(double a, double b) {
    return pow(a, b);
}

/* 求以 b 为底，a 为真数的对数值 */
double mlog(double b, double a) {
    return log(a) / log(b);
}

double arcsin(double x) {
    return asin(x);
}

double arccos(double x) {
    return acos(x);
}

double arctan(double x) {
    return atan(x);
}