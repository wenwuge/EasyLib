#include <iostream>
using namespace std;

class Target{
public:
    virtual void Request() = 0;
};

class ConcreteTarget:public Target{
public:
    virtual void Request()
    {
        cout << "common request send" << endl;
    }
};

class Adaptee{
public:
    void SpecialRequest()
    {
        cout << "special request send" << endl;
    }
};

class Adapter: public Target{
public:
    virtual void Request()
    {
        adaptee_->SpecialRequest();
    }

    Adapter(Adaptee* adaptee):adaptee_(adaptee){}
private:
    Adaptee * adaptee_;
    
};

int main(int argc, char** argv)
{
    Target * target = new ConcreteTarget();

    target->Request();

    Adaptee adaptee;
    Adapter * adapter = new Adapter(&adaptee);

    target = adapter;
    target->Request();
}
