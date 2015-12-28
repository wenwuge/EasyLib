#include<boost/weak_ptr.hpp>
#include<boost/shared_ptr.hpp>
#include<iostream>

using namespace std;
class test{
public:
    ~test()
    {
        cout << "test destruct " << endl;        
    }
    void output()
    {
        cout << "output " << endl;
    }

};

int main(int argc, char** argv)
{
    boost::weak_ptr<test>  test_weak;
    boost::shared_ptr<test> test_shared(new test);

    test_weak = test_shared; 
    //dec ref count
    test_shared.reset();
    boost::shared_ptr<test> tmp = test_weak.lock();
    if(tmp){
        tmp->output();
    }else{
        cout << "share_ptr has gone" << endl;
    }
    return 0;
}
