/* test04_cpu1.c */

#define SHAREMEM_DDR_HEAD 0x14010000
#define ADRS_LOCK_VAR     0x14010020

int main( )
{

  volatile int *ptr_array_a32;
  int *ptr_gloral_mem;
  int i, limit;
  int release_lock_cpu1( );
  int get_lock_cpu0( );

  ptr_array_a32 = (int *)SHAREMEM_DDR_HEAD;

  ptr_gloral_mem = (int *)(*(ptr_array_a32 + 4));
  limit          =         *(ptr_array_a32 + 6);

  for(i = 0; i < limit; i++) {
    get_lock_cpu0( );
    (*(ptr_array_a32 + 1)) ++;
    (*(ptr_array_a32 + 2)) ++;
    release_lock_cpu1( );

    /* make inner loop execution time not constant, that increases logic */
    /*   state coverage more (as expectation)                            */
    if((i % 6) == 0) {
      (*(ptr_gloral_mem + 6)) ++; 
    }
    if((i % 10) == 0) {
      (*(ptr_gloral_mem + 10)) ++;
    }
    if((i % 14) == 0) {
      (*(ptr_gloral_mem + 14)) ++;
    }
  } /* end of (for i = ... */
  /* set complete flag */
  *(ptr_array_a32 + 5) = 1;

  while(1) {
  }
 return(0);
}

int release_lock_cpu1 ( )
{
  volatile char *ptr_lock_var;

  ptr_lock_var =  (char *)ADRS_LOCK_VAR;
  *ptr_lock_var = 0;
  return(0);
}

