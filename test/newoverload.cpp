#include<iostream>
#include<stdlib.h>
using namespace std;

void * operator new(size_t size)
{
    cout << "global new operator call" << endl;
    return malloc(size);
}

class A{
public:
    A(){
        cout<< "A construct " << endl;       
    }
//    static void * operator new(size_t size)
//    {
//        cout << "call new operator" << endl;
//        return malloc(size);
//    }
    static void  operator delete(void* ptr)
    {
        cout << "call delete operator" << endl;
        free(ptr);
    }
};

int main(int argc, char ** argv)
{
    A * test;

    test = new A;

    delete test;

    return 0;
}
