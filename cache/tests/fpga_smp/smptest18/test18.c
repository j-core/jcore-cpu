#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "dma.h"

/* ------------------------------------------------------------------------- */
/*  smptest test18.c */
/*      2015-10-23 O. Nishii */
/*  function: cpu0, cpu1, dma access */
/*    [cpu0 & cpu1] same as test11.c */
/*            random number address read write, from cpu0 and cpu1 */
/*            no w/r data dependency between cpu0, cpu1 (except init) */
/*    [dma] 2MB+2MB 32Byte transfer */
/*  source file organization: test11.c -- for cpu0 (including dma) */
/*                            cpu1.c   -- for cpu1 */
/* ------------------------------------------------------------------------- */

#define ARRSIZE         2560
#define PAT_INDEX_LOWER  512
#define CPU1_ENABLE_ADR   0xabcd0640
/* fixed address */
#define CPU01_COMM_AREA_HEAD 0x8100
#define RTC_SEC           0xabcd0224
#define RTC_NS            0xabcd0228

#define CCR0              0xabcd00c0
#define DMA_CH            32
#define DMA_TRANSFER_BYTES   2097152

unsigned int test_array[ARRSIZE];
unsigned int pattern_mem[ARRSIZE << 3];

int dma_ch =                                                  DMA_CH;
volatile struct dmac_ch_regs *dmac_ch = DMAC_REGS->channels + DMA_CH;

int main ( )
{
  int dummy,  i, j, k, limit, sumcpu, sumdma, pat_index, arr_index2;
  int instbuf[2], poll_count1 = 0, poll_count2 = 0;
  int fd = 0; /* file handle */
  long time_pr_1, time_pr_2;
  int time_r_sec1, time_r_sec2;
  int time_r_ns1,  time_r_ns2;

  void *ptr_void;
  volatile int *ptr_inst, *ptr_data, *ptr_2, *ptr_malal;
  volatile int *ptr_sec, *ptr_ns;


  ptr_void = malloc(1024 * 1024 * 10);
  ptr_sec = (int *)RTC_SEC;
  ptr_ns  = (int *)RTC_NS;
  printf("smp test test18 (snoop by random number)\n");
  printf("%x 10MB area kept\n", (unsigned int) ptr_void);

  ptr_malal = (int *) (((int) ptr_void + 0x1f) & 0xffffffe0);

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
  for(i = 0; i < ((DMA_TRANSFER_BYTES >> 2) | (DMA_TRANSFER_BYTES >> 4));
    i++) {
    *(ptr_malal + (1024 >> 2) + i) = (i & 0x3ff) + (i >> 12);
  }

  /* pre-exe 1: set CPU1 instructions to DDR */
  ptr_inst = (int *)(ptr_malal + 8);
  fd = open("te18c1.bin", O_RDONLY );
  if(fd == -1) {
    printf("file (te18c1.bin) open error\n");
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

  /* dma parameter setting */
  dmac_ch->sar  = (uint32_t) (ptr_malal + (1024 >> 2));
  dmac_ch->dar  = (uint32_t) (ptr_malal + (1024 >> 2) +
                              (DMA_TRANSFER_BYTES >> 2));
  dmac_ch->tcr  = (DMA_TRANSFER_BYTES >> 5);
  dmac_ch->chcr = DMAC_CHCR_RLD_NO_RELOAD |
                  DMAC_CHCR_CHAIN_DIS |
                  DMAC_CHCR_SRC_MODE_INC | DMAC_CHCR_DEST_MODE_INC |
                  DMAC_CHCR_REQ_SRC_PROG_REQ |
                  DMAC_CHCR_TRANSFER_SIZE_32_BYTE |
                  DMAC_CHCR_DME_EN;

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
  *ptr_data = (int) (ptr_malal + 8);
   ptr_data = (int *)0x8004;
  *ptr_data = (int) (ptr_malal + (1024 >> 2) - (16 >> 2));
  for(i = 0; i < 10; i++) {
  }
  printf("end setup CPU1 boot\n");

  time_pr_1 = clock( ); /* timer on */
  time_r_sec1 = *ptr_sec;
  time_r_ns1  = *ptr_ns;

   ptr_data = (int *)CPU1_ENABLE_ADR;
  *ptr_data = 1;           /* activate cpu1 */
  DMAC_REGS->dmaor = 1;    /* activae dma */

  /* main cpu0 execution */
  pat_index = 0;
  for(i = 0; i < limit; i++) {
    for(j = 0; j < (ARRSIZE >> 2); j++) {
      sumcpu = 0;
      for(k = 0; k < 5; k++) {
        if(k < 3) {
          sumcpu += test_array[pattern_mem[pat_index]];
        }
        else if(k == 3) {
          arr_index2 = pattern_mem[pat_index] + (i << 2);
          while(arr_index2 >= ARRSIZE) {
            arr_index2 -= ARRSIZE;
          }
          sumcpu += test_array[arr_index2];
        }
        else {
          arr_index2 = pattern_mem[pat_index] + (i << 1);
          while(arr_index2 >= ARRSIZE) {
            arr_index2 -= ARRSIZE;
          }
                 test_array[arr_index2] = 
          ((sumcpu + 11 + i) & 0x0001FFFF);
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
      sumcpu = 0;
      for(j = 0; j < 100; j++) {
        sumcpu += test_array[j];
      }
      printf("in-loop %x\n", sumcpu);
      printf("array %x %x %x %x %x \n",
        test_array[0], test_array[1], test_array[2], test_array[3],
        test_array[4]);
    }
  }
  while((dmac_ch->chcr & DMAC_CHCR_TRANSFER_END) == 0) {
    poll_count1 ++;
  }
  ptr_2 = ((int *)CPU01_COMM_AREA_HEAD) + 3;
  while(*ptr_2 == 0) {
    poll_count2 ++;
  }
  ptr_2 = (int *) CCR0;
  *ptr_2 = (*ptr_2) | 0x20; /* flush dcache */

  /* result display */
  time_pr_2 = clock( );
  time_r_sec2 = *ptr_sec;
  time_r_ns2  = *ptr_ns;
  sumcpu = 0;
  for(j = 0; j < ARRSIZE; j++) {
    sumcpu += test_array[j];
  }
  sumdma = 0;
  for(i = 0; i < (DMA_TRANSFER_BYTES >> 2); i++) {
    sumdma += *(ptr_malal + (DMA_TRANSFER_BYTES >> 2) + i);
  }
  
  printf("RESULT\n");
  printf("sumdma  %x\n", sumdma);
  printf("sumcpu  %x\n", sumcpu);
  for(j = 0; j <= 8; j++) {
    printf("  array %x %x %x %x %x\n", j,
      test_array[(j << 2) + 0], test_array[(j << 2) + 1],
      test_array[(j << 2) + 2], test_array[(j << 2) + 3]);
  }
  printf("cpu0 poll count1 (for complete dma) = %d\n", poll_count1);
  printf("cpu0 poll count2 (for complete cpu) = %d\n", poll_count2);
  printf("time = (process) %.2f sec, (real-time) %.2f sec\n",
    ((float) (time_pr_2 - time_pr_1) / 1.0e6),
    (time_r_sec2 - time_r_sec1) +
    (time_r_ns2  - time_r_ns1)  / 1.0e9);
 
  free(ptr_void); 
  return(0);
}
