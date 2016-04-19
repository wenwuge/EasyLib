#include <iostream>
#include <fstream>
#include <string>

using namespace std;
void ReadDataFromFileLBLIntoString()
{
    ifstream fin("typeid.cpp");  
    string s;  
    while( getline(fin,s) )
    {    
        cout << "Read from file: " << s << endl; 
    }
}

void ReadDataFromFileLBLIntoCharArray()
{
    ifstream fin("typeid.cpp"); 
    const int LINE_LENGTH = 100; 
    char str[LINE_LENGTH];  
    while( fin.getline(str,LINE_LENGTH) )
    {    
        cout << "Read from file: " << str << endl;
    }
}


int main(int argc, char** argv)
{
    ReadDataFromFileLBLIntoString();
    ReadDataFromFileLBLIntoCharArray();
    return 0;
}
