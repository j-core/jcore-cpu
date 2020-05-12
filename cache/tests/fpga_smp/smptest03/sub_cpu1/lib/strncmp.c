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

#ifndef __HAVE_ARCH_STRNCMP
int strncmp(const char * cs,const char * ct,unsigned long count)
{
  register signed char __res = 0;
  
  while (count) {
    if ((__res = *cs - *ct++) != 0 || !*cs++)
      break;
    count--;
  }
  
  return __res;
}
#endif
