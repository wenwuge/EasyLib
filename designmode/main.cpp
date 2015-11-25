#include "factory.h"
class Person{
};

class Hero: public Person{
};

int main(int argc, char** argv)
{
    Factories<Person> * instance = Factories<Person>::Instance();

    instance->RegisterFactory<Hero>("hero");
    
    Person* person = instance->GetProduct("hero");

    delete person;
    
}
