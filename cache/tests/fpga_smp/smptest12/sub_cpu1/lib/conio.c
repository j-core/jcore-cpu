/* conio.c: Console (RS232) I/O routines for a bare mc68332
 *
 * Copyright (c) 1995 Dionne & Associates Electronic Design
 * Copyright (c) 1995 DKG Technologies
 * Copyright (c) 2001-2003 by Arcturus Networks Inc.
 * Copyright (c) 1999-2000 Rt-Control Inc.
 * All rights reserved.
 *
 * This material is proprietary to Arcturus Networks Inc. and, in
 * addition to the above mentioned Copyright, may be subject to
 * protection under other intellectual property regimes, including
 * patents, trade secrets, designs and/or trademarks.
 *
 * Any use of this material for any purpose, except with an express
 * license from Arcturus Networks Inc. is strictly prohibited.
 *
 * 1999 - Tony added gets().
 */



#include <stdio.h>
#include "string.h"     /* for strlen(), strcpy(), strcat() */

extern  int inch(void);
extern  int outch(int ch);
extern  int read(int fd, char *buf, int count);
extern  int write (int fd, char *buf, int count);


/*
 * function:    putchar - output a character to the console
 *
 * parameters:  ch - the character to output
 *
 * returns:     0 - always completes successfully
 */
int putchar (unsigned int ch)
{
    char buf = ch;

    if (ch == '\n')
        putchar('\r');

    write(stdout, (char *)&buf, 1);

    return 0;
}



/*
 * function:    puts - output a string to the console
 *
 * parameters:  string - the string to output
 *
 * returns:     0 - always completes successfully
 */
int puts (unsigned char *string)
{
    write(stdout, string, strlen(string));
    write(stdout, "\n", 1);

    return 0;
}



/*
 * function:    getchar - get a character from the console
 *
 * parameters:  void
 *
 * returns:     ch - next character from the console
 */
int getchar ()
{
    char    buf;

    read(stdin, &buf, 1);

    return buf;
}



/*
 * function:    gets - get a string from the console, with a hard-coded limit
 *                  as to the maximum number of characters to read
 *
 * parameters:  string - the location where the string should be placed
 *
 * returns:     string - pointer to the string received
 */
char *gets(char *string) {
    char    rec_char;   /* Place for incoming chars */
    int     index = 0;  /* String index */
    int     done = 0;   /* Set when CR rec'd */

    /*** magic number hard-coded for maximum number of characters to read ***/
    while (!done && (index < 60)) {
        rec_char = inch();

        switch(rec_char) {
            case '\b':          /* handle deletes, either BS or DEL key */
            case 0x7f:
                if(index > 0) { /* Don't delete beyond buffer! */
                    index--;

                    outch('\b');
                    outch(' ');
                    outch('\b');
                }
                break;

            case '\r':          /* Handle CR or LF */
            case '\n':
                outch('\n');
                done = 1;
                break;

            default:
                if (rec_char >= ' ') {
                    string[index++] = rec_char;
                    outch(rec_char);    /* Echo data */
                }
                break;
        }
    }

    string[index] = 0;      /* Terminate string */

    return(string);
}



/*
 * function:    fgets - get a string from the console with a
 *                  parameter-controlled maximum number of characters to read
 *
 * parameters:  string - the location where the string should be placed
 *              size - the maximum number of characters to read
 *              fd - file descriptor to read from, not used, it's just here
 *                  for commonality with stdlib's version
 *
 * returns:     string - pointer to the string received
 */

/* NOTE: this is pretty much stolen from gets(), we really should look at
    combining the two */

char *fgets(char *string, int size, int fd) {
    int     rec_char;   /* Place for incoming chars */
    int     index = 0;  /* String index */
    int     done = 0;   /* Set when CR rec'd */

    while (!done && (index < size)) {
        rec_char = inch();

        switch(rec_char) {
            case '\b':          /* handle deletes, either BS or DEL key */
            case 0x7f:
                if(index > 0) { /* Don't delete beyond buffer! */
                    index--;

                    outch('\b');
                    outch(' ');
                    outch('\b');
                }
                break;

            case '\r':          /* Handle CR or LF */
            case '\n':
                /* NOTE: if you cat file > /dev/ttyS0, on a Linux machine,
                    to the same terminal that minicom is connected to, we
                    seemed to spuriously receive a \r or \n at about 660
                    chars. */
                outch('\n');
                done = 1;
                break;

            default:
                if (rec_char >= ' ') {
                    string[index++] = rec_char;
                    outch(rec_char);    /* Echo data */
                }
                break;
        }
    }

    string[index] = 0;      /* Terminate string */

    return(string);
}
