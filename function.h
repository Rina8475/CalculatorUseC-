#include <string>
#include <functional>
#include "table.h"

class Function 
{
protected:
    int arg_len;
    void check_args_len(int len);
    void check_args_type(Pair *args);
public:
    string name;
};

class BuildinFunction : public Function
{
private:
    function<double (double)>           singlefunc;
    function<double (double, double)>   doublefunc;
public:
    BuildinFunction(function<double (double)> func, string name);
    BuildinFunction(function<double (double, double)> func, string name);
    ~BuildinFunction();
    double apply(Pair *);
};

class LambdaFunction : public Function
{
private:
    Pair *formals;
    Pair *body;
    Frame *env;
public:
    LambdaFunction(Pair *formals, Pair *body, Frame *env, string name);
    ~LambdaFunction();
    Frame *make_call_frame(Pair *args);
    Pair *get_body(void);
};