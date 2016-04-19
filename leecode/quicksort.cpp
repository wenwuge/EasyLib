#include<iostream>

int partition(int *arr, int begin, int end)
{
    int val = arr[begin];
    
    while(begin < end){
        while(begin < end){
            if(arr[end] > val)
                end --;
        }

        if(begin < end){
            arr[begin] = arr[end];
        }

        while(begin < end){
            if(arr[begin] < val){
                begin ++;
            }
        }
        if(begin < end){
            arr[end] = arr[begin];
        }
    }

    arr[begin] = val;
    return begin;
}
