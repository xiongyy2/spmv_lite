#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<math.h>
#include<sys/time.h>

#include "allthefunctions.h"

void check(int b,const char* msg)
{
	if(!b)
	{
		fprintf(stderr,"error: %s\n\n",msg);
		exit(-1);
	}
}

void* int_new_array(const size_t N,const char* error_msg)
{
	void* ptr;
	int err;
	/*if (_deviceType == 3){
		err = posix_memalign(&ptr,ACL_ALIGNMENT,N * sizeof(int));
		check(err == 0,error_msg);
	}
	else{
		ptr = malloc(N * sizeof(int));
		check(ptr != NULL,error_msg);
	}*/
	ptr = malloc(N * sizeof(int));
	check(ptr != NULL,error_msg);
	return ptr;
}

void* long_new_array(const size_t N,const char* error_msg)
{
	void* ptr;
	int err;
	/*if (_deviceType == 3){
		err = posix_memalign(&ptr,ACL_ALIGNMENT,N * sizeof(int));
		check(err == 0,error_msg);
	}
	else{
		ptr = malloc(N * sizeof(int));
		check(ptr != NULL,error_msg);
	}*/
	ptr = malloc(N * sizeof(long));
	check(ptr != NULL,error_msg);
	return ptr;
}

void* float_new_array(const size_t N,const char* error_msg)
{
	void* ptr;
	int err;
	/*if (_deviceType == 3){
		err = posix_memalign(&ptr,ACL_ALIGNMENT,N * sizeof(float));
		check(!err,error_msg);
	}
	else{
		ptr = malloc(N * sizeof(float));
		check(ptr != NULL,error_msg);
	}*/
	ptr = malloc(N * sizeof(float));
	check(ptr != NULL,error_msg);
	return ptr;
}

void* float_array_realloc(void* ptr,const size_t N,const char* error_msg)
{
	int err;
	/*if (_deviceType == 3){
		if(ptr != NULL) free(ptr);
		err = posix_memalign(&ptr,ACL_ALIGNMENT,N * sizeof(float));
		check(!err,error_msg);
	}
	else{
		ptr = realloc(ptr,N * sizeof(float));
		check(ptr != NULL,error_msg);
	}*/
	ptr = realloc(ptr,N * sizeof(float));
	check(ptr != NULL,error_msg);
	return ptr;
}

void float_array_comp(const float* control, const float* experimental, const unsigned long N, const unsigned int exec_num)
{
	unsigned long j;
	float diff,perc;
	bool comprslt=1;
	for (j = 0; j < N; j++)
	{
		diff = experimental[j] - control[j];
		if(fabsf(diff) > .001)
		{
			perc = fabsf(diff/control[j]) * 100;
			fprintf(stderr,"Possible error on exec #%u, difference of %.3f (%.1f%% error) [control=%.3f, experimental=%.3f] at row %lu \n",exec_num,diff,perc,control[j],experimental[j],j);
			comprslt=0;
		}
	}
	if(comprslt) printf("computation correct!\n");
}

double dtime()
{
    double tseconds = 0.0;
    struct timeval mytime;
    gettimeofday(&mytime, (struct timezone*)0);
    tseconds = (double)(mytime.tv_sec + mytime.tv_usec* 1.0e-6);
    return (tseconds);
}

void read_csr_arrayonly(unsigned int* num_csr,unsigned long &num_rows, unsigned long &num_cols, unsigned long &num_nonzeros, unsigned long &density_ppm, double &density_perc,double &nz_per_row,double &stddev, unsigned long &rowp, unsigned long &coli, float &valu, const char* file_path)
{
    FILE* fp;
	long i,j,read_count;

	fp = fopen(file_path,"r");
	check(fp != NULL,"sparse_formats.read_csr() - Cannot Open Input File");

	read_count = fscanf(fp,"%u\n\n",num_csr);
	check(read_count == 1,"sparse_formats.read_csr() - Input File Corrupted! Read count for num_csr differs from 1");
	printf("csr strict alloced\n");

	for(j=0; j<*num_csr; j++)
	{
		read_count = fscanf(fp,"%lu\n%lu\n%lu\n%lu\n%lf\n%lf\n%lf\n",num_rows,num_cols,num_nonzeros,density_ppm,density_perc,nz_per_row,stddev);
		check(read_count == 7,"sparse_formats.read_csr() - Input File Corrupted! Read count for header info differs from 7");

		read_count = 0;
		rowp = long_new_array(num_rows+1,"sparse_formats.read_csr() - Heap Overflow! Cannot allocate space for csr.Ap");
		printf("csr.Ap alloced\n");
		for(i=0; i<=num_rows; i++)
			read_count += fscanf(fp,"%lu ",rowp+i);
		check(read_count == (num_rows+1),"sparse_formats.read_csr() - Input File Corrupted! Read count for Ap differs from csr[j].num_rows+1");

		read_count = 0;
		coli = long_new_array(num_nonzeros,"sparse_formats.read_csr() - Heap Overflow! Cannot allocate space for csr.Aj");
		printf("csr.Aj alloced\n");
		for(i=0; i<num_nonzeros; i++)
			read_count += fscanf(fp,"%lu ",coli+i);
		check(read_count == (num_nonzeros),"sparse_formats.read_csr() - Input File Corrupted! Read count for Aj differs from csr[j].num_nonzeros");

		read_count = 0;
		valu = float_new_array(num_nonzeros,"sparse_formats.read_csr() - Heap Overflow! Cannot allocate space for csr.Ax");
		printf("csr.Ax alloced\n");
		for(i=0; i<num_nonzeros; i++)
			read_count += fscanf(fp,"%f ",valu+i);
		check(read_count == (num_nonzeros),"sparse_formats.read_csr() - Input File Corrupted! Read count for Ax differs from csr[j].num_nonzeros");
	}

	fclose(fp);
}