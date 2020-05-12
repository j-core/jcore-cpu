#include <stdio.h>
#include <stdlib.h>
#include "dma.h"

/* test19.c (smp test 19) */
/*   2015-10-26 O. NISHII */
/* function bus mux (cpu0 cpu1 dma) parallel access */
/*                  cpu0 and cpu1 : TAS spinlock */

/* fixed address */
#define CPU1_INSTR_HEAD   0x14001000
#define CPU1_SP_INIT      0x14004ffc
#define SHAREMEM_DDR_HEAD 0x14010000
#define ADRS_LOCK_VAR     0x14010020

/* cache */
#define CCR0              0xabcd00c0

/* dma */
#define DMA_TRANSFER_BYTES    524288
#define DMA_CH            33
#define DMA_ROWMISS_ESC_OFFSET_BYTES 1024
/* expected dma */
#define EXPECTED_DMA      0x41e0000

/* rtc */
#define RTC_SEC           0xabcd0224
#define RTC_NS            0xabcd0228

extern int get_lock_cpu0 ( );
                            /* created as assembler program with TAS.B inst. */

int dma_mem[(DMA_TRANSFER_BYTES >> 1) +
            ((DMA_ROWMISS_ESC_OFFSET_BYTES + 32) >> 2)];

char instbuf[160];
/*                    0  1  2  3  4  5  6  7  8  9 */
int global_mem[20] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

int dma_ch =                                                  DMA_CH;
volatile struct dmac_ch_regs *dmac_ch = DMAC_REGS->channels + DMA_CH;

int main( )
{
  int release_lock_cpu0( );
  int i, j, limit, poll_count1 = 0, poll_count2 = 0, sumdma;
  unsigned int instr0, instr1;
  void *ptr_void;
  volatile int *ptr_array_a32, *ptr_inst, *ptr_data;
  volatile char *ptr_lock;
  FILE *fp1;
  long time_pr_1, time_pr_2;
  int time_r_sec1, time_r_sec2;
  int time_r_ns1,  time_r_ns2;
  volatile int *ptr_sec, *ptr_ns;
  int dma_align32_offset;

  ptr_void = malloc(1024 * 1024 * 10);

  ptr_sec = (int *)RTC_SEC;
  ptr_ns  = (int *)RTC_NS;

  printf("smp test test19 (cpu0 cpu1 dma parallel access\n");
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

  dma_align32_offset = 8 - ((((int)&dma_mem[0]) & 0x1c) >> 2);
  if(dma_align32_offset == 8) {
    dma_align32_offset = 0;
  }

  for(i = 0; i < ((DMA_TRANSFER_BYTES >> 2) + (DMA_TRANSFER_BYTES >> 4));
      i++) {
    dma_mem[i + dma_align32_offset] = (i & 0x3ff) + (i >> 12);
  }

  /* step 1: set CPU1 instructions to DDR */
  ptr_inst = (int *)CPU1_INSTR_HEAD;
  fp1 = fopen ("te19c1.xxd", "r");
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

  /*         setup dma */
  dmac_ch->sar  = (uint32_t) &dma_mem[dma_align32_offset];
  dmac_ch->dar  = ((uint32_t) &dma_mem[dma_align32_offset]) +
                     DMA_TRANSFER_BYTES + DMA_ROWMISS_ESC_OFFSET_BYTES;
  printf(" dma_adres adr0 = %x, off= %d,\n", (unsigned int)(&dma_mem[0]),
      dma_align32_offset);
  dmac_ch->tcr  = (DMA_TRANSFER_BYTES >> 5);
  dmac_ch->chcr = DMAC_CHCR_RLD_NO_RELOAD |
                  DMAC_CHCR_CHAIN_DIS |
                  DMAC_CHCR_SRC_MODE_INC | DMAC_CHCR_DEST_MODE_INC |
                  DMAC_CHCR_REQ_SRC_PROG_REQ |
                  DMAC_CHCR_TRANSFER_SIZE_32_BYTE |
                  DMAC_CHCR_DME_EN;

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

   ptr_data = (int *)0xabcd0640;
  *ptr_data = 1;           /* activate cpu1 */
  DMAC_REGS->dmaor = 1;    /* activate dma */

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

  /* step 5 wait for dma, cpu1 execuion completion */
  while((dmac_ch->chcr & DMAC_CHCR_TRANSFER_END) == 0) {
    poll_count1 ++;
  }
  while(*(ptr_array_a32 + 5) == 0) {
    poll_count2 ++;
  }
  ptr_data = (int *) CCR0;
  *ptr_data = (*ptr_data) | 0x20; /* flush dcache */

  /* result display */
  time_pr_2 = clock( );
  time_r_sec2 = *ptr_sec;
  time_r_ns2  = *ptr_ns;
  sumdma = 0;
  for(i = 0; i < (DMA_TRANSFER_BYTES >> 2); i++) {
    sumdma += dma_mem[
      ((DMA_TRANSFER_BYTES + DMA_ROWMISS_ESC_OFFSET_BYTES) >> 2) + 
       dma_align32_offset + i
    ];
  }

  /* step 6 display result */
  printf("results\n");
  printf("  here, cpu#0 increments array[0] & array[1] with lock\n");
  printf("        cpu#1 increments array[1] & array[2] with lock\n");
  for(i = 0; i < 3; i++) {
    printf("array[%d] = %d (deci) \n", i, *(ptr_array_a32 + i));
  }
  printf("global_mem[ 3] = %d (deci) \n", global_mem[ 3]);
  printf("global_mem[ 5] = %d (deci) \n", global_mem[ 5]);
  printf("global_mem[ 6] = %d (deci) \n", global_mem[ 6]);
  printf("global_mem[ 7] = %d (deci) \n", global_mem[ 7]);
  printf("global_mem[10] = %d (deci) \n", global_mem[10]);
  printf("global_mem[14] = %d (deci) \n", global_mem[14]);
  printf("sum dma         = %x (hex ) \n", sumdma);
  printf("sum dma (expec) = %x (hex ) \n", EXPECTED_DMA);
  printf("cpu0 poll count1 (for completion dma) = %d\n",  poll_count1);
  printf("cpu0 poll count2 (for completion cpu1) = %d\n", poll_count2);

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
