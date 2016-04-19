#include<string>
#include<iostream>
#include<stack>
#include<vector>

using namespace std;
class Solution {
public:
        bool isValidSerialization(string preorder) 
        {
            vector<string> vec;
            string::size_type sep_pos, start_pos = 0;
            
            sep_pos = preorder.find(",", start_pos);
            while(sep_pos != string::npos){
                vec.push_back(preorder.substr(start_pos, sep_pos - start_pos));
                start_pos = sep_pos + 1;
                sep_pos = preorder.find(",", start_pos);
            }
            vec.push_back(preorder.substr(start_pos));

            stack<string> parent;  
            string cur;
            for(int i = 0; i < vec.size();i++){
                cur = vec[i];
                if(cur != "#"){
                    parent.push(cur);
                }else{ 
                    if(i != vec.size()-1 && parent.size() > 0){
                        parent.pop();
                    }else if(i != vec.size() -1){
                        return false;
                    }
                        
                }
            }

            if(parent.size() != 0)
                return false;
            return true;
        }
};


int main(int argc, char** argv)
{
    Solution s;
    string test = "9,3,4,#,#,1,#,#,2,#,6,#,#";
    cout << s.isValidSerialization(test)<< endl;
    return 0;

}
