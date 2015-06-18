#include "bitvec.h"

#include <string.h>
#include <stdlib.h>

int bv_init(struct bv *bv, unsigned int n) {
  bv->n = n;
  bv->max_i = (n + 31) / 32;
  if (bv->max_i > BITVEC_SIZE) {
    return -1;
  }
  memset(bv->vals, 0, sizeof(bv->vals));
  return 0;
}

void bv_set(struct bv *bv, unsigned int i, int v) {
  if (i >= bv->n)
    return;
  if (v) {
    bv->vals[i / 32] |= (1 << (i % 32));
  } else {
    bv->vals[i / 32] &= ~(1 << (i % 32));
  }
}

int bv_get(struct bv *bv, unsigned int i) {
  if (i >= bv->n)
    return 0;
  return (bv->vals[i / 32] & (1 << (i % 32))) ? 1 : 0;
}

void bv_clear(struct bv *bv) {
  int i;
  for (i = 0; i < BITVEC_SIZE; i++) {
    bv->vals[i] = 0;
  }
}

int bv_and_reduce(struct bv *a, struct bv *b) {
  int i;
  int n = a->max_i < b->max_i ? a->max_i : b->max_i;
  for (i = 0; i < n; i++) {
    if (a->vals[i] & b->vals[i]) {
      return 1;
    }
  }
  return 0;
}

#include <stdio.h>
void bv_print(struct bv *bv) {
  int i, j;
  char buf[33];
  buf[32] = '\0';
  for (i = 0; i < bv->max_i; i++) {
    for (j = 0; j < 32; j++) {
      buf[i * 32 + j] = bv_get(bv, i * 32 + j) ? '1' : '0';
    }
    printf("%s", buf);
  }
}
