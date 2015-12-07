#include <vector>
#include <algorithm>
#include <string>
#include "classtemplate.h"
#include "funtemplate.h"
#include "advancetest.h"
#include "messagefactory.h"
int main(int argc, char** argv)
{
#if 0
//function template test begin
    int m = 1, n = 2;
    
    Swap(m, n);
    cout << "m : " << m << " n : " << n << endl;
    
    vector<int> vec1(2, 8);
    vector<int> vec2(2, 7);
    //don't assign the type,auto regnize
    Output(vec1, vec2);

    //assign the type
    Output<int, int>(vec1, vec2);

    Sample<int> s;
    s.test(1);
    s.test<string>("hello,world");

//function template test end

//class template test begin
    Test<int ,string> test(1, "curel world"); 
    test.Out("wenwuge");

    TestDerived<int> TestDerived(6);
 
    Test<int, int> special_test;
    special_test.Output1();
    //return 0;

//overloads test
    Forward forward;
    Random  random;
    Bdirection bdirection;

    Advance(forward, 5); 
    Advance(random, 5); 
    Advance(bdirection, 5); 
#endif
//message factories
    MessageFactories<BaseMessage> factories;
    factories.Register<Message1>(1, "hello");
    factories.Register<Message2>(2, "hello", "world");

    factories.GetMessage(1);
    factories.GetMessage(2);
    return 0;
}
