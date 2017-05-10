#include "allthefunctions.h"

unsigned long binary_search1(size_t size,unsigned long*row_ptr,unsigned long bnd)
{
    unsigned long left=0;
    unsigned long right;
    right=size-1;
    while (left<=right)
    {
        unsigned long m;
        m=(unsigned long)floor((double)(left+right)/(double)2.0);
        //printf("m=%lu\n",m);
        if (row_ptr[m]<bnd)
        {
            left=m+1;
            //printf("left=%lu\n",left);
        }
        else if (row_ptr[m]>bnd)
        {
            right=m-1;
            //printf("right=%lu\n",right);
        }
        else
        {
            return m;
        }
    }
    if (right<0) right=0;
    return right;
}
void segmented_sum1(int* in, int* flag)
{
    int length;
    length=sizeof(*in);
    for (int i=0;i<length;i++)
    {
        if (flag[i])
        {
            int j=i+1;
            while ( (!flag[j]) && (j<length) )
            {
                in[i]=in[i]+in[j];
                j=j+1;
            }
        }
        else
        {
            in[i]=0;
        }
    }
}

void fast_segmented_sum1(float *in,int *seg_offset)
{
    int length=sizeof(*in);
    float* tmp1;
    tmp1=malloc(length*sizeof(float));
    memcpy(tmp1,in,length*sizeof(float));
    //inclusive prefix sum scan for *in
    float iprefixsum=0.0;
    for (int i=0;i<length;i++)
    {
        float temp1;
        temp1=in[i];
        iprefixsum=iprefixsum+temp1;
        in[i]=iprefixsum;
    }
    for (int i=0;i<length;i++)
    {
        in[i]=in[i+seg_offset[i]]-in[i]+tmp1[i];
    }
    free(tmp1);
}


