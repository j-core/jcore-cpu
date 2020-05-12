#ifndef _STDIO_H
#define _STDIO_H

#include <stdarg.h>

#define stdin   0
#define stdout  1
#define stderr  2

#define EOF     -1

typedef int     FILE;

extern  int getchar (void);
#define getc(x) getchar()

extern  char    *gets (char *buf);
/* NOTE: the last argument really should be a pointer to a FILE but we
    don't really have those in the restricted uCbootloader world */
extern  char    *fgets(char *s, int size, int fd);
extern  int     puts (unsigned char *string);
extern  int     putchar (unsigned int outch);

extern  int     vsprintf(char *buf, const char *fmt, va_list args);
extern  int     sprintf(char * buf, const char *fmt, ...);
extern  int     printf(const char *fmt, ...);
#define fprintf(fd,a...)    printf(a)

extern  int read(int fd, char *buf, int count);
extern  int write(int fd, char *buf, int count);
#define fread(p ,s, n, f)   read(0, p, (s) * (n))
#define fwrite(p, s, n, f)  write(1, p, (s) * (n))

#endif /* _STDIO_H */
