#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<math.h>
#include<sys/time.h>


double dtime();
void check(int b,const char* msg);
void* int_new_array(const size_t N,const char* error_msg);
void* long_new_array(const size_t N,const char* error_msg);
void* float_new_array(const size_t N,const char* error_msg);
void* float_array_realloc(void* ptr,const size_t N,const char* error_msg);
void float_array_comp(const float* control, const float* experimental, const unsigned long N, const unsigned int exec_num);

void read_csr_arrayonly(unsigned int* num_csr,unsigned long num_rows, unsigned long num_cols, unsigned long num_nonzeros, unsigned long density_ppm, double density_perc,double nz_per_row,double stddev, unsigned long* rowp, unsigned long* coli, float* valu, const char* file_path);

void spmv_csr_cpu(const unsigned long num_rows,const unsigned long num_cols,const unsigned long num_nonzeros,const unsigned long* row_ptr,const unsigned long* col_idx,const float* val,const float* x,const float* y,float* out);
void spmv_csr_acc(const unsigned long num_rows,const unsigned long num_cols,const unsigned long num_nonzeros,const unsigned long* row_ptr,const unsigned long* col_idx,const float* val,const float* x,const float* y,float* out);