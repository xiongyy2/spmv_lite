#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<math.h>
#include<cmath>

int main(int argc, char *argv[])
{
    int tile_ptr;
    tile_ptr=-0;
    printf("tile_ptr=%d\n",tile_ptr);
    int sign;
    sign=std::signbit(tile_ptr);
    printf("sign=%d\n",sign);
}
