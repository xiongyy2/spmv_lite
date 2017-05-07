#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<math.h>

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


int main(int argc, char *argv[])
{
    int num_row=2048;
    int num_len=2048;
    long num_nonzero=20489;
    unsigned long *row_ptr;
    unsigned long *col_idx;
    float *val;
    //row_ptr=malloc((num_row+1) * sizeof(unsigned long));
    row_ptr=long_new_array(num_row+1,"sparse_formats.read_csr() - Heap Overflow! Cannot allocate space for row_ptr");
    printf("row_ptr alloced %d\n",num_row+1);
    //col_idx=malloc(num_nonzero * sizeof(unsigned long));
    col_idx=long_new_array(num_nonzero,"sparse_formats.read_csr() - Heap Overflow! Cannot allocate space for col_idx");
    printf("col_idx alloced %ld\n",num_nonzero);
    //val=malloc(num_nonzero * sizeof(float));
    val=float_new_array(num_nonzero,"sparse_formats.read_csr() - Heap Overflow! Cannot allocate space for val");
    printf("val alloced %ld\n",num_nonzero);

    free(row_ptr);
    free(col_idx);
    free(val);
    printf("freed\n");

    return 0;
}