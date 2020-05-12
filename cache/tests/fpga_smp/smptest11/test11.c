#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

/* ------------------------------------------------------------------------- */
/*  smptest test11.c */
/*      2015-09-07 O. Nishii */
/*  function: random number address read write, from cpu0 and cpu1 */
/*            no w/r data dependency between cpu0, cpu1 (except init) */
/*  source file organization: test11.c -- for cpu0 */
/*                            cpu1.c   -- for cpu1 */
/* ------------------------------------------------------------------------- */

#define ARRSIZE         2560
#define PAT_INDEX_LOWER  512
#define CPU1_ENABLE_ADR   0xabcd0640
/* fixed address */
#define CPU1_INSTR_HEAD   0x14001000
#define CPU1_SP_INIT      0x14004ffc
#define CPU01_COMM_AREA_HEAD 0x8100
#define RTC_SEC           0xabcd0224
#define RTC_NS            0xabcd0228


unsigned int test_array[ARRSIZE];
unsigned int pattern_mem[ARRSIZE << 3];
int main ( )
{
  int dummy,  i, j, k, limit, sum, pat_index, arr_index2;
  int instbuf[2], poll_count = 0;
  int fd = 0; /* file handle */
  long time_pr_1, time_pr_2;
  int time_r_sec1, time_r_sec2;
  int time_r_ns1,  time_r_ns2;

  void *ptr_void;
  volatile int *ptr_inst, *ptr_data, *ptr_2;
  volatile int *ptr_sec, *ptr_ns;


  ptr_void = malloc(1024 * 1024 * 10);
  ptr_sec = (int *)RTC_SEC;
  ptr_ns  = (int *)RTC_NS;
  printf("smp test test11 (snoop by random number)\n");
  printf("%x 10MB area kept\n", (unsigned int) ptr_void);

  printf("input loop count (decimal)\n");
  printf("  memo: 1 loop <=> %d(dec)load and %d(dec)(store)\n", 
          ARRSIZE, (ARRSIZE >> 2));
  scanf("%d", &limit);
  for(i = 0; i < (ARRSIZE << 1); i++) {
    pat_index = 
    ((i & (~(PAT_INDEX_LOWER - 1))) << 2) |
     (i &   (PAT_INDEX_LOWER - 1));
    scanf("%d %d", &dummy, &pattern_mem[pat_index]);
    pattern_mem[pat_index] &= (~0x00000001); /* make 2n value to 0/1 process */
  }

  for(i = 0; i < ARRSIZE ; i++) {
    test_array[i] = i;
  }

  /* pre-exe 1: set CPU1 instructions to DDR */
  ptr_inst = (int *)CPU1_INSTR_HEAD;
  fd = open("te11c1.bin", O_RDONLY );
  if(fd == -1) {
    printf("file (te11c1.bin) open error\n");
    return(1);
  }
  for(i = 0; i < (84 + 128) ; i++) { /* count by byte */
    read (fd, instbuf, 4);
    if(i >= 84) {
      *(ptr_inst) = instbuf[0];
        ptr_inst++;
    }
  }
  close(fd);

  printf("end set CPU1 instructions\n");
   ptr_data = (int *) CPU01_COMM_AREA_HEAD;
  *ptr_data = (int) &test_array[0];
   ptr_data ++;
  *ptr_data = (int) &pattern_mem[0];
  printf("parameters test_array %x pattern_mem %x\n",
    (unsigned int)(&test_array[0]),
    (unsigned int)(&pattern_mem[0]));
   ptr_data ++;
  *ptr_data =       limit;

  /* step 3: setup CPU1 boot */
   ptr_data = (int *)0x8000;
  *ptr_data = CPU1_INSTR_HEAD;
   ptr_data = (int *)0x8004;
  *ptr_data = CPU1_SP_INIT;
  for(i = 0; i < 10; i++) {
  }
  printf("end setup CPU1 boot\n");

  time_pr_1 = clock( ); /* timer on */
  time_r_sec1 = *ptr_sec;
  time_r_ns1  = *ptr_ns;

   ptr_data = (int *)CPU1_ENABLE_ADR;
  *ptr_data = 1;

  /* main cpu0 execution */
  pat_index = 0;
  for(i = 0; i < limit; i++) {
    for(j = 0; j < (ARRSIZE >> 2); j++) {
      sum = 0;
      for(k = 0; k < 5; k++) {
        if(k < 3) {
          sum += test_array[pattern_mem[pat_index]];
        }
        else if(k == 3) {
          arr_index2 = pattern_mem[pat_index] + (i << 2);
          while(arr_index2 >= ARRSIZE) {
            arr_index2 -= ARRSIZE;
          }
          sum += test_array[arr_index2];
        }
        else {
          arr_index2 = pattern_mem[pat_index] + (i << 1);
          while(arr_index2 >= ARRSIZE) {
            arr_index2 -= ARRSIZE;
          }
                 test_array[arr_index2] = 
          ((sum + 11 + i) & 0x0001FFFF);
        }
        pat_index ++;
        if((pat_index & (PAT_INDEX_LOWER - 1)) == 0) {
          pat_index += ((PAT_INDEX_LOWER << 1) |
                         PAT_INDEX_LOWER);
        }
        if(pat_index >= (ARRSIZE << 3)) {
          pat_index = 3;
    } } }
    if((i & 0xff) == 0) {
      sum = 0;
      for(j = 0; j < 100; j++) {
        sum += test_array[j];
      }
      printf("in-loop %x\n", sum);
      printf("array %x %x %x %x %x \n",
        test_array[0], test_array[1], test_array[2], test_array[3],
        test_array[4]);
    }
  }
  ptr_2 = ((int *)CPU01_COMM_AREA_HEAD) + 3;
  while(*ptr_2 == 0) {
    poll_count ++;
  }

  /* result display */
  time_pr_2 = clock( );
  time_r_sec2 = *ptr_sec;
  time_r_ns2  = *ptr_ns;
  sum = 0;
  for(j = 0; j < ARRSIZE; j++) {
    sum += test_array[j];
  }
  printf("RESULT\n");
  printf("  %x\n", sum);
  for(j = 0; j <= 8; j++) {
    printf("  array %x %x %x %x %x\n", j,
      test_array[(j << 2) + 0], test_array[(j << 2) + 1],
      test_array[(j << 2) + 2], test_array[(j << 2) + 3]);
  }
  printf("cpu0 poll count (for completion) = %d\n", poll_count);
  printf("time = (process) %.2f sec, (real-time) %.2f sec\n",
    ((float) (time_pr_2 - time_pr_1) / 1.0e6),
    (time_r_sec2 - time_r_sec1) +
    (time_r_ns2  - time_r_ns1)  / 1.0e9);
 
  free(ptr_void); 
  return(0);
}
