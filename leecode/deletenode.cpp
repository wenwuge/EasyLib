#include<iostream>
struct TreeNode {
int val;
     TreeNode *left;
     TreeNode *right;
     TreeNode(int x) : val(x), left(NULL), right(NULL) {}
 };
class Solution {
    public:
    TreeNode* invertTree(TreeNode* root) {
        if(!root)
            return NULL;

        TreeNode* tmp;
        tmp = root->left;
        root->left = root->right;
        root->right = tmp;
        
        invertTree(root->left);
        invertTree(root->right);
        return root;
        
    }
};

int main(int argc, char** argv)
{
}
