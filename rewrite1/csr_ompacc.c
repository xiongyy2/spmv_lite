#include"sparse_formats.h"

void spmv_csr_acc(const csr_matrix* csr,const float* x,const float* y,float* out)
{
    unsigned long row,row_start,row_end,jj;
	float sum = 0;
#pragma omp parallel 
	{
	#pragma omp for private(row, row_start, row_end, jj, sum)
		for(row=0; row < csr->num_rows; row++)
		{
			sum = y[row];
			row_start = csr->Ap[row];
			row_end   = csr->Ap[row+1];

			for (jj = row_start; jj < row_end; jj++){
				sum += csr->Ax[jj] * x[csr->Aj[jj]];
			}
			out[row] = sum;
		}
	}
}