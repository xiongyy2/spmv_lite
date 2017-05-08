#include "allthefunctions.h"

void spmv_csr_cpu(const unsigned long num_rows,const unsigned long num_cols,const unsigned long num_nonzeros,const unsigned long &row_ptr,const unsigned long &col_idx,const float &val,const float* x,const float* y,float* out)
{
    unsigned long row,row_start,row_end,jj;
	float sum = 0;
	for(row=0; row < num_rows; row++)
	{
		sum = y[row];
		row_start = row_ptr[row];
		row_end   = row_ptr[row+1];

		for (jj = row_start; jj < row_end; jj++){
			sum += val[jj] * x[col_idx[jj]];
		}
		out[row] = sum;
	}
}