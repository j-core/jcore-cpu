#include <stdio.h>

#define ARRSIZE         2560
#define PAT_INDEX_LOWER  512
#define PAT_CPUID 0

unsigned int test_array[ARRSIZE];
unsigned int pattern_mem[ARRSIZE << 3];
int main ( )
{
  int dummy,  i, j, k, limit, sum, pat_index[2], arr_index2;
  int vcpuid;

  printf("input loop count\n");
  scanf("%d", &limit);

  for(i = 0; i < (ARRSIZE << 1); i++) {
    pat_index[0] = 
    ((i & (~(PAT_INDEX_LOWER - 1))) << 2) |
     (i &   (PAT_INDEX_LOWER - 1));
    scanf("%d %d", &dummy, &pattern_mem[pat_index[0]]);
    pattern_mem[pat_index[0]] &= (~0x00000001);
    pattern_mem[pat_index[0]] += PAT_CPUID;
  }

  for(i = 0; i < ARRSIZE ; i++) {
    test_array[i] = i;
  }

  pat_index[0] = 0;
  pat_index[1] = (17 << 10) | (256 - 1);
  for(i = 0; i < limit; i++) {
    for(vcpuid = 0; vcpuid < 2; vcpuid++) {
      for(j = 0; j < (ARRSIZE >> 2); j++) {
/*    for(j = 0; j < (448         ); j++) { */
  
        sum = 0;
         
        for(k = 0; k < 5; k++) {
          if(k < 3) {
            sum += test_array[pattern_mem[pat_index[vcpuid]] + vcpuid];
          }
          else if(k == 3) {
            arr_index2 = pattern_mem[pat_index[vcpuid]] + (i << 2) + vcpuid;
            while(arr_index2 >= ARRSIZE) {
              arr_index2 -= ARRSIZE;
            }
            sum += test_array[arr_index2];
          }
          else {
            arr_index2 = pattern_mem[pat_index[vcpuid]] + (i << 1) + vcpuid;
            while(arr_index2 >= ARRSIZE) {
              arr_index2 -= ARRSIZE;
            }
                   test_array[arr_index2] = 
            ((sum + 11 + i) & 0x0001FFFF);
          }
          pat_index[vcpuid] ++;
          if((pat_index[vcpuid] & (PAT_INDEX_LOWER - 1)) == 0) {
            pat_index[vcpuid] += ((PAT_INDEX_LOWER << 1) |
                           PAT_INDEX_LOWER);
          }
          if(pat_index[vcpuid] >= (ARRSIZE << 3)) {
            pat_index[vcpuid] = 3;
          }
        }
      }
    } /* end of for-vcpu */
    if((i & 0xff) == 0) {
      sum = 0;
      for(j = 0; j < 100; j++) {
        sum += test_array[j];
      }
      printf("in-loop %x\n", sum);
      printf("array %x %x %x %x %x \n", test_array[0], test_array[1],
        test_array[2], test_array[3], test_array[4]);
    }
  } /* end of for-i */
  sum = 0;
  for(j = 0; j < ARRSIZE; j++) {
    sum += test_array[j];
  }
  printf("%x\n", sum);
  for(j = 0; j <= 8; j++) {
    printf("array %x %x %x %x %x\n", j,
      test_array[(j << 2) + 0], test_array[(j << 2) + 1],
      test_array[(j << 2) + 2], test_array[(j << 2) + 3]);
  }
}
