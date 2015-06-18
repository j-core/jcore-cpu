/*
  Given a op code, simulate decode.v to see what un-pipelined control
  signals are set. This was useful to create the instruction set
  spreadsheet.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/mman.h>
#include <arpa/inet.h>
#include <getopt.h>

#include "simulator.h"
#include "utlist.h"
#include "decode_signals.h"
#include "sh2instr.h"

enum {
#define SIG(name) SIG_##name,
SIGNALS
#undef SIG
  NUM_SIGNALS,
};

struct signal signals[] = {
#define SIG(sig) {.index = SIG_##sig, .name = "decode." #sig},
SIGNALS
#undef SIG
  {.name = NULL},
};

enum {
#define ALU(n, op) ALU_##n,
ALUFUNCS
#undef ALU
  NUM_ALU_FUNCS,
};

const char *alu_names[] = {
#define ALU(n,op) #n,
ALUFUNCS
#undef ALU
};

const char *alu_ops[] = {
#define ALU(n,op) op,
ALUFUNCS
#undef ALU
};

struct sim_cfg cfg = {
  .type = SIM_TYPE_IVERILOG,
  .name = "decode.vvp",
  .signals = signals,
  .num_signals = NUM_SIGNALS,
  .on_exit = SIMEXIT_EXIT
};

int hook_fn(struct simulator *sim, struct sig_hook *hook) {
  return 0;
}

static const char *bus_names[] = {
  "X Bus",
  "Y Bus",
  "Z Bus",
  "W Bus"
};

static const char *ma_sz_names[] = {
  "byte",
  "word",
  "long",
  "ERROR"
};

void print_bus(int bus, const char *val) {
  printf("  %s = %s\n", bus_names[bus], val);
}

void print_bus_reg(int bus, uint32_t r) {
  printf("  %s = R%u\n", bus_names[bus], r);
}

void print_bus_name(struct simulator *sim, int sig, int base_bus,
               const char *name) {
  if (sim_geti(sim, sig))
    print_bus(sig - base_bus, name);
}

static const char *const_names[] = {
  "Z4", "Z42", "Z44",
  "Z8", "Z82", "Z84",
  "S8", "S82", "S122",
};

void print_bus_const(struct simulator *sim, int sig, int base_bus) {
  char *n = "CONST ERROR";
  uint32_t total =  sim_geti(sim, SIG_EX_CONST_ZERO4) +
    sim_geti(sim, SIG_EX_CONST_ZERO42) +
    sim_geti(sim, SIG_EX_CONST_ZERO44) +
    sim_geti(sim, SIG_EX_CONST_ZERO8) +
    sim_geti(sim, SIG_EX_CONST_ZERO82) +
    sim_geti(sim, SIG_EX_CONST_ZERO84) +
    sim_geti(sim, SIG_EX_CONST_SIGN8) +
    sim_geti(sim, SIG_EX_CONST_SIGN82) +
    sim_geti(sim, SIG_EX_CONST_SIGN122);

  if (sim_geti(sim, sig)) {
    if (total == 1) {
      if (sim_geti(sim, SIG_EX_CONST_ZERO4))
        n = "Z4";
      else if (sim_geti(sim, SIG_EX_CONST_ZERO42))
        n = "Z42";
      else if (sim_geti(sim, SIG_EX_CONST_ZERO44))
        n = "Z44";
      else if (sim_geti(sim, SIG_EX_CONST_ZERO8))
        n = "Z8";
      else if (sim_geti(sim, SIG_EX_CONST_ZERO82))
        n = "Z82";
      else if (sim_geti(sim, SIG_EX_CONST_ZERO84))
        n = "Z84";
      else if (sim_geti(sim, SIG_EX_CONST_SIGN8))
        n = "S8";
      else if (sim_geti(sim, SIG_EX_CONST_SIGN82))
        n = "S82";
      else if (sim_geti(sim, SIG_EX_CONST_SIGN122))
        n = "S122";
      print_bus(sig - base_bus, n);
    } else {
      print_bus(sig - base_bus, "CONST ERROR");
    }
  }
}

int rising_edge(struct simulator *sim, int sig) {
  return bv_get(&sim->events, sig) && sim_geti(sim, sig);
}

void print_mem_issue(struct simulator *sim) {
  //  char *op = sim_geti(sim, SIG_EX_MA_WR) ? "write" : "read";
  const char *size = ma_sz_names[sim_geti(sim, SIG_EX_MA_SZ)];
  const char *addr = "ERROR";
  const char *data = "ERROR";
  if (sim_geti(sim, SIG_EX_WRMAAD_Z)) {
    addr = "Z";
  } else if (sim_geti(sim, SIG_EX_WRMAAD_TEMP)) {
    addr = "TEMP";
  } else {
    addr = "X";
  }
  if (sim_geti(sim, SIG_EX_WRMADW_X)) {
    data = "X";
  } else if (sim_geti(sim, SIG_EX_WRMADW_Y)) {
    data = "Y";
  } else {
    data = "Z";
  }
  /*printf("  MEM %s %s", op,
    size);*/
  if (sim_geti(sim, SIG_EX_MA_WR)) {
    printf("  MEM[%s] = %s", addr, data);
  } else {
    printf("  W = MEM[%s]", addr);
  }
  printf(" %s\n", size);
}

