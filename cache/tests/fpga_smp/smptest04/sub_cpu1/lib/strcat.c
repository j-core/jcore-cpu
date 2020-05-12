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

#ifndef __HAVE_ARCH_STRCAT
char * strcat(char * dest, const char * src)
{
  char *tmp = dest;
  
  while (*dest)
    dest++;
  while ((*dest++ = *src++) != '\0');

  return tmp;
}
#endif
