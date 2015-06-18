#include "simulator.h"

#include <stdlib.h>
#include <stdio.h>
#include <termios.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include <ctype.h>

#ifdef __APPLE__
#include <util.h>
#else
#include <pty.h>
#endif

#include "utlist.h"

//#define DEBUG

#define error(fmt, args...) fprintf(stderr, "Error:%s:%d: " fmt, __FILE__, __LINE__, ##args)

#ifdef DEBUG
#define log(args...) printf(args)
#else
#define log(...)
#endif

#define CMD_BUF_LEN 256

static void line_reader_init(struct line_reader *lr, int fd) {
  memset(lr, 0, sizeof(*lr));
  lr->fd = fd;
  lr->line_start = lr->buf;
}

static ssize_t line_reader_next(struct line_reader *lr, char **line) {
  char *i;
  char *end;
  ssize_t len;
  while (1) {
    /* extract any lines already in buffer */
    i = lr->line_start;
    end = lr->buf + lr->buf_used;
    while (i < end) {
      //printf("considering %ld: %hhd\n", i - lr->buf, *i);
      switch (*i) {
      case '\r':
      case '\n':
        *i = '\0';
        /*printf("i=%d ", i);*/
        //printf("new line at index %ld\n", (i - buf));
        *line = lr->line_start;
        lr->line_start = i + 1;
        if (*line < i) {
          //printf("READ LINE(%ld): %s\n", i - *line, *line);
          return i - *line;
        } else {
          //printf("READ EMPTY LINE\n");
        }
        break;
      }
      i++;
    }

    /* no complete lines in buffer */
    lr->buf_used = lr->buf_used - (lr->line_start - lr->buf);
    /* copy unused characters back to the start of the buffer */
    if (lr->buf_used) {
      //printf("memmove(%p, %p, %d)\n", buf, line_start, buf_used);
      memmove(lr->buf, lr->line_start, lr->buf_used);
    }
    lr->line_start = lr->buf;

    /* if entire line is full with no line breaks, just ignore it */
    if (sizeof(lr->buf) - lr->buf_used <= 0) {
      printf("dropping full input line\n");
      lr->buf_used = 0;
    }

    /* read more bytes */
    //printf("Attempting to read %ld bytes\n", sizeof(lr->buf) - lr->buf_used);
    len = read(lr->fd, lr->buf + lr->buf_used, sizeof(lr->buf) - lr->buf_used);
    if (len <= 0) {
      return len;
    }
    lr->buf_used += len;
  }
}

struct signal *sim_lookup_signal(struct simulator *sim, char *name) {
  int i;
  for (i = 0; i < sim->num_signals; i++) {
    if (strcmp(name, sim->signals[i].name) == 0) {
      return sim->signals + i;
    }
  }
  return NULL;
}

static int split_string(char *str, char **parts, int max_parts) {
  int n = 0;
  char* c = str;
  while (1) {
    /* advance to next part */
    while (isblank(*c)) {
      *c = 0;
      c++;
    }
    if (*c == 0) {
      return n;
    }

    parts[n] = c;
    if (n == max_parts)
      return n;
    n++;

    /* advance to next non-part */
    while (*c && !isblank(*c)) c++;
  }
}

enum cmd_type {
  CMD_ECHO,
  CMD_TIME,
  CMD_VALUE,
  CMD_READ,
};

char *cmd_names[] = {
  "echo",
  "time",
  "value",
  "read",
};

struct cmd_echo {
  char *arg;
};

struct cmd_time {
  picos_t picos;
};

struct cmd_value {
  struct signal *signal;
  struct cmd_time time;
  char *value;
};

struct cmd_read {
  struct signal *signal;
  char *value;
};

struct cmd {
  enum cmd_type type;
  union {
    struct cmd_echo echo;
    struct cmd_time time;
    struct cmd_value value;
    struct cmd_read read;
  } data;
};

