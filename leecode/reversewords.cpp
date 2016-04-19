#inclsss<iostream>
#include<string.h>
using namespace std;
void reverseWord(char *begin, char* end)
{
    char tmp;
    while(begin < end){
        tmp = *begin;
        *begin = *end;
        *end = tmp;
        begin ++;
        end --;
    };
}

void skipempty(char *s)
{
    char *curr = s;
    bool exsit = false;

    while(*curr != '\0'){
        if(*curr !=' '){
            break;
        }
        exsit = true;
        curr++;
    }
    
    if(*curr == '\0'){
        *s = '\0';
        return;
    } 
    if(!exsit)
        return;
    while(*curr != '\0'){
    ss  *s++ = *curr++;
    }
    *s = '\0';
    
}

void reverseWords(char *s) {
    if(s==NULL)
        return;
    skipempty(s);
    if(strlen(s) <= 1)
        return;
     
    reverseWord(s, s + strlen(s) - 1);

    char* start = NULL;
    char* end = NULL;
    char* curr = s;
    while(*curr != '\0'){
        if(*curr == ' '){
            if(start){
                end = curr - 1 ;
                reverseWord(start, end);
                start = NULL;
                end = NULL;
            }

        }else{
            if(start == NULL){
                start = curr;
            }
            end = curr;

        }
        curr++;
    }

    reverseWord(start,end);
        
}
int main(int argc, char** argv)
{
    char tmp[] = "  a  b";
    reverseWords(tmp);
    std::cout << tmp << std::endl;
    return 0;
}
