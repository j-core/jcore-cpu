/*
  Uses Verilog's VPI to read simulator commands from standard input
  and respond on standard output. The simulator commands are sent by
  simulator.c and are the same ones sent that the VHDL stubs handle
  when working with GHDL.
 */

#include <vpi_user.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <inttypes.h>

#define HASH_DEBUG
#include "uthash.h"

/* Forms a hash table of monitored signals. Whenenver a command is
   received to read or write a signal for the first time, a
   corresponding signal struct is created and a vpi callback is
   registered for when the signal's value changes. */
struct signal {
  char *name;
  vpiHandle hnd;
  UT_hash_handle hh;
};

struct signal *signals = NULL;

/* Holds information about the current echo request */
struct echo_data {
  int dirty;
  int pending;
  int arg;
};

struct echo_data echo_data = {0, 0, 0};

vpiHandle active_wait = NULL;

int read_cmds_on_val_change = 1;
int is_waiting = 0;

void read_cmds();

uint64_t time2num(s_vpi_time *time) {
  uint64_t t = time->high;
  t = (t << 32) | time->low;
  return t;
}

uint64_t get_sim_time() {
  s_vpi_time time;
  time.type = vpiSimTime;
  vpi_get_time(NULL, &time);
  return time2num(&time);
}

void print_time() {
  vpi_printf("t %" PRIu64 "\n", get_sim_time());
}

void check_wait() {
  if (is_waiting) {
    print_time();
    is_waiting = 0;
  }
}

PLI_INT32 wait_cb(struct t_cb_data *cb) {
  //vpi_printf("wait_cb called\n");
  check_wait();
  if (active_wait) {
    vpi_free_object(active_wait);
    active_wait = 0;
  }
  read_cmds();
  return 0;
}

void wait_sim_time(uint64_t delta) {
  s_vpi_time time;
  time.type = vpiSimTime;
  delta += get_sim_time();
  time.low = (PLI_UINT32) delta;
  time.high = (PLI_UINT32) (delta >> 32);

  struct t_cb_data cb;
  memset(&cb, 0, sizeof(cb));
  cb.reason = cbReadWriteSynch;
  cb.cb_rtn = wait_cb;
  cb.time = &time;
  active_wait = vpi_register_cb(&cb);
}

void schedule_echo(int arg);

PLI_INT32 echo_cb(struct t_cb_data *cb) {
  check_wait();
  echo_data.pending = 0;
  if (echo_data.dirty) {
    schedule_echo(echo_data.arg);
  } else {
    vpi_printf(". %d\n", echo_data.arg);
    read_cmds();
  }
  return 0;
}

void schedule_echo(int arg) {
  vpiHandle hnd;
  if (!echo_data.pending) {
    echo_data.pending = 1;
    echo_data.dirty = 0;
    /* register a read/write synch cb in current sim time */
    s_vpi_time time;
    time.type = vpiSimTime;
    vpi_get_time(NULL, &time);
    struct t_cb_data cb;
    memset(&cb, 0, sizeof(cb));
    cb.reason = cbReadWriteSynch;
    cb.cb_rtn = echo_cb;
    cb.time = &time;
    hnd = vpi_register_cb(&cb);
    vpi_free_object(hnd);
  }
  echo_data.arg = arg;
}

static int split_line(char *line, char **parts) {
  int num = 0;
  char *l = line;
  while (*l) {
    if (isspace(*l)) {
      l++;
    } else if (num == 6) {
      return -1;
    } else {
      parts[num++] = l;
      while (*l && !isspace(*l))
        l++;
      if (*l)
        *(l++) = '\0';
      else
        break;
    }
  }
  return num;
}

PLI_INT32 val_change_cb(struct t_cb_data* cb) {
  is_waiting = 0;
  //check_wait(); // don't need to output time, it is in value change cmd
  //printf("val_change_cb\n");
  struct signal *s = (struct signal *) cb->user_data;
  if (cb->value->format == vpiBinStrVal) {
    vpi_printf("v %s %" PRIu64 " ps %s\n",
               s->name, time2num(cb->time), cb->value->value.str);
  }
  echo_data.dirty = 1;
  if (read_cmds_on_val_change)
    read_cmds();
  return 0;
}

