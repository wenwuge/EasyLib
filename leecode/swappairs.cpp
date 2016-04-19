#include<iostream>

node* SwapPairsNode(node* root)
{
    if(!root || !root->next)
        return root;

    node* head = NULL;
    node* first = root;
    node* second = root->next;
    node* prev = NULL;

    while(first&&second){
        first->next = second->next;
        second->next = first;
        if(head == NULL){
            head = second;
            prev = first;
        }else{
            prev->next = second;
            prev = first;
        }
    }

    if(first&&prev)
        prev->next = first;
    if(second&&prev)
        prev->next = second;
    return head;
}
