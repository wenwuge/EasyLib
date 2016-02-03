#include<string>
#include<iostream>
#include<algorithm>
#include<vector>

using namespace std;
class Test{
public:
    Test(int val):val_(val){}
    void output(){ cout << val_<<endl;}
private:
    int val_;
};

int main(int argc, char** argv)
{
    vector<Test> test;
    test.push_back(1);
    test.push_back(2);
    test.push_back(3);
    for_each(test.begin(), test.end(), mem_fun_ref(&Test::output)); 

    vector<Test*> test1;
    test1.push_back(&test[0]);
    test1.push_back(&test[1]);
    test1.push_back(&test[2]);
    for_each(test1.begin(), test1.end(), mem_fun(&Test::output)); 
    return 0;

}
