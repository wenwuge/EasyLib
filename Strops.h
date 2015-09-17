#ifndef STROPS_
#define STROPS_
#include <string>
#include <vector>
using namespace std;

class EasyString{   
    public:
        static int split(const string& str, const string& token, 
               vector<string>& res);

        static void Format(string& str, const char* szFmt, ...);

        static void TrimLeft(string& str, const string &strTarget=" \r\n\t");

        static void TrimRight(string& str, const string &strTarget=" \r\n\t");
        
        static void Trim(string& str, const string &strTarget=" \r\n\t");

        static void ReplaceAll(string& str, const string& strToReplaced, const string& strReplaced);

        static void Tokenize(const string& str, vector<string>& tokens, const string& delims);
        
        static void SimpleTokenize(const string& str, vector<string>& tokens, const string& delims);

        static bool EndsWith(const string& str, const string& endStr);

        static bool StartsWith(const string& str, const string& startStr);
        
        static string ToString(int a);

        static void ToLowerCase(string& str);

        static void RemoveHtmlTags(string& strText);
                   
        static float CalStringSim(const char* s1, const char* s2, int n);

        static string GetCurrTime();
         
        static void RemoveSubstr(string& songname, string sBegin, string sEnd);

        static bool isallspace(const string &str);

        static bool isallspace(const char *str);
      
        static bool isall(const char *str, int(*fuc)(int));
        
        static bool isChineseChar(char c1, char c2);
          
        static string ExtractSubstr(string& src, string sBegin, string sEnd);

        static bool FromHex(unsigned char& ret, unsigned char x);

        static bool UrlDecode(std::string& ret, const std::string& str);
};
#endif
