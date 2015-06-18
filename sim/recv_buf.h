#ifndef RECV_BUF_H
#define RECV_BUF_H

#include <sys/types.h>

/*
  A buffer to receive data into. Not a circular buffer, so data is
  kept contiguous and easy to parse, although that requires extra
  copying.
 */

#define RECV_BUF_LEN 1024

struct recv_buf {
  char *head;
  char *tail;

  char buf[RECV_BUF_LEN + 1];
};

void recv_buf_init(struct recv_buf *b);


/* Returns the empty region at the end of the buffer in the buf and
   len arguments. */
void recv_buf_get_empty_buf(struct recv_buf *b, char **buf, size_t *len);

/* Informs the buffer that len bytes were added to the empty region
   returned by previous call to recv_buf_get_empty_buf */
void recv_buf_added(struct recv_buf *b, size_t len);

/* returns number of bytes in the buffer which start at b->head */
int recv_buf_len(struct recv_buf *b);

/* removes bytes from the head of the buffer */
void recv_buf_take(struct recv_buf *b, size_t len);

#endif
