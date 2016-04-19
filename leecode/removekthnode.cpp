#include<iostream>

struct ListNode {
    int val;
    ListNode *next;
    ListNode(int x) : val(x), next(NULL) {}
};

class Solution {
    public:
    ListNode* removeNthFromEnd(ListNode* head, int n) {
        if(head == NULL || n < 0)
            return head;

        ListNode * curr = head;
        int i = 1;
        while(curr != NULL && i++ != n){
            curr = curr->next;
        }

        if(curr == NULL)
            return head;

        ListNode * sec = head;
        ListNode * prev = NULL;
        while(curr->next != NULL){
            prev = sec;
            sec = sec->next;
            curr = curr->next;
        }
        
        ListNode* newhead = head;
        if(sec == head){
            newhead = head->next;
            delete sec;
            return newhead;
        }
        
        prev->next = sec->next;
        delete sec; 
        return newhead;

    }
};
int main(int argc ,char** argv)
{
}
