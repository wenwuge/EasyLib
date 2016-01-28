#include <iostream>
#include <list>
#include <vector>
#include <set>
#include <algorithm>

using namespace std;
typedef list<int> LInt;
typedef vector<int> VInt;

void output(const int& t)
{
    cout << t << " ";
}
int main(int argc ,char** argv)
{
    LInt  list_int;
    //for test inserter 
    for(int i = 0; i < 5; i++){
        list_int.push_back(i);
    }
   
    VInt  vec_int;
    for(int i = 5; i <10; i++){
        vec_int.push_back(i);
    }
    
    copy(list_int.begin(), list_int.end(), back_inserter(vec_int));
    for_each(vec_int.begin(), vec_int.end(), output);

    cout << endl;
    copy(vec_int.begin(), vec_int.end(), front_inserter(list_int));
    for_each(list_int.begin(), list_int.end(), output);

    cout << endl;
    //test swap function
    VInt(3, 6).swap(vec_int);
    for_each(vec_int.begin(), vec_int.end(), output);
    
    cout << endl;
    copy(list_int.begin(), list_int.end(), inserter(vec_int, vec_int.begin()  + 1));
    for_each(vec_int.begin(), vec_int.end(), output);

    //delete unused space
    VInt(vec_int).swap(vec_int);

    cout << endl;
    //test remove and erase,just delete one value
    vec_int.erase(remove(vec_int.begin(), vec_int.end(), 6), vec_int.end());
    for_each(vec_int.begin(), vec_int.end(), output);

    cout << endl;
    list_int.remove(6);
    for_each(list_int.begin(), list_int.end(), output);

    cout << endl;
    set<int> set_int;
    set_int.insert(1);
    set_int.insert(2);
    set_int.erase(2);
    for_each(set_int.begin(), set_int.end(), output);
    return 0;
}
