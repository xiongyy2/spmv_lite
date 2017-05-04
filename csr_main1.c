#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<math.h>
#include<sys/time.h>

#include "sparse_formats.h"

double dtime();


void spmv_csr_cpu(const csr_matrix* csr,const float* x,const float* y,float* out);
//void spmv_csr_acc(const csr_matrix* csr,const float* x,const float* y,float* out);

int main(int argc, char **argv)
{

}

double dtime()
{
    double tseconds = 0.0;
    struct timeval mytime;
    gettimeofday(&mytime, (struct timezone*)0);
    tseconds = (double)(mytime.tv_sec + mytime.tv_usec* 1.0e-6);
    return (tseconds);
}