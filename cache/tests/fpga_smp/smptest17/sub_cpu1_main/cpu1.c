/* cpu1.c */
/* post processing (cpu1.c does not treat) */
/*   (1) set cpu1.SR.IMASK zero  */
/*   (2) set cpu1.VBR */

#define COMM_SRAM_HEAD 0x8000

/* AIC register */
#define AIC_CNTL0       0xabcd0500
#define AIC_BITPOS_PITEN 26
#define AIC_BITPOS_PITEN_MASK (1 << AIC_BITPOS_PITEN)
#define AIC_BITPOS_PITPARAMSET 12

/*           AIC parameter */
#define AICP_PITMASK    0x7
#define AICP_PITVECTOR  0x1B

/* cpu0 <-> cpu1 parameters */
/* base             offset(adrs) */
/* COMM_SRAM_HEAD + 0x100 : cpu1 processing complete */
/*                + 0x104 : address of sosu_prime[20000] = prt_sosu_prime */
/*                + 0x108 : limit (cpu0->cpu1) */
/*                + 0x10c : sosu(prime) count (cpu1->cpu0) */
/*                + 0x110 : PIT enable (cpu0->cpu1) */

int main ( )
{
  int count, i, i_large, j, limit, pit_on, quitflg ;
  volatile int *ptr1, *ptr2;
  volatile int *ptr_sosu_prime;

  ptr1 = ((int *)COMM_SRAM_HEAD) + (0x104 >> 2);
  ptr_sosu_prime = (int *) *ptr1;
  ptr1 ++;
  limit = *ptr1;
  ptr1 += 2;
  pit_on = *ptr1;

  /* AIC setting */
  if(pit_on == 1) {
     ptr2 = (int *) AIC_CNTL0 ;
    *ptr2 = ((*ptr2) & (~0x00000fff)) |
             (((AICP_PITMASK << 8) | AICP_PITVECTOR)
              << AIC_BITPOS_PITPARAMSET) ;  /* set testvect */
    *ptr2 = ((*ptr2) |
              AIC_BITPOS_PITEN_MASK ); /* set PIT enable */
  }

  for(i_large = 0; i_large < 1; i_large ++) {
    *(ptr_sosu_prime + 0) = 2;
    count = 1;
    for(i = 3; i < limit; i += 2) {
      quitflg = 0;
      j = 0;
      while(quitflg == 0) {
        if((*(ptr_sosu_prime + j)) *
           (*(ptr_sosu_prime + j)) > i) {
          *(ptr_sosu_prime + count) = i;
          count++;
          quitflg = 1;
        }
        else if((i % (*(ptr_sosu_prime + j))) == 0) {
          quitflg = 1;
        }
        j++;
      }
    }
  }
   ptr2 = ((int *)COMM_SRAM_HEAD) + (0x10c >> 2);
  *ptr2 = count;
  *(ptr2 - 3) = 1;  /* 3 = (0x10c - 0x100) >> 2 */

  /* AIC setting */
  if(pit_on == 1) {
     ptr2 = (int *) AIC_CNTL0 ;
    *ptr2 = 0;
  }

  while(1) { }
  return(0);
}
