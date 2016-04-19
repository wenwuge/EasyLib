#include<iostream>

int atoi(char* str)
{
    //'+','-'
    bool flag = false;
    long long num = 0;

    if(!str)
        return 0;
    //skip space
    while(*str == '')
        str++;
    
    if(*str == '-'){
        flag = true;
        str++;
    }else if(*str == '+'){
        str++;
    }

    while(*str != '\0'){
        if(!(*str>='0' && *str<='9'))
            return 0;
        num = num*10  + *str - '0';
        if(num > (int)0x7fffffff){
            if(flag)
                return  0x80000000;
            else
                return 0x7fffffff;
        }
    }
    
    if(flag)
        num = 0 -num;

    return num;
}