static int parse_time(struct simulator *sim, char *num, char *unit, struct cmd_time *time) {
  int i;
  char *endptr;
  char c;
  time->picos = strtoull(num, &endptr, 10);
  /* support ns or ps units */
  if (unit) {
    if (strcmp(unit, "ns") == 0) {
      time->picos *= 1000;

      /* Support decimal values with ns */
      if (*endptr == '.') {
        endptr++;
        picos_t t = 0;
        for (i = 0; i < 3; i++) {
          t *= 10;
          c = *endptr;
          if (c) {
            endptr++;
            if ('0' <= c && c <= '9') {
              t += c - '0';
            } else {
              return -1;
            }
          }
        }
        time->picos += t;
      }
    } else if (strcmp(unit, "ps") != 0) {
      /* unknown units */
      return -1;
    }
  }
  time->picos *= sim->pico_scale; 
  if (*endptr) {
    return -1;
  }
  return 0;
}

#define MAX_PARTS 10
static int parse_command(struct simulator *sim, char *line, struct cmd *cmd) {
  char *parts[MAX_PARTS];
  int num_parts = split_string(line, parts, MAX_PARTS);
  if (num_parts > 0 && strlen(parts[0]) == 1) {
    switch (*parts[0]) {
    case '.':
      cmd->type = CMD_ECHO;
      if (num_parts > 1) {
        cmd->data.echo.arg = parts[1];
      } else {
        cmd->data.echo.arg = "";
      }
      return 0;
    case 'r':
      cmd->type = CMD_READ;
      if (num_parts == 3) {
        cmd->data.read.signal = sim_lookup_signal(sim, parts[1]);
        cmd->data.read.value = parts[2];
        if (cmd->data.read.signal) {
          return 0;
        }
      }
      break;
    case 't':
      cmd->type = CMD_TIME;
      switch (num_parts) {
      case 2:
        if (parse_time(sim, parts[1], 0, &cmd->data.time) == 0) {
          return 0;
        }
        break;
      case 3:
        if (parse_time(sim, parts[1], parts[2], &cmd->data.time) == 0) {
          return 0;
        }
        break;
      }
      break;
    case 'v':
      cmd->type = CMD_VALUE;
      if (num_parts == 5) {
        //printf("Value of %s = %s\n", parts[1], parts[3]);
        cmd->data.value.signal = sim_lookup_signal(sim, parts[1]);
        if (parse_time(sim, parts[2], parts[3], &cmd->data.value.time) == 0) {
          cmd->data.value.value = parts[4];
          if (cmd->data.value.signal) {
            return 0;
          }
        }
      }
      break;
    }
  }
  int i;
  printf(__FILE__ ":Error:%u:%" PICO "ps: Read invalid cmd: ", __LINE__, sim->picos);
  for (i = 0; i < num_parts; i++) {
    printf("%s ", parts[i]);
  }
  printf("\n");
  return -1;
}

static void signal_set_value(struct signal *s, char *val) {
  if (s->value == 0) {
    /* first time value seen for this signal. allocate a value
       buffer. */
    s->value_len = strlen(val);
    s->value = malloc(s->value_len + 1);
    s->value[s->value_len] = 0;
  }
  strncpy(s->value, val, s->value_len);

  /* convert all non 0/1 characters in value to uppercase */
  char *c = s->value;
  while (*c) {
    *c = toupper(*c);
    c++;
  }
}

#ifdef DEBUG
#define PRINT_CMD(args...) printf(args)
#else
#define PRINT_CMD(args...)
#endif

static void sim_handle_cmd(struct simulator *sim, struct cmd *cmd) {
  struct signal *signal;
  PRINT_CMD("CMD %s", cmd_names[cmd->type]);
  switch (cmd->type) {
  case CMD_ECHO:
    PRINT_CMD(" arg=%s", cmd->data.echo.arg);
    break;
  case CMD_TIME:
    PRINT_CMD(" picos=%" PICO, cmd->data.time.picos);
    sim->picos = cmd->data.time.picos;
    break;
  case CMD_VALUE:
    signal = cmd->data.value.signal;
    PRINT_CMD(" signal %s set to '%s' at %" PICO " ps", signal->name, cmd->data.value.value, cmd->data.value.time.picos);
    if (bv_get(&sim->events, signal->index)) {
      PRINT_CMD("Signal %s set multiple times during time %" PICO " = %s -> %s \n",
                signal->name, cmd->data.value.time.picos, signal->value, cmd->data.value.value);
    }
    signal_set_value(signal, cmd->data.value.value);
    sim->picos = cmd->data.value.time.picos;
    signal->event = 1;
    bv_set(&sim->events, signal->index, 1);
    sim->any_event = 1;
    break;
  case CMD_READ:
    signal = cmd->data.read.signal;
    PRINT_CMD(" %s read '%s'", signal->name, cmd->data.read.value);
    signal_set_value(signal, cmd->data.read.value);
    break;
  }
  PRINT_CMD("\n");
}

