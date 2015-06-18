#include "debug_plan.h"

#include <string.h>
#include <stdio.h>

static int regs_read_next(struct debug_plan *plan,
                          struct debug_reply *rep, struct debug_request *req) {
  struct debug_plan_dump_regs *regs = &plan->state.regs;

  //printf("debug.cnt=%d val=0x%08X\n", regs->i, rep->data);
  /* store read values into regs array */
  if (regs->i >= 6 && regs->i < 22) {
    /* save general registers in order */
    regs->regs[regs->i - 6] = rep->data;
  } else {
    /* save control and system registers. This mapping is messy
       because the order that the STC and STS instructions are easily
       generated is different than the ordering that GDB expects */
    switch (regs->i) {
    case 22:
      regs->regs[REG_SR] = rep->data;
      break;
    case 23:
      regs->regs[REG_GBR] = rep->data;
      break;
    case 24:
      regs->regs[REG_VBR] = rep->data;
      break;
    case 25:
      regs->regs[REG_MACH] = rep->data;
      break;
    case 26:
      regs->regs[REG_MACL] = rep->data;
      break;
    case 27:
      regs->regs[REG_PR] = rep->data;
      break;
    case 32:
      regs->regs[REG_PC] = rep->data;
      break;
    default:
      break;
    }
  }

  /* Setup debug commands */
  req->data_en = 0;
  req->data = 0;
  req->cmd = DBG_CMD_INSERT;
  req->instr = 0x0009;
  if (regs->i < 16) {
    /* MOV Rm, Rn */
    req->instr = 0x6003 | (regs->i << 8) | (regs->i << 4);
  } else if (regs->i < 19) {
    /* STC SR/GBR/VBR, R0 */
    req->instr = 0x0002;
    req->instr |= (regs->i - 16) << 4;
  } else if (regs->i < 22) {
    /* STS MACH/MACL/PR, R0 */
    req->instr = 0x000A;
    req->instr |= (regs->i - 19) << 4;
  } else if (regs->i < 25) {
    /* Insert NOPs so that all written values are gathered */
  } else if (regs->i <= 36) {
    switch (regs->i) {
    case 25:
      /* MOV #0,R0 */
      req->instr = 0xE000;
      break;
    case 26:
      /* To read PC, use a JSR to load it into PR */
      /* JSR @R0 */
      req->instr = 0x400B;
      break;
    case 27:
      /* STS PR, R0 - to let us capture value written to R0 */
      req->instr = 0x002A;
      break;
    case 28:
      /* Use RTS to load PR back into PC */
      /* RTS */
      req->instr = 0x000B;
      break;
    case 29:
      /* write original value to PR */
      /* MOV R0,R0 */
      req->instr = 0x6003;
      req->data_en = 1;
      req->data = regs->regs[REG_PR];
      break;
    case 30:
    /* LDS R0, PR */
      req->instr = 0x402A;
      break;
    case 31:
      /* write original value to R0 */
      /* MOV R0,R0 */
      req->instr = 0x6003;
      req->data_en = 1;
      req->data = regs->regs[REG_R0];
      break;
    case 36:
      req->instr = 0x0009;
      /* return 0 to signal last request */
      return 0;
    default:
      /* NOP - to allow above writes to complete */
      req->instr = 0x0009;
      break;
    }
  }
  regs->i++;
  return 1; /* more requests to come */
}

static int regs_write_next(struct debug_plan *plan,
                          struct debug_reply *rep, struct debug_request *req) {
  struct debug_plan_dump_regs *regs = &plan->state.regs;
  int i;

  /* Setup debug commands */
  req->data_en = 0;
  req->data = 0;
  req->cmd = DBG_CMD_INSERT;
  req->instr = 0x0009;

  switch (regs->i) {
  /* Load PC via PR and an RTS instruction */
  case 0:
    /* write PC value to PR */
    /* LDS R0, PR */
    req->instr = 0x402a;
    req->data_en = 1;
    req->data = regs->regs[REG_PC];
    break;
  case 1:
    /* RTS */
    req->instr = 0x000B;
    break;
  case 2:
    /* NOP in delay slot of RTS - not sure if necessary when
       inserting */
    req->instr = 0x0009;
    break;
  /* Load status registers */
  case 3:
    /* LDS R0, MACH */
    req->instr = 0x400a;
    req->data_en = 1;
    req->data = regs->regs[REG_MACH];
    break;
  case 4:
    /* LDS R0, MACL */
    req->instr = 0x401a;
    req->data_en = 1;
    req->data = regs->regs[REG_MACL];
    break;
  case 5:
    /* LDS R0, PR */
    req->instr = 0x402a;
    req->data_en = 1;
    req->data = regs->regs[REG_PR];
    break;
  /* Load control registers */
  case 6:
    /* LDC R0, SR */
    req->instr = 0x400e;
    req->data_en = 1;
    req->data = regs->regs[REG_SR];
    break;
  case 7:
    /* LDC R0, GBR */
    req->instr = 0x401e;
    req->data_en = 1;
    req->data = regs->regs[REG_GBR];
    break;
  case 8:
    /* LDC R0, VBR */
    req->instr = 0x402e;
    req->data_en = 1;
    req->data = regs->regs[REG_VBR];
    break;
  /* Load control registers */
  case 9:
    break;
  }
  /* load general purpose registers */
  if (regs->i >= 9 && regs->i < 25) {
    i = regs->i - 9;
    /* MOV Ri, Ri */
    req->instr = 0x6003 | (i << 8) | (i << 4);
    req->data_en = 1;
    req->data = regs->regs[i];
  }
  if (regs->i == 26) {
    return 0;
  }
  regs->i++;
  return 1; /* more requests to come */
}

