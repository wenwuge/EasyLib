#include <iostream>
using namespace std;
class GiveGift{
public:
    virtual void GiveDoll() = 0;
    virtual void GiveFlower() = 0;
};

class Proxy: public GiveGift{
public:
    Proxy(GiveGift* givegift):lover_(givegift){}
    virtual void GiveDoll()
    {
        lover_->GiveDoll();
    }
    
    virtual void GiveFlower()
    {
        lover_->GiveFlower();
    }
private:
    GiveGift * lover_;
};

class Boy: public GiveGift{
public:
    Boy(){}

    virtual void GiveDoll()
    {
        cout << "boy give doll to girl" << endl;
    }
    
    virtual void GiveFlower()
    {
        cout << "boy give flower to girl" << endl;
    }

private:
};

int main(int argc, char** argv)
{

    Boy boy;
    Proxy proxy(&boy);

    proxy.GiveDoll();
    proxy.GiveFlower();
    return 0;
}