static void sim_clear_events(struct simulator *sim) {
  int i;
  for (i = 0; i < sim->num_signals; i++) {
    sim->signals[i].event = 0;
  }
  sim->any_event = 0;
  bv_clear(&sim->events);
}

/*static void sim_debug_hooks(struct simulator *sim) {
  if (!sim->do_debug) return;
  struct sig_hook *h;
  printf("Hooks start\n");
  DL_FOREACH(sim->hooks, h) {
    printf("  %p <- %p -> %p\n", h->prev, h, h->next);
  }
  printf("Hooks end\n");
}*/

static void sim_fire_events(struct simulator *sim) {
  struct sig_hook *hooks, *h, *t;
  picos_t orig_target;
  int is_target_time;
  hooks = sim->hooks;
  sim->hooks = 0;
  sim->calling_hooks = 1;
  DL_FOREACH_SAFE(hooks, h, t) {
    is_target_time = (h->time && h->time <= sim->picos);
    if (is_target_time || (sim->any_event && bv_and_reduce(&sim->events, &h->signals))) {
      orig_target = h->time;
      if (h->cb(sim, h)) {
        h->time = 0;
        DL_DELETE(hooks, h);
      } else {
        if (h->time <= sim->picos) {
          /* disable times in the past */
          h->time = 0;
        }
        if (h->time != orig_target) {
          /* reschedule if time changed */
          DL_DELETE(hooks, h);
          DL_APPEND(sim->hooks, h);
        }
      }
    }
  }
  sim->calling_hooks = 0;
  /* swap new_hooks and hooks */
  t = sim->hooks;
  sim->hooks = hooks;
  hooks = t;
  DL_FOREACH_SAFE(hooks, h, t) {
    sim_hook_add(sim, h);
  }
  DL_FOREACH(sim->immediate_hooks, h) {
    h->cb(sim, h);
  }
  sim->immediate_hooks = 0;
}

static int check_child_running(struct simulator *sim) {
  int status;
  pid_t pid;
  if (sim->child_running) {
    pid = waitpid(sim->pid, &status, WNOHANG);
    if (pid == -1) {
      perror("waitpid");
    } else if (pid == 0) {
      /* child still running */
    } else {
      /* child exited */
      sim->child_running = 0;
      if (WIFEXITED(status)) {
        sim->exit_status = WEXITSTATUS(status);
        if (sim->on_exit == SIMEXIT_EXIT || (sim->exit_status && sim->on_exit == SIMEXIT_ERROR)) {
          exit(sim->exit_status);
        }
      } else if (WIFSIGNALED(status)) {
        if (sim->on_exit == SIMEXIT_EXIT || sim->on_exit == SIMEXIT_ERROR) {
          exit(1);
        }
      }
    }
  }
  return sim->child_running;
}

