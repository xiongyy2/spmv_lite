#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<math.h>
#include<sys/time.h>

#include "allthefunctions.h"


int main(int argc, char *argv[])
{
    int num_wg,verbosity = 0,do_print=0,do_affirm=0,do_mem_align=0,opt, option_index=0;
	unsigned long density_ppm = 500000;
	unsigned int N = 512,num_execs=1,num_matrices,i,ii,iii,j,k,num_wg_sizes=0,num_kernels=0;
	unsigned long start_time, end_time;
	//struct timeval *tv;
	char* file_path,*optptr;
	void* tmp;

	//The other arrays
	float *x_host , *y_host , /* *device_out[num_matrices],*/ *host_out;
	float *para_out;//store parallel result
	unsigned long max_row_len=0,max_col_len=0;

	unsigned long num_rows;
    unsigned long num_cols;
    unsigned long num_nonzeros;
	//unsigned long density_ppm;
	double density_perc,nz_per_row,stddev;
    unsigned long *row_ptr;
    unsigned long *col_idx;
    float *val;

/*
setting up parameters for default-----------------------------
*/
	k=0;
	i=0;
	ii=0;
	do_affirm=1;
    verbosity=1;
	do_print=1;
    num_matrices=1;
    //file_path="csrmatrix_R1_N1024_D5000_S01_17-5-7-14-39";
//---------------------------------------------------------
	file_path=(char*) malloc (99);
	FILE *file_in;
	file_in=fopen("input","r");
	check(file_in != NULL,"Cannot Open Parameter Input File");
	fscanf(file_in,"%s\n",file_path);
	if(verbosity) printf("file_path=%s\n",file_path);
	fscanf(file_in,"%d\n%d\n%d\n%u\n",&do_affirm,&verbosity,&do_print,&num_matrices);
	if(verbosity) printf("finished reading parameters from input file.\n");
	fclose(file_in);
	if(verbosity) printf("num_matrices=%u\n",num_matrices);


	//for test
	//do_print=1;

    //csr_matrix* csr = read_csr(&num_matrices,file_path);
	//read_csr_arrayonly(&num_matrices,&num_rows,&num_cols,&num_nonzeros,&density_ppm,density_perc,nz_per_row,stddev,row_ptr,col_idx,val,file_path);

	FILE* fp;
	long ir,jr,read_count;

	fp = fopen(file_path,"r");
	//check(fp != NULL,"sparse_formats.read_csr() - Cannot Open Input File");
	printf("csr matrix file opened\n");
	read_count = fscanf(fp,"%u\n\n",&num_matrices);
	if(verbosity) printf("now read_count=%u\n",read_count);
	//check(read_count == 1,"sparse_formats.read_csr() - Input File Corrupted! Read count for num_csr differs from 1");
	printf("starting allocation and reading data\n");

	for(jr=0; jr<num_matrices; jr++)
	{
		read_count = fscanf(fp,"%lu\n%lu\n%lu\n%lu\n%lf\n%lf\n%lf\n",&num_rows,&num_cols,&num_nonzeros,&density_ppm,&density_perc,&nz_per_row,&stddev);
		check(read_count == 7,"sparse_formats.read_csr() - Input File Corrupted! Read count for header info differs from 7");
		if(verbosity) printf("read parameters of the matrix.\n");

		read_count = 0;
		row_ptr = long_new_array(num_rows+1,"sparse_formats.read_csr() - Heap Overflow! Cannot allocate space for csr.Ap");
		printf("csr.Ap alloced\n");
		for(ir=0; ir<=num_rows; ir++)
			read_count += fscanf(fp,"%lu ",row_ptr+ir);
		check(read_count == (num_rows+1),"sparse_formats.read_csr() - Input File Corrupted! Read count for Ap differs from csr[j].num_rows+1");

		read_count = 0;
		col_idx = long_new_array(num_nonzeros,"sparse_formats.read_csr() - Heap Overflow! Cannot allocate space for csr.Aj");
		printf("csr.Aj alloced\n");
		for(ir=0; ir<num_nonzeros; ir++)
			read_count += fscanf(fp,"%lu ",col_idx+ir);
		check(read_count == (num_nonzeros),"sparse_formats.read_csr() - Input File Corrupted! Read count for Aj differs from csr[j].num_nonzeros");

		read_count = 0;
		val = float_new_array(num_nonzeros,"sparse_formats.read_csr() - Heap Overflow! Cannot allocate space for csr.Ax");
		printf("csr.Ax alloced\n");
		for(ir=0; ir<num_nonzeros; ir++)
			read_count += fscanf(fp,"%f ",val+ir);
		check(read_count == (num_nonzeros),"sparse_formats.read_csr() - Input File Corrupted! Read count for Ax differs from csr[j].num_nonzeros");
	}

	fclose(fp);

	if(verbosity) printf("finished reading csr_matrix from file.\n");

	if(do_print)
	{
		int row_count=0, nz_count=0;
		float val1;
		while(row_ptr[row_count+1]==nz_count)
			row_count++;
		for(int ind=0; ind<num_rows; ind++)
		{
			printf("[");
			for(int ind2=0; ind2<num_cols; ind2++)
			{
				if(ind == row_count && ind2 == col_idx[nz_count])
				{
					val1 = val[nz_count++];
					while(row_ptr[row_count+1] == nz_count)
						row_count++;
				}
				else
					val1 = 0.0;
				printf("%6.2f",val1);
			}
			printf("]\n");
		}
		printf("\n");
	}

	if(verbosity) printf("now ii=%u\n",ii);
    if(max_row_len < num_rows)
    {
        max_row_len = num_rows;
		if(verbosity) printf("max_row_len=%lu\n",max_row_len);
		y_host=float_new_array(num_rows,"y_host - Heap Overflow! Cannot allocate space for y_host");
		if(verbosity) printf("y_host alloced\n");
		para_out=float_new_array(num_rows,"para_out - Heap Overflow! Cannot allocate space for para_out");
		memset(para_out,0,num_rows*sizeof(float));
		if(verbosity) printf("para_out alloced\n");
        if(do_affirm)
        {
			host_out=float_new_array(num_rows,"host_out - Heap Overflow! Cannot allocate space for host_out");
			memset(host_out,0,num_rows*sizeof(float));
			if(verbosity) printf("host_out alloced\n");
        }
    }
    if(max_col_len < num_cols)
    {
        max_col_len = num_cols;
		x_host=float_new_array(num_cols,"x_host - Heap Overflow! Cannot allocate space for x_host");
		if(verbosity) printf("x_host alloced\n");
    }
    for(int i1 = 0; i1 < max_col_len; i1++)
	{
		x_host[i1] = rand() / (RAND_MAX + 1.0);
		if(do_print) printf("x[%d] = %6.2f\n",i1,x_host[i1]);
	}
	for(int i2 = 0; i2 < max_row_len; i2++)
	{
		//y_host[i2] = rand() / (RAND_MAX + 2.0);
		y_host[i2]=0.0;
		if(do_print) printf("y[%d] = %6.2f\n",i2,y_host[i2]);
	}

	if(verbosity) printf("Input Generated.\n");
//other arrays end

	double t0, t1;
	if(do_affirm)
	{
		t0=dtime();
		//spmv_csr_cpu(&csr[0],x_host,y_host,host_out);
		spmv_csr_cpu(num_rows,num_cols,num_nonzeros,row_ptr,col_idx,val,x_host,y_host,host_out);
		t1=dtime();
		printf("cpu time(s): %lf\n", t1-t0);
	}
	t0=dtime();
	//spmv_csr_acc(&csr[0],x_host,y_host,para_out);
	spmv_csr_acc(num_rows,num_cols,num_nonzeros,row_ptr,col_idx,val,x_host,y_host,para_out);
	t1=dtime();
	printf("acc/omp time(s): %lf\n", t1-t0);

	if(do_affirm)
	{
		float_array_comp(host_out,para_out,num_rows,i+1);
	}



    if(verbosity) printf("Released context\n");
//	free(kernel_files);
//	free(wg_sizes);
//	free(tv);
	free(x_host);
	if(verbosity) printf("freed x_host\n");
	free(y_host);
	if(verbosity) printf("freed y_host\n");
	if(do_affirm) free(host_out);
	if(verbosity) printf("freed host_out\n");
	free(para_out);
	if(verbosity) printf("freed para_out\n");
	//free_csr(csr,num_matrices);
	free(row_ptr);
	free(col_idx);
	free(val);
	free(file_path);
	if(verbosity) printf("freed csr\n");
	return 0;
}
