#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<math.h>

unsigned long binary_search1(size_t size,unsigned long*row_ptr,unsigned long bnd)
{
    unsigned long left=0;
    unsigned long right;
    right=size-1;
    while (left<=right)
    {
        unsigned long m;
        m=(unsigned long)floor((double)(left+right)/(double)2.0);
        printf("m=%lu\n",m);
        if (row_ptr[m]<bnd)
        {
            left=m+1;
            printf("left=%lu\n",left);
        }
        else if (row_ptr[m]>bnd)
        {
            right=m-1;
            printf("right=%lu\n",right);
        }
        else
        {
            return m;
        }
    }
    if (right<0) right=0;
    return right;
}

int main(int argc, char *argv[])
{
    unsigned long tile_ptr[4];
    unsigned long row_ptr[9];
    row_ptr[0]=0;
    row_ptr[1]=5;
    row_ptr[2]=7;
    row_ptr[3]=7;
    row_ptr[4]=14;
    row_ptr[5]=17;
    row_ptr[6]=19;
    row_ptr[7]=26;
    row_ptr[8]=34;
    for (unsigned long tid=0;tid<4;tid++)
    {
        unsigned long bnd;
        bnd=tid*16;
        tile_ptr[tid]=binary_search1(9,row_ptr,bnd);
        printf("%d\n",tile_ptr[tid]);
    }
    //free(tile_ptr);
    //free(row_ptr);
    return 0;
}