static ssize_t sim_write_pty(struct simulator *sim, const void *buf, size_t count) {
  ssize_t r;
  /* Hack to catch when ghdl exits while C side is in sim_wait loop.
     Better way would be make pipe non blocking and detect full? Or
     treat closed pipe as dead ghdl? */
  static int num_calls = 0;
  if (num_calls++ > 10) {
      check_child_running(sim);
      num_calls = 0;
  }
  r = write(sim->pty, buf, count);
  if (sim->debug) {
    char printbuf[1024];
    memcpy(printbuf, buf, r);
    printbuf[r] = 0;
    printf("SEND CMD: %s", printbuf);
  }
  if (r == -1) {
    if (check_child_running(sim)) {
      error("write to child ghdl failed");
    }
  }
  return r;
}
static void sim_go_to_idle(struct simulator *sim) {
  char cmd_buf[CMD_BUF_LEN];
  ssize_t len;
  struct cmd cmd;
  static uint16_t echo_arg = 0;
  int i;
  if (sim->io_error) {
    return;
  }

  /* Repeatedly read and handle commands until the simulator is not
     sending any more. To determine when no more data is coming, send
     an echo command with an incrementing argument after each non-echo
     command. Only return once the last sent echo is received. */
  memset(cmd_buf, 0, sizeof(cmd_buf));
  i = snprintf(cmd_buf, sizeof(cmd_buf), ". %hu\n", echo_arg++);
  if (i > 0) {
    sim_write_pty(sim, cmd_buf, i);
    cmd_buf[i-1] = 0;
  }
  char *line;
  int en;
  while ((len = line_reader_next(&sim->lr, &line))) {
    if (len == -1) {
      en = errno; /* save errno around check_child_running call */
      sim->io_error = 1;
      check_child_running(sim);
      errno = en;
      perror("read");
      break;
    }

    if (sim->debug) {
      printf("READ LINE(%zd): %s\n", len, line);
    }
    if (parse_command(sim, line, &cmd) == 0) {
      if (cmd.type != CMD_ECHO) {
        i = snprintf(cmd_buf, sizeof(cmd_buf), ". %hu\n", echo_arg++);
        if (i > 0) {
          sim_write_pty(sim, cmd_buf, i);
          cmd_buf[i-1] = 0;
        }
        sim_handle_cmd(sim, &cmd);
      } else if (strcmp(cmd.data.echo.arg, cmd_buf + 2) == 0) {
        /* have reached idle state */
        return;
      }
    }
  }
}

static void sim_startup(struct simulator *sim) {
  int i;
  struct signal *signal;

#ifdef DEBUG
  log("SIGNALS:\n");
  for (i = 0; i < sim->num_signals; i++) {
    signal = sim->signals + i;
    log("- %s = %s ro? %d\n", signal->name, signal->type, signal->read_only);
  }
#endif

  /* issue reads for all signals */
  for (i = 0; i < sim->num_signals; i++) {
    signal = sim->signals + i;
    sim_write_pty(sim, "r ", 2);
    sim_write_pty(sim, signal->name, strlen(signal->name));
    sim_write_pty(sim, "\n", 1);

    /* read commands periodically to avoid filling pipe and
       deadlocking */
    if ((i + 1) % 10 == 0)
      sim_go_to_idle(sim);
  }
  sim_go_to_idle(sim);
  sim_clear_events(sim);
}

static int sig_wait_cb(struct simulator *sim, struct sig_hook *hook) {
  sim->sig_wait_occurred = 1;
  return 0;
}

static int _sim_hook_init(struct simulator *sim, struct sig_hook *h, hook_cb cb) {
  memset(h, 0, sizeof(*h));
  h->cb = cb;
  if (bv_init(&h->signals, sim->num_signals) != 0) {
    return -1;
  }
  return 0;
}

#define CMP_PREFIX(str, prefix, val) ((strncmp(str, prefix, strlen(prefix)) == 0) && (val = str + strlen(prefix)))

