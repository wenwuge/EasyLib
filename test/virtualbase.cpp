#include <iostream>
#include <string>

using namespace std;
class VirtualAnimal{
    public:
        virtual ~VirtualAnimal() = 0;
        VirtualAnimal& operator=(const VirtualAnimal& other){
            cout << " virtualanimal operator = called" <<endl;
            age_ = other.age_;
        }
    private:
        int age_;
};

VirtualAnimal::~VirtualAnimal()
{
    cout << " virtual animal destruct" << endl;
}

class Animal: public VirtualAnimal{
    private:
        int sex_;
};

class Bird: public VirtualAnimal{
    private:
        std::string nick_;
};

int main(int argc, char** argv)
{
    
    Animal animal1;
    Animal animal2;

    VirtualAnimal* vanimal1 = &animal1;
    VirtualAnimal* vanimal2 = &animal2;

    *vanimal1 = *vanimal2;
    return 0;
};
