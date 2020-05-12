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

char * ___strtok = NULL;

#ifndef __HAVE_ARCH_STRTOK
char * strtok(char * s,const char * ct)
{
  char *sbegin, *send;
  
  sbegin  = s ? s : ___strtok;
  if (!sbegin) {
    return NULL;
  }
  sbegin += strspn(sbegin,ct);
  if (*sbegin == '\0') {
    ___strtok = NULL;
    return( NULL );
  }
  send = strpbrk( sbegin, ct);
  if (send && *send != '\0')
    *send++ = '\0';
  ___strtok = send;
  return (sbegin);
}
#endif
