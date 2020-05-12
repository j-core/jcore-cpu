OBJS =	vsprintf.o setjmp.o conio.o libsyscall.o read_write.o\
	strtoul.o strtol.o strcpy.o strncpy.o strcat.o strncat.o \
	strcmp.o strncmp.o strchr.o strlen.o strnlen.o strspn.o \
	strpbrk.o strtok.o memset.o bcopy.o memcpy.o memmove.o \
	memcmp.o memscan.o strstr.o ctype.o bzero.o

ARCH = sh2-elf
CC = $(ARCH)-gcc
AR = $(ARCH)-ar
RANLIB = $(ARCH)-ranlib

CFLAGS = -fno-builtin -I../include -O2

all: libc.a

libc.a: $(OBJS)
	$(AR) -cur libc.a $(OBJS)
	$(RANLIB) libc.a

.S.o:
	$(CC) $(CFLAGS) -c $<

clean:
	@rm -f *.[oa] contab.* *.bdmmb *.map *.log *.s19 core *~