int change_fn(struct simulator *sim, struct sig_hook *hook) {
  int i;
  int print;
  uint32_t u;

  /* check assumptions */

  /* Memory access */
  if (rising_edge(sim, SIG_EX_MA_ISSUE)) {
  } else {
  }
  /* Reads: MA_ISSUE, WRMAAD and WRMADW */

  if (bv_get(&sim->events, SIG_CLK) && sim_geti(sim, SIG_CLK)) {
    for (i = 0; i < NUM_SIGNALS; i++) {
      /*if (bv_get(&sim->events, i)) {
        printf("sig %s changed = %s\n", signals[i].name, signals[i].value);
        }*/
      print = 0;

      switch (i) {
      case SIG_INSTR_STATE:
      case SIG_CLK:
      case SIG_SLOT:
      case SIG_EVENT_REQ:
      case SIG_EVENT_ACK:
      case SIG_EX_REGNUM_X:
      case SIG_EX_REGNUM_Y:
      case SIG_EX_REGNUM_Z:
      case SIG_WB_REGNUM_W:

      case SIG_EX_MA_WR:
      case SIG_EX_MA_SZ:
      case SIG_EX_WRMAAD_Z:
      case SIG_EX_WRMAAD_TEMP:
      case SIG_EX_WRMADW_X:
      case SIG_EX_WRMADW_Y:
        print = 0;
        break;
      case SIG_INSTR_SEQ:
        print = 0;
        break;
      case SIG_IF_DR:
        if (bv_get(&sim->events, i)) {
          printf("  Instr ");
          print_instr(sim_geti(sim, i));
          printf(" %s\n", signals[i].value);
        }
        break;
      case SIG_EX_ALUFUNC:
        u = sim_geti(sim, i);
        if (u)
          printf("  ALUFUNC = %s %s\n", alu_names[u], alu_ops[u]);
        break;

      case SIG_EX_RDREG_X:
      case SIG_EX_RDREG_Y:
      case SIG_EX_WRREG_Z:
      case SIG_WB_WRREG_W:
        if (sim_geti(sim, i))
          print_bus_reg(i - SIG_EX_RDREG_X, sim_geti(sim, i - SIG_EX_RDREG_X + SIG_EX_REGNUM_X));
        break;

      case SIG_EX_RDSR_X:
      case SIG_EX_RDSR_Y:
      case SIG_EX_WRSR_Z:
      case SIG_WB_WRSR_W:
        print_bus_name(sim, i, SIG_EX_RDSR_X, "SR");
        break;

      case SIG_EX_RDGBR_X:
      case SIG_EX_RDGBR_Y:
      case SIG_EX_WRGBR_Z:
      case SIG_WB_WRGBR_W:
        print_bus_name(sim, i, SIG_EX_RDGBR_X, "GBR");
        break;

      case SIG_EX_RDVBR_X:
      case SIG_EX_RDVBR_Y:
      case SIG_EX_WRVBR_Z:
      case SIG_WB_WRVBR_W:
        print_bus_name(sim, i, SIG_EX_RDVBR_X, "VBR");
        break;

      case SIG_EX_RDPR_X:
      case SIG_EX_RDPR_Y:
      case SIG_EX_WRPR_Z:
      case SIG_WB_WRPR_W:
        print_bus_name(sim, i, SIG_EX_RDPR_X, "PR");
        break;

      case SIG_EX_RDPC_X:
      case SIG_EX_RDPC_Y:
      case SIG_EX_WRPC_Z:
      case SIG_EX_WRPC_W:
        print_bus_name(sim, i, SIG_EX_RDPC_X, "PC");
        break;

      case SIG_EX_RDCONST_X:
      case SIG_EX_RDCONST_Y:
        print_bus_const(sim, i, SIG_EX_RDCONST_X);
        break;

      case SIG_EX_CONST_ZERO4:
      case SIG_EX_CONST_ZERO42:
      case SIG_EX_CONST_ZERO44:
      case SIG_EX_CONST_ZERO8:
      case SIG_EX_CONST_ZERO82:
      case SIG_EX_CONST_ZERO84:
      case SIG_EX_CONST_SIGN8:
      case SIG_EX_CONST_SIGN82:
      case SIG_EX_CONST_SIGN122:
        if (!sim_geti(sim, SIG_EX_RDCONST_X) && !sim_geti(sim, SIG_EX_RDCONST_Y)
            && sim_geti(sim, i))
          printf("  CONST = %s\n", const_names[i - SIG_EX_CONST_ZERO4]);
        break;

        /*case SIG_MULCOM1:
        break;
      case SIG_MULCOM2:
      break;*/

      case SIG_EX_MA_ISSUE:
        if (sim_geti(sim, i)) {
          print_mem_issue(sim);
        }
        break;
      default:
        print = sim_geti(sim, i);
        break;
      }
      if (print)
        printf("  %s = %s\n", signals[i].name + 7, signals[i].value);
    }
  }
  return 0;
}

