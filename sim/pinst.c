#include <stdio.h>
#include <stdlib.h>

#include "sh2instr.h"

/*
  The binary built from this file can be a gtkwave Translate Filter
  Process to translate opcodes to instruction names. Right-click on
  the opcode line in gtkwave, go to Data Format > Translate Filter
  Process. Be sure to also set data format to decimal and not hex
  because this uses atoi to parse.
 */

char buf[256];

int
main(int argc, char *argv[])
{
   unsigned short i;

   while(fgets(buf, sizeof(buf), stdin)) {
     i = atoi(buf);
     op_name(buf, sizeof(buf), i);
	 printf("%s\n", buf);
     fflush(stdout);
   }
   return 0;
}
