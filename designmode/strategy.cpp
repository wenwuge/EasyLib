#include "factory.h"
#include <iostream>

class Strategy{
public:
    virtual void Method() = 0;
};

class StrategyOne: public Strategy{
public:
    virtual void Method()
    {
        cout << "method1" << endl;
    }
};

class StrategyTwo: public Strategy{
public:
    virtual void Method()
    {
        cout << "method2" << endl;
    }
};

int main(int argc, char** argv)
{
    
    Factories<Strategy> * factories = Factories<Strategy>::Instance();
    
    factories->RegisterFactory<StrategyOne>("method1");  
    factories->RegisterFactory<StrategyTwo>("method2");  

    Strategy * method1 = factories->GetProduct("method1");
    Strategy * method2 = factories->GetProduct("method2");

    method1->Method();
    method2->Method();
    return 0;
}

