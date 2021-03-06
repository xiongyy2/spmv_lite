#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<math.h>
#include<sys/time.h>

#include "sparse_formats.h"

double dtime();

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

void spmv_csr_cpu(const csr_matrix* csr,const float* x,const float* y,float* out);
void spmv_csr_acc(const csr_matrix* csr,const float* x,const float* y,float* out);

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

int main(int argc, char *argv[])
{
    int num_wg,verbosity = 0,do_print=0,do_affirm=0,do_mem_align=0,opt, option_index=0;
	unsigned long density_ppm = 500000;
	unsigned int N = 512,num_execs=1,num_matrices,i,ii,iii,j,k,num_wg_sizes=0,num_kernels=0;
	unsigned long start_time, end_time;
	//struct timeval *tv;
	char* file_path,*optptr;
	void* tmp;

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



    csr_matrix* csr = read_csr(&num_matrices,file_path);
	if(verbosity) printf("finished reading csr_matrix from file.\n");

//The other arrays
	float *x_host , *y_host , /* *device_out[num_matrices],*/ *host_out;
	float *para_out;//store parallel result
	unsigned long max_row_len=0,max_col_len=0;
	if(verbosity) printf("new ii=%u\n",ii);
    if(max_row_len < csr[ii].num_rows)
    {
        max_row_len = csr[ii].num_rows;
		if(verbosity) printf("max_row_len=%lu\n",max_row_len);
        //y_host = float_array_realloc(y_host,csr[ii].num_rows,"csr.main() - Heap Overflow! Cannot Allocate Space for y_host");
		y_host=float_new_array(csr[ii].num_rows,"y_host - Heap Overflow! Cannot allocate space for y_host");
		if(verbosity) printf("y_host alloced\n");
		//para_out = realloc(para_out,sizeof(float)*max_row_len);
		//check(para_out != NULL,"csr.main() - Heap Overflow! Cannot Allocate Space for 'para_out'");
		//para_out=float_array_realloc(para_out,csr[ii].num_rows,"csr.main() - Heap Overflow! Cannot Allocate Space for 'para_out'");
		para_out=float_new_array(csr[ii].num_rows,"para_out - Heap Overflow! Cannot allocate space for para_out");
		if(verbosity) printf("para_out alloced\n");
        if(do_affirm)
        {
            //host_out = realloc(host_out,sizeof(float)*max_row_len);
            //check(host_out != NULL,"csr.main() - Heap Overflow! Cannot Allocate Space for 'host_out'");
			//host_out=float_array_realloc(host_out,csr[ii].num_rows,"csr.main() - Heap Overflow! Cannot Allocate Space for 'host_out'");
			host_out=float_new_array(csr[ii].num_rows,"host_out - Heap Overflow! Cannot allocate space for host_out");
			if(verbosity) printf("host_out alloced\n");
        }
    }
    if(max_col_len < csr[ii].num_cols)
    {
        max_col_len = csr[ii].num_cols;
        //x_host = float_array_realloc(x_host,csr[ii].num_cols,"csr.main() - Heap Overflow! Cannot Allocate Space for x_host");
		x_host=float_new_array(csr[ii].num_cols,"x_host - Heap Overflow! Cannot allocate space for x_host");
		if(verbosity) printf("x_host alloced\n");
    }
    for(int i1 = 0; i1 < max_col_len; i1++)
	{
		x_host[i1] = rand() / (RAND_MAX + 1.0);
		if(do_print) printf("x[%d] = %6.2f\n",i1,x_host[i1]);
	}
	for(int i2 = 0; i2 < max_row_len; i2++)
	{
		y_host[i2] = rand() / (RAND_MAX + 2.0);
		if(do_print) printf("y[%d] = %6.2f\n",i2,y_host[i2]);
	}

	if(verbosity) printf("Input Generated.\n");
//other arrays end

	double t0, t1;
	if(do_affirm)
	{
		t0=dtime();
		spmv_csr_cpu(&csr[0],x_host,y_host,host_out);
		t1=dtime();
		printf("cpu time(s): %lf\n", t1-t0);
	}
	t0=dtime();
	spmv_csr_acc(&csr[0],x_host,y_host,para_out);
	t1=dtime();
	printf("acc/omp time(s): %lf\n", t1-t0);

	if(do_affirm)
	{
		float_array_comp(host_out,para_out,csr[k].num_rows,i+1);
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
	free_csr(csr,num_matrices);
	if(verbosity) printf("freed csr\n");
	return 0;
}

double dtime()
{
    double tseconds = 0.0;
    struct timeval mytime;
    gettimeofday(&mytime, (struct timezone*)0);
    tseconds = (double)(mytime.tv_sec + mytime.tv_usec* 1.0e-6);
    return (tseconds);
}