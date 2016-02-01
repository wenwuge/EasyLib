#include <vector>
#include <iostream>
#include <algorithm>

using namespace std;
template<typename T> void output(const T& t)
//void output(const int&t)
{
    cout << t << " " ;
}

int main(int argc, char** argv)
{
    vector<int> vec(3,6);
    vector<int> vec1(3,8);

    copy(vec1.begin(), vec1.end(), vec.begin());
    vec.push_back(9);
    for_each(vec.begin() , vec.end(), output<int>); 


    cout << endl;
    for_each(vec.rbegin(), vec.rend(), output<int>);
    return 0;
}

