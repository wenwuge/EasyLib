#include <stdio.h>

int is1[] = {9, 0, 8, 5, 7, 2, 6, 1, 4, 3};
int is2[10];

void merge(int low,int mid, int high)
{
    int i = low;
    int j = mid+1;
    int k = low;

    while (i <= mid && j <= high) {
        if(is1[i] <= is1[j]) // 此处为排序顺序的关键，用小于表示从小到大排序
            is2[k++] = is1[i++];
        else
            is2[k++] = is1[j++];
    }

    while (i<=mid) {
        is2[k++] = is1[i++];
    }

    while (j<=high) {
        is2[k++] = is1[j++];
    }

    for (i = low; i <= high; i++) {// 写回原数组
        is1[i] = is2[i];
    }
}

void merge_sort(int a,int b)// 下标，例如数组int is[5]，全部排序的调用为merge_sort(0,4)
{
    if (a < b) {
        int mid = (a + b) / 2;
        merge_sort(a, mid);
        merge_sort(mid + 1, b);
        merge(a, mid, b);
    }
}

int main(void)
{
    int i = 0;
    merge_sort(0, 9);

    for (i = 0; i < 10; i++) {
        printf("%d ", is1[i]);
    }
    printf("\n");

    return 0;
}

