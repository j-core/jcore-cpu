#ifndef DELAYS_H
#define DELAYS_H

#include <stdint.h>

enum delay_type {
  DELAY_MASK,
  DELAY_RANGE,
};

struct delay_match {
  uint32_t mask;
  uint32_t match;
};

struct delay_range {
  uint32_t start;
  uint32_t end;
};

struct delay {
  enum delay_type type;
  union {
    struct delay_match match;
    struct delay_range range;
  } info;
  uint32_t rd_delay;
  uint32_t rd_drop_delay;
  uint32_t wr_delay;
  uint32_t wr_drop_delay;

  struct delay *next;
};

struct delay_set {
  struct delay *delays;
};

int delays_init_cfg(struct delay_set *delays, const char *filename);
int delays_free(struct delay_set *delays);

int delays_lookup(struct delay_set *delays, uint32_t addr, int is_read,
                  uint32_t *delay, uint32_t *drop_delay);

#endif