int sim_cfg_parse(struct sim_cfg *cfg, int argc, char **argv) {
  int i;
  char *arg;
  char *val;
  for (i = 0; i < argc; i++) {
    arg = argv[i];
    if (CMP_PREFIX(arg, "--ieee=", val)) {
      cfg->cfg.ghdl.ieee = val;
    } else if (CMP_PREFIX(arg, "--ieee-asserts=", val)) {
      cfg->cfg.ghdl.ieee_asserts = val;
    } else if (CMP_PREFIX(arg, "--work=", val)) {
      cfg->cfg.ghdl.work_name = val;
    } else if (CMP_PREFIX(arg, "--std=", val)) {
      cfg->cfg.ghdl.std = val;
    } else if (CMP_PREFIX(arg, "--vcd=", val)) {
      cfg->cfg.ghdl.vcd = val;
    } else if (CMP_PREFIX(arg, "--wave=", val)) {
      cfg->cfg.ghdl.wave = val;
    } else if (CMP_PREFIX(arg, "--stop-time=", val)) {
      cfg->stop_time = val;
    } else if (strcmp(arg, "-fexplicit") == 0) {
      cfg->cfg.ghdl.explicit = 1;
    } else if (CMP_PREFIX(arg, "--vpi-dir=", val)) {
      cfg->cfg.iverilog.vpi_dir = val;
    } else if (arg[0] == '-') {
      return -i - 1;
    } else {
      break;
    }
  }
  return i;
}

int sim_new(struct simulator *sim, struct sim_cfg *cfg) {
  pid_t pid;
  int master, slave;
  struct termios modes;
  memset(sim, 0, sizeof(*sim));
  sim->type = cfg->type;
  sim->name = cfg->name;
  sim->signals = cfg->signals;
  sim->num_signals = cfg->num_signals;
  sim->on_exit = cfg->on_exit;
  sim->debug = cfg->debug;
  if (cfg->pico_scale)
    sim->pico_scale = cfg->pico_scale;
  else
    sim->pico_scale = 1; // assume no scale
  if (_sim_hook_init(sim, &sim->sig_wait, sig_wait_cb)) {
    return -1;
  }
  if (bv_init(&sim->events, sim->num_signals) != 0) {
    return -1;
  }
  if (openpty(&master, &slave, NULL, NULL, NULL)) {
    perror("openpty");
    return -1;
  }
  pid = fork();
  if (pid == -1) {
    perror("fork");
    return -1;
  } else if (pid == 0) {
    close(master);
    if (dup2(slave, STDOUT_FILENO) == -1) {
      perror("dup2");
      return 1;
    }
    if (dup2(slave, STDIN_FILENO) == -1) {
      perror("dup2");
      return 1;
    }
    char cmd_buf[2048];
    char *cmd = cmd_buf;
    char *cmd_end = cmd_buf + sizeof(cmd_buf);

#define MAX_NUM_ARG 15

    char *argv[MAX_NUM_ARG];
    int argc = 0;

#define ADD_LONG_OPTION(name, val)                                           \
    if (val) {                                                          \
      if (argc >= MAX_NUM_ARG) goto cmd_error_num;                      \
      argv[argc++] = cmd;                                               \
      cmd += 1 + snprintf(cmd, cmd_end - cmd, "--%s=%s", name, val);    \
      if (cmd >= cmd_end) goto cmd_error_len;                           \
    }

#define ADD_OPTION(name, val)                                           \
    if (val) {                                                          \
      if (argc >= MAX_NUM_ARG) goto cmd_error_num;                      \
      argv[argc++] = cmd;                                               \
      cmd += 1 + snprintf(cmd, cmd_end - cmd, "-%s%s", name, val);      \
      if (cmd >= cmd_end) goto cmd_error_len;                           \
    }

    switch (cfg->type) {
    case SIM_TYPE_GHDL:

#ifdef GHDL_MCODE_BACKEND
      /* On OS X, run using ghdl -c -r to ensure it's up to date */
      argv[argc++] = "ghdl";
      argv[argc++] = "-c";
      /* On OS X, the ghdl -c takes additional options that appear to be
         baked into the elaborated binary in linux. */
      ADD_LONG_OPTION("ieee", cfg->cfg.ghdl.ieee);
      ADD_LONG_OPTION("work", cfg->cfg.ghdl.work_name);
      ADD_LONG_OPTION("std", cfg->cfg.ghdl.std);
      if (cfg->cfg.ghdl.explicit)
        argv[argc++] = "-fexplicit";
      argv[argc++] = "-r";
      argv[argc++] = sim->name;
#else
      /* On Linux, exec the sim binary directly. This avoids an
         additional fork-exec that ghdl -r does on linux which would
         make killing the simulator process trickier. */
      argv[argc++] = cmd;
      cmd += 1 + snprintf(cmd, cmd_end - cmd, "./%s", sim->name);
      if (cmd >= cmd_end) goto cmd_error_len;
      /* always want to disable at 0 */
      ADD_LONG_OPTION("ieee-asserts", "disable-at-0");
      ADD_LONG_OPTION("ieee-asserts", cfg->cfg.ghdl.ieee_asserts);
#endif
      ADD_LONG_OPTION("vcd", cfg->cfg.ghdl.vcd);
      ADD_LONG_OPTION("wave", cfg->cfg.ghdl.wave);
      ADD_LONG_OPTION("stop-time", cfg->stop_time);
      break;

    case SIM_TYPE_IVERILOG:
      argv[argc++] = "vvp";
      ADD_OPTION("M", cfg->cfg.iverilog.vpi_dir ? cfg->cfg.iverilog.vpi_dir : "sim");
      ADD_OPTION("m", cfg->cfg.iverilog.vpi_name ? cfg->cfg.iverilog.vpi_name : "vpibridge");
      argv[argc++] = sim->name;
      break;
    }

#undef ADD_LONG_OPTION
#undef ADD_OPTION

    /*error("Constructed command line: len %d\n", argc);
    int i;
    for (i = 0; i < argc; i++) {
      error("arg %d: %s\n", i, argv[i]);
      }*/

    argv[argc] = 0;
    execvp(argv[0], argv);
    perror("exec");
    exit(1);

  cmd_error_num:
    error("Error: Constructed ghdl command line too many args\n");
    exit(1);

  cmd_error_len:
    error("Error: Constructed ghdl command line too long\n");
    exit(1);
  } else {
    /* disable the echoing in the pty */
    if (tcgetattr(slave, &modes)) {
      perror("tcgetattr");
      return 1;
    }
    /* ICANON: canonical or noncanonical mode. Affects whether text is
       sent line by line */
    modes.c_lflag &= ~(ECHO | ECHONL);
    if (tcsetattr(slave, TCSANOW, &modes)) {
      perror("tcsetattr");
      return 1;
    }
    close(slave);
    fcntl(master, F_SETFD, FD_CLOEXEC);

    sim->pid = pid;
    sim->pty = master;

    sim->child_running = 1;
    line_reader_init(&sim->lr, sim->pty);
    sim_startup(sim);
    if (!sim->child_running) {
      return -1;
    }
  }
  return 0;
}

