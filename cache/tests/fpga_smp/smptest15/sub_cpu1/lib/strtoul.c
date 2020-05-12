/*
 *  Copyright (C) 1991, 1992  Linus Torvalds
 */

/*
 * D. Jeff Dionne, 1995, 1999.
 */

/* Additional hole filled: strtol
 * M. Schlifer, NOV 1995.
*/



#include <string.h>
#include <ctype.h>



#ifndef __HAVE_ARCH_STRTOUL
/*
 * function:    strtoul - convert a string to an unsigned long integer
 *
 * parameters:  cp - pointer to string to be converted
 *              endp - pointer to where pointer to the first invalid character
 *                  should be stored
 *              base - the base to use for string conversion
 *
 * returns:     value
 */
unsigned long   strtoul(const char *cp, char **endp, unsigned int base)
{
    unsigned long   result = 0, value;
 
    /* if base = 0 */
    if (!base) {
        /* then default to base 10 */
        base = 10;

        /* if string's first character is a 0 */
        if (*cp == '0') {
            /* then default to base 8 */
            base = 8;
            cp++;

            /* if string's second character is an x and the next character is
                a hex digit */
            if ((*cp == 'x') && isxdigit(cp[1])) {
                cp++;
                base = 16;
            }
        }
    }

    while (isxdigit(*cp) && (value = isdigit(*cp) ? *cp - '0' :
        (islower(*cp) ? toupper(*cp) : *cp) - 'A' + 10) < base) {
        result = result * base + value;
        cp++;
    }

    /* if caller passed in a non-NULL endp value */
    if (endp)
        /* then save location of first non-valid character */
        *endp = (char *)cp;

    return result;
}
#endif
