#include <iostream> 
#include "Strops.h"
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <map>
#include <vector>
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

void EasyString::Format(string& str, const char* szFmt, ...)
{
	va_list argList;
	int len;
	char* buf;

	va_start( argList, szFmt );
	len = 1024;

	buf = new char[len];
	bzero(buf, len);

	vsprintf(buf, szFmt, argList);
	str = string(buf, len - 1);

    delete[] buf;

	va_end( argList );
}

void EasyString::TrimLeft(string& str, const string &strTarget/* =" \r\n\t" */)
{
	string::size_type pos = str.find_first_not_of(strTarget);
	if(pos != string::npos)
    {
		str.assign(str.substr(pos));
	}
    else
    {
		str.clear();
	}
}

void EasyString::TrimRight(string& str, const string &strTarget/* =" \r\n\t" */)
{
	string::size_type pos = str.find_last_not_of(strTarget);	
	if(pos != string::npos)
    {
		str.assign(str.substr(0,pos+1));
	}
    else
    {
		str.clear();
	}
}

void EasyString::Trim(string& str, const string &strTarget/* =" \r\n\t" */)
{
	TrimLeft(str, strTarget);
	TrimRight(str, strTarget);
}

void EasyString::Tokenize(const string& str, vector<string>& tokens, const string& delims)
{
	//ensure the tokens vector is empty.
	tokens.clear();
	string::size_type last_pos,pos;	
	pos = last_pos = 0;	

	do
    {
		last_pos = pos;
		last_pos = str.find_first_not_of(delims,last_pos);
		if(last_pos == string::npos)
        {
			break;
		}
		pos = str.find_first_of(delims, last_pos);
        string token;
		if(pos != string::npos){
            token = str.substr(last_pos, pos-last_pos);
        }else{
            token = str.substr(last_pos, str.size() - last_pos);
        }
		tokens.push_back(token);
	}while(pos != string::npos);
}

bool EasyString::EndsWith(const string& str, const string& endStr)
{
	string::size_type pos = str.find(endStr,str.length() - endStr.length());
	return pos != string::npos;
}

bool EasyString::StartsWith(const string& str, const string& startStr)
{
	string::size_type pos = str.find(startStr);
	return pos == 0;
}

string EasyString::ToString(int a)
{
	char buf[64];
	sprintf(buf, "%d", a);
	string strRet = buf;
	return strRet;
}

void EasyString::ReplaceAll(string& str, const string& strToReplaced, const string& strReplaced)
{
	string strTemp;
	string::size_type pos, tpos = 0;
	bool bHasReplaced = false;

	pos = str.find(strToReplaced);
	while(pos != string::npos) 
    {
		strTemp += str.substr(tpos, pos - tpos) + strReplaced;		
		tpos = pos + strToReplaced.length();
		pos = str.find(strToReplaced, tpos);
		bHasReplaced = true;
	}

	if(bHasReplaced) 
    {
		strTemp += str.substr(tpos);
		str = strTemp;
	}
}

void EasyString::ToLowerCase(string& str)
{
	for(size_t i=0; i<str.length(); ++i) 
    {
		char ch = str.at(i);
		if(ch >= 'A' && ch <= 'Z')
			str.at(i) = ch + 'a' - 'A';	
	}
}

void EasyString::RemoveHtmlTags(string& strText)
{
	string::size_type pos = 0, end = 0;

	pos = strText.find("<", pos);
	while(pos != string::npos) 
    {
		end = strText.find(">", pos);
		if(end == string::npos)
			break;

		if(end-pos+1 < 1024) 
        {
			strText.replace(pos, end-pos+1, "");
			pos = strText.find("<", pos);
		}
        else 
        {
			pos = strText.find("<", end+1);
		}
	}
}

