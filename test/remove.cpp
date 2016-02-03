#include<string>
#include<iostream>
#include<vector>
#include<algorithm>

using namespace std;
bool filter(const int& v)
{
    return v >= 6;
}

void output(const int&v)
{
    cout << v << " ";
}

int main(int argc, char** argv)
{
    int tmp[] = {1,2,3,4,6,1,3,7};
    vector<int> test(tmp, tmp + 8);
    vector<int> second;
#if 0
    remove_if(test.begin(), test.end(), filter);
#else
    cout << "orig cap is : " << second.capacity() << endl;
#if 1
    remove_copy_if(test.begin(), test.end(), back_inserter(second), filter);
#else     
    remove_copy_if(test.begin(), test.end(), second.begin(), filter);
#endif    
    cout << "cur cap is : " << second.capacity() << endl;
#endif
    for_each(test.begin(), test.end(), output);
    cout << endl;
    for_each(second.begin(), second.end(), output);
    cout << endl;

    return 0;
}