struct signal *get_signal(char *name) {
  struct signal *s;
  int len = strlen(name);
  HASH_FIND(hh, signals, name, len, s);
  if (s == 0) {
    vpiHandle hnd = vpi_handle_by_name(name, 0);
    if (hnd == 0) {
      fprintf(stderr, "unknown signal \"%s\"\n", name);
      return 0;
    }
    //printf("creating signal \"%s\" of len %d\n", name, len);
    s = (struct signal*) malloc(sizeof(struct signal) + len + 1);
    if (s == 0) {
      return 0;
    }
    s->name = ((char*)s) + sizeof(struct signal);
    s->hnd = hnd;
    memcpy(s->name, name, len + 1);

    s_vpi_time time;
    time.type = vpiSimTime;

    s_vpi_value value;
    value.format = vpiBinStrVal;

    struct t_cb_data cb;
    memset(&cb, 0, sizeof(cb));
    cb.reason = cbValueChange;
    cb.obj = hnd;
    cb.cb_rtn = val_change_cb;
    cb.value = &value;
    cb.time = &time;
    cb.user_data = (char*)s;
    hnd = vpi_register_cb(&cb);
    vpi_free_object(hnd);

    HASH_ADD_KEYPTR(hh, signals, s->name, len, s);
  }
  return s;
}

void read_signal(char *name) {
  s_vpi_value value;
  struct signal *s = get_signal(name);
  //vpi_printf("READ %s %p\n", name, s);
  if (s) {
    value.format = vpiBinStrVal;
    vpi_get_value(s->hnd, &value);
    if (value.format >= vpiBinStrVal && value.format <= vpiHexStrVal) {
      printf("r %s %s\n", name, value.value.str);
    }
  }
}

void write_signal(char *name, char *val) {
  struct signal *s = get_signal(name);
  s_vpi_value value;
  //vpi_printf("WRITE %s=%s %p\n", name, val, s);
  if (s) {
    value.format = vpiBinStrVal;
    value.value.str = val;
    read_cmds_on_val_change = 0;
    vpi_put_value(s->hnd, &value, NULL, vpiNoDelay);
    read_cmds_on_val_change = 1;
  }
}

void read_cmds() {
  char buf[1024];
  int num_parts;
  char *parts[10];
  char *s;
  uint64_t t;
  int echo_arg;
  if (active_wait) {
    vpi_remove_cb(active_wait);
    active_wait = 0;
  }
  while(fgets(buf, sizeof(buf), stdin)) {
    num_parts = split_line(buf, parts);
    if (num_parts <= 0) {
      continue;
    }
    //vpi_printf("Got %d parts\n", num_parts);
    if (strcmp(parts[0], ".") == 0) {
      if (num_parts == 2) {
        echo_arg = strtoul(parts[1], &s, 10);
        if (*s) {
          fprintf(stderr, "echo: invalid arg \"%s\"\n", parts[1]);
        } else {
          schedule_echo(echo_arg);
          return;
        }
      } else {
        fprintf(stderr, "echo: invalid args\n");
      }
    } else if (strcmp(parts[0], "t") == 0) {
      print_time();
    } else if (strcmp(parts[0], "r") == 0) {
      if (num_parts == 2) {
        read_signal(parts[1]);
      } else {
        fprintf(stderr, "read: invalid args\n");
      }
    } else if (strcmp(parts[0], "w") == 0) {
      if (num_parts == 3) {
        write_signal(parts[1], parts[2]);
      } else {
        fprintf(stderr, "read: invalid args\n");
      }
    } else if (strcmp(parts[0], "wait") == 0) {
      switch (num_parts) {
      case 1:
        //vpi_printf("WAIT for anything\n");
        is_waiting = 1;
        return;
      case 2:
        //vpi_printf("WAIT for %s %s\n", parts[1], parts[2]);
        t = strtoull(parts[1], &s, 10);
        if (*s != 0) {
          fprintf(stderr, "invalid wait time \"%s\"\n", parts[1]);
          break;
        }
        wait_sim_time(t);
        is_waiting = 1;
        return;
      default:
        fprintf(stderr, "wait: invalid args\n");
        break;
      }
    }
    /* simulator.c doesn't send hwait cmds. Don't handle them. */
    /*else if (strcmp(parts[0], "hwait") == 0) {
        switch (num_parts) {
        case 2:
        parts[2] = "ns";
        case 3:
        vpi_printf("HWAIT for %s %s\n", parts[1], parts[2]);
        return;
        break;
        default:
        fprintf(stderr, "hwait: invalid args\n");
        break;
        }
        }*/ else {
      fprintf(stderr, "invalid command\n");
    }
  }
  vpi_sim_control(vpiFinish);
}

PLI_INT32 startup(struct t_cb_data*cb) {
  read_cmds();
  return 0;
}

void startup_fn() {
  vpiHandle hnd;
  struct t_cb_data start_cb;
  memset(&start_cb, 0, sizeof(start_cb));
  start_cb.reason = cbStartOfSimulation;
  start_cb.cb_rtn = startup;
  hnd = vpi_register_cb(&start_cb);
  vpi_free_object(hnd);
}

void (*vlog_startup_routines[])() = {
    startup_fn,
    0
};
