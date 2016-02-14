#include<iostream>
#include<algorithm>
#include<vector>

using namespace std;

int main(int argc, char** argv)
{
    int data[] = {1,2,3,3,3,4,5,6,8,9};
    vector<int> vec(data, data  + 9);

    sort(vec.begin(), vec.end());
    cout << count(vec.begin(), vec.end(), 3) << endl;

    if(find(vec.begin(), vec.end(), 6) != vec.end())
            cout << "find 6" << endl;

    if(binary_search(vec.begin(), vec.end(), 3))
        cout << "binary search find 6" << endl;

    //lower_bound, if exsit where? if not exsit, shall be where?
    //less or equal
    vector<int>::iterator it;
    it = lower_bound(vec.begin(), vec.end(), 7);
    cout << "it is " << *it << endl;
    
    it = upper_bound(vec.begin(), vec.end(), 5);
    cout << "it is " << *it << endl;

    pair<vector<int>::iterator, vector<int>::iterator> range;
    range = equal_range(vec.begin(), vec.end(), 3);
    cout << "have found 3, count is " << range.second - range.first 
        <<endl;
    
    range = equal_range(vec.begin(), vec.end(),5);
    cout << "have found 5, count is " << range.second - range.first 
        <<endl;


    return 0;
}