int sim_free(struct simulator *sim) {
  if (sim->pty == -1 || sim->pid == -1) {
    return -1;
  }
  close(sim->pty);
  log("calling waitpid\n");
  if (sim->child_running) {
    kill(sim->pid, SIGKILL);
    waitpid(sim->pid, NULL, 0);
    sim->child_running = 0;
  }
  sim->pty = -1;
  sim->pid = -1;
  return 0;
}

void sim_hook_init(struct simulator *sim, struct sig_hook *h, hook_cb cb) {
  _sim_hook_init(sim, h, cb);
}

/*
  Adds a hook to the simulators list of hooks. A hook may have a
  target time. Hooks with a target time are kept at the front of the
  list in increasing order of time to make it simple to find the next
  target time.
 */
void sim_hook_add(struct simulator *sim, struct sig_hook *h) {
  if (!sim->calling_hooks && h->time && sim->hooks) {
    /* Add hook to start of the list in order of target_time */
    struct sig_hook *i;

    if (sim->hooks->time == 0 || sim->hooks->time > h->time) {
      DL_PREPEND(sim->hooks, h);
    } else {
      DL_FOREACH(sim->hooks, i) {
        if (i->time == 0 || i->time > h->time) {
          h->next = i;
          h->prev = i->prev;
          i->prev->next = h;
          i->prev = h;
          return;
        }
      }
      DL_APPEND(sim->hooks, h);
    }
  } else {
    DL_APPEND(sim->hooks, h);
  }
}

void sim_hook_remove(struct simulator *sim, struct sig_hook *h) {
  DL_DELETE(sim->hooks, h);
}

void sim_immediate_hook_add(struct simulator *sim, struct sig_hook *h) {
  DL_APPEND(sim->immediate_hooks, h);
}

