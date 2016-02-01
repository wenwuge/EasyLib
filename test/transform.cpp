#include<iostream>
#include<vector>
#include<string>
#include <algorithm>

using namespace std;
template<typename T> T trans(const T & t)
{
    return t*3;
}

template<typename T> T output(const T & t)
{
    cout << t << " ";
}

int main(int argc, char** argv)
{
    vector<int> vec(5, 6);
    vector<int> vec1;

    transform(vec.begin(), vec.end(), back_inserter(vec1), trans<int>);
    for_each(vec1.begin(), vec1.end(), output<int>);
}
