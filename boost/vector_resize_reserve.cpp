#include <iostream>
#include <vector>

using namespace std;

int main(int argc, char ** argv)
{
    vector<int>  test;

    test.push_back(3);
    test.push_back(4);
    
    cout << test.size() << test[0] << " " << test.capacity() <<  endl;
    test.resize(6);
    cout << test.size() << test[0] << " " << test.capacity() <<  endl;

    test.reserve(102);
    cout << test.size() << test[0] << " "  << test.capacity() << endl;
}
