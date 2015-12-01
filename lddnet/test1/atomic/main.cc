// main.cc (2013-07-16)
// Yan Gaofeng (yangaofeng@360.cn)

#include <iostream>

#include "ldd/util/atomic.h"
#include "ldd/util/atomic_int.h"

using namespace std;

int main()
{
    ldd::util::AtomicInt my_int;

    my_int++;
    cout << my_int << endl;
    my_int++;
    cout << my_int << endl;
    my_int--;
    cout << my_int << endl;


    ldd::util::Atomic<int64_t> my_int64(0);
    my_int64++;
    cout << my_int64 << endl;
    my_int64+=100;
    cout << my_int64 << endl;
    my_int64-=10;
    cout << my_int64 << endl;

    return 0;
}

