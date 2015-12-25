#include <string>
#include <iostream>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/enable_shared_from_this.hpp>
using namespace std;
class implementation:public boost::enable_shared_from_this<implementation>
{
    public:
        ~implementation() { std::cout <<"destroying implementation\n"; }
        void do_something() 
        { 
            std::cout << "did something\n"; 
            do_detail();
        }
        void do_detail()
        {
            boost::shared_ptr<implementation>  p = shared_from_this();
            cout << p.use_count() << endl;
        }
};

void test()
{
        boost::shared_ptr<implementation> impl = boost::make_shared<implementation>();
        impl->do_something();
}

class Base{
public:
    virtual ~Base()
    {
        cout << "base destruct !" << endl;
    }


};

class Derive: public Base{
public:
    virtual ~Derive()
    {
        cout << "Derive destruct !" << endl;
    }
};

int main()
{
    std::cout<<"Test Begin ... \n";
    test();
    std::cout<<"Test End.\n";

    //test assign derived pointer 
    boost::shared_ptr<Base> base_ptr;
    base_ptr = boost::make_shared<Derive>();

    //test derived shared_ptr is assigned to base shared_ptr
    boost::shared_ptr<Derive> derive_ptr(new Derive);
    boost::shared_ptr<Derive> base_ptr1;
    base_ptr1 = derive_ptr;

    return 0; 

}
