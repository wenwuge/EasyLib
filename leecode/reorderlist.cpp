#include<iostream>
#include<stdlib.h>
/**
 *  * Definition for singly-linked list.
 *  * struct ListNode {
 *  *     int val;
 *  *     ListNode *next;
 *  *     ListNode(int x) : val(x), next(NULL) {}
 *  * };
 *  */
 struct ListNode {
     int val;
     ListNode *next;
     ListNode(int x) : val(x), next(NULL) {}
 };
class Solution {
    public:
            void reorderList(ListNode* head) {
                if(!head || !head->next)
                    return;

                ListNode* onestep = head;
                ListNode* twostep = head;

                while(twostep&&twostep->next){
                    twostep = twostep->next->next;
                    onestep = onestep->next;
                }

                //form two list
                ListNode* first = head;
                ListNode* second = onestep->next;
                onestep->next = NULL;

                //reverse the second list
                if(!second)
                    return;

                ListNode* prev = NULL,*next = NULL;
                ListNode* cur = second;
                while(cur != NULL){
                    next = cur->next;
                    cur->next = prev;
                    prev = cur;
                    cur = next; 
                }

                second = prev;

                //merge two list
                ListNode* cur1 = first;
                ListNode* cur2 = second;

                while(cur1!=NULL&&cur2!=NULL){
                    ListNode* tmp1;
                    ListNode* tmp2;

                    tmp1 = cur1->next;
                    tmp2 = cur2->next;
                    cur1->next = cur2;
                    cur2->next = tmp1;

                    cur1 = tmp1;
                    cur2 = tmp2;

                }
            }
};
using namespace std;
int main(int argc, char** argv)
{
    Solution s;
    ListNode* head = new ListNode(1);
    ListNode* cur = head;
    
    ListNode* tail = new ListNode(2);
    cur->next = tail;
    cur = tail;

    tail = new ListNode(3);
    cur->next = tail;
    cur = tail;
    
    tail = new ListNode(4);
    cur->next = tail;
    cur = tail;

    tail = new ListNode(5);
    cur->next = tail;
    cur = tail;
    s.reorderList(head);

    cur = head;
    while(cur != NULL){
        cout << cur->val << " ";
        cur = cur->next;
    }
    return 0;
}
