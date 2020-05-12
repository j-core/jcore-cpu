#include <stdio.h>
#include <stdlib.h>

/* test03_cpu1.c */

int main( )
{
  volatile int *ptr_array_a32;
  int i, sum, poll_count = 0;

  ptr_array_a32 = (int *)(0x14010000);

  while (*(ptr_array_a32 + 0) == 0) {
  }
  sum = 0;
  for (i = 0; i < 100; i ++) {
    sum += *(ptr_array_a32 + 2 + i);
    *(ptr_array_a32 + 102 + i) = sum;
  }
  *(ptr_array_a32 + 1) = 1;
  while(1) {
  }
 return(0);
}

