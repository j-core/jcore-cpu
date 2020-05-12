/* ------------------------------------------------------------------------- */
/* cpu1.c for smp test08  O. Nishii */
/* ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
/* file create procedure                                                     */
/* (1) cc cpu1.c -> cpu1                                                     */
/*                                                                           */
/* (2) hexdump (xxd) cpu1 -> sm08c1.xxd (cpu1 image)                         */
/*                                                                           */
/* Note ro compile env.:  cpu1_main is created as relocatable .              */
/* ------------------------------------------------------------------------- */

/* fixed address */
#define ADR_CPU01_COMM_HEAD 0x80e0
#define ADR_CPU0_FORV     0x8100
#define ADR_CPU1_FORV     0x8102
#define ADR_CPU0_SUMVL    0x8108
#define ADR_CPU1_SUMVL    0x810a
#define ADR_CPU0_SUMVH    0x810e
#define ADR_CPU1_SUMVH    0x810c

int main( )
{
  int limit, sum_int;
  volatile short *ptr_i, *ptr_sh_1;
  volatile unsigned short *ptr_suml, *ptr_sumh;
  volatile int *ptr_data;

  /* get limit for cpu1 */

  ptr_sh_1 = (short *)ADR_CPU01_COMM_HEAD;
  limit = (int)(*ptr_sh_1);
  ptr_i = (short *) ADR_CPU1_FORV;
  ptr_sumh = (unsigned short *) ADR_CPU1_SUMVH;
  ptr_suml = (unsigned short *) ADR_CPU1_SUMVL;
  sum_int = 0;
  *ptr_sumh = 0;
  *ptr_suml = 0;
  
  /* step 4: sum 1 - to limit */
  for((*ptr_i) = 1; (*ptr_i) <= (short)limit; (*ptr_i)++) {
    sum_int = ((int)*ptr_sumh) << 16 | ((int)*ptr_suml);
    sum_int += (*ptr_i);
    *ptr_suml = (unsigned short) (sum_int & 0xffff);
    *ptr_sumh = (unsigned short) ((sum_int >> 16) & 0xffff);
    if((*ptr_i) == ((short)limit) >> 1) { /* adhoc trial inner loop time */
                                          /* different to cpu0 */
      ptr_data = (int *)ADR_CPU01_COMM_HEAD + 1; *ptr_data = 0x12345;
    }
  } /* end of (for i = ... */

  /* step 5 wait for cpu1 execuion completion */
  ptr_data = (int *)ADR_CPU01_COMM_HEAD + 1; *ptr_data = sum_int;
  ptr_sh_1 = (short *)ADR_CPU01_COMM_HEAD + 1; *ptr_sh_1 = 1;

  /* infinite loop */
  while (1) { 
  }
}
