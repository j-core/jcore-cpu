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

#ifndef __HAVE_ARCH_STRSPN
unsigned long strspn(const char *s, const char *accept)
{
  const char *p;
  const char *a;
  unsigned long count = 0;
  
  for (p = s; *p != '\0'; ++p) {
    for (a = accept; *a != '\0'; ++a) {
      if (*p == *a)
	break;
    }
    if (*a == '\0')
      return count;
    ++count;
  }

  return count;
}
#endif
