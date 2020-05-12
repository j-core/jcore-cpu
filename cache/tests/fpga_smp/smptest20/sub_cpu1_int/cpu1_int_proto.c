/* cpu1_int.c */

/* ------------------------------------------------------------------------- */
/* file create procedure                                                     */
/* (1) cc cpu1_int_proto.c -> cpu1_int_proto.s                               */
/*                                                                           */
/* (2) manually modify cpu1_int_proto.s -> cpu1_int.s                        */
/*                     (add rte, adjust stack push pop)                      */
/*                                                                           */
/* (3) cc cpu1_int.s -> cpu1_int                                             */
/*                                                                           */
/* (4) hexdump (xxd) cpu1_int -> sm05c1i.xxd (handler image)                 */
/*                                                                           */
/* Note: cpu1_int is created as relocatable.                                 */
/* ------------------------------------------------------------------------- */

#define MSG_AREA 0x8100
#define CCR0 0xabcd00c0

int main( )
{
  volatile char *ptr1, *ptr2;
  volatile int *ptr3, *ptr4, *ptr5;
  int i;
  char string_from_cpu1_int[] = "IPI occur (by CPU1 int)\n";
  /*                             ....+....1....+....2....  (24 chars) */
  int generate_ipi_to_cpu0( );
 
  ptr3 = ((int *)MSG_AREA) + 1;  /* address + 4 */
  ptr1 = (char *) (*ptr3);
  ptr2 = &string_from_cpu1_int[0];
  while (*ptr1 != (char)0) {
    ptr1++;
  }
  for(i = 0; i < 24; i++) {
    *ptr1 = *ptr2;
    ptr1 ++;
    ptr2 ++;
  }
  *ptr2 = (char)0;
  /* copy cpu1 main for i variable into MSG_AREA to see concurrency */
  /* as SMP observation */
  ptr4 = ((int *)MSG_AREA) + 5;
  ptr5 = ((int *)MSG_AREA) + 7;
  if(*ptr4 == 0) {
    *ptr4 = *ptr5;
  }
  else {
    *(ptr4 + 1) = *ptr5;
  }
  /* generate cpu0 IPI interrupt */
  ptr3 = (int *)CCR0;
  *ptr3 = *ptr3 | 0x10000000;

  return(0);
}

