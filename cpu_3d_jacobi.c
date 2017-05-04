
#define Index3D(_nx,_ny,_i,_j,_k) ((_i)+(_nx)*((_j)+(_ny)*(_k)))

void cpu_3d_jacobi(float *A, float *A_new, int m, int n, int z, int it)
{
    int i, j, k;
    int ii;
    for(ii = 0; ii < it; ii++)
    {
        for(i = 1; i < z+1; i++)
        {
            for(j = 1; j < n+1; j++)
            {
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

        float *temp = A;
        A = A_new;
        A_new = temp;
    }

}
