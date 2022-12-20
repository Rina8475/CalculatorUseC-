/** 此文件对语法树进行求值
 * Author:  Rina
 * Date:    2022/11/22
 */
#include <iostream>
#include <cassert>
#include "flags.h"
#include "buildin.h"
#include "function.h"

#define FLAG_IN_SET(flag, set)      (((flag) | (set)) == (set))

using std::to_string;

Pair *calculator_spform(string keyword, Pair *expr, Frame *env);
Pair *calculator_apply(Pair *procedure, Pair *args, Frame *env);
Pair *spform_define(Pair *expr, Frame *env);


string Pair::to_str(void) {
    if (flag == PTREE_NUMBER) {
        return to_string(*(double *) val);
    } else if (flag == PTREE_STRING) {
        return *(string *) val;
    } else if (flag == PTREE_PAIR) {
        return string("Pair Object");
    } else if (flag == PTREE_BUILDIN) {
        return "#Build-in: [" + ((BuildinFunction *) val)->name + "]";
    } else if (flag == PTREE_LAMBDA) {
        return "#Lambda: [" + ((LambdaFunction *) val)->name + "]";
    }
    return "to_string error";
}

/* 计算一个 Pair 对象的长度 */
int pair_len(Pair *ptr) {
    return ptr == nullptr ? 0 : ptr->len();
}

/** 检查输入参数是否全为指定类型
 * 如果不是 则抛出错误 */
void check_formals(Pair *formals, int expected_flag) {
    for (; formals != NULL; formals = formals->next) {
        if (!FLAG_IN_SET(formals->flag, expected_flag)) {
            throw(string("args errro: intput type is not expected type."));
        }
    }
}

/** 检查符号表中所查询到的值是否为所预期的值 
 * 如果不是 则抛出错误
 * P.S. expected_flag 是一个集合 */
void is_node_expect(Node *node, int expected_flag) {
    string lstr;
    if (node->flag == ENV_ILLEGAL) {
    /* 如果不存在此值 */
        lstr = node->name;
        delete node;            /* 释放对应节点 */
        throw string("Error: Don't find symbol " + lstr + "");
    } else if (!FLAG_IN_SET(node->flag, expected_flag)) {
        throw string("Error: Expected type is .. but you input ..");
    }
}

/** 计算求值函数    
 * 根据传入的环境 ENV 计算语法树 EXPR 的值
 * 返回值也为 Pair 节点  节点中的值为该语句计算后的值
 * 保证返回值节点为输入语法树的头节点 方便后续处理 */
Pair *calculator_eval(Pair *expr, Frame *env) {
    Node *elem;
    Pair *body, *value;
    if (expr->flag == PTREE_NUMBER) {
    /* 如果是一个数字 则返回其本身 */
        return expr;
    } else if (expr->flag == PTREE_STRING) {
    /* 如果是一个 symbol  则返回其值 */
        elem = env->get_symbol(*(string *) expr->val);
        is_node_expect(elem, ENV_DIGIT | ENV_FUNCTION);
        expr->val = elem->val;
        expr->flag = (elem->flag == ENV_DIGIT ? PTREE_NUMBER : elem->flag);
        return expr;
    } else {
        body = (Pair *) expr->val;
        if (body->flag == PTREE_STRING && *(string *) body->val == "define") {
        /* 如果是一个 spform */
            value = calculator_spform((string) "define", body->next, env);       
            expr->val = value->val;         /* 将返回值复制到头结点中 */
            expr->flag = value->flag;
            delete value;    delete body;
            return expr;
        }
        body = calculator_eval(body, env);         /* 对函数进行求值 */
        /* 此处先假设对 expr 的求值不会影响其 next 域 */
        if (FLAG_IN_SET(body->flag, PTREE_FUNCTION)) {
            value = body->next->map([env](Pair *x) -> Pair *{return calculator_eval(x, env);});
            value = calculator_apply(body, value, env);
            expr->val = value->val;         /* 将返回值复制到头结点中 */
            expr->flag = value->flag;
            delete value;
            return expr;
        }
    }
    throw string("Eval Error");
}

/** 对输入中的 spform 进行求值
 * 具体来说就是 define 关键字  文法设计中的关键字只有 define */
Pair *calculator_spform(string keyword, Pair *expr, Frame *env) {
    if (keyword == (string) "define") {
        return spform_define(expr, env);
    }
    throw string("Spform Error");
}

/** 对 define 语句进行求值 */
Pair *spform_define(Pair *expr, Frame *env) {
    Pair *symbol, *body, *value;
    LambdaFunction *func;
    string name;
    body = expr->next;
    
    if (expr->flag == PTREE_PAIR) {
    /* 函数定义 */
        symbol = (Pair *) expr->val;
        check_formals(symbol->next, PTREE_STRING);      /* 参数必须全为 string 类型 */
        name = *(string *) symbol->val;
        func = new LambdaFunction(symbol->next, body, env, name);
        env->define(name, func, ENV_LAMBDA);
        delete expr;        /* 释放 */
        return symbol;
    } else {
    /* 变量定义 */
        value = calculator_eval(body, env);
        assert(FLAG_IN_SET(value->flag, PTREE_FUNCTION | PTREE_NUMBER));
        env->define(*(string *) expr->val, value->val, 
            value->flag == PTREE_NUMBER ? ENV_DIGIT : value->flag);
        delete value;       /* 释放 body */
        return expr;
    }
}

