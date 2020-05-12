#ifndef _STRING_H_
#define _STRING_H_

#ifndef NULL
#define NULL ((void *) 0)
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern char * ___strtok;
extern char * strcpy(char *,const char *);
extern char * strncpy(char *,const char *,unsigned long);
extern char * strcat(char *, const char *);
extern char * strncat(char *, const char *, unsigned long);
extern char * strchr(const char *,int);
extern char * strpbrk(const char *,const char *);
extern char * strtok(char *,const char *);
extern char * strstr(const char *,const char *);
extern unsigned long strlen(const char *);
extern unsigned long strnlen(const char *,unsigned long);
extern unsigned long strspn(const char *,const char *);
extern int strcmp(const char *,const char *);
extern int strncmp(const char *,const char *,unsigned long);
extern unsigned long strtoul(const char *cp,char **endp,unsigned int base);

extern signed long strtol(const char *cp,char **endp,unsigned int base);
extern int intodec(char * dest,signed int arg,unsigned short places,unsigned int base);

extern void * memset(void *,char,unsigned long);
extern void * memcpy(void *,const void *,unsigned long);
extern void * memmove(void *,const void *,unsigned long);
extern void * memscan(void *,int,unsigned long);
extern int memcmp(const void *,const void *,unsigned long);

/*
 * Include machine specific inline routines
 */

#ifdef __cplusplus
}
#endif

#endif /* _STRING_H_ */
