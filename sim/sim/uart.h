#ifndef UART_H
#define UART_H

#include "mem_bus.h"

#define LINE_BUF_SIZE 1024

struct uart {
  struct simulator *sim;
  struct mem_bus *bus;
  struct mem_range range;

  uint8_t ier;
  uint16_t divisor_latch;
  uint8_t lcr;
  uint8_t mcr;
  uint8_t lsr;
  uint8_t msr;
  uint8_t scr;

  int line_len;
  uint8_t line_buf[LINE_BUF_SIZE + 1];

  int prefix_len;
  char *prefix;
  int read_fd;
  int write_fd;

  int debug;
};

int uart_init(struct uart *uart, struct mem_bus *bus, uint32_t base_addr,
              int read_fd, int write_fd, char *prefix, int debug);
int uart_pty_init(struct uart *uart, struct mem_bus *bus, uint32_t base_addr,
                  char *name_buf, size_t buflen, int debug);
int uart_free(struct uart *uart);

#endif
