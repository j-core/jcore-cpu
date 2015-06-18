#define _XOPEN_SOURCE 600

#include "uart.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#define REGS \
  REG(RTX) REG(IER) REG(IIR) REG(LCR) REG(MCR) REG(LSR) REG(MSR) REG(SCR)
static const char *reg_names_array[] = {
#define REG(name) #name,
  "UNKNOWN",
  REGS
#undef REG
};

static const char **reg_names = reg_names_array + 1;

enum reg_id {
  UNKNOWN = -1,
#define REG(name) name,
  REGS
#undef REG
  NUM_REGS,
};
#undef REGS

static enum reg_id get_reg_id(struct uart *uart, uint32_t addr) {
  if (addr < uart->range.start || addr % 4 != 0 || addr >= uart->range.end)
    return UNKNOWN;
  enum reg_id id = (addr - uart->range.start) / 4;
  if (id >= NUM_REGS)
    return UNKNOWN;
  return id;
}

static int is_dlab(struct uart *uart) {
  return !!(uart->lcr & 0x80);
}

static int last_op = 0;

static int uart_read(struct mem_bus *bus, struct mem_range *range, uint32_t addr,
                     int num_bytes, uint32_t *val,
                     uint32_t *ack_delay, uint32_t *drop_ack_delay) {
  struct uart *uart = container_of(range, struct uart, range);
  enum reg_id id = get_reg_id(uart, addr);
  uint8_t byte_val;
  ssize_t len;
  char print_buf[2];
  *val = 0;
  /*printf("UART: READ from %s (0x%X) num_bytes: %d at %lu ns\n",
    reg_names[id], addr, num_bytes, bus->sim->nanos);*/

  switch (id) {
  case RTX:
    if (is_dlab(uart)) {
      *val = uart->divisor_latch & 0xFF;
    } else {
      if (uart->debug) {
        if (last_op != 1) {
          last_op = 1;
          printf("\n> ");
        }
      }
      do {
        len = read(uart->read_fd, &byte_val, 1);
      } while (len != 1);
      *val = byte_val;
      if (uart->debug) {
        if (isprint(byte_val)/* || byte_val == '\n'*/) {
          print_buf[0] = (char)byte_val;
          print_buf[1] = 0;
          printf("%s", print_buf);
        } else {
          printf("?");
        }
      }
    }
    break;
  case IER:
    if (is_dlab(uart))
      *val = uart->divisor_latch >> 8;
    else
      *val = uart->ier;
    break;
  case LCR:
    *val = uart->lcr;
    break;
  case MCR:
    *val = uart->mcr;
    break;
  case LSR:
    /**val = uart->lsr;*/
    /* always ready to transmit or have bytes to receive */
    *val = 0x21;
    break;
  case MSR:
    *val = uart->msr;
    break;
  case SCR:
    *val = uart->scr;
    break;

  default:
    printf("UART: Unhandled READ from %s (0x%X) num_bytes: %d at %" PICO " ps\n",
           reg_names[id], addr, num_bytes, bus->sim->picos);
    return -1;
  }
  return 0;
}

