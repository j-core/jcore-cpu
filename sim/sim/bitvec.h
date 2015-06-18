#ifndef BITVEC_H
#define BITVEC_H

#include <stdint.h>

/* BITVEC_MAX_BITS controls the maximum number of bits in a bit
   vector.  */
#ifndef BITVEC_SIZE
#define BITVEC_SIZE 4
#endif

struct bv {
  unsigned int n;
  int max_i;
  uint32_t vals[BITVEC_SIZE];
};

int bv_init(struct bv *bv, unsigned int n);

void bv_set(struct bv *bv, unsigned int i, int v);
int bv_get(struct bv *bv, unsigned int i);
void bv_clear(struct bv *bv);

int bv_and_reduce(struct bv *a, struct bv *b);
void bv_print(struct bv *bv);

#endif
