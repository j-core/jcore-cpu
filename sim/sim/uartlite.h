#ifndef UARTLITE_H
#define UARTLITE_H

#include "mem_bus.h"

#define LINE_BUF_SIZE 1024

struct uartlite {
  struct simulator *sim;
  struct mem_bus *bus;
  struct mem_range range;

  int line_len;
  uint8_t line_buf[LINE_BUF_SIZE + 1];

  int prefix_len;
  char *prefix;
  int read_fd;
  int write_fd;

  int debug;
};

int uartlite_init(struct uartlite *uart, struct mem_bus *bus, uint32_t base_addr,
              int read_fd, int write_fd, char *prefix, int debug);
int uartlite_pty_init(struct uartlite *uart, struct mem_bus *bus, uint32_t base_addr,
                  char *name_buf, size_t buflen, int debug);
int uartlite_free(struct uartlite *uart);

#endif
