/* cpu1_main.c */

/* ------------------------------------------------------------------------- */
/* file create procedure                                                     */
/* (1) cc cpu1_main.c -> cpu1_main                                           */
/*                                                                           */
/* (2) hexdump (xxd) cpu1_main -> sm05c1m.xxd (main-routine image)           */
/*                                                                           */
/* Note:  cpu1_main is created as relocatable ...                            */
/* ------------------------------------------------------------------------- */

#define MSG_AREA 0x8100
/*           AIC register */
#define AIC_ILEVEL      0xabcd0508
/*           reference aic.vhd line 173 */
/*           memo: 2016-01-18 write to AIC_ILEVEL has no effect. */
/*           it is kept because, this write has no side effect */

int main( )
{
  int sum = 0;
  volatile int *ptr_i; /* instead of i */
  volatile int *ptr1, *ptr2;

  ptr1 = ((int *)MSG_AREA) + 11;
  *ptr1 = 11;
  /* set cpu1 AIC */
  ptr2 = (int *)AIC_ILEVEL;
  *ptr2 = 0x00006000; /* set ilevel(3) non zero */

  ptr_i = ((int *)MSG_AREA) + 7;
  for((*ptr_i) = 0; ((*ptr_i) < 5001); (*ptr_i)++) {
    sum = sum + (*ptr_i);
  }
  ptr1 = ((int *)MSG_AREA) + 3;
  *ptr1 = sum;
  ptr1 = ((int *)MSG_AREA) + 4;
  *ptr1 = 1;

  /* infinite loop */
  while (1) { 
  }
}
