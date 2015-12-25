#include <boost/scoped_ptr.hpp>
#include <iostream>

using namespace std;
class test{
public:
~test(){
    cout << "test" <<endl;
}
};

class testuser{
public:
    testuser():test_ptr_(new test){}
private:
    boost::scoped_ptr<test> test_ptr_;
};
//test points:
//1.scoped_ptr in class
//2.scoped_ptr reset
int main(int argc, char** argv)
{
    //point 1 test
    testuser user;
    
    //point 2 test
    {
        cout << "enter zone" << endl;
        boost::scoped_ptr<test> test_ptr;
        test_ptr.reset(new test);
        test_ptr.reset();
        cout << "exit zone" << endl;
        
    }
    return 0;
}
