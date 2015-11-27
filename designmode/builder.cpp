#include <iostream>
using namespace std;
class Product{
};

class Builder{
public:
    virtual void BuildHead() = 0;
    virtual void BuildBody() = 0;
    virtual void BuildFeet() = 0;
    virtual void BuildHand() = 0;
};

class FatPerson: public Builder{
public:
    virtual void BuildHead()
    {
    }
    virtual void BuildBody()
    {
    }
    virtual void BuildFeet()
    {
    }
    virtual void BuildHand()
    {
    }
    
    FatPerson(Product* product):product_(product){}
private:
    Product *product_;
};

class ThinPerson: public Builder{
public:
    virtual void BuildHead()
    {
    }
    virtual void BuildBody()
    {
    }
    virtual void BuildFeet()
    {
    }
    virtual void BuildHand()
    {
    }
    ThinPerson(Product* product):product_(product){}
private:
    Product * product_;
};

class Director{
public:
    void Build(Builder* builder)
    {
        builder->BuildHead();
        builder->BuildBody();
        builder->BuildFeet();
        builder->BuildHand();
    }

};

int main(int argc, char** argv)
{
    Product Product1;
    Product Product2;
    
    FatPerson fatbuilder(&Product1);
    ThinPerson thinbuilder(&Product2);

    Director director;
    director.Build(&fatbuilder);
    director.Build(&thinbuilder);
    return 0;
}
