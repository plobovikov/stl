#include <iostream>
#include <string>

#include "../include/functor.hpp"

struct functional_object{
    void operator ()(const std::string& arg){
        std::cout << "I'm functional object! My arg = " << arg << std::endl;
    }
};

struct functional_object_2{
    void operator ()(int i, double d){
        std::cout << "I'm functional object 2! My args: int = " << i << " double = " << d << std::endl;
    }
};

void hello(){
    std::cout << "I am function pointer\n";
}

struct class_member_functions{
    void fun1(){
        std::cout << "I'm class member function!\n";
    }
    double fun2(double op1, double op2){
		std::cout << "I'm class member function! My args: op1 = " << op1 << " op2 = " << op2 << std::endl;
        return op1 + op2;
    }
};

int main()
{
    functor<void(std::string)>f((functional_object()));
    f("plobovikov");

    functor<void(int,double)> f2((functional_object_2()));
    f2(1,3.5);

    functor<void()> f3(&hello);
	f3();

    functor<std::string(std::string)>f4([](const std::string& arg){
        std::cout << "I am lyamda expression! My arg = " << arg << std::endl;
        return arg;
    });
	f4("plobovikov");

	class_member_functions member_functions;

    functor<void()>f5(&member_functions,&class_member_functions::fun1);
	f5();

    functor<double(double,double)>f6(&member_functions,&class_member_functions::fun2);
    std::cout << "f6 call result :" << f6(3.5,6.5);

    return 0;
}

