/*
Define and generate "debug plans" which are sequences of debug
requests sent to the CPU to accomplish useful things like read the
values of all the registers or read and write memory.

This is intended to be generic and to be usable by both the cpusim and
the gdb stub that communicates through JTAG.
*/

#ifndef DEBUG_PLAN_H
#define DEBUG_PLAN_H

#include <inttypes.h>
#include <stddef.h>

enum debug_cmd {
  DBG_CMD_BREAK,
  DBG_CMD_STEP,
  DBG_CMD_INSERT,
  DBG_CMD_CONTINUE
};

struct debug_request {
  enum debug_cmd cmd;
  uint16_t instr;
  uint32_t data;
  int data_en;
};

struct debug_reply {
  uint32_t data;
};

enum registers {
  REG_R0,
  REG_R1,
  REG_R2,
  REG_R3,
  REG_R4,
  REG_R5,
  REG_R6,
  REG_R7,
  REG_R8,
  REG_R9,
  REG_R10,
  REG_R11,
  REG_R12,
  REG_R13,
  REG_R14,
  REG_R15,
  REG_PC,
  REG_PR,
  REG_GBR,
  REG_VBR,
  REG_MACH,
  REG_MACL,
  REG_SR,
  REG_NUM_REGS
};

struct debug_plan_dump_regs {
  int i;
  uint32_t *regs;
};

struct debug_plan_mem {
  int i;
  int num_words;
  uint32_t addr;
  uint32_t *buf;
  uint32_t r0;
};

struct debug_plan {
  int (*next)(struct debug_plan *plan,
              struct debug_reply *prev_reply, struct debug_request *next_request);
  union {
    struct debug_plan_dump_regs regs;
    struct debug_plan_mem mem;
  } state;
};

/* Initialize a plan for reading all the value of all the CPU
   registers into the given regs array. The values are stored in the
   order defined by the registers enum. */
void debug_plan_init_regs_read(struct debug_plan *plan, uint32_t *regs);

/* Initialize a plan for writing all the value of all the CPU
   registers into the given regs array. The values are stored in the
   order defined by the registers enum. */
void debug_plan_init_regs_write(struct debug_plan *plan, uint32_t *regs);

/* Initialize a plan for reading a block of N 4-byte words starting at CPU
   memory address addr and storing the values in the given buffer */
void debug_plan_init_mem_read(struct debug_plan *plan, uint32_t addr, uint32_t num_words, uint32_t *buf, uint32_t r0);

/* Initialize a plan for writing a block of N 4-byte words from the
   given buffer to CPU memory bus starting at address addr */
void debug_plan_init_mem_write(struct debug_plan *plan, uint32_t addr, uint32_t num_words, uint32_t *buf, uint32_t r0);

/* Returns the next debug_request of a plan, storing it in the
   next_request argument. All plans generate at least one request. In
   multi-request plans, the caller must pass the reply to the previous
   request in for each request after the first. The prev_reply
   argument is ignored during the first debug_plan_next call.

   Returns 0 when the return request is the last in the plan. Returns
   1 if there are more requests.

   Calling debug_plan_next again on the same plan after it has
   returned 0 is undefined.
*/
int debug_plan_next(struct debug_plan *plan,
                    struct debug_reply *prev_reply, struct debug_request *next_request);

#endif
