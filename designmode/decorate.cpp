#include <iostream>
using namespace std;
class Component{
public:
    virtual void Operation() = 0;
};

class ConcreteComponent: public Component{
public:
    virtual void Operation()
    {
        cout << "in concretecomponent" << endl;
    }
};

class Decorator: public Component{
public:
    Decorator(Component * component):component_(component)
    {
    }

    virtual void Operation()
    {
        component_->Operation();
    }
protected:
    Component* component_;

};

class ConcreteDecoratorA: public Decorator{
public:
    ConcreteDecoratorA(Component* component):Decorator(component){}
    virtual void Operation()
    {
        OperationA();
        Decorator::Operation();
    }
private:
    void OperationA()
    {
        cout<< "operation A" << endl;
    }
};

class ConcreteDecoratorB: public Decorator{
public:
    ConcreteDecoratorB(Component* component):Decorator(component){}
    virtual void Operation()
    {
        OperationB();
        Decorator::Operation();
    }
private:
    void OperationB()
    {
        cout<< "operation B" << endl;
    }
};

int main(int argc, char** argv)
{
    ConcreteComponent component;

    ConcreteDecoratorA decoratorA(&component);
    ConcreteDecoratorB decoratorB(&component);
    
    decoratorA.Operation();
    decoratorB.Operation();
    return 0;

}

