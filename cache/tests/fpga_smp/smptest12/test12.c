#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

/* ------------------------------------------------------------------------- */
/*  smptest test12.c */
/*      2015-09-16 O. Nishii */
/*  function: cpu1 character display program */
/*            cpu1 pseudo - top, and restart */
/* ------------------------------------------------------------------------- */

#define PAT_INDEX_LOWER  512
#define CPU1_ENABLE_ADR   0xabcd0640
/* fixed address */
#define CPU1_INSTR_HEAD   0x14001000
#define CPU1_SP_INIT      0x14004ffc
#define CPU01_COMM_AREA_HEAD 0x8100
#define RTC_SEC           0xabcd0224
#define RTC_NS            0xabcd0228


int main ( )
{
  int i;
  int instbuf[2], poll_count = 0;
  int fd = 0; /* file handle */
  long time_pr_1, time_pr_2;
  int time_r_sec1, time_r_sec2;
  int time_r_ns1,  time_r_ns2;

  void *ptr_void;
  volatile int *ptr_inst, *ptr_data, *ptr_2;
  volatile int *ptr_sec, *ptr_ns;

  ptr_void = malloc(1024 * 1024 * 10);
   ptr_data = (int *)0x8020;
  *ptr_data = 0;

  ptr_sec = (int *)RTC_SEC;
  ptr_ns  = (int *)RTC_NS;
  printf("smp test test12 (cpu1 uart output)  malloc = %x\n",
    (unsigned int) ptr_void);

  /* pre-exe 1: set CPU1 instructions to DDR */
  /* removed because second malloc returns differt address (linux) */
/*  if((CPU1_INSTR_HEAD < (int) ptr_void) ||
 *     (CPU1_INSTR_HEAD > (int) (ptr_void + (1024 * 1024 * 9)))) {
 *    printf("malloc out of range\n");
 *    return(0);
 *  }
 */

  ptr_inst = (int *)CPU1_INSTR_HEAD;
  fd = open("te12c1.bin", O_RDONLY );
  if(fd == -1) {
    printf("file (te12c1.bin) open error\n");
    return(1);
  }
  for(i = 0; i < ((24 << 2) + (127 << 2)) ; i++) { /* count by byte */
    read (fd, instbuf, 4);
    if(i >= (24 << 2)) {
      *(ptr_inst) = instbuf[0];
        ptr_inst++;
    }
  }
  close(fd);

  /* pre-exe 2: rewrite cpu1 cpu1_restart_smp( ) routine to one */
  /* similar to power ont reset jump */
  /*  start address 14001420 is map-result of cpu1 compile and link */
  /*   asm  */
  /*  14001420 df02 MOV.L @(disp,PC),R15 ; constant CPU1_SP_INIT */
  /*      1422 d003 MOV.L @(disp,PC),R0  ; constant CPU1_INSTR_HEAD */
  /*      1424 402b JMP @R0 */
  /*      1426 0009 NOP */
  /*      -- (4 byte space) -- */
  /*      142C .data.l  0x14004ffc = CPU1_SP_INIT */
  /*      1430 .data.l  0x14001000 = CPU1_INSTR_HEAD */

   ptr_inst = (int *)0x14001420;
  *ptr_inst = 0xdf02d003;     ptr_inst++;
  *ptr_inst = 0x402b0009;     ptr_inst++;
  *ptr_inst = 0x00090009;     ptr_inst++;
  *ptr_inst = CPU1_SP_INIT;   ptr_inst++;
  *ptr_inst = CPU1_INSTR_HEAD;

  printf("end set CPU1 instructions\n");
   ptr_data = (int *) CPU01_COMM_AREA_HEAD;
  *ptr_data = 0;

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
   ptr_data = (int *)0x8020;
  *ptr_data = 1;               /* software (co-operative) awake */


  /*  cpu1 execution */

  ptr_2 = ((int *)CPU01_COMM_AREA_HEAD) + 0;
  while(*ptr_2 == 0) {
    poll_count ++;
  }

  /* result display */
  time_pr_2 = clock( );
  time_r_sec2 = *ptr_sec;
  time_r_ns2  = *ptr_ns;
  printf("time = (process) %.2f sec, (real-time) %.2f sec\n",
    ((float) (time_pr_2 - time_pr_1) / 1.0e6),
    (time_r_sec2 - time_r_sec1) +
    (time_r_ns2  - time_r_ns1)  / 1.0e9);

  free (ptr_void); 
  return(0);
}
