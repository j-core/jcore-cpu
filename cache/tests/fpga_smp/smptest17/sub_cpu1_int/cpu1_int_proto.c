/* cpu1_int_proto.c */

/* ------------------------------------------------------------------------- */
/* file create procedure                                                     */
/* (1) cc cpu1_int_proto.c -> cpu1_int_proto.s                               */
/*                                                                           */
/* (2) script (semi-manually) modify cpu1_int_proto.s -> cpu1_int.s          */
/*                     (add rte, adjust stack push pop)                      */
/*                                                                           */
/* (3) cc cpu1_int.s -> cpu1_int                                             */
/*                                                                           */
/* (4) copy cpu1_int to te17ih.bin                                           */
/*                                                                           */
/* Note: cpu1_int is created as relocatable.                                 */
/* ------------------------------------------------------------------------- */

/* cpu0 <-> cpu1 parameters */
/* base             offset(adrs) */
/* COMM_SRAM_HEAD + 0x100 : cpu1 processing complete */
/*                + 0x104 : address of sosu_prime[20000] = prt_sosu_prime */
/*                + 0x108 : limit (cpu0->cpu1) */
/*                + 0x10c : sosu(prime) count (cpu1->cpu0) */
/*                + 0x110 : PIT enable (cpu0->cpu1) */
/*                + 0x114 : int counter (init0 by cpu0, count up by cpu1-int) */

#define COMM_SRAM_HEAD   0x8000

int main( )
{
  volatile int *ptr1;

   ptr1 = ((int *)COMM_SRAM_HEAD) + (0x114 >> 2);
  *ptr1 = *ptr1 + 1;

  return(0);
}
