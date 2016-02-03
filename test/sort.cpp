#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

template<typename T>void output(const T& t)
{
    cout << t << " " ;
}

bool partition_func(const int & v)
{
    if(v < 20)
        return true;
    else
        return false;
}
int main(int argc, char** argv)
{
    vector<int> vec;

    for(int i = 0; i < 100; i++)
        vec.push_back(i);

    random_shuffle(vec.begin(), vec.end());

    //for_each(vec.begin(), vec.end(), output<int>);
    sort(vec.begin(), vec.end());

    random_shuffle(vec.begin(), vec.end());
    partial_sort(vec.begin(), vec.begin() + 20, vec.end());
    for_each(vec.begin(), vec.begin() + 20, output<int>);

    cout << endl;
    random_shuffle(vec.begin(), vec.end());
    nth_element(vec.begin(), vec.begin()  + 20,vec.end());
    for_each(vec.begin(), vec.begin() + 20, output<int>);
    
    cout << endl;
    random_shuffle(vec.begin(), vec.end());
    partition(vec.begin(), vec.end(), partition_func);
    for_each(vec.begin(), vec.begin() + 20, output<int>);

    cout << endl;
    //use unique,must be order container
    vector<int> vec1;
    vec1.push_back(1);
    vec1.push_back(1);
    vec1.push_back(3);
    vec1.push_back(4);
    vec1.push_back(5);
    vec1.push_back(6);
    vec1.push_back(5);
    sort(vec1.begin(), vec1.end());
    vector<int>::iterator it = unique(vec1.begin(), vec1.end());
    vec1.erase(it, vec1.end());
    for_each(vec1.begin(), vec1.end(), output<int>);

    return 0;
}
