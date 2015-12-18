#include <boost/any.hpp>
#include <iostream>
#include <string>
#include <vector>

using namespace boost;

int main(int argc, char** argv)
{
    //basic usage
    any a(100);

    std::cout << any_cast<int>(a) << ":" << a.type().name()<< std::endl;

    any b(std::string("hello"));

    std::cout << any_cast<std::string>(b) << ":" << b.type().name()<< std::endl;
    b = a;

    std::cout << "a" << ":" << a.type().name()<< std::endl;
    std::cout << "b" << ":" << b.type().name()<< std::endl;

    any c;
    c = std::string("wenwuge");
    std::cout << "c" << ":" << c.type().name()<< std::endl;

    //container
    std::vector<any> any_vector;
    any_vector.push_back(1);
    any_vector.push_back(std::string("hello"));

    
    return 0;
}
