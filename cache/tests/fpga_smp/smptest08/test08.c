#include <stdio.h>
#include <stdlib.h>

/* test08.c (dense and same memory location access to shared-ram */
/*           by cpu0 & cpu1) */
/*   2015-08-19 O. NISHII */

/* fixed address */
#define CPU1_INSTR_HEAD   0x14001000
#define CPU1_SP_INIT      0x14004ffc
#define RTC_SEC           0xabcd0224
#define RTC_NS            0xabcd0228
#define ADR_CPU01_COMM_HEAD 0x80e0
#define ADR_CPU0_FORV     0x8100
#define ADR_CPU1_FORV     0x8102
#define ADR_CPU0_SUMVL    0x8108
#define ADR_CPU1_SUMVL    0x810a
#define ADR_CPU0_SUMVH    0x810e
#define ADR_CPU1_SUMVH    0x810c

char instbuf[160];

int main( )
{
  int i, j, limit, limit_cpu1, poll_count = 0, sum_int;
  unsigned int instr0, instr1;
  void *ptr_void;
  volatile int *ptr_array_a32, *ptr_inst, *ptr_data;
  volatile short *ptr_i;
  short          data_sample;
  volatile unsigned short  *ptr_suml, *ptr_sumh;
  volatile short *ptr_sh_1;
  long time_pr_1,   time_pr_2;
  int  time_r_sec1, time_r_sec2;
  int  time_r_ns1,  time_r_ns2;
  volatile int *ptr_sec, *ptr_ns;
  FILE *fp1;

  ptr_void = malloc(1024 * 1024 * 10);
  ptr_sec = (int *)RTC_SEC;
  ptr_ns  = (int *)RTC_NS;

  /* config */
  if(sizeof(data_sample) != 2) {
    printf("short not 2 byte compu. env.  quit.\n");
    exit (0);
  }

  printf("smp test test08 (dense access to shared mem by cpu0&cpu1\n");
  printf("%x 10MB area kept\n", (unsigned int) ptr_void);
  printf("input sum limit for cpu0 (1-32767)\n");
  printf("  (limit for cpu1 is chosen to program)\n");
  scanf("%d", &limit);
  if(limit > 32767) {
     limit = 32767; printf("  clip limit 32767\n");
  }

  ptr_array_a32 = (int *)CPU1_INSTR_HEAD ;
  if(
     (ptr_array_a32 <  (int*)(ptr_void) ) ||
     (ptr_array_a32 > ((int*)(ptr_void) + (1024 * 1024 * 10) - 16))) {
    printf("fixed address variable out of range of allocated mem.\n");
    return(1);
  }

  if     (limit <    10)         { limit_cpu1 = limit +     2; }
  else if(limit <   100)         { limit_cpu1 = limit +    20; }
  else if(limit <  1000)         { limit_cpu1 = limit +   200; }
  else if(limit < 10000)         { limit_cpu1 = limit +  2000; }
  else if(limit < 32767 - 10000) { limit_cpu1 = limit + 10000; }
  else                           { limit_cpu1 = 32767; }

  /* step 1: set CPU1 instructions to DDR */
  ptr_inst = (int *)CPU1_INSTR_HEAD;
  fp1 = fopen ("sm08c1.xxd", "r");
  for(i = 0; i < 102 ; i++) {
    fgets(instbuf, 160, fp1);
    if(i >= 21) {
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

  /* step 2: set shared ram */
  ptr_sh_1 = (short *)ADR_CPU0_FORV  ; *ptr_sh_1 = 0;
  ptr_sh_1 = (short *)ADR_CPU1_FORV  ; *ptr_sh_1 = 0;
  ptr_sh_1 = (short *)ADR_CPU0_SUMVL ; *ptr_sh_1 = 0;
  ptr_sh_1 = (short *)ADR_CPU1_SUMVL ; *ptr_sh_1 = 0;
  ptr_sh_1 = (short *)ADR_CPU0_SUMVH ; *ptr_sh_1 = 0;
  ptr_sh_1 = (short *)ADR_CPU1_SUMVH ; *ptr_sh_1 = 0;
  for (i = 0; i < 30; i ++) {
    *ptr_sh_1 = 0;
    ptr_sh_1++;
  }
  ptr_sh_1 = (short *)ADR_CPU01_COMM_HEAD    ; *ptr_sh_1 = (short) limit_cpu1;
  ptr_sh_1 = (short *)ADR_CPU01_COMM_HEAD + 1; *ptr_sh_1 = 0;
  ptr_data = (int *)  ADR_CPU01_COMM_HEAD + 1; *ptr_data = 0;

  /* step 3: setup CPU1 boot */
   ptr_data = (int *)0x8000;
  *ptr_data = CPU1_INSTR_HEAD;
   ptr_data = (int *)0x8004;
  *ptr_data = CPU1_SP_INIT;
  for(i = 0; i < 10; i++) {
  }
  printf("end setup CPU1 boot\n");

  time_pr_1 = clock( ); /* timer (start point) */
  time_r_sec1 = *ptr_sec;
  time_r_ns1  = *ptr_ns;

   ptr_data = (int *)0xabcd0640;
  *ptr_data = 1;

  ptr_sumh = (unsigned short *)ADR_CPU0_SUMVH;
  ptr_suml = (unsigned short *)ADR_CPU0_SUMVL;
  ptr_i    = (         short *)ADR_CPU0_FORV;

  
  ptr_sh_1 = (short *)ADR_CPU0_SUMVL ; *ptr_sh_1 = 0;
  ptr_sh_1 = (short *)ADR_CPU0_SUMVH ; *ptr_sh_1 = 0;
  sum_int = 0;
  /* step 4: sum 1 - to limit */
  for((*ptr_i) = 1; (*ptr_i) <= (short)limit; (*ptr_i)++) {
    sum_int = ((int)*ptr_sumh) << 16 | ((int)*ptr_suml);
    sum_int += (*ptr_i);
    *ptr_suml = (unsigned short) (sum_int & 0xffff);
    *ptr_sumh = (unsigned short) ((sum_int >> 16) & 0xffff);
  } /* end of (for i = ... */

  /* step 5 wait for cpu1 execuion completion */
  ptr_sh_1 = (short *)ADR_CPU01_COMM_HEAD + 1;
  while((* ptr_sh_1) == 0) {
    poll_count ++;
  }
  time_pr_2 = clock( );
  time_r_sec2 = *ptr_sec;
  time_r_ns2  = *ptr_ns;

  ptr_data = (int *)ADR_CPU01_COMM_HEAD + 1;
  /* step 6 display result */
  printf("results\n");
  printf("cpu0 limit %d\n", limit);
  printf("cpu0 sum %d\n",      sum_int);
  printf("cpu1 limit %d\n", limit_cpu1);
  printf("cpu1 sum %d\n",      *ptr_data);
  printf("cpu0 poll count (for completion) = %d\n", poll_count);
  printf("time = (process) %.2f sec, (real-time) %.2f sec\n",
    ((float) (time_pr_2 - time_pr_1) / 1.0e6),
    (time_r_sec2 - time_r_sec1) +
    (time_r_ns2  - time_r_ns1)  / 1.0e9);

  free(ptr_void);
  return(0);
}

