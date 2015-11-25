#ifndef __FACTORY_MODE
#define __FACTORY_MODE
#include <string>
#include <map>
using namespace std;
//template will aviod more code write
//single factory
template<typename base, typename special> class Factory{
public:
   static base* Produce()
   {
       return new special();
   }
        
};

template<typename product> class Factories{
public:
    typedef product*(*ProduceProxy)();
    template<typename specialproduct> void RegisterFactory(string name)
    {
        factories_[name] = &Factory<product, specialproduct>::Produce;
    }
    
    product* GetProduct(string name)
    {
        typename map<string, ProduceProxy>::iterator iter;
        iter = factories_.find(name);
         
        if(iter != factories_.end()){
            return factories_[name]();
        }else{
            return NULL;
        }
    }
    
    static Factories * Instance()
    {
        if(!instance_){
            instance_ = new Factories();
        }
        return instance_;
    }

private:
    Factories(){}
    ~Factories(){}
    Factories(Factories& fac){}
    map<string, ProduceProxy> factories_;
    static Factories* instance_ ; 
};

template<typename T>  Factories<T>* Factories<T>::instance_ = NULL;

#endif
