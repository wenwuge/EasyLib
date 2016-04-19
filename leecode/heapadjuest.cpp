#include<iostream>

void HeapAdjust(int * arr, int i ,int len)
{
    int target = arr[i];
    
    int left = 2*i + 1;
    while(left <= len){

        int high = left;
        if(left < len && arr[left + 1] > arr[left])
            high = left + 1;

        if(arr[high] > arr[i]){
            swap(arr[high], arr[i]);
            left = 2*high + 1;
        }else{
            return;
        }
    }
}
