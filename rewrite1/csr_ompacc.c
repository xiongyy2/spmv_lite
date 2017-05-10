#include "allthefunctions.h"

void spmv_csr_acc(const unsigned long num_rows,const unsigned long num_cols,const unsigned long num_nonzeros,const unsigned long* row_ptr,const unsigned long* col_idx,const float* val,const float* x,const float* y,float* out)
{
#pragma acc data copy(row_ptr[0:num_rows],col_idx[0:num_nonzeros-1],val[0:num_nonzeros-1],x[0:num_cols-1],y[0:num_rows-1],out[0:num_rows-1])
{
    unsigned long row,row_start,row_end,jj;
	float sum = 0;
#pragma omp parallel 
	{
	#pragma omp for private(row, row_start, row_end, jj, sum)
#pragma acc kernels loop independent
		for(row=0; row < num_rows; row++)
		{
			sum = y[row];
			row_start = row_ptr[row];
			row_end   = row_ptr[row+1];
#pragma acc loop
			for (jj = row_start; jj < row_end; jj++){
				sum += val[jj] * x[col_idx[jj]];
			}
			out[row] = sum;
		}
	}
}
}