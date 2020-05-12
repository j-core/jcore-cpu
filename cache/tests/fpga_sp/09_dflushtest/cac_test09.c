#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define CCR 0xabcd00c0

int sosu_prime[20000];

int main ( )
{
  /* integer for calc */
  int count, i, i_large, j, limit, quitflg, inner_loop_count;
  /* integer for control */
  int i_dummy;
  volatile int * ptr_1;
  int log2_cache_flush_interval;
  int cache_flush_interval;
  long time_1, time_2;

  printf("cache_test_09: input max number (<200000)\n");
  scanf("%d", &limit);
  printf("cache_test_09: input log2(cache flush interval) (0<=x<=8)\n");
  printf("               if x=0 no flush\n");
  scanf("%d", &log2_cache_flush_interval);

  time_1 = clock( );

    for(i_dummy = 0; i_dummy < 100; i_dummy ++) { }

  if     (log2_cache_flush_interval == 0)  {
               cache_flush_interval = 0x0; }
  else if(log2_cache_flush_interval == 1)  {
               cache_flush_interval = 0x1; }
  else if(log2_cache_flush_interval == 2)  {
               cache_flush_interval = 0x3; }
  else if(log2_cache_flush_interval == 3)  {
               cache_flush_interval = 0x7; }
  else if(log2_cache_flush_interval == 4)  {
               cache_flush_interval = 0xf; }
  else if(log2_cache_flush_interval == 5)  {
               cache_flush_interval = 0x1f; }
  else if(log2_cache_flush_interval == 6)  {
               cache_flush_interval = 0x3f; }
  else if(log2_cache_flush_interval == 7)  {
               cache_flush_interval = 0x7f; }
  else if(log2_cache_flush_interval == 8)  {
               cache_flush_interval = 0xff; }
 
  inner_loop_count = 0;
  for(i_large = 0; i_large < 1; i_large ++) {
    sosu_prime[0] = 2;
    sosu_prime[19999] = 20000 - i_large;
    count = 1;
    for(i = 3; i < limit; i += 2) {
      quitflg = 0;
      j = 0;
      while( quitflg == 0) {
        if(sosu_prime[j] * sosu_prime[j] > i) {
          sosu_prime[count] = i;
          count++;
          quitflg = 1;
        }
        else if((i % sosu_prime[j]) == 0) {
          quitflg = 1;
        }
        j++;
        inner_loop_count ++;
        if((cache_flush_interval != 0) &&
           ((inner_loop_count & cache_flush_interval) == 0)) {
          ptr_1 = (int *) CCR; *ptr_1 = (*ptr_1 | 0x00000200);
        }
      }
    }
    printf("count %d\n", count);
  }
  time_2 = clock( );
  printf("compu time = %.2f \n", ((float) (time_2 - time_1) / 1.0e6));
  return(0);
}
