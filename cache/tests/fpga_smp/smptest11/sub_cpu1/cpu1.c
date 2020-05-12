/* cpu1.c */
#define ARRSIZE         2560
#define PAT_INDEX_LOWER  512
#define CPUID 1

/* unsigned int test_array[ARRSIZE]; */
/* unsigned int pattern_mem[ARRSIZE << 3]; */

int main ( )
{
  int i, j, k, limit, sum, pat_index, arr_index2;
  int *pt_test_array ;
  int *pt_pattern_mem ;
  volatile int *ptr_1;

  pat_index = (17 << 10) | (256 - 1);
  pt_test_array =  (int *)*(int *)0x8100;
  pt_pattern_mem = (int *)*(int *)0x8104;
  limit =                 *(int *)0x8108;

  for(i = 0; i < limit; i++) {
    for(j = 0; j < (ARRSIZE >> 2); j++) {

      sum = 0;
       
      for(k = 0; k < 5; k++) {
        if(k < 3) {
          sum += *(pt_test_array + (*(pt_pattern_mem + pat_index)) + CPUID);
        }
        else if(k == 3) {
          arr_index2 = (*(pt_pattern_mem + pat_index)) + (i << 2) + CPUID;
          while(arr_index2 >= ARRSIZE) {
            arr_index2 -= ARRSIZE;
          }
          sum += *(pt_test_array + arr_index2);
        }
        else {
          arr_index2 = *(pt_pattern_mem + pat_index) + (i << 1) + CPUID;
          while(arr_index2 >= ARRSIZE) {
            arr_index2 -= ARRSIZE;
          }
                 *(pt_test_array + arr_index2) = 
          ((sum + 11 + i) & 0x0001FFFF);
        }
        pat_index ++;
        if((pat_index & (PAT_INDEX_LOWER - 1)) == 0) {
          pat_index += ((PAT_INDEX_LOWER << 1) |
                         PAT_INDEX_LOWER);
        }
        if(pat_index >= (ARRSIZE << 3)) {
          pat_index = 3;
        }
      }
    }
  }
  /* finish_flg */
  ptr_1 = (int *) 0x810c; *ptr_1 = 1;
  while(1) {
  }
  return(0);
}
