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
    int sigma=4;

    unsigned long p;//number of tiles
    p=(unsigned long)ceil((double)num_nonzeros/(double)omega/(double)sigma);
    printf("p=%lu\n",p);

    unsigned long p_cmplt;//number of complete tiles
    p_cmplt=(unsigned long)floor((double)num_nonzeros/(double)omega/(double)sigma);
    printf("p_cmplt=%lu\n",p_cmplt);

    unsigned long* tile_ptr;
    tile_ptr=long_new_array(p+1,"Heap Overflow! Cannot allocate space for tile_ptr\n");
    printf("tile_ptr allocated\n");

    int* tile_ptr_empty;
    tile_ptr_empty=malloc((p+1)*sizeof(int));
    memset(tile_ptr_empty,0,(p+1)*sizeof(int)); //0 if tile contains no empty row

//generating tile_ptr and tile_ptr_empty--------------
    for (unsigned long tid=0;tid<p+1;tid++)
    {
        unsigned long bnd;
        bnd=tid*omega*sigma;
        tile_ptr[tid]=binary_search1(num_rows+1,row_ptr,bnd);
        //if (tile_ptr[tid]<0) tile_ptr[tid]=0;
    }
    for (unsigned long tid=0;tid<p;tid++)
    {
        for (unsigned long rid=tile_ptr[tid];rid<tile_ptr[tid+1]+1;rid++)
        {
            if (row_ptr[rid]==row_ptr[rid+1])
            {
                tile_ptr_empty[tid]=1; //1 if tile contains empty row
                break;
            }
        }
    }
    printf("tile_ptr generated\n");
//generating bit_flag-----------------------------
    int* bit_flag;
    bit_flag=malloc((p_cmplt*omega*sigma)*sizeof(int));
    memset(bit_flag,0,(p_cmplt*omega*sigma)*sizeof(int));
    for (unsigned long i=0;i<num_rows;i++)
    {
        bit_flag[row_ptr[i]]=1;//first nonzero entry of a row
    }
    for (unsigned long i=0;i<p_cmplt;i++)
    {
        bit_flag[i*omega*sigma]=1;//first entry of each tile
    }
    printf("bit_flag generated\n");



    for (unsigned long tid=0;tid<p_cmplt;tid++)//loop over complete tiles
    {
        //generating y_offset and seg_offset----------
        int* y_offset;
        int* seg_offset;
        y_offset=malloc(omega*sizeof(int));
        seg_offset=malloc(omega*sizeof(int));
        int* tmp_bit;
        tmp_bit=malloc(omega*sizeof(int));
        memset(tmp_bit,0,omega*sizeof(int));
        for (int i=0;i<omega;i++)
        {
            y_offset[i]=0;
            for (int j=0;j<sigma;j++)
            {
                unsigned long index=tid*omega*sigma+i*sigma+j;
                y_offset[i]=y_offset[i]+bit_flag[index];
                if( tmp_bit[i] || bit_flag[index] )
                {
                    tmp_bit[i]=1;
                }
            }
            seg_offset[i]=1-tmp_bit[i];
        }
        //exclusive prefix sum scan y_offset
        int eprefixsum=0;
        for (int i=0;i<omega;i++)
        {
            int temp1;
            temp1=y_offset[i];
            y_offset[i]=eprefixsum;
            eprefixsum=eprefixsum+temp1;
        }
        //segmented_sum for seg_offset
        segmented_sum1(seg_offset,tmp_bit);
        free(tmp_bit);
        printf("y_offset seg_offset generated\n");
        //generating empty_offset-------------------------
        if(tile_ptr_empty[tid])//if this tile has empty row
        {
            int* empty_offset;
            int size_empty_offset=0;
            for (int i=0;i<(omega*sigma);i++)
            {
                if(bit_flag[tid*omega*sigma+i]) size_empty_offset+=1;
            }
            empty_offset=malloc(size_empty_offset*sizeof(int));
            int eid=0;
            for (int i=0;i<omega;i++)
            {
                for (int j=0;j<sigma;j++)
                {
                    unsigned long index=tid*omega*sigma+i*sigma+j;
                    if (bit_flag[index])
                    {
                        unsigned long idx=binary_search1(num_rows+1,row_ptr,index);
                        //if (idx<0) idx=0;
                        idx=idx-tile_ptr[tid];
                        empty_offset[eid]=idx;
                        printf("tid=%lu, empty_offset[%d]=%lu\n",tid,eid,idx);
                        eid=eid+1;
                    }
                }
            }
            for (int i=0;i<omega;i++)
            {
                y_offset[i]=empty_offset[y_offset[i]];
            }
            printf("empty_offset generated\n");
        }
        


        float* tmp;
        tmp=malloc(omega*sizeof(float));
        memset(tmp,0,omega*sizeof(float));
        float* last_tmp;
        last_tmp=malloc(omega*sizeof(float));
        for (int i=0;i<omega;i++)
        {
            float sum=0;//first ignore y array
            for (int j=0;j<sigma;j++)
            {
                unsigned long ptr=tid*omega*sigma+i*sigma+j;
                //unsigned long ptr1=tid*omega*sigma+j*omega+i;
                sum=sum+val[ptr]*x[col_idx[ptr]];
                //check bit_flag[ptr]
                int seal_head=0;
                int seal_tail=0;
                for (int jj=0;jj<j+1;jj++)
                {
                    if (bit_flag[tid*omega*sigma+i*sigma+jj])
                    {
                        seal_head=1;
                    }
                }
                if(ptr<num_nonzeros-sigma)
                {
                    for (int jj=j+1;jj<sigma+1;jj++)
                    {
                        if (bit_flag[tid*omega*sigma+i*sigma+jj])
                        {
                            seal_tail=1;
                        }
                    }
                }
                else
                {
                    seal_tail=1;
                }
                int next_bit_flag=0;
                if (ptr<num_nonzeros-1) next_bit_flag=bit_flag[ptr+1];
                else next_bit_flag=1;
                if (((!seal_head) && seal_tail && next_bit_flag) || ( (!seal_head) && (!seal_tail) && (j==sigma-1) ) )//end of a red sub-segment
                {
                    tmp[i-1]=sum;
                    sum=0;
                }
                else if ( seal_head && seal_tail && next_bit_flag )//end of a green segment
                {
                    out[tile_ptr[tid]+y_offset[i]]=sum/*+y[tile_ptr[tid]+y_offset[i]]*/;
                    y_offset[i]=y_offset[i]+1;
                    sum=0;
                }
            }
            last_tmp[i]=sum; //end of a blue sub-segment
        }
        fast_segmented_sum1(tmp,seg_offset);
        
        for (int i=0;i<omega;i++)
        {
            last_tmp[i]=last_tmp[i]+tmp[i];
            out[tile_ptr[tid]+y_offset[i]]=last_tmp[i]/*+y[tile_ptr[tid]+y_offset[i]]*/;
        }
        free(tmp);
        free(last_tmp);
    }
    
    if(p>p_cmplt)
    {
        //last incomplete tile
        unsigned long row,row_start,row_end,jj;
        float sum = 0;
        //tile_ptr[0 to p]
        for(row=tile_ptr[p-1]; row < num_rows; row++)
        {
            if (!bit_flag[row_ptr[row]])
            {
                sum=0;
            }
            else
            {
                sum = y[row];
            }
            row_start = row_ptr[row];
            row_end   = row_ptr[row+1];

            for (jj = row_start; jj < row_end; jj++){
                sum += val[jj] * x[col_idx[jj]];
            }
            out[row] += sum;
        }
    }
}