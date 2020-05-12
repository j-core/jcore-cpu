#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

/* smptest test13.c */
/*  2015-09-17 O. Nishii */
/*     merge sort (cpu0, cpu1) cache hit */
/*     array 2kB, work 4kB = total 6kB */

#define          ARRAY_SIZE       512
#define      LOG_ARRAY_SIZE       9

/* fpga board */
#define RTC_SEC         0xabcd0224
#define RTC_NS          0xabcd0228

/* smp register */
#define CPU1_ENABLE_ADR   0xabcd0640
#define CPU1_INSTR_HEAD   0x14001000
#define CPU1_SP_INIT      0x14004ffc

/* cpu0 cpu1 communication */
#define COMMU_CPU01CNTL   0x8100
#define COMMU_CPU10CNTL   0x8104
/* initial value ffff -> 0 -> 1 -> (ARRAY_SIZE - 1) */
#define COMMU_CPU01PTRA   0x8108

int in_array    [ARRAY_SIZE];
/* dispatch design cpu0 : [0]               - [(ARRAY_SIZE >> 1) - 1] */
/*                 cpu1 : [ARRAY_SIZE >> 1] - [ARRAY_SIZE - 1] */

int workarray[3][ARRAY_SIZE]; /* because of 32B align processing, */
                              /* not 2, but 3 */
int stride[20];

int main( )
{
  int i_loop, limit /* repeat time x (ARRAY_SIZE) merge sort */;

  int i, level, sum1 = 0, sum2 = 0, exp1, exp2;
  int merge( );
  long time_pr_1, time_pr_2;
  int time_r_sec1, time_r_sec2;
  int time_r_ns1,  time_r_ns2;
  volatile int *ptr_sec, *ptr_ns;

  /* smp addition */
  int instbuf[2], offset, poll_count = 0;
  int fd = 0; /* file handle */
  void *ptr_void;
  volatile int *ptr_inst, *ptr_data;

  ptr_void = malloc(1024 * 1024 * 10);
  for(i = 0; i < 20; i++) {
    stride[i] = 2 << i;
  }
  offset = ((8 - ((((int)&(workarray[0][0])) >> 2) & 7)) & 7);

  /* init array */
  scanf("%d", &limit);
  for(i = 0; i < ARRAY_SIZE; i++) {
    scanf("%d", &in_array[i]);
  }

  ptr_inst = (int *)CPU1_INSTR_HEAD;
  fd = open("te13c1.bin", O_RDONLY );
  if(fd == -1) {
    printf("file (te13c1.bin) open error\n");
    return(1);
  }
  for(i = 0; i < ((16 << 2) + (32 << 2)) ; i++) { /* 16, 32 - xxd lines-based */
                                                  /* i count by byte */
    read (fd, instbuf, 4);
    if(i >= (16 << 2)) {
      *(ptr_inst) = instbuf[0];
        ptr_inst++;
    }
  }
  close(fd);
  /* expected value (for data 0, 1, ..., (ARRAY_SIZE - 1)) */
  exp1 = (ARRAY_SIZE * (ARRAY_SIZE - 1)) >> 1;
  exp2 = (2 * (ARRAY_SIZE - 1) * (ARRAY_SIZE - 1) * (ARRAY_SIZE - 1) + 
          3 * (ARRAY_SIZE - 1) * (ARRAY_SIZE - 1)                    + 
              (ARRAY_SIZE - 1) ) / 6;

  /* step 3: setup CPU1 boot */
   ptr_data = (int *)0x8000;
  *ptr_data = CPU1_INSTR_HEAD;
   ptr_data = (int *)0x8004;
  *ptr_data = CPU1_SP_INIT;

   ptr_data = (int *)COMMU_CPU01CNTL;
  *ptr_data = 0xffff;
   ptr_data = (int *)COMMU_CPU10CNTL;
  *ptr_data = 0xffff;
   ptr_data = (int *)COMMU_CPU01PTRA;
  *ptr_data = (int)&(workarray[0][offset]);

   ptr_data = (int *)CPU1_ENABLE_ADR;
  *ptr_data = 1;

  /* time beginning */
  ptr_sec = (int *)RTC_SEC;
  ptr_ns  = (int *)RTC_NS;
  time_pr_1 = clock( ); /* timer on */
  time_r_sec1 = *ptr_sec;
  time_r_ns1  = *ptr_ns;

  for(i_loop = 0; i_loop < limit; i_loop ++) {
    /* initial value */
    for(i = 0; i < ARRAY_SIZE; i++) {
      workarray[0][i + offset] = in_array[i];
      workarray[1][i + offset] = 0;
    }

    /* let cpu1 start i_loop'th processing */
     ptr_data = (int *)COMMU_CPU01CNTL;
    *ptr_data = i_loop;

    /* sort cpu0 part */
    for(level = 0; level < LOG_ARRAY_SIZE - 1; level++) {
      for(i = offset; i < (ARRAY_SIZE >> 1) + offset; i += stride[level]) {
        merge(level, i);
      }
    }

    /* wait for cpu1 complete */ 
     ptr_data = (int *)COMMU_CPU10CNTL;
    while(*ptr_data != i_loop) { poll_count++; }

    merge(LOG_ARRAY_SIZE - 1, 0 + offset); /* cpu0, cpu1 merge */ 

    sum1 = 0; /* output result */
    sum2 = 0;
  
    for(i = offset; i < (ARRAY_SIZE + offset); i ++) {
      sum1 += workarray[1][i];
      sum2 += (i - offset)  * workarray[1][i];
    }
    if((sum1 != exp1) || (sum2 != exp2)) {
      printf("diff (loop) %d %d %d %d %d\n", i_loop, sum1, exp1, sum2, exp2);
    }
  } /* end of for(i_loop */

  /* disable notice to cpu1 */
   ptr_data = (int *)COMMU_CPU01CNTL;
  *ptr_data = 0xffff;

  /* time ending */
  time_pr_2 = clock( );
  time_r_sec2 = *ptr_sec;
  time_r_ns2  = *ptr_ns; 
  printf("test13 (merge sort, 512 elements) results\n");
  printf("sum1 %d (deci) %x (hex)\n", sum1, sum1);
  printf("sum2 %d (deci) %x (hex)\n", sum2, sum2);
  printf("expected sum1 %d (deci) %x (hex)\n", exp1, exp1);
  printf("expected sum2 %d (deci) %x (hex)\n", exp2, exp2);
  printf("time = (process) %.2f sec, (real-time) %.2f sec\n",
    ((float) (time_pr_2 - time_pr_1) / 1.0e6),
    (time_r_sec2 - time_r_sec1) +
    (time_r_ns2  - time_r_ns1)  / 1.0e9);
  printf("poll_count %d\n", poll_count);

  /* output array */  
  /*  for(i = 0; i < ARRAY_SIZE; i++)  {
   *    printf("%d %d\n", i, workarray[1][i]);
   *  }
   */
  
  free(ptr_void);
  return(0);
}

int merge(level, index)
int level, index;
{
  int *ptr_a, *ptr_alim, *ptr_b, *ptr_blim, *ptr_d, *ptr_s;
  int i, src_bank, quitflg;

  src_bank = level & 0x1;

  ptr_a    = &workarray[    src_bank][index];
  ptr_d    = &workarray[1 - src_bank][index];
  ptr_b    = ptr_a + (stride[level] >> 1);
  ptr_alim = ptr_b;
  ptr_blim = ptr_a +  stride[level];

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
  for(     ; i < stride[level]; i++) {
    *ptr_d = *ptr_s; ptr_s ++; ptr_d ++;
  }
  return(0);
}
