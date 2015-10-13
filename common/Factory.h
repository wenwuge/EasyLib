#ifndef FACTORY_
#define FACTORY_
#include <map>
#include <string>
#include <stdlib.h>

using namespace std;
//real producer
template <typename product,typename realproduct> class Producer{
    public:
        static product* GetProduct()
        {
            return new realproduct();
        }
};

//factory can produce various product
template <typename product> class Factory{
    
    public:
        typedef product*(*ProduceFunc)();
        static Factory& Instance()
        {
            static Factory<product> factory;
            return factory;
        }

        template <typename realproduct >
        void RegisterProducer(string name)
        {
            producers[name] = Producer<product, realproduct>::GetProduct;
        }

        product* Produce(string name)
        {
            if(producers.find(name) != producers.end()){
                return producers[name]();
            }else{
                return NULL;
            }
        }
    private:
        Factory()
        {
        }
        map<string, ProduceFunc> producers;

};

#if 0
//example:
class Person{
    int age;
};

class Student:public Person{
    int score;
};

int main(int argc, char** argv)
{
    Factory<Person> &factory = Factory<Person>::Instance();
    factory.RegisterProducer<Student>("student"); 
    factory.Produce("student");
    return 0;
    
}
#endif

#endif
