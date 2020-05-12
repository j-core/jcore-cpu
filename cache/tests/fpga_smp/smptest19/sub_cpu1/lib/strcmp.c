/*
 *  Copyright (C) 1991, 1992  Linus Torvalds
 */

/* 
 * D. Jeff Dionne, 1995.
 */

/* Additional hole filled: strtol
 * M. Schlifer, NOV 1995.
*/

#include <string.h>
#include <ctype.h>

#ifndef __HAVE_ARCH_STRCMP
int strcmp(const char * cs,const char * ct)
{
  register signed char __res;
  
  while (1) {
    if ((__res = *cs - *ct++) != 0 || !*cs++)
      break;
  }
  
  return __res;
}
#endif
