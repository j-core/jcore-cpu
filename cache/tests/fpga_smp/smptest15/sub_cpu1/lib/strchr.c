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

#ifndef __HAVE_ARCH_STRCHR
char * strchr(const char * s, int c)
{
  for(; *s != (char) c; ++s)
    if (*s == '\0')
      return NULL;
  return (char *) s;
}
#endif
