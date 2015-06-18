#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <stdio.h>
#include <inttypes.h>
#include <unistd.h>

#include "bitvec.h"

/*
  This simulator library supports running and interacting with a GHDL
  simulator. A C program can read or write VHDL signals and can also
  wait for or be notified of changes to those signals.

  The sim functions are not thread-safe, and are not all reentrant
  during the hook callbacks. See the comment describing sig_hooks below
  for more details.
 */

#define PICO PRIu64
#define NANOS(p) (p/1000)
typedef uint64_t picos_t;

#if 0
#define SIMLOG(sim, fmt, args...) \
  printf(__FILE__ ":%u:@%" PICO "ps: " fmt, __LINE__, sim->picos, ##args)
#else
#define SIMLOG(...)
#endif

#define SIMERR(sim, fmt, args...) \
  printf(__FILE__ ":Error:%u:@%" PICO "ps: " fmt, __LINE__, sim->picos, ##args)

/*
  Represents a VHDL signal. To interact with a signal, the VHDL side
  of a simulation must also be aware that the signal is important to
  the simulation. The intention is that signals are defined in a
  common header file, and then both the C side and the VHDL side
  includes sim_macros.h to build an array of signal structs on the C
  side and the necessary processes on the VHDL side.
 */
struct signal {
  int index;
  char *name;
  char *type;
  char *value; /* a null-terminated string that is the signal's value */
  int value_len; /* the number of characters in value */
  int read_only; /* non-zero if signal is read-only */

  int event; /* set non-zero if an event occured on this signal (ie.
                the simulator caused it's value to change). sig_hook
                callbacks can examine this flag to determine what
                changed. The event flags for all signals are cleared
                after each round of sig_hook callbacks */
};

struct line_reader {
  int fd;
  char buf[2049];
  int buf_used;
  char *line_start;
};

struct sig_hook;
struct simulator;

/*
  hook_cb is a callback function called when a sig_hook is triggered
  either due to an event on a signal or a certain simulator time being
  reached.

  During callback, not all simulator functions are safe to call. See
  the sig_hook comment below for more details.

  If a callback returns zero, the hook will stay scheduled. If a
  callback returns non-zero, the hook is removed.

  The time of a sig_hook can directly modified during the a callback.
  If it returns zero, the simulator will detect the changed time and
  reschedule the hook.
*/
typedef int (*hook_cb)(struct simulator *sim, struct sig_hook *hook);

struct sig_hook {
  hook_cb cb;

  /* A bit vector of which signals this hook is interested in */
  struct bv signals;
  /* The target time of this hook. Zero corresponds to no time */
  picos_t time;

  struct sig_hook *next;
  struct sig_hook *prev;
};

/* Possible values for on_exit in the sim_cfg. See below. */
enum sim_on_exit {
  /* if ghdl exits... */
  SIMEXIT_NONE, /* nothing happens */
  SIMEXIT_EXIT, /* the process exits */
  SIMEXIT_ERROR, /* the process exits only if ghdl's exit status was
                    non-zero or ghdl was signalled */
};

enum sim_type {
  SIM_TYPE_GHDL,
  SIM_TYPE_IVERILOG,
};

struct sim_cfg {
  enum sim_type type;
  char *name;
  struct signal *signals;
  int num_signals;
  int debug;
  /* Number of picos per sim time unit. If 0, then it defaults to 1.
     For GHDL should always be 1. For iverilog should be set to the
     sim units. */
  picos_t pico_scale;

  /* Determine what happens when ghdl exits before sim_free is called.
     If you're running a single ghdl simulator, and may be using the
     stop_time option, it's useful for the entire process to exit when
     the ghdl process exists. */
  enum sim_on_exit on_exit;

  /* TIME simulator will stop at */
  char *stop_time;

  union {
    struct {
      /* (none|standard|synopsys|mentor) */
      char *ieee;
      char *ieee_asserts;
      char *work_name;
      /* (87|93|93c|00|02) */
      char *std;
      /* FILENAME of vcd file */
      char *vcd;
      /* FILENAME of wave file */
      char *wave;
      /* if non-zero, -fexplicit will be set */
      int explicit;
    } ghdl;
    struct {
      char *vpi_dir;
      char *vpi_name;
    } iverilog;
  } cfg;
};

/* Initializes the optional ghdl configuration from the given
   command-line */
int sim_cfg_parse(struct sim_cfg *cfg, int argc, char **argv);

struct simulator {
  enum sim_type type;
  char *name;
  struct signal *signals;
  int num_signals;
  enum sim_on_exit on_exit;
  int debug;

  pid_t pid;
  int pty;
  int child_running;
  int exit_status;

  picos_t picos;
  picos_t pico_scale;
  struct line_reader lr;
  int io_error;
  int any_event;
  int dirty_writes;
  struct bv events;
  int calling_hooks;
  struct sig_hook *hooks;
  /* hooks that will be immediately called after a the current round
     of callbacks is complete */
  struct sig_hook *immediate_hooks;
  struct sig_hook sig_wait;
  int sig_wait_occurred;
};

/* Initialize a given simulator struct and start the ghdl process. */
int sim_new(struct simulator *sim, struct sim_cfg *cfg);

/* Stop a simulator and frees any associated resources. */
int sim_free(struct simulator *sim);

/* Lookup a signal by name. Returns NULL if the signal is not found. */
struct signal *sim_lookup_signal(struct simulator *sim, char *name);

/*******************************************************************
 sig_hook - Hooks provide a callback when a signal's value changes or
 a certain simulator time is reached.

 In the context of a callback, some of the sim functions are not
 reentrant. During a callback, only sim_hook_add, sim_hook_remove,
 sim_immediate_hook_add, sim_set, sim_seti, sim_get, and sim_geti are
 safe to call. The sim_hook_add and sim_hook_remove functions should
 only be called for new sig_hooks, not hooks already added to the
 simulator. Any sig_hook callback added during a hook callback won't
 be called until the next time the hooks are checked (after a sim_wait
 or a get after a set).
********************************************************************/

/* Initialize a sig_hook with a given callback. The hook_cb function
   will be called either when an event occurs on a signal or the
   simulator reaches a certain time.

   To specify which signals you are interested in, use bv_set to set
   bits bit in the sig_hook's signals bit vector. The index of the
   bits correspond to the index of the simulator's signals.

   To specify a simulator time, assign the sig_hook's time field. A
   value of zero correspond to no timeout.
 */
void sim_hook_init(struct simulator *sim, struct sig_hook *h, hook_cb cb);

/* Add a sig_hook to the simulator. */
void sim_hook_add(struct simulator *sim, struct sig_hook *h);

/* Remove a sig_hook from the simulator. */
void sim_hook_remove(struct simulator *sim, struct sig_hook *h);

/* Adds a hook that will be called immediately after the normal hooks
   are called. The hook is called only once and then automatically
   removed, regardless of the callbacks return value. The restriction
   on what functions can be called during an immediate hook callback
   are slightly different; sim_hook_add and sim_hook_remove can be
   called for any hook. The intended purpose of an immediate hook is
   to support a normal hook that removes multiple existing hooks by
   scheduling an immediate hook to do the work. */
void sim_immediate_hook_add(struct simulator *sim, struct sig_hook *h);

/*******************************************************************
 The sim_wait* functions run a VHDL wait statement in the simulator
 process. If a signal's value changes during the wait it may cause
 signal change events to call hook callbacks. The sim_wait* functions
 serve to advance the simulator time.
********************************************************************/

void sim_idle(struct simulator *sim);

/* Wait for an event on any signal */
void sim_wait(struct simulator *sim);

/* Wait for an event on any signal or for a given delay to expire. If
   force is zero, then the function returns after the first event or
   the delay has passed. If force is non-zero then the function will
   not return until the delay has passed, regardless of signal events. */
void sim_wait_time(struct simulator *sim, uint64_t picos, int force);

/* Wait for a clk edge. If rising is zero, the function returns after
   the rising clock edge. If rising is non-zero, it returns after the
   falling clock edge.

   This functions exists so that it's possible to wait for clock edges
   without including the clock in the list of active signals. */
void sim_wait_clk_edge(struct simulator *sim, int rising);

/* Wait for any event on a single signal. This is a convenience
   functino that could be accomplished using a sig_hook. */
void sim_wait_signal(struct simulator *sim, int sig_index);

/* Write a signal  */
int sim_set(struct simulator *sim, int sig_index, char *val);

/* Write a signal as an int */
int sim_seti(struct simulator *sim, int sig_index, uint32_t val);

/* Read a signal. The returned pointer is the same as the value field
   in the signal struct. It is null-terminated. Do not modify it.
   Reading a signal after writing signals causes any updated values to
   be fetched from the simulator which may cause signal change events
   to call hook callbacks.
 */
char *sim_get(struct simulator *sim, int sig_index);

/* Read a signal as an int. If the signal's value cannot be read or
   cannot be parsed to a int, returns 0. Reading a signal after
   writing signals causes any updated values to be fetched from the
   simulator which may cause signal change events to call hook
   callbacks.
 */
uint32_t sim_geti(struct simulator *sim, int sig_index);
int sim_geti_check(struct simulator *sim, int sig_index, uint32_t *val);
void sim_force_read(struct simulator *sim, int sig_index);

int sim_set_all(struct simulator *sim, int sig_index, char c);

#endif
