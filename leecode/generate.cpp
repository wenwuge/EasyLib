#include<iostream>

void generate(int leftNum,int rightNum,string s,vector<string> &result)  
{
    if(leftNum == 0 && rightNum ==0){
        result.push_back(s);
        return;
    }

    if(leftNum > 0){
        generate(leftNum - 1, rightNum, s + '(',  result);
    }

    if(leftNum < rightNum){
        generate(leftNum, rightNum, s + ')', result);
    }

}