static int uart_write(struct mem_bus *bus, struct mem_range *range, uint32_t addr,
                      int num_bytes, uint32_t val,
                      uint32_t *ack_delay, uint32_t *drop_ack_delay) {
  struct uart *uart = container_of(range, struct uart, range);
  enum reg_id id = get_reg_id(uart, addr);
  uint8_t byte_val;
  char print_buf[2];
  /*printf("UART: WRITE 0x%X to %s (0x%X) num_bytes: %d at %lu ns\n",
    val, reg_names[id], addr, num_bytes, bus->sim->nanos);*/
  val &= 0xFF; /* ignore high bits */
  switch (id) {
  case RTX:
    if (is_dlab(uart))
      uart->divisor_latch = (uart->divisor_latch & 0xFF00) | val;
    else {
      if (uart->debug) {
        if (last_op != 2) {
          last_op = 2;
          printf("\n< ");
        }
      }
      byte_val = (uint32_t) val;
      write(uart->write_fd, &byte_val, 1);

      if (uart->debug) {
        if (isprint(byte_val)/* || byte_val == '\n'*/) {
          print_buf[0] = (char)byte_val;
          print_buf[1] = 0;
          printf("%s", print_buf);
        } else {
          printf("?");
        }
      }
      /*
      if (uart->line_len == LINE_BUF_SIZE) {
        if (uart->write_fd != -1) {
          if (uart->prefix)
            write(uart->write_fd, uart->prefix, uart->prefix_len);
          write(uart->write_fd, uart->line_buf, uart->line_len);
          write(uart->write_fd, "\n", 1);
        }
        //printf("UART: %s\n", uart->line_buf, );
        uart->line_len = 0;
      }
      uart->line_buf[uart->line_len++] = val;
      if (val == '\n') {
        uart->line_buf[uart->line_len] = 0;
        if (uart->write_fd != -1) {
          if (uart->prefix)
            write(uart->write_fd, uart->prefix, uart->prefix_len);
          write(uart->write_fd, uart->line_buf, uart->line_len);
        }
        //printf("UART: %s", uart->line_buf);
        uart->line_len = 0;
      }
      */
      //printf("Write 0x%hhX\n", (uint8_t) val);
    }
    break;
  case IER:
    if (is_dlab(uart))
      uart->divisor_latch = (val << 8) | (uart->divisor_latch & 0xFF);
    else
      uart->ier = val & 0xF; /* clear reserved bits */
    break;
  case LCR:
    uart->lcr = val;
    break;
  case MCR:
    uart->mcr = val & 0x1F; /* clear reserved bits */
    break;
  case SCR:
    uart->scr = val;
    break;

  default:
    printf("UART: Unhandled WRITE 0x%X to %s (0x%X) num_bytes: %d at %" PICO " ps\n",
           val, reg_names[id], addr, num_bytes, bus->sim->picos);
    return -1;
  }
  return 0;
}
/*
static int open_pty(char *name) {
  int i, j, fd;
  memcpy(name, "/dev/pty", 8);
  name[10] = 0;
  // /dev/pty[p-za-e][0-9a-f] (BSD master devices)
  // /dev/tty[p-za-e][0-9a-f] (BSD slave devices)
  for (i = 0; i < ('z' - 'p' + 1) + ('e' - 'a' + 1); i++) {
    if (i < ('z' - 'p' + 1))
      name[8] = 'p' + i;
    else
      name[8] = 'a' + i - ('z' - 'p' + 1);
    for (j = 0; j < 16; j++) {
      if (j < 10)
        name[9] = '0' + j;
      else
        name[9] = 'a' + j - 10;
      printf("Attempt: %s\n", name);
      fd = open(name, O_RDWR);
      if (fd == -1) {
        perror("open");
      } else {
        name[5] = 't';
        return fd;
      }
    }
  }
  return -1;
}*/

int uart_init(struct uart *uart, struct mem_bus *bus, uint32_t base_addr,
              int read_fd, int write_fd, char *prefix, int debug) {
  memset(uart, 0, sizeof(*uart));
  uart->bus = bus;
  uart->sim = bus->sim;

  uart->range.start = base_addr;
  uart->range.end = base_addr + 4 * NUM_REGS;
  uart->range.read_fn = uart_read;
  uart->range.write_fn = uart_write;

  mem_bus_range_add(bus, &uart->range);
  uart->prefix = prefix;
  uart->prefix_len = prefix ? strlen(prefix) : 0;
  uart->read_fd = read_fd;
  uart->write_fd = write_fd;
  uart->debug = debug;
  return 0;

}

int uart_pty_init(struct uart *uart, struct mem_bus *bus, uint32_t base_addr,
                  char *name_buf, size_t buflen, int debug) {
  char *name;
  struct termios modes;
  int fd;
  fd = open("/dev/ptmx", O_RDWR | O_NOCTTY);
  if (fd == -1) {
    perror("posix_openpt");
    return -1;
  }
  if (grantpt(fd)) {
    perror("grantpt");
    goto close_fd;
  }
  if (unlockpt(fd)) {
    perror("unlockpt");
    goto close_fd;
  }
  /* disable the echoing in the pty */
  if (tcgetattr(fd, &modes)) {
    perror("tcgetattr");
    goto close_fd;
  }
  modes.c_lflag &= ~(ECHO | ECHONL);
  if (tcsetattr(fd, TCSANOW, &modes)) {
    perror("tcsetattr");
    goto close_fd;
  }

  name = ptsname(fd);
  strncpy(name_buf, name, buflen);
  if (buflen > 0)
    name_buf[buflen - 1] = '\0';

  if (uart_init(uart, bus, base_addr, fd, fd, 0, debug))
    goto close_fd;
  return 0;
 close_fd:
  close(fd);
  return -1;
}

int uart_free(struct uart *uart) {
  mem_bus_range_remove(uart->bus, &uart->range);
  return 0;
}
