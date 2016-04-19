#include<iostream>
int partition(int *arr, int begin, int end)
{
    int target = arr[begin];

    int i = begin;
    int j = end;

    while(i < j){
        while(i < j){
            if(arr[j] > target)
                j --;
            else
                break;
        }
        if(i < j)
            a[i] = arr[j];

        while(i < j){
            if(arr[i] < target)
                i++;
            else 
                break;
        }
        if(i < j)
            arr[j] = arr[i];
    }

    arr[i] = target;
    return i;
}
