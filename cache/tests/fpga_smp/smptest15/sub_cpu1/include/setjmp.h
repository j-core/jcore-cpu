#ifndef _SETJMP_H_
#define _SETJMP_H_

typedef struct __jmpbuf {
	unsigned int __j0;	/* 0  */
	unsigned int __j1;
	unsigned int sp;	/* 8  */
	unsigned int pc;	/* 12 */
	unsigned int __j3;
	unsigned int d2;	/* 20 */
        unsigned int d3;
        unsigned int d4;
        unsigned int d5;
        unsigned int d6;
        unsigned int d7;
        unsigned int a2;
        unsigned int a3;
        unsigned int a4;
        unsigned int a5;
        unsigned int fp;
} __jmp_buf[1];

typedef __jmp_buf jmp_buf;

int setjmp(jmp_buf jp);
int longjmp(jmp_buf jp, int ret);

#endif /* _332_SETJMP_H_ */
