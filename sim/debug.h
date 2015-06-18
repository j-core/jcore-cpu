/*
  Support for the CPU's debug features.
 */
#ifndef DEBUG_H
#define DEBUG_H

#include <stddef.h>

#include "simulator.h"
#include "mem_bus.h"
#include "utlist.h"
#include "debug_plan.h"

struct debug_hw;

struct delay_hook {
  struct debug_hw *debug;
  int scheduled;
  struct sig_hook hook;
};

struct debug_command {
  enum debug_cmd cmd;
  uint16_t ir;
  uint32_t d;
  int d_en;
  void (*on_done)(struct debug_hw *debug, struct debug_command *cmd);
  struct debug_command *prev, *next;
};

struct debug_hw {
  struct simulator *sim;
  //struct sig_hook ack_hook;
  //debug_rdy_fn rdy_cb;
  struct sig_hook ack_hook;
  struct delay_hook drop_en_hook;

  int calling_cmd_done;
  int is_paused;
  int clock_period;

  /* queue of commands to send */
  struct debug_command *cmds;
  struct {
    int en;
    int cmd;
    int din;
    int din_en;
    int ir;

    int ack;
    int rdy;
    int dout;
  } sig;
};

void debug_hw_init(struct debug_hw *debug, struct simulator *sim, int clock_period);

void debug_add_cmd(struct debug_hw *debug, struct debug_command *cmd);
void debug_add_request(struct debug_hw *debug, struct debug_command *cmd, struct debug_request *req);

void debug_break(struct debug_hw *debug, struct debug_command *cmd);
void debug_step(struct debug_hw *debug, struct debug_command *cmd);
void debug_continue(struct debug_hw *debug, struct debug_command *cmd);
void debug_insert(struct debug_hw *debug, struct debug_command *cmd, uint16_t ir);
uint32_t debug_read_data(struct debug_hw *debug);
void debug_write_data(struct debug_hw *debug, struct debug_command *cmd, uint32_t data);

#endif
