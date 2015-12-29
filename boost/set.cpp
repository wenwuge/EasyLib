#include <set>
#include <iostream>

using namespace std;
int main(int argc, char** argv)
{
    set<int> test;
    test.insert(5);
    test.insert(6);
    test.insert(8);
    test.insert(10);

    set<int>::iterator lower = test.lower_bound(6);
    cout << "lower : " << *lower <<endl;
    set<int>::iterator higher = test.upper_bound(6);
    cout << "higher : " << *higher << endl;
    
    set<int>::iterator tmp = test.upper_bound(11);
    if(tmp == test.end()){
        cout << "11 " << "upper bound not found" << endl;
    }
    return 0;
}
