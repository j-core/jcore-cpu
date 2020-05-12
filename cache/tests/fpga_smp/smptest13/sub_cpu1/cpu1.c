/* smptest (test13.c)'s cpu.1  */
/*  2015-09-18 O. Nishii */
/*     merge sort (cpu0, cpu1) cache hit */
/*     array 2kB, work 4kB = total 6kB */

#define          ARRAY_SIZE       512
#define      LOG_ARRAY_SIZE       9
#define      stride(x)            ((2) << x)

/* cpu0 cpu1 communication */
#define COMMU_CPU01CNTL   0x8100
#define COMMU_CPU10CNTL   0x8104
/* initial value ffff -> 0 -> 1 -> (ARRAY_SIZE - 1) */
#define COMMU_CPU01PTRA   0x8108

/* int *pt_in_array ==== */
/* int (cpu0's) in_array    [ARRAY_SIZE]; */
/* dispatch design cpu0 : [0]               - [(ARRAY_SIZE >> 1) - 1] */
/*                 cpu1 : [ARRAY_SIZE >> 1] - [ARRAY_SIZE - 1] */

/* int *pt_workarray ==== */
/* int (cpu0's) workarray[2][ARRAY_SIZE]; */


int main( )
{
  int i_loop /* repeat time x (ARRAY_SIZE) merge sort */;

  int i, level;
  int merge( );
  int *pt_workarray;

  /* smp addition */
  void *ptr_void;
  volatile int *ptr_inst, *ptr_data;

  /* wait until first cpu0->cpu1 activate */
   ptr_data = (int *)COMMU_CPU01CNTL;
  while(*ptr_data == 0xffff) { }

   ptr_data = (int *)COMMU_CPU01PTRA;
  pt_workarray = (int *)*ptr_data;

  /* main loop */
  for(i_loop = 0; i_loop < 0x7fffffff; i_loop ++) {

    /* sort cpu0 part */
    for(level = 0; level < LOG_ARRAY_SIZE - 1; level++) {
      for(i = (ARRAY_SIZE >> 1); i < ARRAY_SIZE - 1; ) {
        mergecpu1(level, i, pt_workarray);
        i += stride(level);
      }
    }

    /* notice cpu0 to i_loop's processing end */
     ptr_data = (int *)COMMU_CPU10CNTL;
    *ptr_data = i_loop; /* let cpu0 proceed merge between cpu0 and cpu1 */

    ptr_data = (int *)COMMU_CPU01CNTL;
    while((*ptr_data == 0xffff) || /* cpu0 finish */
          (*ptr_data == i_loop)) { /* wait until next i_loop start */
    }
  } /* end of for(i_loop */

  /* time ending */

  return(0);
}

int mergecpu1(level, index, pt_workarray)
int level, index;
int *pt_workarray;
{
  int *ptr_a, *ptr_alim, *ptr_b, *ptr_blim, *ptr_d, *ptr_s;
  int i, src_bank, quitflg;
  int work1;

  int *ptr_dbg;

  src_bank = level & 0x1;

  ptr_a = pt_workarray + ((src_bank       << LOG_ARRAY_SIZE) + index);
  ptr_d = pt_workarray + (((1 - src_bank) << LOG_ARRAY_SIZE) + index);

  ptr_b    = ptr_a + (stride(level) >> 1);
  ptr_alim = ptr_b;
  ptr_blim = ptr_a +  stride(level);

  quitflg = 0;
  for(i = 0; quitflg == 0; i++) {
      if( *ptr_a < *ptr_b) {
          *ptr_d = *ptr_a;
           ptr_a ++;
          if(ptr_a == ptr_alim) { quitflg = 1;
                                  ptr_s = ptr_b; } }
      else {
          *ptr_d = *ptr_b;
           ptr_b ++;
          if(ptr_b == ptr_blim) { quitflg = 1;
                                  ptr_s = ptr_a; } }
    ptr_d ++;
  } 
  for(     ; i < stride(level); i++) {
    *ptr_d = *ptr_s; ptr_s ++; ptr_d ++;
  }
  return(0);
}
