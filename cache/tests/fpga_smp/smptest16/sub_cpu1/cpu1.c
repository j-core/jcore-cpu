/* cpu1.c */

#define AIC_HEAD 0xabcd0500
#define COMM_SRAM_HEAD 0x8000

int main ( )
{
  int i;
  unsigned int write_data;
  volatile unsigned int *ptr1, *ptr2;


  /* read aic registers */
  for(i = 0; i < 12; i++) {
     ptr1 = ((unsigned int *)AIC_HEAD) + i;
     ptr2 = ((unsigned int *)COMM_SRAM_HEAD) + i + 8;
    *ptr2 = *ptr1;
  }
  /* write & read aic registers */
  for(i = 1; i < 12; i++) {
    if(i == 0) {
      write_data = 0xf8ffffff;
    }
    else {
      write_data = 0xffffffff;
    }
     ptr1 = ((unsigned int *)AIC_HEAD) + i;
     ptr2 = ((unsigned int *)COMM_SRAM_HEAD) + i + 20;
    *ptr1 = write_data;
    *ptr2 = *ptr1;
    if(i == 0) {
      *ptr1 = 0;
    }
  }
  
   ptr2 = ((unsigned int *)COMM_SRAM_HEAD) + 40;
  *ptr2 = 1;

  while(1) { }
  return(0);
}
