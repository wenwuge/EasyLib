#include <iostream>
using namespace std;

class Modle{
public:
    virtual void Start() = 0;
    virtual void Runing() = 0;
    virtual void Run()
    {
        Start();
        Runing();
    }
        
};


class Modle1: public Modle{
public:
    virtual void Runing()
    {
        cout << "Runing" << endl;
    }
    
    virtual void Start()
    {
        cout << "Start" << endl;
    }
};


int main(int argc, char** argv)
{
    Modle1 mod1;

    mod1.Run();
    return 0;
}