static picos_t next_hook_delay(struct simulator *s) {
  if (s->hooks && s->hooks->time && s->hooks->time > s->picos)
    return s->hooks->time - s->picos;
  return 0;
}

void sim_idle(struct simulator *sim) {
    sim_go_to_idle(sim);
    sim_fire_events(sim);
    sim_clear_events(sim);
}

void sim_wait(struct simulator *sim) {
  if (sim->calling_hooks) return;
  picos_t target = next_hook_delay(sim);
  if (target) {
    sim_wait_time(sim, target, 0);
  } else {
    sim_write_pty(sim, "wait\n", 5);
    sim_go_to_idle(sim);
    sim_fire_events(sim);
    sim_clear_events(sim);
  }
}

void sim_wait_time(struct simulator *sim, uint64_t picos, int force) {
  if (sim->calling_hooks) return;
  if (force) {
    picos_t target = sim->picos + picos;
    while (sim->picos < target) {
      sim_wait_time(sim, target - sim->picos, 0);
    }
    return;
  }
  char cmd[CMD_BUF_LEN];
  /* target time is either the given one or a hook target time */
  picos_t delay = next_hook_delay(sim);
  if (delay == 0 || delay > picos) {
    delay = picos;
  }

  // divide by scale rounding up
  delay = (delay + sim->pico_scale - 1) / sim->pico_scale;
  int len;
  if (sim->type == SIM_TYPE_IVERILOG) {
    /* don't send units to iverilog. times are in sim time */
    len = snprintf(cmd, sizeof(cmd), "wait %" PICO "\n", delay);
  } else {
    len = snprintf(cmd, sizeof(cmd), "wait %" PICO " ps\n", delay);
  }
  if (len >= sizeof(cmd)) {
    cmd[sizeof(cmd)-1] = '\n';
    len = sizeof(cmd);
  }
  log("%s", cmd);
  sim_write_pty(sim, cmd, len);

  sim_go_to_idle(sim);
  sim_fire_events(sim);
  sim_clear_events(sim);
  log("IDLE AT %" PICO "\n", sim->picos);
}

void sim_wait_clk_edge(struct simulator *sim, int rising) {
  if (sim->calling_hooks) return;
  sim_write_pty(sim, rising ? "clkwait r\n" : "clkwait f\n", 10);
  sim_go_to_idle(sim);
  sim_fire_events(sim);
  sim_clear_events(sim);
}

void sim_wait_signal(struct simulator *sim, int sig_index) {
  if (sim->calling_hooks) return;
  if (sig_index < 0 || sig_index > sim->num_signals) {
    return;
  }
  bv_set(&sim->sig_wait.signals, sig_index, 1);
  sim_hook_add(sim, &sim->sig_wait);
  while (sim->sig_wait_occurred == 0) {
    sim_wait(sim);
  }
  sim->sig_wait_occurred = 0;
  sim_hook_remove(sim, &sim->sig_wait);
  bv_set(&sim->sig_wait.signals, sig_index, 0);
}

int sim_set(struct simulator *sim, int sig_index, char *val) {
  if (sig_index < 0 || sig_index > sim->num_signals) {
    return -1;
  }
  struct signal *sig = sim->signals + sig_index;
  if (sig->read_only) {
    SIMLOG(sim, "Error: attempt to write read-only signal\n");
    return -1;
  }
  if (strlen(val) != sig->value_len) {
    return -1;
  }
  char cmd_buf[CMD_BUF_LEN];

  int len = snprintf(cmd_buf, sizeof(cmd_buf), "w %s %s\n", sig->name, val);
  if (len >= sizeof(cmd_buf)) {
    cmd_buf[sizeof(cmd_buf)-1] = '\n';
    len = sizeof(cmd_buf);
  }

  /* convert all non 0/1 characters in value to the case appropriate
     for the simulator. */
  char *c = cmd_buf + len;
  if (sim->type == SIM_TYPE_GHDL) {
    while (c >= cmd_buf && *c != ' ') {
      *c = toupper(*c);
      c--;
    }
  } else {
    while (c >= cmd_buf && *c != ' ') {
      /* verilog only has z and x, not all the other std_logic
         characters. Convert them. */
      switch (*c) {
      case 'H':
        *c = '1';
        break;
      case 'L':
        *c = '0';
        break;
      case 'U':
      case 'W':
        *c = 'x';
        break;
      default:
        *c = tolower(*c);
        break;
      }
      c--;
    }    
  }
  sim_write_pty(sim, cmd_buf, len);
  sim->dirty_writes = 1;
  return 0;
}

