#include <stdio.h>
#include <stdlib.h>

/* test09.c (smp test 09) */

/*   smptest04.c 2015-04-21 O. NISHII */
/*      (smptest04 : lock var on DDR) */
/*   smptest09.c 2015-08-18 O. NISHII */
/*      (smptest09 : lock var on shared SRAM) */

/* fixed address */
#define CPU1_INSTR_HEAD   0x14001000
#define CPU1_SP_INIT      0x14004ffc
#define SHAREMEM_DDR_HEAD 0x14010000
#define ADRS_LOCK_VAR     0x00008100
#define RTC_SEC           0xabcd0224
#define RTC_NS            0xabcd0228

extern int get_lock_cpu0 ( );
                            /* created as assembler program with TAS.B inst. */

char instbuf[160];
/*                    0  1  2  3  4  5  6  7  8  9 */
int global_mem[20] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

int main( )
{
  int release_lock_cpu0( );
  int i, j, limit, poll_count = 0;
  unsigned int instr0, instr1;
  void *ptr_void;
  volatile int *ptr_array_a32, *ptr_inst, *ptr_data;
  volatile char *ptr_lock;
  long time_pr_1, time_pr_2;
  int time_r_sec1, time_r_sec2;
  int time_r_ns1, time_r_ns2;
  volatile int *ptr_sec, *ptr_ns;
  FILE *fp1;

  ptr_void = malloc(1024 * 1024 * 10);
  ptr_sec = (int *)RTC_SEC;
  ptr_ns  = (int *)RTC_NS;
  printf("smp test test09 (TAS spinlock, lock-var on shared SRAM)\n");
  printf("%x 10MB area kept\n", (unsigned int) ptr_void);
  printf("global_mem head is %x\n", (unsigned int)(&global_mem[0]));

  ptr_array_a32 = (int *)SHAREMEM_DDR_HEAD ;
  if(
     (ptr_array_a32 <  (int*)(ptr_void) ) ||
     (ptr_array_a32 > ((int*)(ptr_void) + (1024 * 1024 * 10) - 16))) {
    printf("fixed address variable out of range of allocated mem.\n");
    return(1);
  }

  printf("input spin lock cpu0, cpu1 count\n");
  scanf("%d", &limit);

  /* step 1: set CPU1 instructions to DDR */
  ptr_inst = (int *)CPU1_INSTR_HEAD;
  fp1 = fopen ("te09c1.xxd", "r");
  for(i = 0; i < 102 ; i++) {
    fgets(instbuf, 160, fp1);
    if(i >= 16) {
      for(j = 0; j < 4; j++) {
        sscanf(&instbuf[10 * j +  9], "%x", &instr0);
        sscanf(&instbuf[10 * j + 14], "%x", &instr1);
        *(ptr_inst) = (int)((instr0 << 16) | instr1);
        ptr_inst++;
      }
    }
  }
  fclose(fp1);
  printf("end set CPU1 instructions\n");

  /* step 2: clear array[0] - array[5] */
  for (i = 0; i < 6; i ++) {
    *(ptr_array_a32 + i) = 0;
  }
   ptr_lock = (char *)ADRS_LOCK_VAR;
  *ptr_lock = 0;
  *(ptr_array_a32 + 4) = (int)(&  global_mem[0]);
  *(ptr_array_a32 + 6) = limit;

  /* step 3: setup CPU1 boot */
   ptr_data = (int *)0x8000;
  *ptr_data = CPU1_INSTR_HEAD;
   ptr_data = (int *)0x8004;
  *ptr_data = CPU1_SP_INIT;
  for(i = 0; i < 10; i++) {
  }

  time_pr_1 = clock( );
  time_r_sec1 = *ptr_sec;
  time_r_ns1  = *ptr_ns;

  printf("end setup CPU1 boot\n");
   ptr_data = (int *)0xabcd0640;
  *ptr_data = 1;

  /* step 4: increment array[0] & array[1] with spin-lock */
  for(i = 0; i < limit; i++) {
    get_lock_cpu0( );
    (*(ptr_array_a32 + 0)) ++;
    (*(ptr_array_a32 + 1)) ++;
    release_lock_cpu0( );

    /* make inner loop execution time not constant, that increases logic */
    /*   state coverage more (as expectation)                            */
    if((i % 3) == 0) {
      global_mem[3] ++;
      if((i % 5) == 0) {
        global_mem[5] ++;
          if((i % 7) == 0) {
          global_mem[7] ++;
        }
      }
    }
  } /* end of (for i = ... */

  /* step 5 wait for cpu1 execuion completion */
  while(*(ptr_array_a32 + 5) == 0) {
    poll_count ++;
  }
  time_pr_2 = clock( );
  time_r_sec2 = *ptr_sec;
  time_r_ns2  = *ptr_ns;

  /* step 6 display result */
  printf("results\n");
  printf("  here, cpu#0 increments array[0] & array[1] with lock\n");
  printf("        cpu#1 increments array[1] & array[2] with lock\n");
  for(i = 0; i < 3; i++) {
    printf("array[%d] = %d\n", i, *(ptr_array_a32 + i));
  }
  printf("global_mem[ 3] = %d\n", global_mem[ 3]);
  printf("global_mem[ 5] = %d\n", global_mem[ 5]);
  printf("global_mem[ 6] = %d\n", global_mem[ 6]);
  printf("global_mem[ 7] = %d\n", global_mem[ 7]);
  printf("global_mem[10] = %d\n", global_mem[10]);
  printf("global_mem[14] = %d\n", global_mem[14]);
  printf("cpu0 poll count (for completion) = %d\n", poll_count);
  printf("time = (process) %.2f sec, (real-time) %.2f sec\n",
    ((float) (time_pr_2 - time_pr_1) / 1.0e6),
    (time_r_sec2 - time_r_sec1) +
    (time_r_ns2  - time_r_ns1)  / 1.0e9);
  return(0);
}

int release_lock_cpu0 ( )
{
  volatile char *ptr_lock_var;

  ptr_lock_var =  (char *)ADRS_LOCK_VAR;
  *ptr_lock_var = 0;
  return(0);
}
