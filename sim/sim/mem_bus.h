#ifndef MEM_BUS_H
#define MEM_BUS_H

#include <stddef.h>

#include "simulator.h"

#define container_of(ptr, type, field) \
  ((type*)((char*)(ptr) - offsetof(type, field)))

#ifdef __APPLE__
#define MAP_ANONYMOUS MAP_ANON
#endif

struct mem_range;
struct mem_bus;

typedef struct mem_range *(*find_range_fn)(struct mem_bus *bus, uint32_t addr);

typedef int (*mem_read_fn)(struct mem_bus *bus, struct mem_range *range,
                           uint32_t addr, int num_bytes, uint32_t *val,
                           uint32_t *ack_delay, uint32_t *drop_ack_delay);
typedef int (*mem_write_fn)(struct mem_bus *bus, struct mem_range *range,
                            uint32_t addr, int num_bytes, uint32_t val,
                            uint32_t *ack_delay, uint32_t *drop_ack_delay);

struct mem_range {
  uint32_t start;
  uint32_t end;
  mem_read_fn read_fn;
  mem_write_fn write_fn;

  struct mem_range *prev, *next;
};

struct mem_bus_signals {
  /* input */
  int en;
  int a;

  int din;
  int rd;
  int wr;
  int we;

  /* output */
  int dout;
  int ack;
};

struct mem_bus {
  char *name;
  struct simulator *sim;
  uint32_t read_val;
  uint32_t ack_delay;
  uint32_t ack_drop_delay;
  struct sig_hook rw_hook;
  struct sig_hook drop_ack_hook;
  struct sig_hook cancel_drop_hook;

  int ack_pending;
  int op_count;
  int ack_drop_op;
  struct mem_range *ranges;
  find_range_fn range_fn;

  struct {
    uint32_t ack_read;
    uint32_t ack_write;
    uint32_t drop_ack;
  } delays;

  struct mem_bus_signals sig;
};

void mem_bus_init(struct mem_bus *bus, struct simulator *sim);
void instr_fetch_init(struct mem_bus *bus, struct simulator *sim);

void mem_bus_range_add(struct mem_bus *bus, struct mem_range *range);
void mem_bus_range_remove(struct mem_bus *bus, struct mem_range *range);

#endif
