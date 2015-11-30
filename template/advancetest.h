#ifndef __ADVANCETEST
#define __ADVANCETEST
#include <iostream>
using namespace std;
struct ForwardTag{
};

struct BdirectionTag:public ForwardTag{
};

struct RandomTag:public BdirectionTag{
};

//user data type, including type tags
class Random{
public:
    typedef RandomTag Type;
};

class Forward{
public:
    typedef ForwardTag Type;
};

class Bdirection{
public:
    typedef BdirectionTag Type;
};

//overload function
template <typename T> void DoAdvance(T iter, int n, RandomTag)
{
    cout << "do random advance" << endl;
}

template <typename T> void DoAdvance(T iter, int n, ForwardTag)
{
    cout << "do forward advance" << endl;
}

template <typename T> void DoAdvance(T iter, int n, BdirectionTag)
{
    cout << "do bdirection advance" << endl;
}

//T is in Random, Forward, Bdirection
template <typename T> void Advance(T iter, int n)
{
    DoAdvance(iter, n , typename T::Type());        
}
#endif
