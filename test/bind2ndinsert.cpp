#include<iostream>
#include<vector>
#include<algorithm>
using namespace std;
class add:public  std::binary_function<int, int, int>{
public:
int operator()(const int& u, const int& v)const
{
   return u + v;
}
};
void output(const int& v)
{
    cout << v << endl;
}

int main(int argc, char** argv)
{
    int test[] = {1, 2, 3, 4};
    vector<int> vec(test, test + 4);
    vector<int> vec1;

    transform(vec.begin(), vec.end(), inserter(vec1,vec1.begin()), bind2nd(add(),5) );
    for_each(vec1.begin(), vec1.end(), output);

    return 0;
}
