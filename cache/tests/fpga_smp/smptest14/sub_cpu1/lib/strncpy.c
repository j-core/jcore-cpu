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

#ifndef __HAVE_ARCH_STRNCPY
char * strncpy(char * dest,const char *src,unsigned long count)
{
  char *tmp = dest;
  
  while (count-- && (*dest++ = *src++) != '\0')
    /* nothing */;
  
  return tmp;
}
#endif
