#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<math.h>

int main(int argc, char *argv[])
{
    int num_row=2048;
    int num_len=2048;
    long num_nonzero=20489;
    unsigned long *row_ptr;
    unsigned long *col_idx;
    float *val;
    row_ptr=malloc((num_row+1) * sizeof(unsigned long));
    printf("row_ptr alloced %d\n",num_row+1);
    col_idx=malloc(num_nonzero * sizeof(unsigned long));
    printf("col_idx alloced %ld\n",num_nonzero);
    val=malloc(num_nonzero * sizeof(float));
    printf("val alloced %ld\n",num_nonzero);

    free(row_ptr);
    free(col_idx);
    free(val);
    printf("freed\n");

    return 0;
}