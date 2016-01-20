#include <vector>
#include <iostream>
#include <algorithm>
using namespace std;

void output(int& i)
{
    cout << i <<endl;
}

int main(int argc, char** argv)
{
    vector<int> first(10,1), second(10,2);
    //test back_inserter 
    copy(second.begin() , second.end() , back_inserter(first));
    for_each(first.begin(), first.end(), output);
    
    //test back_insert_iterator
    back_insert_iterator<vector<int> > back(first);
    *back++ = 500;
    *back = 600;
    
    for_each(first.begin(), first.end(), output);
    
    vector<int> third(3, 6), fourth(3, 8);
    third.insert(third.begin(), fourth.begin(), fourth.end());
    for_each(third.begin(), third.end(), output);
    third.insert(third.end(),  fourth.begin(), fourth.end());
    for_each(third.begin(), third.end(), output);
    return 0;
}
