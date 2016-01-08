#include <iostream>
#include <map>
using namespace std;
struct NoneArg;
template<typename B> class BaseFactory{
public:
    virtual B* Produce() = 0;

};

template <typename B, typename D, typename ARG1, typename ARG2> class Factory: public BaseFactory<B>{
public:
    Factory(ARG1 arg1, ARG2 arg2)
    {
        arg1_ = arg1;
        arg2_ = arg2;
    }
    
    B* Produce()
    {
        return new D(arg1_, arg2_);
    }
private:
    ARG1 arg1_;
    ARG2 arg2_;
};
template <typename B, typename D> class Factory<B,D,NoneArg,NoneArg>: public BaseFactory<B>{
public:
    Factory()
    {
         
    }
    
    B* Produce()
    {
        return new D;
    }
//private:
//    ARG1 arg1_;
//    ARG2 arg2_;
};

template <typename B, typename D, typename ARG1> class Factory<B,D,ARG1,NoneArg>: public BaseFactory<B>{
public:
    Factory(ARG1 arg1)
    {
         arg1_ = arg1;
    }
    
    B* Produce()
    {
        return new D(arg1_);
    }
private:
    ARG1 arg1_;
//    ARG2 arg2_;
};

template <typename B> class Factories {
public:
    template<typename T> void RegisterFactory(int type_id){
        factories_[type_id] = new Factory<B,T, NoneArg, NoneArg>();
                    
    }

    template<typename T , typename ARG1 > void RegisterFactory(int type_id, ARG1 arg1){
        factories_[type_id] = new Factory<B,T, ARG1, NoneArg>(arg1);
                    
    }

    template<typename T , typename ARG1, typename ARG2> void RegisterFactory(int type_id, 
                                        ARG1 arg1, ARG2 arg2){
        factories_[type_id] = new Factory<B,T, ARG1, ARG2>(arg1 , arg2);
                    
    }
    
    B* GetProduct(int type_id)
    {
        factories_[type_id]->Produce();
    }
private:
    map<int, BaseFactory<B>*> factories_;
};

class Base{
};

class Derive: public Base{
public:
    Derive()
    {
        cout << "derive construct" << endl;
    }
    
    Derive(int t)
    {
        cout << "arg1 call" << endl;
    }
};

class Derive1: public Base{
public:
    Derive1()
    {
        cout << "derive1 construct" << endl;
    }
    
    Derive1(int t, int m)
    {
        cout << "arg2 call" << endl;
    }
};

int main(int argc, char** argv)
{
    Factories<Base> factories;
    
    factories.RegisterFactory<Derive>(1);
    factories.RegisterFactory<Derive>(3, 2);
    factories.RegisterFactory<Derive1>(2);
    factories.RegisterFactory<Derive1>(4, 3 ,4);

    factories.GetProduct(1);
    factories.GetProduct(2);
    factories.GetProduct(3);
    factories.GetProduct(4);
    return 0;

}
