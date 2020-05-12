#include <stdio.h>
#include <stdlib.h>

/* --------------------------------------------------------------- */
/* (smp_test07.c  O. Nishii 2015-05-14)                            */
/* smp_test20.c  O. Nishii 2016-01-18                              */
/* function : test cpu0->cpu1 cpu1->cpu0 IPI (inter-processor-     */
/* interrupt) h/w and interrupt handler mechinism confirmation     */
/* --------------------------------------------------------------- */
/* this test uses         | cpu0, cpu1, aic0(aic of cpu0),         */
/*                        | aic1(aic of cpu1)                      */
/* this test does not use | dma                                    */
/* --------------------------------------------------------------- */

/*           SMP register */
#define CPU1EN          0xabcd0640
/*           CCR register */
#define CCR1            0xabcd00c4
/*           AIC registers (channel 0) */
/*           AIC register */
/* #define AIC_ILEVEL      0xabcd0208 */
/*           reference aic.vhd line 173 */

/*           IRQ3 INTERRUPT CODE */
#define IRQ3_INTCODE     0x61
/*           make same number as soc_1v1_evb_2v0_smpaic2/devices.vhd */
/*                IRQ_SI0_NUM => 97, (must appear twice) */

/* communication variable design */
#define MSG_AREA         0x8100

/* ------------------------------------------------------------------------- */
/*          sender  receiver meaning                                         */
/* b(case)=0x8100                                                            */
/* ------------------------------------------------------------------------- */
/* b+0x04 : c0-main c0-int&  head address of sharing string varible          */
/*                  c1-int                                                   */
/* b+0x0c : c1-main c0-main  cpu1 main computaion result                     */
/* ------------------------------------------------------------------------- */
/* b+0x10 : c1-main c0-main  done status (0/1) of cpu1 main computaion       */
/*                           result                                          */
/* b+0x14 : c1-int  c0-main  for(i) variable of cpu1 main cpatured first     */
/*                           cpu1-int processes first time                   */
/* b+0x18 : c1-int  c0-main  for(i) variable of cpu1 main cpatured first     */
/*                           cpu1-int processes second time                  */
/* b+0x18 : c1-main c1-int   for(i) variable itself of cpu1 main to easily   */
/*                           visible for cpu1-int                            */
/* ------------------------------------------------------------------------- */
/* b+0x20 : c0-main& c0-int  counter of c0-int (IRQ(3)) active               */
/*          c0-int                                                           */
/* ------------------------------------------------------------------------- */

/* CPU1 int jump PC table (in sram) */
#define VBR_CPU1         0x8180

/* memory map in DDR malloc( ) area */
/* up   higher address */
/*    pos                                           actual size              */
/*  ^ (byte)                                       (byte)                    */
/*  | ---------------------------------------------------------------------  */
/*  | +0x3F00 * CPU1 stack area                                              */
/*  |  +0x520 * CPU1 IPI interrupt handler (instr) (0x190) sm20c1i.xxd       */
/*  |  +0x320 * CPU1 main program (instr)          ( 0xd0) sm20c1a.xxd       */
/*  |  +0x120 * CPU0 IPI interrupt handler (instr) (0x150) sm20c0i.xxd       */
/*  |   +0x20 * CPU0/CPU1 shared variable (string) (0x100)                   */
/*  V      +0 * head = malloc( )(add align 32B for portability)              */
/* down lower address */
#define OFFSET_CPU1_STACK         0x3F00
#define OFFSET_CPU1_INT           0x520
#define OFFSET_CPU1_MAIN          0x320
#define OFFSET_CPU0_INT           0x120


volatile int *g_ptr_inst_c0i, *g_ptr_inst_c1a, *g_ptr_inst_c1i;
volatile int *g_ptr2;

