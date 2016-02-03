#include<iostream>
#include<string>

using namespace std;
void test(const string & t)
{
    //can compile,modify the copy
    static_cast<string>(t) = "aaappp";
}
int main(int argc, char** argv)
{
    const string name = "hello,world";
    static_cast<string>(name) = "ppp";//ok,modify the copy
    cout << name << endl;

    test(name);

    return 0;
}
