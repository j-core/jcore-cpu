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

#ifndef __HAVE_ARCH_STRCPY
char * strcpy(char * dest,const char *src)
{
  char *tmp = dest;
  
  while ((*dest++ = *src++) != '\0')
    /* nothing */;
  return tmp;
}
#endif
