#include<iostream>
#include<stack>

using namespace std;

struct node{
    node* left;
    node* right;
    int val;
};

stack<node*> FindPath(node* root, int key)
{
    stack<node*> s;
    if(!root)
        return s;

    node* curr = root;
    
    while(curr != NULL){
       if(curr->val == key){
            return s; 
       }else{
           s.push(curr);
           curr = curr->left;
           
           while(curr){
                if(curr->val == key){
                    return s;
                }else{
                    s.push(curr);
                    curr = curr->left;
                }

           }

           if(!s.empty()){
                curr = s.top();
                curr = curr->right;
                s.pop();
           }

       }

    }
}
