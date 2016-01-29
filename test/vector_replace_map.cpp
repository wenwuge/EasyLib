#include <vector>
#include <string>
#include <iostream>
#include <map>
#include <algorithm>

using namespace std;
typedef pair<string ,int> Data;

class DataCompare{
public:
    bool operator()(const Data& lhs, const Data& rhs)const
    {
        return KeyLess(lhs.first, rhs.first);
    }

    bool operator()(const Data& lhs, const Data::first_type& k)const
    {
        return KeyLess(lhs.first, k);
    }

    bool operator()(const Data::first_type& k, const Data&rhs)const
    {
        return KeyLess(k, rhs.first);
    }
private:
    bool KeyLess(const Data::first_type &k1, const Data::first_type&k2)const
    {
        return k1 < k2;
    }
};
int main(int argc, char** argv)
{
    vector<Data>  tmp_vec;

    tmp_vec.push_back(make_pair("libin", 33));
    tmp_vec.push_back(make_pair("niwa", 32));
    tmp_vec.push_back(make_pair("xiaoke",1));

    sort(tmp_vec.begin(), tmp_vec.end(), DataCompare());

    if(binary_search(tmp_vec.begin(), tmp_vec.end(),"xiaoke", DataCompare())){
        cout << "find xiaoke " << endl;
    }
    return 0;
    
}
