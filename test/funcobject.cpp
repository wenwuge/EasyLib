#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

template<typename T> class DelObject{
    public:
       void  operator()(T* t){
            delete t;  
       }
};

class Test{
    public:
        ~Test(){
            cout<< "test destruct" << endl;
        }
};
int main(int argc, char** argv)
{
    DelObject<Test> test_del;
    vector<Test*> vec;

    vec.push_back(new Test);
    vec.push_back(new Test);

    for_each(vec.begin(), vec.end(), test_del);
    return 0;    
}