int sim_seti(struct simulator *sim, int sig_index, uint32_t val) {
  if (sig_index < 0 || sig_index > sim->num_signals) {
    return -1;
  }
  struct signal *sig = sim->signals + sig_index;
  if (sig->read_only) {
    SIMLOG(sim, "Error: attempt to write read-only signal\n");
    return -1;
  }
  int i;

  char cmd_buf[CMD_BUF_LEN];
  int len = snprintf(cmd_buf, sizeof(cmd_buf), "w %s ", sig->name);
  for (i = sig->value_len - 1; i >= 0; i--) {
    cmd_buf[len++] = (val & (1 << i)) ? '1' : '0';
  }
  cmd_buf[len++] = '\n';
  /*cmd_buf[len] = '\0';
  printf("set int %s", cmd_buf);*/
  sim_write_pty(sim, cmd_buf, len);
  sim->dirty_writes = 1;
  return 0;
}

char *sim_get(struct simulator *sim, int sig_index) {
  if (sig_index < 0 || sig_index > sim->num_signals) {
    return 0;
  }
  if (!sim->calling_hooks && sim->dirty_writes) {
    sim->dirty_writes = 0;
    sim_go_to_idle(sim);
    sim_fire_events(sim);
    sim_clear_events(sim);
  }
  return sim->signals[sig_index].value;
}

int sim_geti_check(struct simulator *sim, int sig_index, uint32_t *result) {
  if (sig_index < 0 || sig_index > sim->num_signals) {
    return -1;
  }
  if (!sim->calling_hooks && sim->dirty_writes) {
    sim->dirty_writes = 0;
    sim_go_to_idle(sim);
    sim_fire_events(sim);
    sim_clear_events(sim);
  }
  struct signal *sig = sim->signals + sig_index;
  char *val = sig->value;
  int i;
  uint32_t v = 0;
  for (i = 0; i < sig->value_len; i++) {
    v <<= 1;
    switch (val[i]) {
    case '0':
    case 'L':
      break;
    case '1':
    case 'H':
      v |= 1;
      break;
    default:
      return -1;
    }
  }
  *result = v;
  return 0;
}

uint32_t sim_geti(struct simulator *sim, int sig_index) {
  uint32_t v;
  if (sim_geti_check(sim, sig_index, &v))
    return 0;
  return v;
}

int sim_set_X(struct simulator *sim, int sig_index) {
  if (sig_index < 0 || sig_index > sim->num_signals) {
    return -1;
  }
  struct signal *sig = sim->signals + sig_index;
  return sim_set(sim, sig_index, "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
                 + 64 - sig->value_len);
}

int sim_set_all(struct simulator *sim, int sig_index, char c) {
  if (sig_index < 0 || sig_index > sim->num_signals) {
    return -1;
  }
  struct signal *sig = sim->signals + sig_index;
  char buf[sig->value_len + 1];
  memset(buf, c, sig->value_len);
  buf[sig->value_len] = 0;
  return sim_set(sim, sig_index, buf);
}

void sim_force_read(struct simulator *sim, int sig_index) {
  if (sig_index < 0 || sig_index > sim->num_signals) {
    return;
  }
  struct signal *sig = sim->signals + sig_index;
  char cmd_buf[CMD_BUF_LEN];
  int len = snprintf(cmd_buf, sizeof(cmd_buf), "r %s\n", sig->name);
  sim_write_pty(sim, cmd_buf, len);
  sim_go_to_idle(sim);
  sim_fire_events(sim);
  sim_clear_events(sim);
}
