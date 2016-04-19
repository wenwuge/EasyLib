#include<stack>

bool valid(string str)
{
    if(str.size() == 0)
        return true;

    stack<char> s;
    int i = 0;
    char expect ;
    while(i != str.length()){
        if(str[i] == '[' || str[i] == '{' || str[i] == '('){
            s.push(str[i]);
            i++;
            continue;
        }

        if(str[i] == ']')
            expect = '[';
        else if(str[i] == '}')
            expect = '{';
        else if(str[i] == ')')
            expect = '(';
        else
            return false;

        if(s.empty())
            return false;

        if(s.top() != expect)
            return false;
        else
            s.pop();

    }

    if(s.empty())
        return true;
    else
        return false;

}
