/* get_lock_cpu0_stub.c (smp test 04) */
/*   2015-04-21 O. NISHII */

#define CPU1_SP_INIT      0x14004ffc
#define ADRS_LOCK_VAR     0x14010020

int get_lock_cpu0_stub( )
{
  volatile char *ptr_lock;  
  volatile int *ptr_data;  

  ptr_lock = (char *)ADRS_LOCK_VAR;
  ptr_data = (int *)CPU1_SP_INIT;

  *ptr_data = ((int) (*ptr_lock)) + 9;
  while( (*ptr_data) == 0) {
    (*(ptr_data + 11)) ++;
  }
  return(0);
}