void spmv_csr_acc(const unsigned long num_rows,const unsigned long num_cols,const unsigned long num_nonzeros,const unsigned long* row_ptr,const unsigned long* col_idx,const float* val,const float* x,const float* y,float* out)
{
    int omega=4;
    //int sigma=4;
    int sigma;
    int nz_row=ceil((double)num_nonzeros/(double)num_rows);
    if (nz_row<5) sigma=4;
    else sigma=nz_row*2;
    printf("omega=%d\nsigma=%d\n",omega,sigma);

    unsigned long p;//number of tiles
    p=(unsigned long)ceil((double)num_nonzeros/(double)omega/(double)sigma);
    printf("p=%lu\n",p);

    unsigned long p_cmplt;//number of complete tiles
    p_cmplt=(unsigned long)floor((double)num_nonzeros/(double)omega/(double)sigma);
    printf("p_cmplt=%lu\n",p_cmplt);

    unsigned long* tile_ptr;
    tile_ptr=long_new_array(p+1,"Heap Overflow! Cannot allocate space for tile_ptr\n");
    printf("tile_ptr allocated\n");

    char* tile_ptr_empty;
    tile_ptr_empty=malloc((p+1)*sizeof(char));
    memset(tile_ptr_empty,0,(p+1)*sizeof(char)); //0 if tile contains no empty row

//generating tile_ptr and tile_ptr_empty--------------
    unsigned long tid=0;
    unsigned long bnd=0;
    unsigned long rid=0;
#pragma omp parallel for private(tid, bnd)
    for (tid=0;tid<p+1;tid++)
    {
        bnd=tid*omega*sigma;
        tile_ptr[tid]=binary_search1(num_rows+1,row_ptr,bnd);
        //if (tile_ptr[tid]<0) tile_ptr[tid]=0;
    }
#pragma omp parallel for private(tid, rid)
    for (tid=0;tid<p;tid++)
    {
        for (rid=tile_ptr[tid];rid<tile_ptr[tid+1]+1;rid++)
        {
            if (row_ptr[rid]==row_ptr[rid+1])
            {
                tile_ptr_empty[tid]=1; //1 if tile contains empty row
                //break;
            }
        }
        if(tile_ptr_empty[tid]) printf("tile-%lu has empty row\n",tid);
    }
    printf("tile_ptr generated\n");
//generating bit_flag-----------------------------
    char* bit_flag;
    bit_flag=malloc((p_cmplt*omega*sigma)*sizeof(char));
    memset(bit_flag,0,(p_cmplt*omega*sigma)*sizeof(char));
    unsigned long i1;
#pragma omp parallel for private(i1)
    for (i1=0;i1<num_rows;i1++)
    {
        bit_flag[row_ptr[i1]]=1;//first nonzero entry of a row
    }
#pragma omp parallel for private(i1)
    for (i1=0;i1<p_cmplt;i1++)
    {
        bit_flag[i1*omega*sigma]=1;//first entry of each tile
    }
    printf("bit_flag generated\n");


#pragma omp parallel private(tid)
{
#pragma omp for schedule(dynamic)

    for (tid=0;tid<p_cmplt;tid++)//loop over complete tiles
    {
        //generating y_offset and seg_offset----------
        int* y_offset;
        int* seg_offset;
        y_offset=malloc(omega*sizeof(int));
        seg_offset=malloc(omega*sizeof(int));
        int* tmp_bit;
        tmp_bit=malloc(omega*sizeof(int));
        memset(tmp_bit,0,omega*sizeof(int));
        int i2=0;
        int j2=0;
        for (i2=0;i2<omega;i2++)
        {
            y_offset[i2]=0;
            for (j2=0;j2<sigma;j2++)
            {
                unsigned long index=tid*omega*sigma+i2*sigma+j2;
                y_offset[i2]=y_offset[i2]+bit_flag[index];
                if( tmp_bit[i2] || bit_flag[index] )
                {
                    tmp_bit[i2]=1;
                }
            }
            seg_offset[i2]=1-tmp_bit[i2];
        }
        //exclusive prefix sum scan y_offset
        int eprefixsum=0;
        int i3=0;
        for (i3=0;i3<omega;i3++)
        {
            int temp1;
            temp1=y_offset[i3];
            y_offset[i3]=eprefixsum;
            eprefixsum=eprefixsum+temp1;
        }
        //segmented_sum for seg_offset
        segmented_sum1(seg_offset,tmp_bit);
        free(tmp_bit);
        //printf("y_offset seg_offset generated\n");
        //generating empty_offset-------------------------
        if(tile_ptr_empty[tid])//if this tile has empty row
        {
            int* empty_offset;
            int size_empty_offset=0;
            int i4=0;
            for (i4=0;i4<(omega*sigma);i4++)
            {
                if(bit_flag[tid*omega*sigma+i4]) size_empty_offset+=1;
            }
            empty_offset=malloc(size_empty_offset*sizeof(int));
            int eid=0;
            int i5=0;
            int j5=0;
            for (i5=0;i5<omega;i5++)
            {
                for (j5=0;j5<sigma;j5++)
                {
                    unsigned long index=tid*omega*sigma+i5*sigma+j5;
                    if (bit_flag[index])
                    {
                        unsigned long idx=binary_search1(num_rows+1,row_ptr,index);
                        //if (idx<0) idx=0;
                        idx=idx-tile_ptr[tid];
                        empty_offset[eid]=idx;
                        //printf("tid=%lu, empty_offset[%d]=%lu\n",tid,eid,idx);
                        eid=eid+1;
                    }
                }
            }
            int i6=0;
            for (i6=0;i6<omega;i6++)
            {
                y_offset[i6]=empty_offset[y_offset[i6]];
            }
            printf("empty_offset generated\n");
        }
        
        float* tmp;
        tmp=malloc(omega*sizeof(float));
        memset(tmp,0,omega*sizeof(float));
        float* last_tmp;
        last_tmp=malloc(omega*sizeof(float));
        memset(last_tmp,0,omega*sizeof(float));
        int i7=0;
        int j7=0;
        int jj7=0;
        for (i7=0;i7<omega;i7++)
        {
            float sum=0;//first ignore y array
            for (j7=0;j7<sigma;j7++)
            {
                unsigned long ptr=tid*omega*sigma+i7*sigma+j7;
                //unsigned long ptr1=tid*omega*sigma+j*omega+i;
                sum=sum+val[ptr]*x[col_idx[ptr]];
                //check bit_flag[ptr]
                int seal_head=0;
                int seal_tail=0;
                for (jj7=0;jj7<j7+1;jj7++)
                {
                    if (bit_flag[tid*omega*sigma+i7*sigma+jj7])
                    {
                        seal_head=1;
                    }
                }
                for (jj7=j7+1;jj7<sigma;jj7++)
                {
                    if (bit_flag[tid*omega*sigma+i7*sigma+jj7])
                    {
                        seal_tail=1;
                    }
                }
                //if(ptr<num_nonzeros-sigma)
                /*if(i<omega-1)
                {
                    for (int jj=j+1;jj<sigma;jj++)
                    {
                        if (bit_flag[tid*omega*sigma+i*sigma+jj])
                        {
                            seal_tail=1;
                        }
                    }
                }
                else
                {
                    for (int jj=j+1;jj<sigma;jj++)
                    {
                        if (bit_flag[tid*omega*sigma+i*sigma+jj])
                        {
                            seal_tail=1;
                        }
                    }
                }*/
                int next_bit_flag=0;
                if (j7<sigma-1) next_bit_flag=bit_flag[ptr+1];
                else next_bit_flag=1;
                //if (i<omega-1) next_bit_flag=bit_flag[ptr+1];
                //else if(j<sigma-1) next_bit_flag=bit_flag[ptr+1];
                //else next_bit_flag=1;
                if (((!seal_head) && seal_tail && next_bit_flag) || ( (!seal_head) && (!seal_tail) && (next_bit_flag) ) )//end of a red sub-segment
                //if (((!seal_head) && seal_tail && next_bit_flag) )//end of a red sub-segment
                {
                    tmp[i7-1]=sum;
                    sum=0;
                }
                else if ( seal_head && seal_tail && next_bit_flag )//end of a green segment
                {
                    #pragma omp atomic
                    out[tile_ptr[tid]+y_offset[i7]]+=sum;//confirmed correct
                    y_offset[i7]=y_offset[i7]+1;
                    sum=0;
                }
            }
            int seal_head2=0;
            for (j7=0;j7<sigma;j7++)
            {
                if (bit_flag[tid*omega*sigma+i7*sigma+j7])
                {
                    seal_head2=1;
                }
            }
            if(seal_head2) last_tmp[i7]=sum; //end of a blue sub-segment
            //last_tmp[i]=sum;//end of a blue sub-segment
        }
        fast_segmented_sum1(tmp,seg_offset);
        
        int i8=0;
        for (i8=0;i8<omega;i8++)
        {
            last_tmp[i8]=last_tmp[i8]+tmp[i8];
            #pragma omp atomic
            out[tile_ptr[tid]+y_offset[i8]]+=last_tmp[i8]/*+y[tile_ptr[tid]+y_offset[i]]*/;
        }
        free(tmp);
        free(last_tmp);
    }
}
    if(p>p_cmplt)
    {
        //last incomplete tile
        unsigned long row,row_start,row_end,jj;
        float sum = 0;
        //tile_ptr[0 to p]
#pragma omp parallel for private(row, row_start, row_end, jj, sum)
        for(row=tile_ptr[p_cmplt]; row < num_rows; row++)
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
}