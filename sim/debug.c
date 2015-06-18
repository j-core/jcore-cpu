#include "debug.h"

#include "sh2instr.h"

static void init_delay_hook(struct delay_hook *hook, struct debug_hw *debug, hook_cb cb) {
  hook->debug = debug;
  hook->scheduled = 0;
  sim_hook_init(debug->sim, &hook->hook, cb);
}

static void unschedule(struct delay_hook *hook) {
  if (hook->scheduled) {
    hook->scheduled = 0;
    sim_hook_remove(hook->debug->sim, &hook->hook);
  }
}

static void schedule(struct delay_hook *hook) {
  unschedule(hook);
  hook->scheduled = 1;
  sim_hook_add(hook->debug->sim, &hook->hook);
}

static void schedule_delay(struct delay_hook *hook, picos_t delay) {
  struct debug_hw *debug = hook->debug;
  hook->hook.time = debug->sim->picos + delay;
  schedule(hook);
}

/*static void schedule_cycle_delay(struct delay_hook *hook) {
  schedule_delay(hook, hook->debug->clock_period);
  }*/

static picos_t round_up(picos_t picos, int quantum) {
  picos += quantum - 1;
  picos = (picos / quantum) * quantum;
  return picos;
}

static int next_cmd(struct debug_hw *debug) {
  struct debug_command *cmd = debug->cmds;
  if (cmd == 0) {
    schedule_delay(&debug->drop_en_hook, debug->clock_period / 4);
    return 0;
  }
  unschedule(&debug->drop_en_hook);

  if (cmd->cmd != DBG_CMD_CONTINUE) {
    /* TODO: Instead of this simple tracking, should be monitoring
       some "in debug mode" signal from the simulation. Will also need
       that to notify of spontaneously entering debug mode due to a
       breakpoint */
    debug->is_paused = 1;
  }
  sim_seti(debug->sim, debug->sig.en, 1);
  sim_seti(debug->sim, debug->sig.cmd, cmd->cmd);
  sim_seti(debug->sim, debug->sig.ir, cmd->ir);
  sim_seti(debug->sim, debug->sig.din, cmd->d);
  sim_seti(debug->sim, debug->sig.din_en, cmd->d_en);
  debug->ack_hook.time = round_up(debug->sim->picos + debug->clock_period, debug->clock_period);
  return 1;
}

static int drop_en_cb(struct simulator *sim, struct sig_hook *hook) {
  struct debug_hw *debug = container_of(hook, struct debug_hw, drop_en_hook.hook);
  sim_seti(debug->sim, debug->sig.en, '0');
  debug->drop_en_hook.scheduled = 0;
  return -1;
}

static int ack_hook_cb(struct simulator *sim, struct sig_hook *hook) {
  //  printf("ack_hook cb at %" PICO "\n", sim->picos);
  struct debug_hw *debug = container_of(hook, struct debug_hw, ack_hook);
  struct debug_command *cmd = 0;
  
  /*if (debug->sim->picos % debug->clock_period != 0) {
    printf("Delaying ack read until next \n");
    return 0;
    }*/  
  if (sim_geti(debug->sim, debug->sig.ack)) {
    /* command has been acked */
    cmd = debug->cmds;
    DL_DELETE(debug->cmds, cmd);
    cmd->prev = 0;

    //printf("Saw command ACK %d\n", cmd->cmd);

    // TODO: use output in debug_o to determine state
    if (cmd->cmd == DBG_CMD_CONTINUE) {
      debug->is_paused = 0;
    } else {
      debug->is_paused = 1;
    }
    
    if (cmd->on_done) {
      debug->calling_cmd_done = 1;
      cmd->on_done(debug, cmd);
      debug->calling_cmd_done = 0;
    }
    if (next_cmd(debug) == 0) {
      return -1;
    }
  }
  if (debug->cmds) {
    // active command so rescedule for next clock edge
    hook->time = debug->sim->picos + debug->clock_period;
    return 0;
  } else {
    return -1;
  }
}


void debug_hw_init(struct debug_hw *debug, struct simulator *sim, int clock_period_ns) {
  debug->sim = sim;
  /*debug->rdy_cb = cb;
  init_delay_hook(&debug->delay_check_rdy, debug, delay_check_rdy_cb);
  init_delay_hook(&debug->delay_hold, debug, delay_hold_cb);
  init_delay_hook(&debug->delay_din_en, debug, delay_reg_wr_en);*/
  debug->is_paused = 0;
  debug->clock_period = clock_period_ns * 1000;
  debug->cmds = 0;
  debug->calling_cmd_done = 0;
  /*sim_hook_init(sim, &debug->rdy_hook, rdy_cb);
  bv_set(&debug->rdy_hook.signals, debug->sig.rdy, 1);
  sim_hook_add(sim, &debug->rdy_hook);*/

  init_delay_hook(&debug->drop_en_hook, debug, drop_en_cb);
  //bv_set(&debug->ack_hook.signals, debug->sig.ack, 1);
  sim_hook_init(sim, &debug->ack_hook, ack_hook_cb);
  //sim_hook_init(sim, &debug->ack_hook, ack_cb);
  //sim_hook_add(sim, &debug->rdy_hook);

  sim_seti(sim, debug->sig.en, 0);
  sim_seti(sim, debug->sig.cmd, DBG_CMD_BREAK);
  sim_seti(sim, debug->sig.din, 0);
  sim_seti(sim, debug->sig.ir, 0);
}

void debug_break(struct debug_hw *debug, struct debug_command *cmd) {
  printf("break\n");
  cmd->cmd = DBG_CMD_BREAK;
  debug_add_cmd(debug, cmd);
}

void debug_step(struct debug_hw *debug, struct debug_command *cmd) {
  printf("step\n");
  cmd->cmd = DBG_CMD_STEP;
  debug_add_cmd(debug, cmd);
}

void debug_continue(struct debug_hw *debug, struct debug_command *cmd) {
  printf("continue\n");
  cmd->cmd = DBG_CMD_CONTINUE;
  debug_add_cmd(debug, cmd);
}

void debug_insert(struct debug_hw *debug, struct debug_command *cmd, uint16_t ir) {
  /*char buf[256];
  op_name(buf, sizeof(buf), ir);
  printf("insert %hx: %s at time %" PICO "\n",
  ir, buf, debug->sim->picos);*/
  //debug->is_paused = 0;
  cmd->cmd = DBG_CMD_INSERT;
  cmd->ir = ir;
  debug_add_cmd(debug, cmd);
}

uint32_t debug_read_data(struct debug_hw *debug) {
  return sim_geti(debug->sim, debug->sig.dout);
}

void debug_write_data(struct debug_hw *debug, struct debug_command *cmd, uint32_t data) {
  cmd->d = data;
  cmd->d_en = 1;
}

void debug_add_cmd(struct debug_hw *debug, struct debug_command *cmd) {
  if (cmd->prev) {
    printf("cmd %p already has prev %p\n", cmd, cmd->prev);
  }
  DL_APPEND(debug->cmds, cmd);
  if (debug->calling_cmd_done == 0 && debug->cmds == cmd) {
    /* queued a command when none was active. */
    if (next_cmd(debug)) {
      //printf("schedule ack check\n");
      sim_hook_add(debug->sim, &debug->ack_hook);
    }
  }
}

void debug_add_request(struct debug_hw *debug, struct debug_command *cmd, struct debug_request *req) {
  cmd->cmd = req->cmd;
  cmd->ir = req->instr;
  cmd->d = req->data;
  cmd->d_en = req->data_en;
  debug_add_cmd(debug, cmd);
}