int main ( )
{
  void *ptr1;
  volatile int *ptr3, *ptr4_cpu0_jumppc_irq3, *ptr5;
  volatile int *ptr6_cpu1_jumppc_irq3;
  int cpu0_reg_vbr, flg, i, i_work, j, poll_count = 0;
  int get_vbr( );
  int set_software_codes( );

  /* ----------------------------------------------------------------------- */
  /* step 1: memory alloc */
  ptr1 = malloc(0x5000); /* larger than 0x3f00 + 0x1c */
  i_work = (int) ptr1;
  while((i_work & 0x1f) != 0) {
    i_work ++;
  }
  g_ptr2 = (int *) i_work;
  printf("malloc (algn32) %x\n", (unsigned int)g_ptr2);

  /* ----------------------------------------------------------------------- */
  /* step 2: set all codes (cpu0-int, cpu1-main, cpu1-int) */
  set_software_codes( );

  /* ----------------------------------------------------------------------- */
  /* step 3: clear memory (sram and ddr) , set software variable */
  ptr3 = (int *)MSG_AREA;
  for (i = 0; i < 9; i++) { *(ptr3 + i) = 0; }
  ptr3 = (int *)(MSG_AREA) + 1;
  *ptr3 = (int)(g_ptr2 + 8);
  ptr3 = g_ptr2 + 8;
  for (i = 0; i < 32; i++) { *(ptr3 + i) = 0; }

  /* ----------------------------------------------------------------------- */
  /* step 4: update int. pc boot pc */
  /* step 4-a (cpu0) */
  cpu0_reg_vbr = get_vbr( ); /* assembly routine including "stc vbr" */
  ptr4_cpu0_jumppc_irq3 = (((int *)cpu0_reg_vbr) + IRQ3_INTCODE);
                       /* IRQ3_INTCODE is outside of (int *), */
                       /* this is scaled x4 */
  printf    ("cpu0 jumppc_irq3 (modified adr.) = %x\n", (unsigned int)
         ptr4_cpu0_jumppc_irq3);
        *ptr4_cpu0_jumppc_irq3 = (int)(g_ptr2) + OFFSET_CPU0_INT;
                       /* cpu0 handler head address */
  /* step 4-b (cpu1) */
   ptr5 = (int *)0x8000;
  *ptr5 = (int)g_ptr2 + OFFSET_CPU1_MAIN ; /* reset pc */
   ptr5 = (int *)0x8004;
  *ptr5 = (int)g_ptr2 + OFFSET_CPU1_STACK ; /* reset sp */
   ptr6_cpu1_jumppc_irq3 = (int *)VBR_CPU1 + IRQ3_INTCODE;
  *ptr6_cpu1_jumppc_irq3 = (int)g_ptr2 + OFFSET_CPU1_INT;
 
  /* ----------------------------------------------------------------------- */
  /* step 5: set cpu0-AIC reg */
  /* ptr5 = (int *)AIC_ILEVEL; *ptr5 = *ptr5 | 0x6000; */
                           /* set ilevel(3) non zero */ 
                           /* remove no AIC_LEVEL reg, 2016-01 */

  /* ----------------------------------------------------------------------- */
  /* step 6: execution */
  sprintf((char *)(g_ptr2 + 8) , "exec (by cpu0 main)\n");
  ptr5 = (int *)CPU1EN;
  *ptr5 = 1;
  flg =                   *(((int *)MSG_AREA) + 7);
  while(flg == 0) { flg = *(((int *)MSG_AREA) + 7); }
    /* trivial synchronization, detect non-zero and understand CPU1 */
    /* AIC_LEVEL is set */
  /* generate IPI to CPU1 (twice time) */
  for(i = 0; i < 2; i++) {
    ptr5 = (int *)CCR1;
    *ptr5 = *ptr5 | 0x10000000;
    for(j = 0; j < 1000; j++) { }
  }

  /* ----------------------------------------------------------------------- */
  /* step 7: wait for cpu1 main completes computation */
  flg =                   *(((int *)MSG_AREA) + 4);
  while(flg == 0) { flg = *(((int *)MSG_AREA) + 4); poll_count++; }

  /* ----------------------------------------------------------------------- */
  /* step 8: display result, free alloced mem. */
  printf("result\n");
  puts(                                  (char *) g_ptr2    + 32);
  printf("computation of cpu1 %d\n",    *(((int *)MSG_AREA) + 3));
  printf("captured loop var cpu1 %d\n", *(((int *)MSG_AREA) + 5));
  printf("captured loop var cpu1 %d\n", *(((int *)MSG_AREA) + 6));
  printf("poll_count(wait until cpu1 comu): %d\n", poll_count);
  free(ptr1);
  
  return(0);
}

