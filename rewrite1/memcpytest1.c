#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<math.h>

int main(int argc, char *argv[])
{
	int A[4]={1,2,3,4};
	int B[4]={5,6,7,8};
	memcpy(A,B,4*sizeof(int));
	for (int i=0;i<4;i++)
	{
		printf("A %d ",A[i]);
		printf("B %d ",B[i]);
	}
	return 0;
}
