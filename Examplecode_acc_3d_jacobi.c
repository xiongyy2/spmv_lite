#define Index3D(_nx,_ny,_i,_j,_k) ((_i)+(_nx)*((_j)+(_ny)*(_k)))

void acc_3d_jacobi(float *restrict A, float *restrict A_new, int m, int n, int z, int it)
{

    int total = (m+2)*(n+2)*(z+2);
    int i, j, k;
    int ii;
#pragma acc data copy(A[0:total],A_new[0:total])
    for(ii = 0; ii < it; ii++)
    {
#pragma acc data present(A[0:total],A_new[0:total]) 
{
#pragma acc parallel
    {
#pragma omp parallel for private(i, j, k)
#pragma acc loop 
        for(i = 1; i < z+1; i++)
        {
#pragma acc loop 
            for(j = 1; j < n+1; j++)
            {
#pragma acc loop 
                for(k = 1; k < m+1; k++)
                {
                    A_new[Index3D (m+2, n+2, k  , j  , i  )] =
                       (A[Index3D (m+2, n+2, k  , j  , i+1)] +
                        A[Index3D (m+2, n+2, k  , j  , i-1)] +
                        A[Index3D (m+2, n+2, k  , j+1, i  )] +
                        A[Index3D (m+2, n+2, k  , j-1, i  )] +
                        A[Index3D (m+2, n+2, k+1, j  , i  )] +
                        A[Index3D (m+2, n+2, k-1, j  , i  )])*0.17
                        - A[Index3D (m+2, n+2, k, j, i)]*0.1;
                }
            }
        }
    }
}

        float *temp = A;
        A = A_new;
        A_new = temp;
    }

}