/** 对函数调用进行求值 */
Pair *calculator_apply(Pair *procedure, Pair *args, Frame *env) {
    BuildinFunction *buildin;
    LambdaFunction *userdef;
    Pair *result;
    Frame *subenv;
    double val;
    if (procedure->flag == PTREE_BUILDIN) {
        buildin = (BuildinFunction *) procedure->val;
        val = buildin->apply(args);
        result = new Pair(new double(val), NULL, PTREE_NUMBER);
    } else if (procedure->flag == PTREE_LAMBDA) {
        userdef = (LambdaFunction *) procedure->val;
        subenv = userdef->make_call_frame(args);
        result = calculator_eval(userdef->get_body(), subenv);
    }
    procedure->free();
    return result;
}


/******************
 *  函数类设计
 ******************/

/* 输入的 len 为实参的长度 */
void Function::check_args_len(int len) {
    if (len != arg_len) {
        throw string("Formals Number Error: " + name + " dismatched amount.");
    }
}

/* 简化版本   仅确认实参是否全为 double 类型 */
void Function::check_args_type(Pair *args) {
    check_formals(args, PTREE_NUMBER);
}


BuildinFunction::BuildinFunction(function<double (double)> func, string name)
{
    this->singlefunc = func;
    this->arg_len = 1;
    this->name = name;
}

BuildinFunction::BuildinFunction(function<double (double, double)> func, string name)
{
    this->doublefunc = func;
    this->arg_len = 2;
    this->name = name;
}

BuildinFunction::~BuildinFunction()
{
}

/* 返回调用此函数后的结果 */
double BuildinFunction::apply(Pair *args) {
    double x, y, result;

    check_args_len(pair_len(args));
    check_args_type(args);
    /* 为了保证 buildin 模块的简洁，此处使用枚举方式 */
    if (arg_len == 1) {
        x = *(double *) args->get(0)->val;
        result = singlefunc(x);
    } else if (arg_len == 2) {
        x = *(double *) args->get(0)->val;
        y = *(double *) args->get(1)->val;
        result = doublefunc(x, y);
    }
    return result;
}


LambdaFunction::LambdaFunction(Pair *formals, Pair *body, Frame *env, string name)
{
    this->formals = formals;
    this->body = body;
    this->env = env;
    this->name = name;
    this->arg_len = pair_len(formals);
}

LambdaFunction::~LambdaFunction()
{
}

Frame *LambdaFunction::make_call_frame(Pair *args) {
    check_args_len(pair_len(args));
    check_args_type(args);
    return env->make_child_frame(formals, args);
}

/* 将函数体的复制返回 */
Pair *LambdaFunction::get_body(void) {
    return body->copy();
}


/*******************
 *   主程序部分     
 *******************/

extern Pair* parse(void);

/* 对全局环境进行预处理，即对全局变量的定义 */
void define_global(Frame *env) {
    env->define("+", new BuildinFunction(add, "+"), ENV_BUILDIN);
    env->define("-", new BuildinFunction(sub, "-"), ENV_BUILDIN);
    env->define("*", new BuildinFunction(mul, "*"), ENV_BUILDIN);
    env->define("/", new BuildinFunction(mdiv, "/"), ENV_BUILDIN);

    env->define("sin", new BuildinFunction(msin, "sin"), ENV_BUILDIN);
    env->define("cos", new BuildinFunction(mcos, "cos"), ENV_BUILDIN);
    env->define("tan", new BuildinFunction(mtan, "tan"), ENV_BUILDIN);

    env->define("arcsin", new BuildinFunction(arcsin, "arcsin"), ENV_BUILDIN);
    env->define("arccos", new BuildinFunction(arccos, "arccos"), ENV_BUILDIN);
    env->define("arctan", new BuildinFunction(arctan, "arctan"), ENV_BUILDIN);

    env->define("pow", new BuildinFunction(mpow, "pow"), ENV_BUILDIN);
    env->define("log", new BuildinFunction(mlog, "log"), ENV_BUILDIN);
}

void read_eval_print_loop(Frame *global_env) {
    while (true) {
        Pair *parse_tree, *result;
        try {
			std::cout << "> ";
            parse_tree = parse();
            result = calculator_eval(parse_tree, global_env);
            std::cout << result->to_str() << std::endl;
        } catch(string s) {
            std::cerr << s << std::endl;
        }
    }
}

int main(void) {
    Frame global_env(NULL, 100);
    define_global(&global_env);
    read_eval_print_loop(&global_env);
}