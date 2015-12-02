#include <stdio.h>

void merge(int array[], int start, int mid, int end)
{
    int i = 0;
    int j = 0;
    int k = 0;
    int temp1[10], temp2[10];
    int n1, n2;
    n1 = mid - start + 1;
    n2 = end - mid;

    //拷贝前半部分数组
    for (i = 0; i < n1; i++)
        temp1[i] = array[start + i];

    //拷贝后半部分数组
    for (i = 0; i < n2; i++)
        temp2[i] = array[mid + i + 1];

    //把后面的元素设置的很大
    temp1[n1] = temp2[n2] = 1000;

    //逐个扫描两部分数组然后放到相应的位置去
    for (k = start, i = 0, j = 0; k <= end; k++)
    {
        if (temp1[i] <= temp2[j]) {
            array[k] = temp1[i];
            i++;
        }
        else {
            array[k] = temp2[j];
            j++;
        }
    }
}

//归并排序
void merge_sort(int array[], int start, int end)
{
    if (start < end) {//递归的结束条件
        int i;

        i = (end + start) / 2;

        //对前半部分进行排序
        merge_sort(array, start, i);

        //对后半部分进行排序
        merge_sort(array, i + 1, end);

        //合并前后两部分
        merge(array, start, i, end);
    }
}

int main(void)
{
    int i = 0;
    int a[] = {5, 2, 7, 1, 4, 6, 3};

    merge_sort(a, 0, 6);

    for (i = 0; i < 7; i++) {
        printf("%d ", a[i]);
    }

    return 0;
}

