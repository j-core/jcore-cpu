/* Debug skt is a listening TCP socket and at most one accepted
   connection on that socket. The connection allows an external client
   to send debug requests to the simulation and receive the replies.
   This is meant to be used by gdbproxy to let gdb control the cpu
   simulation. */

#ifndef DEBUG_SKT_H
#define DEBUG_SKT_H

#include <inttypes.h>
#include "debug_plan.h"
#include "recv_buf.h"

struct debug_skt {
  int listener;
  int conn;
  struct recv_buf buf;
};

int debug_skt_init(struct debug_skt *skt, uint16_t *port);
void debug_skt_close(struct debug_skt *skt);

int debug_skt_handle(struct debug_skt *skt, struct debug_request *request, int *do_notify);
void debug_skt_reply(struct debug_skt *skt, struct debug_reply *reply);
void debug_skt_notify_paused(struct debug_skt *skt, uint32_t data);

#endif