void cycle_clock(struct simulator *s) {
  sim_seti(s, SIG_CLK, 1);
  sim_wait_time(s, 5000, 1);
  sim_seti(s, SIG_CLK, 0);
  sim_wait_time(s, 5000, 1);
}

int send_instr(struct simulator *s, uint16_t inst) {
  int num_cycles = 0;
  printf("Send instr %04X ", inst);
  print_instr(inst);
  printf(" at %" PICO "\n", s->picos / 1000);
  sim_seti(s, SIG_CLK, 0);
  sim_seti(s, SIG_IF_DR, inst);

  do  {
    printf("Microcode step %d\n", num_cycles);
    num_cycles++;
    cycle_clock(s);
  } while (sim_geti(s, SIG_DISPATCH) == 0);
  return num_cycles;
}

int main(int argc, char **argv) {
  struct simulator sim;
  struct simulator *s = &sim;
  int err = 0;
  int i;
  int instr;
  char *str;

  sim_cfg_parse(&cfg, argc-1, argv+1);
  if (sim_new(s, &cfg)) {
    fprintf(stderr, "sim_new failed\n");
    return 1;
  }

  sim_seti(s, SIG_CLK, 0);
  sim_seti(s, SIG_RST, 1);
  sim_seti(s, SIG_SLOT, 1);
  sim_seti(s, SIG_IF_DR, 0);
  sim_seti(s, SIG_IF_STALL, 0);

  sim_seti(s, SIG_MAC_BUSY, 0);
  sim_seti(s, SIG_T_BCC, 0);
  sim_seti(s, SIG_EVENT_REQ, 7);
  sim_seti(s, SIG_EVENT_INFO, 0);
  sim_seti(s, SIG_IBIT, 0);

  // drop RST
  sim_wait_time(s, 20000, 1);
  sim_seti(s, SIG_RST, 0);
  sim_wait_time(s, 10000, 1);
  sim_seti(s, SIG_INSTR_SEQ, 0);
  sim_wait_time(s, 5000, 1);

  struct sig_hook h1;
  sim_hook_init(s, &h1, change_fn);
  for (i = 0; i < NUM_SIGNALS; i++)
    bv_set(&h1.signals, i, 1);
  sim_hook_add(s, &h1);

  //  send_instr(s, 0x8);

  for (i = 1; i < argc; i++) {
    instr = strtol(argv[i], &str, 16);
    if (*str == 0 && instr < 0x10000) {
      send_instr(s, instr);
    } else {
      printf("invalid %s\n", argv[i]);
    }
  }

  // exit_sim:
  sim_free(s);
  return err;
}

