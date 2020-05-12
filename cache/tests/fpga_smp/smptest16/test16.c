#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

/* smptest test16.c */
/*  2015-10-21 O. Nishii */
/*     cpu1 aic register read/write */

/* fpga board */
#define RTC_SEC         0xabcd0224
#define RTC_NS          0xabcd0228

/* smp register */
#define CPU1_ENABLE_ADR   0xabcd0640
#define CPU1_INSTR_HEAD   0x14001000
#define CPU1_SP_INIT      0x14004ffc

/* cpu0 cpu1 communication */
#define CPU01COMM 0x8000

int main( )
{
  int i, quitflg;
  long time_pr_1, time_pr_2;
  int time_r_sec1, time_r_sec2;
  int time_r_ns1,  time_r_ns2;
  volatile int *ptr_sec, *ptr_ns;

  /* smp addition */
  int instbuf[2], poll_count = 0;
  int fd = 0; /* file handle */
  void *ptr_void;
  volatile int *ptr_inst, *ptr_data;

  ptr_void = malloc(1024 * 1024 * 10);

  ptr_inst = (int *)CPU1_INSTR_HEAD;
  fd = open("te16c1.bin", O_RDONLY );
  if(fd == -1) {
    printf("file (te13c1.bin) open error\n");
    return(1);
  }
  for(i = 0; i < ((22 << 2) + (20 << 2)) ; i++) { /* 16, 32 - xxd lines-based */
                                                  /* i count by byte */
    read (fd, instbuf, 4);
    if(i >= (22 << 2)) {
      *(ptr_inst) = instbuf[0];
        ptr_inst++;
    }
  }
  close(fd);

  /* step 3: setup CPU1 boot */
   ptr_data = (int *)0x8000;
  *ptr_data = CPU1_INSTR_HEAD;
   ptr_data = (int *)0x8004;
  *ptr_data = CPU1_SP_INIT;

   ptr_data = ((int *)CPU01COMM + 32);
  *ptr_data = 0;

  /* time beginning */
  ptr_sec = (int *)RTC_SEC;
  ptr_ns  = (int *)RTC_NS;
  time_pr_1 = clock( ); /* timer on */
  time_r_sec1 = *ptr_sec;
  time_r_ns1  = *ptr_ns;

   ptr_data = (int *)CPU1_ENABLE_ADR;
  *ptr_data = 1;

  quitflg = 0;
  for(i = 0; (i < 10000) && (quitflg == 0); i++) {
    ptr_data = ((int *)CPU01COMM + 40);
    quitflg = *ptr_data ;
  }
  /* time ending */
  time_pr_2 = clock( );
  time_r_sec2 = *ptr_sec;
  time_r_ns2  = *ptr_ns; 
  printf("test16 (cpu1-aic read write) results\n");

  for(i = -1; i < 24; i++) {
    ptr_data = ((int *)CPU01COMM + 8 + i);
    printf("i (deci) %d, data (hex) %x\n", i, *ptr_data);
  }

  printf("time = (process) %.2f sec, (real-time) %.2f sec\n",
    ((float) (time_pr_2 - time_pr_1) / 1.0e6),
    (time_r_sec2 - time_r_sec1) +
    (time_r_ns2  - time_r_ns1)  / 1.0e9);
  printf("poll_count %d\n", poll_count);

  free(ptr_void);
  return(0);
}

