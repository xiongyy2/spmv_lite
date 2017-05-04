#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<math.h>
#include <sys/time.h>

#define Index3D(_nx,_ny,_i,_j,_k) ((_i)+(_nx)*((_j)+(_ny)*(_k)))
double dtime();

void cpu_3d_jacobi(float *A, float *A_new, int m, int n, int z, int it);
void acc_3d_jacobi(float *A, float *A_new, int m, int n, int z, int it);

#define EPSILON 0.1
bool AreSame(double a, double b)
{
     return fabs(a - b) < EPSILON;
}


bool check(float *A, float *dA, int m, int n, int z)
{
    int i, j, k;
    float err = 0;
    for(k = 0; k < z+2; k++) 
    {
        for(j = 0; j < n+2; j++) 
        {
            for(i = 0; i < m+2; i++) 
            {
                if(!AreSame(A[Index3D(m+2, n+2, i, j, k)],dA[Index3D(m+2, n+2, i, j, k)]))
                {
                    printf("%.6f->%.6f\n", fabs(A[Index3D(m+2, n+2, i, j, k)], dA[Index3D(m+2, n+2, i, j, k)]));
                    return false;
                }
            }
        }
    }
    return true;
}
int main(int argc, char **argv)
{
    int m = 512;
    int n = 512;
    int z = 512;
    int it = 100;

    int total = (m+2)*(n+2)*(z+2);
    float *A     = (float*)malloc(sizeof(float)*total);
    float *A_new = (float*)malloc(sizeof(float)*total);

    float *dA     = (float*)malloc(sizeof(float)*total);
    float *dA_new = (float*)malloc(sizeof(float)*total);
    int i, j, k;
    for(k = 0; k < z+2; k++) 
    {
        for(j = 0; j < n+2; j++) 
        {
            for(i = 0; i < m+2; i++) 
            {
                if(k==0 || j==0 || i==0 || k==z+1 || j==n+1 || i==m+1)
                {
                    A[Index3D(m+2, n+2, i, j, k)] = 0;
                    dA[Index3D(m+2, n+2, i, j, k)] = 0;
                    A_new[Index3D(m+2, n+2, i, j, k)] = 0;
                    dA_new[Index3D(m+2, n+2, i, j, k)] = 0;
                } else
                {
                    float tmp = ((float) rand() / (RAND_MAX)) + 1;
                    A[Index3D(m+2, n+2, i, j, k)] = tmp;
                    dA[Index3D(m+2, n+2, i, j, k)] = tmp;
                    A_new[Index3D(m+2, n+2, i, j, k)] = 0;
                    dA_new[Index3D(m+2, n+2, i, j, k)] = 0;
                }
                // printf("%.2f,", A[Index3D(m+2, n+2, i, j, k)]);
            }
            // printf("\n");
        }
        // printf("\n");
    }

 
    double t0, t1;
    t0 = dtime();
    cpu_3d_jacobi(A, A_new, m, n, z, it);
    t1 = dtime();
    printf("cpu time(s): %lf\n", t1-t0);
    
    t0 = dtime();
    acc_3d_jacobi(dA, dA_new, m, n, z, it);
    t1 = dtime();
    printf("acc/omp time(s): %lf\n", t1-t0);

    /*
    for(k = 0; k < z+2; k++) 
    {
        for(j = 0; j < n+2; j++) 
        {
            for(i = 0; i < m+2; i++) 
            {
                printf("%.2f,", dA_new[Index3D(m+2, n+2, i, j, k)]);
            }
            printf("\n");
        }
        printf("\n");
    }
    for(k = 0; k < z+2; k++) 
    {
        for(j = 0; j < n+2; j++) 
        {
            for(i = 0; i < m+2; i++) 
            {
                printf("%.2f,", A_new[Index3D(m+2, n+2, i, j, k)]);
            }
            printf("\n");
        }
        printf("\n");
    }
    */

    bool check_A, check_A_new;
    check_A = check(A, dA, m, n, z);
    if(check_A) printf("A is right!\n");
    else printf("A is wrong!\n");
    check_A_new = check(A_new, dA_new, m, n, z);
    if(check_A_new) printf("A_new is right!\n");
    else printf("A_new is wrong!\n");


    free(A);
    free(A_new);
    free(dA);
    free(dA_new);
}

double dtime()
{
    double tseconds = 0.0;
    struct timeval mytime;
    gettimeofday(&mytime, (struct timezone*)0);
    tseconds = (double)(mytime.tv_sec + mytime.tv_usec* 1.0e-6);
    return (tseconds);
}
