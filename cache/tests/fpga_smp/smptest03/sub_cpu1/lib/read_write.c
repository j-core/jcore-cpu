#include "syscalls.h"
#include "errno.h"

int errno;

#if 0

int _trap34 (int sysno,int arg1,int arg2,int arg3);


int
open ( char *name, int flag, int mode)
{
 return _trap34 (__NR_open, (int)name, flag, mode);
}

int
close ( int fd )
{
 return _trap34 (__NR_close, fd, 0, 0);
}

int
read ( int fd, void *ptr, int len)
{
  return _trap34 (__NR_read, fd, (int)ptr, len);
}

int
write ( int fd, void *ptr, int len)
{
  return _trap34 (__NR_write, fd, (int)ptr, len);
}

int
lseek ( int fd, long offset, int flag)
{
  return _trap34 (__NR_lseek, fd, offset, flag);
}

int
inch ()
{
  char ret;

  read(0, &ret, 1);
  return ret;
}

int
outch(char ch)
{
  write(1, &ch, 1);
}


#else

/* File IO uses uarts directly */
int
open ( char *name, int flag, int mode)
{
  errno = ENOENT;
  return -1;
}

int
close ( int fd )
{
  errno = ENOENT;
  return -1;
}

int
read ( int fd, void *ptr, int len)
{
  if (fd == 0) {
    /* stdin */
    errno = EINVAL;
    return -1;
  } else {
    errno = EINVAL;
    return -1;
  }
}

struct st_uartlite
{
  unsigned int rxfifo;
  unsigned int txfifo;
  unsigned int status;
  unsigned int control;
};

#define sys_UART0_BASE 0xabcd0100
#define sys_UART1_BASE 0xabcd0300

static const struct st_uartlite* devices[] = {
  (struct st_uartlite *) sys_UART0_BASE,
  (struct st_uartlite *) sys_UART1_BASE
};

#define UARTLITE(n) (*(volatile struct st_uartlite *) devices[n])
#define UART_NUM 0

void
uart_tx (int dev, unsigned char data)
{
  while (UARTLITE(dev).status & 0x08);
  UARTLITE(dev).txfifo = data;
}

int
write ( int fd, void *ptr, int len)
{
  int i;
  unsigned char c;
  if (fd == 1)
    {
      /* stdout */
      for (i = 0; i < len; i++)
        {
          c = ((unsigned char*)ptr)[i];
          if (c == '\n')
            uart_tx(UART_NUM, '\r');
          uart_tx(UART_NUM, c);
        }
      return i;
    }
  else
    {
      errno = EINVAL;
      return -1;
    }
}

int
lseek ( int fd, long offset, int flag)
{
  errno = ENOENT;
  return -1;
}

char
inch ()
{
  char ret = 0;
  read(0, &ret, 1);
  return ret;
}

int
outch(char ch)
{
  return write(1, &ch, 1);
}

#endif
