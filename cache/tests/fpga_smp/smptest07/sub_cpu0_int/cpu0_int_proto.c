/* cpu0_int_proto.c */

/* ------------------------------------------------------------------------- */
/* file create procedure                                                     */
/* (1) cc cpu0_int_proto.c -> cpu0_int_proto.s                               */
/*                                                                           */
/* (2) manually modify cpu0_int_proto.s -> cpu0_int.s                        */
/*                     (add rte, adjust stack push pop)                      */
/*                                                                           */
/* (3) cc cpu0_int.s -> cpu0_int                                             */
/*                                                                           */
/* (4) hexdump (xxd) cpu0_int -> sm05c0i.xxd (handler image)                 */
/*                                                                           */
/* Note: cpu0_int is created as relocatable.                                 */
/* ------------------------------------------------------------------------- */

#define MSG_AREA 0x8100

int main( )
{
  volatile char *ptr1, *ptr2;
  volatile int *ptr3;
  int i;
  char string_from_cpu0_int[] = "occur ipi (by CPU0 int)\n";
  /*                             ....+....1....+....2...+  (24 chars) */
 
  ptr3 = ((int *)MSG_AREA) + 1;  /* address + 4 */
  ptr1 = (char *) (*ptr3);
  ptr2 = &string_from_cpu0_int[0];
  while (*ptr1 != (char)0) {
    ptr1++;
  }
  for(i = 0; i < 24; i++) {
    *ptr1 = *ptr2;
    ptr1 ++;
    ptr2 ++;
  }
  return(0);
}

