#include "Strops.h"
#include <iostream> 
using namespace std;
int EasyString::split(const string& str, const string& token, 
                vector<string>& res)
{
    string::size_type pos;
    int size = str.size();
    

    for(int i = 0 ; i < size; i++){
        string sub;
        pos = str.find(token, i);
        if (pos < size){
            sub = str.substr(i, pos - i);
            res.push_back(sub);
            cout << pos << " " << token.size() << endl;
            i = pos + token.size() -1;
        } else{
            sub = str.substr(i, size - i);
            res.push_back(sub);
            break;
        }
    }

    return 0;
}
