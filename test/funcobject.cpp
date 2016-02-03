#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
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

class Print{
    public:
        template<typename T> void operator()(const T& t)
        {
            cout << t << " ";
        }
};

class Sum{
    public:
        Sum():sum_(0){}
        void operator()(int t){
            sum_ += t;
        }

        int get(){
            return sum_; 
        }
    private:
        int sum_;
};
int main(int argc, char** argv)
{
    DelObject<Test> test_del;
    vector<Test*> vec;

    vec.push_back(new Test);
    vec.push_back(new Test);

    for_each(vec.begin(), vec.end(), test_del);

    vector<int> vec1(3,8);
    for_each(vec1.begin(), vec1.end(), Print());
    cout << endl;
    
    vector<string> vec2(3,"aaa");
    for_each(vec2.begin(), vec2.end(), Print());
    cout << endl;

    cout << for_each(vec1.begin(), vec1.end(), Sum()).get()<< endl;
    return 0;    
}
