#include <vector>
#include <map>
#include <list>
#include <deque>
#include <set>
#include <algorithm>
#include <iostream>
#include <string>
#include <tr1/functional>
#include <iterator>
using namespace std;
template<class T>
struct plus2{
    void operator()(T&x)const
    {
        x+=2;
    }
};

void printval(const int& elem)
{
    cout << elem << " " ;
}

int cmp(int& ele, int i)
{
    return (ele > i);
}

int main(int argc, char** argv)
{
#if 0
    vector<int> vector1(8,3);
    vector1.assign(6,5);
    vector<int>::iterator iter = vector1.begin();
    vector1.insert(iter + 1, 6, 5);
    for(int i=0; i<vector1.size(); i++){
        cout << vector1[i] << " " ;
    }
    cout << endl;
    iter = vector1.begin();
    vector1.erase(iter, iter + 2);
    for(int i=0; i<vector1.size(); i++){
        cout << vector1[i] << " " ;
    }

    multimap<string ,string> mul_map;

    mul_map.insert(pair<string, string>("wenwuge", "manong"));
    mul_map.insert(pair<string, string>("wenwuge", "playgame"));
    mul_map.insert(pair<string, string>("xiaoniwa", "venus"));

    multimap<string, string>::iterator iter = mul_map.begin();

    for(; iter!= mul_map.end(); iter++){
        cout << iter->first << " : " << iter->second <<endl;
    }
    return 0;

    //althori test
    vector<int> vector1(8,3);
    deque<int> deque1(8,4);
    for_each(vector1.begin(), vector1.end(), plus2<int>());
    for_each(vector1.begin(), vector1.end(), printval);
    tr1::function<void(int&)> fun;
    fun = tr1::bind(printval, tr1::placeholders::_1);
    
    for_each(vector1.begin(), vector1.end(), fun);
    vector<int>::iterator iter;

    cout << endl;
    iter = find(vector1.begin(), vector1.end(), 5);
    if(iter != vector1.end()){
        cout << *iter << endl;
    }
    copy(vector1.begin(), vector1.end(), deque1.begin());
    for_each(deque1.begin(), deque1.end(), printval);
#endif
    int ia1[]={1,2,3,6,5,4};
    int ia2[]={8,7,9,1};

    vector<int> vec1(ia1, ia1 + 6);
    vector<int> vec2(ia2, ia2 + 4); 
    //test sort, merge, binary_search, for_each
    sort(vec1.begin(), vec1.end());
    for_each(vec1.begin(), vec1.end(), printval);
    cout << endl;
    sort(vec2.begin(), vec2.end());

    vector<int> vec3;
    vec3.assign(vec1.size() + vec2.size(), 12345);
    merge(vec1.begin(), vec1.end(), vec2.begin(), vec2.end(), vec3.begin());    
    for_each(vec3.begin(), vec3.end(), printval);
    cout << endl;
    //binary_search test
    cout<< "find 9:" <<binary_search(vec3.begin(), vec3.end(), 9) << endl;
    cout<< "find 10:" <<binary_search(vec3.begin(), vec3.end(), 10) << endl;


    //unique test, swap test
    unique(vec3.begin(), vec3.end());
    for_each(vec3.begin(), vec3.end(), printval);
    cout <<endl;
    swap(vec1, vec2);

    for_each(vec2.begin(), vec2.end(), printval);
    cout << endl; 
    //count, find, fill, copy, equal test
    cout << count(vec3.begin(), vec3.end(), 1) <<endl;
    cout << count(vec3.begin(), vec3.end(), 10) <<endl;
    cout << count_if(vec3.begin(), vec3.end(), tr1::bind(less<int>(), tr1::placeholders::_1, 7)) << endl;

    cout << *find(vec3.begin(), vec3.end(), 4) << endl;
    cout << *find_if(vec3.begin(), vec3.end(), tr1::bind(less<int>(), tr1::placeholders::_1, 3)) << endl;

    fill(vec1.begin(), vec1.end(), 9);
    for_each(vec1.begin(), vec1.end(), printval);
    cout << endl; 
    fill_n(vec2.begin(), 3 ,11);
    for_each(vec2.begin(), vec2.end(), printval);
    cout << endl;

    copy(vec1.begin(), vec1.end(), vec2.begin());
    for_each(vec2.begin(), vec2.end(), printval);
    cout << endl;

    copy(ia2 , ia2 + 4, vec1.begin());
    copy(ia2 , ia2 + 4, vec2.begin());
    cout << equal(vec1.begin(), vec1.end(), vec2.begin())<<endl;
    return 0;
}
