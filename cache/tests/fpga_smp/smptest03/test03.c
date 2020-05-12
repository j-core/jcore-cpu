#include <stdio.h>
#include <stdlib.h>

#define CPU1_INSTR_HEAD   0x14001000
#define CPU1_SP_INIT      0x14004ffc
#define SHAREMEM_DDR_HEAD 0x14010000

char instbuf[160];
int main( )
{
  void *ptr_void;
  volatile int *ptr_array_a32;
  volatile int *ptr_inst ;
  volatile int *ptr_data ;
  int i, j, sum, sum96, sum98, poll_count = 0;
  unsigned int instr0, instr1;
  FILE *fp1;

  ptr_void = malloc(1024 * 1024 * 10);
  printf("smp test test03\n");
  printf("%x 10MB area kept\n", (unsigned int) ptr_void);

  ptr_array_a32 = (int *)SHAREMEM_DDR_HEAD ;
  if(
     (ptr_array_a32 <  (int*)(ptr_void) ) ||
     (ptr_array_a32 > ((int*)(ptr_void) + (1024 * 1024 * 10) - 302))) {
    printf("fixed address variable out of range of allocated mem.\n");
    return(1);
  }

  /* step 1: set CPU1 instructions */
  ptr_inst = (int *)CPU1_INSTR_HEAD;
  fp1 = fopen ("te03c1.xxd", "r");
  for(i = 0; i < 75 ; i++) {
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

  /* step 2: clear array[0] - array[301] */
  for (i = 0; i < 302; i ++) {
    *(ptr_array_a32 + i) = 0;
  }
 
  /* step 3: setup CPU1 boot boot */
   ptr_data = (int *)0x8000;
  *ptr_data = CPU1_INSTR_HEAD;
   ptr_data = (int *)0x8004;
  *ptr_data = CPU1_SP_INIT;
  for(i = 0; i < 10; i++) {
  }
   ptr_data = (int *)0xabcd0640;
  *ptr_data = 1;
  printf("end setup CPU1 boot\n");

  /* step 4: create array[2] - array[101] */
  for (i = 0; i < 100; i ++) {
    *(ptr_array_a32 + 2 + i) = 2 * i;
  }
  *(ptr_array_a32 + 0) = 1;

  /* step 5: polling array[1] */
  while (*(ptr_array_a32 + 1) == 0) {
    poll_count ++;
  }
  
  /* step6: create array[2] - array[301] */
  sum = 0;
  for (i = 0; i < 100; i ++) {
    sum += *(ptr_array_a32 + 102 + i);
    *(ptr_array_a32 + 202 + i) = sum;
    if     (i == 95) sum96 = sum;
    else if(i == 97) sum98 = sum;
  }
  printf("sum96 = %d\n",      sum96);
  printf("sum98 = %d\n",      sum98);
  printf("sum   = %d\n",      sum  );
  printf("poll count = %d\n", poll_count);

  return(0);
}

