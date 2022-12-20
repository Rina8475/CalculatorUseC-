/** 内置函数的声明
 * Author:  Rina
 * Date:    2022/11/22
*/

/* 四则运算 */
double add(double a, double b);
double sub(double a, double b);
double mul(double a, double b);
double mdiv(double a, double b);

/* 三角运算 */
double msin(double x);
double mcos(double x);
double mtan(double x);

/* 反三角运算 */
double arcsin(double x);
double arccos(double x);
double arctan(double x);

/* 幂运算 */
double mpow(double a, double b);

/** 对数运算
 * 求以 b 为底，a 为真数的对数值 */
double mlog(double b, double a);