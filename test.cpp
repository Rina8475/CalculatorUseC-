/** 此文件对此程序中的各个板块进行测试 
 * Author:  Rina
 * Date:    2022/11/22
 */
#include <iostream>
#include <cassert>
#include <string>
#include "buildin.h"
#include "flags.h"
#include "function.h"

using std::cout;
using std::endl;
using std::string;
using std::to_string;

/**********************
 *  Symbol table 测试
 **********************/

/** 初始化环境 */
void define_global(Frame *env);

void test_env(void) {
    Node *node;
    Frame env(nullptr, 100);
    define_global(&env);

    /* test 1  测试正常的查询 */
    node = env.get_symbol("pow");
    assert(node->name == "pow");
    assert(node->flag == ENV_BUILDIN);
    cout << "Env_test: Test 1 passed" << endl;

    /* test 2 测试非法查询 */
    node = env.get_symbol("pi");
    assert(node->name == "pi");
    assert(node->flag == ENV_ILLEGAL);
    assert(node->val == nullptr);
    cout << "Env_test: Test 2 passed" << endl;

    /* test 3 测试插入 */
    env.define("pi", new double(3.14), ENV_DIGIT);
    node = env.get_symbol("pi");
    assert(node->name == "pi");
    assert(node->flag == ENV_DIGIT);
    assert(*(double *) node->val == 3.14);
    cout << "Env_test: Test 3 passed" << endl;

    /* test 4 测试重复插入 */
    env.define("sin", new double(2.48), ENV_DIGIT);
    node = env.get_symbol("sin");
    assert(node->name == "sin");
    assert(node->flag == ENV_DIGIT);
    assert(*(double *) node->val == 2.48);
    cout << "Env_test: Test 4 passed" << endl;
}

int main(void) {
    test_env();
}