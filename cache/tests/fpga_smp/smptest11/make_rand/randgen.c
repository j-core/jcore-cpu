#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main( )
{
  int i;
  int count = 0;
  unsigned int udt;
  unsigned int udt2;

  srand((unsigned)time(NULL));
  rand( );
  rand( );
  rand( );
  rand( );
  rand( );

  for(i = 0; (i < 10000000) && (count <= 6000); i++) {
    udt = rand( );
    udt2 = udt & 0xfff;
    if(udt2 < 2560) {
      printf("%3d %d\n", count, udt2);
      count ++;
    }
  }

  exit(0);
}
