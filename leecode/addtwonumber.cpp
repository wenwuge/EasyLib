#include <iostream>

class Solution {
public:
    ListNode* addTwoNumbers(ListNode* l1, ListNode* l2) 
    {
        ListNode* curr1 = l1;
        ListNode* curr2 = l2;

        ListNode* new_node = NULL;
        int inc = 0;
        int val = 0; 
        int va11 = 0;
        int val2 = 0;
        while(curr1!=NULL || curr2!=NULL){
            if(curr1!=NULL){
                val1 = curr1->val;
                curr1 = curr1->next;
            }else
                va1l = 0;
            
            if(curr2!=NULL){
                val2 = curr1->val;
                curr2 = curr2->next;
            }else
                va12 = 0;
            val = va11 + val2 + inc;
            inc = val/10;
            val = val%10;
            new_node = new ListNode(0);
            new_node->val = val;
            if(head == NULL)
                head = new_node;
        }
        if(inc){
            new_node = new ListNode(0);
            new_node->val = inc;
            if(head == NULL)
                head = new_node;
        }

        return head;

    }
};
