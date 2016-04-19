#include <iostream>
#include <typeinfo>
using namespace std;
class base{
public:
    virtual void test(){}
};

class derived: public base{
};

int main(int argc, char** argv)
{
    derived b;
    base*   a = &b;

    cout << "typeid of a is " << typeid(a).name() << endl;
    cout << "typeid of *a is " << typeid(*a).name() << endl;
    cout << "typeid of b is " << typeid(b).name() << endl;
    return 0;
}

