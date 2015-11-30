#ifndef _FUNTEMPLATE
#define _FUNTEMPLATE
#include <iostream>
using namespace std;
//simple template function
template <typename T> void Swap(T& m, T& n)
{
    T i;
    i = n;
    n = m;
    m = i;
}
template <class V>  void PrintVal(V val)
{
    cout << val << endl;
}

//more complex template function ,one Class will be assigned
template <class M, class N> void Output(vector<M>&m, vector<N>& n)
{
    //function PrintVal type is assigned by manual
    for_each(m.begin(), m.end(), PrintVal<M>);
    for_each(n.begin(), n.end(), PrintVal<N>);
}

template <class T> class Sample{
    public:
        template <class U> void test(U val)
        {
            cout << "paras is : "<< val << endl;
        }
};
#endif
