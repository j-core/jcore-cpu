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

/*
 * find the first occurrence of byte 'c', or 1 past the area if none
 */
#ifndef __HAVE_ARCH_MEMSCAN
void * memscan(void * addr, int c, unsigned long size)
{
  unsigned char * p = (unsigned char *) addr;

  while (size) {
    if (*p == c)
      return (void *) p;
    p++;
    size--;
  }
  return (void *) p;
}
#endif
