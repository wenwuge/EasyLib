#include<iostream>

listnode* MergeTwoLists(listnode *l1, listnode *l2)
{
    if(l1 == NULL)
        return l2;
    
    if(l2 == NULL)
        return l1;

    listnode* head = NULL;
    listnode* prev = NULL;
    listnode* first = NULL;
    listnode* second = NULL;
    
    if(head == NULL){
        if(l1->val < l2->val){
            head = l1;
            prev = l1;
            first = l1->next;
            second = l2;
        }else{
            head = l2;
            prev = l2;
            first = l2->next;
            second = l1;
        ]
    }

    while(first&&second){
        if(first->val < second->val){
            prev = first;
            first = first->next; 
        }else{
            prev->next = second;
            prev = second;
            second = second->next;
        }
    }

    if(first)
        prev->next = first;
    if(second)
        prev->next = second;
    return head;
}
