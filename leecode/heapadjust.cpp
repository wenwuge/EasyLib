#include<iostream>
#define LEFT(i) (2*i)
#define RIGHT(i) (2*i + 1)
void HeapAdjust(int * arr, int i ,int len)
{
    int val = arr[i];
    
    while(i < len){
        int higher = val;
        int index = i;
        if(LEFT(i) <= len && RIGHT(i) <= len){
            if(arr[LEFT(i)] < arr[RIGHT(i)]){
                higher = RIGHT(i);
                index = RIGHT(i)
            }else{
                tmp = LEFT(i);
                index = LEFT(i);
            }


        }else if(LEFT(i) <= len){
            higher = arr[LEFT(i)];
            index = LEFT(i);
        }
        
        if(higher > val){
            swap(arr[i], arr[index]);
            i = index;
        }else{
            return;
        }

    }
}
