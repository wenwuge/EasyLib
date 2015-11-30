#ifndef CLASSTEMPLATE
#define CLASSTEMPLATE
#include <iostream>
using namespace std;
template <class T, class U> class Test{
public:
    Test(T v1 , U v2):val1_(v1),val2_(v2){}
    void Output()
    {
        cout << "val1_ : "<< val1_ << "val2_ : "<< val2_ << endl;
    }
    
    template <class V> void Out(V v3)
    {
        cout << "val3_ : " << v3 << endl;
    }
private:
    T val1_;
    U val2_;
};

//special Test template
template <> class Test<int ,int>{
    public:
    void Output1()
    {
        cout << "special test" << endl;
    }
};

template <class T> class TestDerived: public Test<T, string>{
public:
    TestDerived(T i):Test<T, string>(i, "hello,lixiaoke")
    {
    }
    void  CallOutput()
    {
        //for special class Test which has no Output function, the compiler can't goto the class Test to
        //find the Output, so we must assign this->Output to indicate that the base has the method
        this->Output();
    }
};
#endif
