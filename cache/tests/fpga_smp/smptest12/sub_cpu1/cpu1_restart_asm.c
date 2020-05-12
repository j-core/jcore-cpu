int cpu1_restart_asm ( )
{
  int *ptr_1;

  /* this c will be replaced by hand coded asm */

  ptr_1 = (int *)0x77c;

  while (1) {
    ptr_1 += 7;
  }
}