int set_software_codes( )
{
  int i, j;
  volatile int *ptr_inst_work;
  unsigned int instr0, instr1;
  char instbuf[160]; /* file read line buffer */
  FILE *fp1;

  /* cpu0-int */
  g_ptr_inst_c0i = g_ptr2 + (OFFSET_CPU0_INT >> 2);
  ptr_inst_work = g_ptr_inst_c0i;
  fp1 = fopen ("sm20c0i.xxd", "r");
  for(i = 0; i < 37 ; i++) { /* lines 22-36 = 15 lines x 16 byte = 240 byte */
    fgets(instbuf, 160, fp1);
    if(i >= 21) {
      for(j = 0; j < 4; j++) {
        sscanf(&instbuf[10 * j +  9], "%x", &instr0);
        sscanf(&instbuf[10 * j + 14], "%x", &instr1);
        *(ptr_inst_work) = (int)((instr0 << 16) | instr1);
        ptr_inst_work++;
      }
    }
  }
  fclose(fp1);
  /* cpu1-main */
  g_ptr_inst_c1a = g_ptr2 + (OFFSET_CPU1_MAIN >> 2);
  ptr_inst_work = g_ptr_inst_c1a;

  /* -------------------------------------------------------------- */
  /* manual asm (hex punch-in) processing VBR = 0x8180 */
  *(ptr_inst_work) = 0xe17f7102; /* manual asm.  mov   #127,r1 */
                                 /* manual asm.  add   #2,r1 */
    ptr_inst_work++;
  *(ptr_inst_work) = 0x4118e27f; /* manual asm.  shll8 r1 */
                                 /* manual asm.  mov   #127,r2 */
    ptr_inst_work++;
  *(ptr_inst_work) = 0x7201212b; /* manual asm.  add   #1,r2 */
                                 /* manual asm.  or    r2,r1 --(r1=r2|r1) */
    ptr_inst_work++;
  *(ptr_inst_work) = 0x412e0009; /* manual asm.  ldc   r1,vbr */
                                 /* manual asm.  nop */
    ptr_inst_work++;
  /* -------------------------------------------------------------- */
  *(ptr_inst_work) = 0xe100410e; /* manual asm.  mov   #0,r1  */
                                 /* manual asm.  ldc   r1,sr  */
    ptr_inst_work++;
  for(i = 0; i < 3; i ++) {
    *(ptr_inst_work) = 0x00090009; /* manual asm.  nop */
                                   /* manual asm.  nop */
      ptr_inst_work++;
  }
  /* -------------------------------------------------------------- */

  fp1 = fopen ("sm20c1a.xxd", "r");
  for(i = 0; i < 35 ; i++) { /* lines 22-34 = 13 lines x 16 byte =  208 byte */
    fgets(instbuf, 160, fp1);
    if(i >= 21) {
      for(j = 0; j < 4; j++) {
        sscanf(&instbuf[10 * j +  9], "%x", &instr0);
        sscanf(&instbuf[10 * j + 14], "%x", &instr1);
        *(ptr_inst_work) = (int)((instr0 << 16) | instr1);
        ptr_inst_work++;
      }
    }
  }
  fclose(fp1);
  /* cpu1-int */
  g_ptr_inst_c1i = g_ptr2 + (OFFSET_CPU1_INT >> 2);
  ptr_inst_work = g_ptr_inst_c1i;
  fp1 = fopen ("sm20c1i.xxd", "r");
  for(i = 0; i < 45 ; i++) { /* lines 22-44 = 23 lines x 16 byte = 368 byte */
    fgets(instbuf, 160, fp1);
    if(i >= 21) {
      for(j = 0; j < 4; j++) {
        sscanf(&instbuf[10 * j +  9], "%x", &instr0);
        sscanf(&instbuf[10 * j + 14], "%x", &instr1);
        *(ptr_inst_work) = (int)((instr0 << 16) | instr1);
        ptr_inst_work++;
      }
    }
  }
  fclose(fp1);
  return(0);
}