float EasyString::CalStringSim(const char* s1, const char* s2, int n)
{
	if(!s1 || *s1==0 || !s2 || *s2==0)//NULL or empty string
		return 0;

	int len1=strlen(s1);
	int len2=strlen(s2);
	int common=0;
	if(len1<n && len1<=len2)
	{
		//common=(count_ngram(s2, s1, len1)>0?1:0);
		common=((strstr(s2, s1)==NULL)?0:1);
		return (float(common))/(len2-len1+1);
	}
	if(len2<n && len2<=len1)
	{
		common=((strstr(s1, s2)==NULL)?0:1);
		return (float(common))/(len1-len2+1);
	}       
	typedef map<string, int> ngram_map;
	ngram_map imap; 
	ngram_map::iterator itr;
	for(int i=0; i<len2-n+1; i++)
		imap[string(&s2[i], n)]++;
	for(int i=0; i<len1-n+1; i++)
	{
		itr=imap.find(string(&s1[i], n));
		if(itr!=imap.end())
		{
			common++;
			itr->second--;
			if(itr->second==0)
				imap.erase(itr);
		}               
	}                       
	return (float(2*common))/(len1+len2-2*n+2);
}

string EasyString::GetCurrTime()
{
	time_t a = time(0);
	char buf[32];

	if(ctime_r(&a, buf) == NULL)
		return "unknown time";

	string t = string(buf);
	return t.substr(0, t.length()-1);
}

void EasyString::SimpleTokenize(const string& str, vector<string>& tokens, const string& delims)
{
    tokens.clear();
    string::size_type p0 = 0, p1;

    while(true) 
    {
        p1 = str.find(delims, p0);
        if(p1 == string::npos) 
        {
            tokens.push_back(str.substr(p0));
            break;
        }
        else 
        {
            tokens.push_back(str.substr(p0, p1-p0));
            p0 = p1 + delims.length();
        }
    }
}

void EasyString::RemoveSubstr(string& songname, string sBegin, string sEnd)
{
    string::size_type p0, p1;

    p0 = songname.find(sBegin);
    if(p0 == string::npos)
        return;

    p1 = songname.rfind(sEnd);
    if(p1 == string::npos)
        return;
    p1 += sEnd.length();

    string s = songname.substr(p0, p1-p0);
    EasyString::ReplaceAll(songname, s, "");
}

bool EasyString::isChineseChar(char c1, char c2)
{
	return (unsigned char)c1>=0x80&&(unsigned char)c1<=0xFE&&(unsigned char)c2>=0x40&&(unsigned char)c2<=0xFE;
}

bool EasyString::isallspace(const string &str)
{
	return isallspace(str.c_str());
}

bool EasyString::isallspace(const char *str)
{
	while (*str != '\0')
	{
		if (!isspace(*str++))
			return false;
	}

	return true;
}

bool EasyString::isall(const char *str, int(*func)(int))
{
    while (*str != '\0')
    {
        if (!func(*str++))
            return false;
    }
    return true;
}

string EasyString::ExtractSubstr(string& src, string sBegin, string sEnd)
{
    string::size_type p = src.find(sBegin);
    if(p == string::npos)
        return "";
    p += sBegin.length();
    string::size_type q = src.find(sEnd, p);
    if(q == string::npos)
        return "";
    return src.substr(p, q-p); 
}


bool EasyString::FromHex(unsigned char& ret, unsigned char x)
{
    unsigned char y;
    if (x >= 'A' && x <= 'Z') y = x - 'A' + 10;
    else if (x >= 'a' && x <= 'z') y = x - 'a' + 10;
    else if (x >= '0' && x <= '9') y = x - '0';
    else
    {
        return false;
    }
    ret = y;
    return true;
}

bool EasyString::UrlDecode(std::string& ret, const std::string& str)
{
    ret = "";
    size_t length = str.length();
    for (size_t i = 0; i < length; i++)
    {
        if (str[i] == '%')
        {
            if (i + 2 > length)
            {
                return false;
            }
            bool isHok, isLok;
            unsigned char high;
            unsigned char low;
            isHok = FromHex(high, (unsigned char)str[++i]);
            isLok = FromHex(low, (unsigned char)str[++i]);
            if (!isHok || !isLok)
            {
                return false;
            }
            ret += high * 16 + low;
        }
        else
        {
            ret += str[i];
        }
    }

    return true;
}