static int mem_read_next(struct debug_plan *plan,
                         struct debug_reply *rep, struct debug_request *req) {
  struct debug_plan_mem *mem = &plan->state.mem;
  /* copy read value */
  if (mem->i >= 6) {
    if (mem->buf) {
      *mem->buf = rep->data;
      mem->buf++;
    }
  }
  /* Setup debug commands */
  req->data_en = 0;
  req->data = 0;
  req->cmd = DBG_CMD_INSERT;
  req->instr = 0x0009; /* default to NOP */
  if (mem->num_words > 0) {
    /* MOV.L @R1,R0
       previously used MOV.L @R0,R0 but that caused register conflicts
       which mean fewer instructions could be in the pipeline at once */
    req->instr = 0x6012;
    req->data_en = 1;
    req->data = mem->addr;
    mem->addr += 4;
  } else {
    switch (mem->num_words) {
    case -2:
      /* restore R0 value */
      /* MOV R0,R0 */
      req->instr = 0x6003;
      req->data_en = 1;
      req->data = mem->r0;
      break;
    /* More NOPs until num_words==-5 to collect the read data */
    case -5:
      return 0;
    }
  }
  mem->i++;
  mem->num_words--;
  return 1;
}

static int mem_write_next(struct debug_plan *plan,
                          struct debug_reply *rep, struct debug_request *req) {
  struct debug_plan_mem *mem = &plan->state.mem;
  /* Setup debug commands */
  req->data_en = 0;
  req->data = 0;
  req->cmd = DBG_CMD_INSERT;
  req->instr = 0x0009; /* default to NOP */
  if (mem->i == 0) {
    /* first load the address into R0 */
    /* MOV R0, R0 */
    req->instr = 0x6003;
    req->data_en = 1;
    req->data = mem->addr;
  } else if (mem->num_words >= 0) {
    /* MOV.L R1, @-R0 */
    req->instr = 0x2016;
    req->data_en = 1;
    req->data = *mem->buf;
    mem->buf--; /* dec because writing buffer from highest addr to lowest */
  } else if (mem->num_words == -1) {
    /* restore R0 value */
    /* MOV R0, R0 */
    req->instr = 0x6003;
    req->data_en = 1;
    req->data = mem->r0;
  } else if (mem->num_words == -3) {
    /* NOP to allow above write to complete before finishing */
    return 0;
  }
  mem->i++;
  mem->num_words--;
  return 1;
}

void debug_plan_init_regs_read(struct debug_plan *plan, uint32_t *regs) {
  memset(plan, 0, sizeof(*plan));
  plan->next = regs_read_next;
  plan->state.regs.regs = regs;
}

void debug_plan_init_regs_write(struct debug_plan *plan, uint32_t *regs) {
  memset(plan, 0, sizeof(*plan));
  plan->next = regs_write_next;
  plan->state.regs.regs = regs;
}

void debug_plan_init_mem_read(struct debug_plan *plan, uint32_t addr, uint32_t num_words, uint32_t *buf, uint32_t r0)
{
  memset(plan, 0, sizeof(*plan));
  plan->next = mem_read_next;
  plan->state.mem.addr = addr;
  plan->state.mem.num_words = num_words;
  plan->state.mem.buf = buf;
  plan->state.mem.r0 = r0;
}

void debug_plan_init_mem_write(struct debug_plan *plan, uint32_t addr, uint32_t num_words, uint32_t *buf, uint32_t r0)
{
  memset(plan, 0, sizeof(*plan));
  plan->next = mem_write_next;
  /* start address past the end of the buffer in memory between we use
     MOV.L Rm, @-Rn to write the values */
  plan->state.mem.addr = addr + (num_words * 4);
  plan->state.mem.buf = buf + num_words - 1;
  plan->state.mem.num_words = num_words;
  plan->state.mem.r0 = r0;
}


/*  */
int debug_plan_next(struct debug_plan *plan,
                    struct debug_reply *prev_reply, struct debug_request *next_request) {
  return plan->next(plan, prev_reply, next_request);
}

