#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<math.h>

void segmented_sum1(int* in, int* flag)
{
    int length;
    length=sizeof(*in);
    for (int i=0;i<length;i++)
    {
        if (flag[i])
        {
            int j=i+1;
            while ( (!flag[j]) && (j<length) )
            {
                in[i]=in[i]+in[j];
                j=j+1;
            }
        }
        else
        {
            in[i]=0;
        }
    }
}

int main(int argc, char *argv[])
{
    int omega=4;
    int tmp_bit[4]={1,1,0,1};
    int seg_offset[4]={0,0,1,0};
    segmented_sum1(seg_offset, tmp_bit);
    for (int i=0;i<omega;i++)
    {
        printf("%d\n",seg_offset[i]);
    }
    return 0;
}
