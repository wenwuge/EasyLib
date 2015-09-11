#ifndef STROPS_
#define STROPS_
#include <string>
#include <vector>
using namespace std;

class EasyString{   
    public:
       static int split(const string& str, const string& token, 
               vector<string>& res);

};
#endif
