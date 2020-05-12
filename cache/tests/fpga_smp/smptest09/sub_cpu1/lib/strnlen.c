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

#ifndef __HAVE_ARCH_STRNLEN
unsigned long strnlen(const char * s, unsigned long count)
{
  const char *sc;
  
  for (sc = s; count-- && *sc != '\0'; ++sc)
    /* nothing */;
  return sc - s;
}
#endif
