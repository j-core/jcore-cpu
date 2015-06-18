#include "recv_buf.h"

#include <string.h>
#include <inttypes.h>

void recv_buf_init(struct recv_buf *b) {
  b->head = b->tail = b->buf;
}

void recv_buf_get_empty_buf(struct recv_buf *b, char **buf, size_t *len) {
  if (b->tail == b->buf + RECV_BUF_LEN) {
    /* occupied region of buffer touches right edge. Try to copy it
       back to left to make room */
    if (b->head > b->buf) {
      memmove(b->buf, b->head, recv_buf_len(b));
      b->tail -= b->head - b->buf;
      b->head = b->buf;
    }
  }
  *buf = b->tail;
  *len = RECV_BUF_LEN - (b->tail - b->buf);
}

void recv_buf_added(struct recv_buf *b, size_t len) {
  b->tail += len;
}

int recv_buf_len(struct recv_buf *b) {
  return b->tail - b->head;
}

void recv_buf_take(struct recv_buf *b, size_t len) {
  b->head += len;
}
