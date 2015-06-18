#include "debug_skt.h"

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stddef.h>
#include <errno.h>
#include <fcntl.h>
#include <arpa/inet.h>

static uint16_t skt_port(int sockfd) {
  struct sockaddr_storage ss;
  socklen_t addr_len = sizeof(ss);
  if (getsockname(sockfd, (struct sockaddr*)&ss, &addr_len) == 0) {
    switch (ss.ss_family) {
    case AF_INET:
      return ntohs(((struct sockaddr_in*)&ss)->sin_port);
    case AF_INET6:
      return ntohs(((struct sockaddr_in6*)&ss)->sin6_port);
    default:
      return 0;
    }
  } else {
    return 0;
  }
}

static int set_nonblock(int fd) {
  int error = 0;
  int flags = fcntl(fd, F_GETFL);
  if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
    error = -errno;
  }
  return error;
}

static int set_block(int fd) {
  int error = 0;
  int flags = fcntl(fd, F_GETFL);
  if (fcntl(fd, F_SETFL, flags & ~O_NONBLOCK) == -1) {
    error = -errno;
  }
  return error;
}

int debug_skt_init(struct debug_skt *skt, uint16_t *port) {
  int error = 0;
  int true_opt = 1;
  recv_buf_init(&skt->buf);
  skt->conn = -1;
  skt->listener = socket(AF_INET, SOCK_STREAM, 0);
  if (skt->listener == -1) {
    error = -errno;
    perror("socket");
    return -errno;
  }

  /* set SO_REUSEADDR for TCP sockets to avoid TIME_WAIT state delays */
  if (setsockopt(skt->listener, SOL_SOCKET, SO_REUSEADDR,
                 &true_opt, sizeof(true_opt)) == -1) {
    perror("setsockopt SO_REUSEADDR");
  }
  if ((error = set_nonblock(skt->listener))) {
    perror("fcntl nonblock");
    goto err;
  }

  struct sockaddr_in in4;
  memset(&in4, 0, sizeof(in4));
  in4.sin_family = AF_INET;
  in4.sin_addr.s_addr = INADDR_ANY;
  in4.sin_port = htons(*port);

  if ((error = bind(skt->listener, (struct sockaddr*)&in4, sizeof(in4))) != 0) {
    error = -errno;
    perror("bind");
    goto err;
  }
  *port = skt_port(skt->listener);
  if ((error = listen(skt->listener, 5))) {
    error = -errno;
    perror("listen");
    goto err;
  }
  return 0;
 err:
  debug_skt_close(skt);
  return error;
}

void debug_skt_close(struct debug_skt *skt) {
  if (skt->listener != -1) {
    close(skt->listener);
    skt->listener = -1;
  }
  if (skt->conn != -1) {
    close(skt->conn);
    skt->conn = -1;
  }
}


int debug_skt_handle(struct debug_skt *skt, struct debug_request *request, int *do_notify) {
  /* This is ugly. We're polling the sockets to either accept an
     incoming connection or to receive a command message. The
     alternative would be to refactor the simulator and check all
     file descriptors in a poll or epoll, but that's too much work
     right now */
  int result = -1;
  char *buf;
  size_t len;
  ssize_t r;
  *do_notify = 0;
  if (skt->conn == -1) {
    /* no connection, try to accept one */
    skt->conn = accept(skt->listener, NULL, NULL);
    if (skt->conn != -1) {
      if (set_nonblock(skt->conn)) {
        perror("fcntl nonblock");
        close(skt->conn);
        skt->conn = -1;
      }
      printf("Accepted debug connection\n");
    }
  } else {
    /* see if connection has data to receive */
    recv_buf_get_empty_buf(&skt->buf, &buf, &len);
    if (len > 0) {
      r = recv(skt->conn, buf, len, 0);
      if (r == -1 && errno != EAGAIN) {
        printf("Debug skt recv failed. Closing.\n");
        close(skt->conn);
        skt->conn = -1;
      } else if (r == 0) {
        printf("Debug skt closed.\n");
        close(skt->conn);
        skt->conn = -1;
      } else if (r > 0) {
        recv_buf_added(&skt->buf, r);
        skt->buf.tail[0] = '\0';
        //printf("Received \"%s\"\n", buf);

        /* Parse debug cmd */
        if (recv_buf_len(&skt->buf) >= 8) {
          //printf("Received msg = %hhd\n", skt->buf.head[0]);

          switch (skt->buf.head[0]) {
          case DBG_CMD_BREAK:
          case DBG_CMD_STEP:
          case DBG_CMD_INSERT:
          case DBG_CMD_CONTINUE:
            request->cmd = skt->buf.head[0];
            request->data_en = skt->buf.head[1];
            memcpy(&request->instr, skt->buf.head + 2, 2);
            memcpy(&request->data, skt->buf.head + 4, 4);
            request->instr = ntohs(request->instr);
            request->data = ntohl(request->data);

            result = 0; /* tell caller that the request is valid */
            break;
          case 4:
            /* special case to request a notifcation when in debug mode */
            *do_notify = 1;
            break;
          }
          recv_buf_take(&skt->buf, 8);
        }
      }
    }
  }
  return result;
}

static int send_all(int fd, void *buf, size_t len) {
  ssize_t r;
  /* send entire buffer so allow send to block */
  set_block(fd);
  while (len > 0) {
    r = send(fd, buf, len, 0);
    if (r == -1) {
      perror("send");
      return -errno;
    }
    buf += r;
    len -= r;
  }
  set_nonblock(fd);
  return 0;
}

/*
  Messages sent to the debug socket are 5 bytes long. The first byte
  is the type, bytes 1-4 are a uint32_t in network byte order.
*/
#define MSG_REPLY 0
#define MSG_NOTIFY 1

void debug_skt_notify_paused(struct debug_skt *skt, uint32_t data) {
  char msg[5];
  data = htonl(data);
  msg[0] = MSG_NOTIFY;
  memcpy(msg + 1, &data, 4);
  //printf("sending notify\n");
  if (send_all(skt->conn, &msg, sizeof(msg))) {
    printf("Debug skt notify send failed. Closing.\n");
    close(skt->conn);
    skt->conn = -1;
  }
}

void debug_skt_reply(struct debug_skt *skt, struct debug_reply *reply) {
  char msg[5];
  uint32_t data = htonl(reply->data);
  msg[0] = MSG_REPLY;
  memcpy(msg + 1, &data, 4);
  if (send_all(skt->conn, &msg, sizeof(msg))) {
    printf("Debug skt reply send failed. Closing.\n");
    close(skt->conn);
    skt->conn = -1;
  }
}
