#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/mman.h>
#include <arpa/inet.h>
#include <getopt.h>
#include <termios.h>
#include <sys/select.h>
#include <errno.h>
#include <limits.h>

#include "simulator.h"
#include "mem_bus.h"
#include "utlist.h"

#include "sh2instr.h"
#include "uart.h"
#include "uartlite.h"
#include "delays.h"
#include "debug.h"
#include "debug_plan.h"
#include "debug_skt.h"

#include "cpu_signals.h"
#include "sim_macros.h"

#include "tests/sim_instr.h"

struct cpu_cfg {
  int uart_pty;
  char *delay_file;
  int log_opcodes;
  int debug_skt;
  uint16_t debug_skt_port;
};

struct cpu_cfg cpu_cfg = {
  /* default everything to 0 */
};

struct sim_cfg cfg = {
  .name = "cpu_tb",
  .signals = signals,
  .num_signals = SIG_NUM_SIGNALS,
  .on_exit = SIMEXIT_EXIT,
  .type = SIM_TYPE_GHDL,
  .cfg = {
    .ghdl = {
      .ieee = "synopsys",
      .explicit = 1,
    },
  },
};

struct delay_set delays;
uint32_t success_instruction_address = 0xFFFFFFFF;
uint32_t fail_instruction_address = 0xFFFFFFFF;

const char *img_file_name = "ram.img";

/* A mem_bus connecting to the memory bus VHDL signals */
struct mem_bus sram_bus = {
  .name = "SRAM",
  .delays = {
    .ack_read = 12,
    .drop_ack = 4,
  },
  .sig = {
    .en = SIG_db_sram_o_en,
    .a = SIG_db_sram_o_a,
    .din = SIG_db_sram_o_d,
    .wr = SIG_db_sram_o_wr,
    .rd = SIG_db_sram_o_rd,
    .we = SIG_db_sram_o_we,
    .dout = SIG_db_sram_i_d,
    .ack = SIG_db_sram_i_ack,
  },
};

struct mem_bus ddr_bus = {
  .name = "DDR",
  .delays = {
    .ack_read = 12,
    .drop_ack = 4,
  },
  .sig = {
    .en = SIG_db_ddr_o_en,
    .a = SIG_db_ddr_o_a,
    .din = SIG_db_ddr_o_d,
    .wr = SIG_db_ddr_o_wr,
    .rd = SIG_db_ddr_o_rd,
    .we = SIG_db_ddr_o_we,
    .dout = SIG_db_ddr_i_d,
    .ack = SIG_db_ddr_i_ack,
  },
};

struct mem_bus pio_bus = {
  .name = "PIO",
  .delays = {
    .ack_read = 12,
    .drop_ack = 4,
  },
  .sig = {
    .en = SIG_db_pio_o_en,
    .a = SIG_db_pio_o_a,
    .din = SIG_db_pio_o_d,
    .wr = SIG_db_pio_o_wr,
    .rd = SIG_db_pio_o_rd,
    .we = SIG_db_pio_o_we,
    .dout = SIG_db_pio_i_d,
    .ack = SIG_db_pio_i_ack,
  },
};

struct mem_bus uart0_bus = {
  .name = "UART0",
  .delays = {
    .ack_read = 12,
    .drop_ack = 4,
  },
  .sig = {
    .en = SIG_db_uart0_o_en,
    .a = SIG_db_uart0_o_a,
    .din = SIG_db_uart0_o_d,
    .wr = SIG_db_uart0_o_wr,
    .rd = SIG_db_uart0_o_rd,
    .we = SIG_db_uart0_o_we,
    .dout = SIG_db_uart0_i_d,
    .ack = SIG_db_uart0_i_ack,
  },
};

/* A mem_bus connecting to the instruction fetch VHDL signals */
struct mem_bus if_bus = {
  .name = "IF",
  .delays = {
    .ack_read = 4,
    .ack_write = 32,
    .drop_ack = 6,
  },
  .sig = {
    .en = SIG_inst_o_en,
    .a = SIG_inst_o_a,
    .dout = SIG_inst_i_d,
    .ack = SIG_inst_i_ack,
    .we = SIG_inst_o_we,
  },
};

struct mem_map {
  uint8_t *ptr;
  struct mem_range range;
};

int mmap_read(struct mem_bus *bus, struct mem_range *range, uint32_t addr,
              int num_bytes, uint32_t *val,
              uint32_t *ack_delay, uint32_t *drop_ack_delay) {
  struct mem_map *mem_map = container_of(range, struct mem_map, range);
  uint32_t offset = addr - mem_map->range.start;
  *val = 0;
  while (num_bytes--) {
    *val = (*val << 8) | mem_map->ptr[offset++];
  }
  delays_lookup(&delays, addr, 1, ack_delay, drop_ack_delay);
  return 0;
}

int mmap_write(struct mem_bus *bus, struct mem_range *range, uint32_t addr,
               int num_bytes, uint32_t val,
               uint32_t *ack_delay, uint32_t *drop_ack_delay) {
  struct mem_map *mem_map = container_of(range, struct mem_map, range);
  uint32_t offset = addr - range->start;
  //printf("WRITE MEM[%08x] = 0x%x\n", addr, val);
  while (num_bytes--) {
    mem_map->ptr[offset + num_bytes] = (uint8_t) val;
    val >>= 8;
  }
  delays_lookup(&delays, addr, 0, ack_delay, drop_ack_delay);
  return 0;
}

int mmap_instr_read(struct mem_bus *bus, struct mem_range *range, uint32_t addr,
                    int num_bytes, uint32_t *val,
                    uint32_t *ack_delay, uint32_t *drop_ack_delay) {
  /* detect fail loop addresses */
  /*switch (addr) {
  case 0x18c8:
  case 0x1588:
    printf("Reached fail loop at addr 0x%X\n", addr);
    //sim_free(bus->sim);
    exit(0);
    break;
    }*/
  int r = mmap_read(bus, range, addr, num_bytes, val, ack_delay, drop_ack_delay);
  if (r == 0) {
    if (cpu_cfg.log_opcodes) {
      char buf[256];
      op_name(buf, sizeof(buf), *val);
      printf("IF 0x%06X 0x%04X %s\n", addr, *val, buf);
    }
  }
  delays_lookup(&delays, addr, 1, ack_delay, drop_ack_delay);
  return r;
}

int trigger_event(struct mem_bus *bus, struct mem_range *range, uint32_t addr,
                  int num_bytes, uint32_t val,
                  uint32_t *ack_delay, uint32_t *drop_ack_delay) {
  uint32_t req, info;
  if (num_bytes == 4) {
    /* event_req is bits 14-12 */
    req = (val >> 12) & 0x7;
    /* event info is bits 11-0 */
    info = val & 0xFFF;
    printf("Trigger event request=0x%X info=0x%03X\n", req, info);
    sim_seti(bus->sim, SIG_event_req_i, req);
    sim_seti(bus->sim, SIG_event_info_i, info);
  }
  return 0;
}

int test_result(struct mem_bus *bus, struct mem_range *range, uint32_t addr,
                int num_bytes, uint32_t val,
                uint32_t *ack_delay, uint32_t *drop_ack_delay) {
  if (num_bytes == 4) {
    if (val) {
      SIMERR(bus->sim, "Test failed. Result=%u\n", val);
      //    sim_free(bus->sim);
      //    exit(1);
    } else {
      printf("Test Passed\n");
      sim_free(bus->sim);
      exit(0);
    }
  }
  return 0;
}

struct mem_map mem_bus_map = {
  .range = {
    .start = 0x0,
    .end = 0x0,
    .read_fn = mmap_read,
    .write_fn = mmap_write,
  },
};

struct mem_map if_bus_map = {
  .range = {
    .start = 0x0,
    .end = 0x0,
    .read_fn = mmap_instr_read,
  },
};

struct mem_map mem_bus_stack_map = {
  .range = {
    .start = 0,
    .end = 0x1000000,
    .read_fn = mmap_read,
    .write_fn = mmap_write,
  },
};

struct mem_map if_bus_stack_map = {
  .range = {
    .start = 0,
    .end = 0x1000000,
    .read_fn = mmap_instr_read,
  },
};

int dump_stack(struct mem_bus *bus, struct mem_range *range, uint32_t addr,
               int num_bytes, uint32_t val,
               uint32_t *ack_delay, uint32_t *drop_ack_delay) {
  uint32_t mem_val;
  uint32_t _ack_delay, _drop_ack_delay;
  int num_items = (0x100000 - val) / 4;
  if (num_bytes == 4) {
    printf("Dump stack from 0x%x with %u elements\n", val, num_items > 0 ? num_items : 0);
    while (val < 0x100000) {
      mmap_read(bus, &mem_bus_stack_map.range, val,
                4, &mem_val,
                &_ack_delay, &_drop_ack_delay);
      printf("  %08x: %08x\n", val, mem_val);
      val += 4;
    }
  }
  return 0;
}

int evt_ack_cb(struct simulator *sim, struct sig_hook *hook) {
  uint32_t ack = sim_geti(sim, SIG_event_ack_o);
  if (ack == 0) {
    if (sim_geti(sim, SIG_event_req_i) != 7) {
      /* set no event */
      sim_seti(sim, SIG_event_req_i, 7);
      sim_seti(sim, SIG_event_info_i, 0);
    } else {
      SIMERR(sim, "event_ack went low but event_req = 111. This should happen only once because of RESET\n");
    }
  }
  return 0;
}

struct sig_hook evt_ack_hook;

struct mem_map evt_trigger_map = {
  .range = {
    .start = EVENT_TRIGGER_ADDRESS,
    .end = EVENT_TRIGGER_ADDRESS + 4,
    .read_fn = 0,
    .write_fn = trigger_event,
  },
};

struct mem_map test_result_map = {
  .range = {
    .start = TEST_RESULT_ADDRESS,
    .end = TEST_RESULT_ADDRESS + 4,
    .read_fn = 0,
    .write_fn = test_result,
  },
};

struct mem_map dump_stack_map = {
  .range = {
    .start = DUMP_STACK_ADDRESS,
    .end = DUMP_STACK_ADDRESS + 4,
    .read_fn = 0,
    .write_fn = dump_stack,
  },
};

int pio_write(struct mem_bus *bus, struct mem_range *range, uint32_t addr,
              int num_bytes, uint32_t val,
              uint32_t *ack_delay, uint32_t *drop_ack_delay) {
  printf("LED: WRITE 0x%02hhX at %" PICO " ns\n", (uint8_t)val, NANOS(bus->sim->picos));
  delays_lookup(&delays, addr, 0, ack_delay, drop_ack_delay);
  return 0;
}

#define PIO_ADDR 0xABCD0000

struct mem_range pio_range = {
  .start = PIO_ADDR,
  .end = PIO_ADDR + 1,
  .write_fn = pio_write,
};


int ddr_read(struct mem_bus *bus, struct mem_range *range, uint32_t addr,
              int num_bytes, uint32_t *val,
              uint32_t *ack_delay, uint32_t *drop_ack_delay) {
  printf("ddr read addr=0x%X\n", addr);
  *val = 0;
  delays_lookup(&delays, addr, 1, ack_delay, drop_ack_delay);
  return 0;
}

int ddr_write(struct mem_bus *bus, struct mem_range *range, uint32_t addr,
               int num_bytes, uint32_t val,
               uint32_t *ack_delay, uint32_t *drop_ack_delay) {
  printf("ddr write addr=0x%X\n", addr);
  delays_lookup(&delays, addr, 0, ack_delay, drop_ack_delay);
  return 0;
}

struct mem_range ddr_range = {
  .start = 0x10000000,
  .end = 0x14000000,
  .read_fn = ddr_read,
  .write_fn = ddr_write,
};

enum dbg_state {
  DBG_STATE_RUN,
  DBG_STATE_STEP,
  DBG_STATE_PAUSE,
  DBG_STATE_DUMP,
  DBG_STATE_STORE,
  DBG_STATE_READ_MEM,
  DBG_STATE_WRITE_MEM
};

struct debug_state {
  enum dbg_state state;
  uint32_t addr;
  uint32_t *buf;
  int num_words;

  struct debug_plan plan;
};

static struct debug_state debug = {
  .state = DBG_STATE_RUN
};

static const char *debug_msg = "Paused: Press 's' to step, 'c' to continue, 'd' to dump state, 'D' to store state, 'r' to read mem, 'w' to write mem\n";

struct cpu_state {
  uint32_t regs[REG_NUM_REGS];
};

struct cpu_state cpu_state;


static void debug_rdy(struct debug_hw *dbg, struct debug_command *cmd) {
  cmd->d_en = 0;
  int i;
  struct debug_reply rep;
  struct debug_request req;
  switch (debug.state) {
  case DBG_STATE_RUN:
    break;
  case DBG_STATE_STEP:
  case DBG_STATE_PAUSE:
    printf("%s", debug_msg);
    break;
  case DBG_STATE_DUMP:
  case DBG_STATE_STORE:
  case DBG_STATE_READ_MEM:
  case DBG_STATE_WRITE_MEM:
    rep.data = debug_read_data(dbg);
    if (debug_plan_next(&debug.plan, &rep, &req) == 0) {
      switch (debug.state) {
      case DBG_STATE_DUMP:
        for (i = 0; i < 10; i+=2) {
          printf("   R%d = 0x%08X", i, cpu_state.regs[i]);
          printf("   R%d = 0x%08X\n", i+1, cpu_state.regs[i+1]);
        }
        for (i = 10; i < 16; i+=2) {
          printf("  R%d = 0x%08X", i, cpu_state.regs[i]);
          printf("  R%d = 0x%08X\n", i+1, cpu_state.regs[i+1]);
        }
        printf("   SR = 0x%08X", cpu_state.regs[REG_SR]);
        printf(" MACH = 0x%08X\n", cpu_state.regs[REG_MACH]);
        printf("  GBR = 0x%08X", cpu_state.regs[REG_GBR]);
        printf(" MACL = 0x%08X\n", cpu_state.regs[REG_MACL]);
        printf("  VBR = 0x%08X", cpu_state.regs[REG_VBR]);
        printf("   PR = 0x%08X\n", cpu_state.regs[REG_PR]);
        printf("   PC = 0x%08X\n", cpu_state.regs[REG_PC]);
        printf("%s", debug_msg);
        break;
      default:
        break;
      }
      debug.state = DBG_STATE_PAUSE;
    }
    debug_add_request(dbg, cmd, &req);
    break;
  default:
    break;
  }  
}

static void debug_dump_state(struct debug_hw *dbg, struct debug_command *cmd) {
  printf("start dump\n");
  debug_plan_init_regs_read(&debug.plan, cpu_state.regs);
  debug.state = DBG_STATE_DUMP;
  debug_rdy(dbg, cmd);
}

static void debug_store_state(struct debug_hw *dbg, struct debug_command *cmd) {
  printf("start store\n");
  debug_plan_init_regs_write(&debug.plan, cpu_state.regs);
  debug.state = DBG_STATE_STORE;
  debug_rdy(dbg, cmd);
}

static void debug_read_mem(struct debug_hw* dbg, struct debug_command *cmd, uint32_t addr, int num_words) {
  printf("read %d words starting at 0x%08X\n", num_words, addr);
  if (num_words > 0) {
    debug.num_words = num_words;
    debug_plan_init_mem_read(&debug.plan, addr, num_words, NULL, cpu_state.regs[REG_R0]);
    debug.state = DBG_STATE_READ_MEM;
    debug_rdy(dbg, cmd);
  }
}

static void debug_write_mem(struct debug_hw* dbg, struct debug_command *cmd, uint32_t addr, uint32_t *vals, int num_words) {
  printf("write %d words to addr 0x%08X\n", num_words, addr);
  if (num_words > 0) {
    debug.buf = vals;
    debug.num_words = num_words;
    debug_plan_init_mem_write(&debug.plan, addr, num_words, vals, cpu_state.regs[REG_R0]);
    debug.state = DBG_STATE_WRITE_MEM;
    debug_rdy(dbg, cmd);
  }
}

struct debug_hw debug_hw = {
  .sig = {
    .en = SIG_debug_i_en,
    .cmd = SIG_debug_i_cmd,
    .ir = SIG_debug_i_ir,
    .din = SIG_debug_i_d,
    .din_en = SIG_debug_i_d_en,

    .ack = SIG_debug_o_ack,
    .dout = SIG_debug_o_d,
    .rdy = SIG_debug_o_rdy
  }
};

/* Callback to perdiodically raise an interrupt. Program needs to
   have CMD_ENABLE_EVENT_TRIGGER enabled so that evt_ack_cb is
   registered to lower interrupt when it's acked. */
int time_cb(struct simulator *sim, struct sig_hook *hook) {
  /*printf("TIME CB\n");
  hook->time += 1000000;
  sim_seti(sim, SIG_event_req_i, 0);
  sim_seti(sim, SIG_event_info_i, 0x110);*/
  //debug_break(&debug_hw);
  return 0;
}
struct sig_hook time_hook;

static int parse_arg_int(char *str, int *val) {
  char *end_ptr;
  long v;
  errno = 0;
  v = strtol(str, &end_ptr, 10);
  if ((errno == ERANGE && (v == LONG_MAX || v == LONG_MIN)) || (errno != 0 && v == 0)) {
    return -1;
  }
  if (end_ptr == str) {
    /* no number characters */
    return -1;
  }
  if (*end_ptr != '\0') {
    /* non-number suffix */
    return -1;
  }
  *val = v;
  return 0;
}

static int parse_args(struct sim_cfg *cfg, struct cpu_cfg *cpu_cfg,
                      int argc, char **argv) {
  int c;
  int val;
  while (1) {
    int option_index = 0;
    static struct option long_options[] = {
      {"ieee",         required_argument, 0, 0 },
      {"ieee-asserts", required_argument, 0, 1 },
      {"work",         required_argument, 0, 2 },
      {"std",          required_argument, 0, 3 },
      {"vcd",          required_argument, 0, 4 },
      {"wave",         required_argument, 0, 5 },
      {"stop-time",    required_argument, 0, 6 },
      {"fexplicit",    no_argument, 0, 7 },
      {"log-ops",      no_argument, 0, 8 },
      {"skt",          no_argument, 0, 9 },
      {"skt-port",     required_argument, 0, 10 },
      {"uartpty",      no_argument, 0, 'u' },
      {"delays",       required_argument, 0, 'd' },
      {"img",          required_argument, 0, 'i' },
      {0, 0, 0, 0 }
    };
    c = getopt_long(argc, argv, "ud:i:",
                    long_options, &option_index);
    if (c == -1)
      break;
    switch (c) {
    case 0:
      cfg->cfg.ghdl.ieee = optarg;
      break;
    case 1:
      cfg->cfg.ghdl.ieee_asserts = optarg;
      break;
    case 2:
      cfg->cfg.ghdl.work_name = optarg;
      break;
    case 3:
      cfg->cfg.ghdl.std = optarg;
      break;
    case 4:
      cfg->cfg.ghdl.vcd = optarg;
      break;
    case 5:
      cfg->cfg.ghdl.wave = optarg;
      break;
    case 6:
      cfg->stop_time = optarg;
      break;
    case 7:
      cfg->cfg.ghdl.explicit = 1;
      break;
    case 8:
      cpu_cfg->log_opcodes = 1;
      break;
    case 9:
      cpu_cfg->debug_skt = 1;
      break;
    case 10:
      if (parse_arg_int(optarg, &val) || val < 0 || val > 65535) {
        fprintf(stderr, "Invalid tcp port %s\n", optarg);
        return -1;
      }
      cpu_cfg->debug_skt = 1;
      cpu_cfg->debug_skt_port = (uint16_t) val;
      break;
    case 'u':
      cpu_cfg->uart_pty = 1;
      break;
    case 'd':
      cpu_cfg->delay_file = optarg;
      break;
    case 'i':
      img_file_name = optarg;
      break;
    case '?':
      printf("Usage: %s\n"
             ""
             , argv[0]);
      return -1;
    }
  }
  return optind;
}

/* Some executables contain additional information to configure the
   cpu simulator. Parse that information, which is located immediately
   after the vector table at the start of the executable. */
int parse_sim_instructions(uint8_t *buf, size_t len) {
  uint32_t i;
  uint32_t *ibuf = ((uint32_t *)buf) + 64;
  uint32_t v;
  int enabled_evt_trigger = 0;
  int enabled_test_result = 0;
  int enabled_dump_stack = 0;

  if (len < 64 * 4 + 8)
    goto out;
  v = ntohl(*ibuf++);
  if (v != SIM_INSTR_MAGIC)
    goto out;

  uint32_t num_vals = ntohl(*ibuf++) / 4 - 66;
  printf("MAGIC NUMBER? %x end_vals=%u\n", v, num_vals);
  if (len < 64 * 4 + 8 + num_vals * 4)
    goto out;

  if (num_vals < 2)
    goto out;

  success_instruction_address = ntohl(*ibuf++);
  fail_instruction_address = ntohl(*ibuf++);
  num_vals -= 2;

  for (i = 0; i < num_vals; i++) {
    v = ntohl(ibuf[i]);
    printf("%u: %x\n", i, v);
    switch (v) {
    case CMD_BAD_INSTR:
      i++;
      if (i == num_vals)
        goto out;
      v = ntohl(ibuf[i]);
      printf("Make instruction at 0x%X illegal\n", v); 
      break;
    case CMD_ENABLE_EVENT_TRIGGER:
      if (!enabled_evt_trigger) {
        enabled_evt_trigger = 1;
        mem_bus_range_add(&sram_bus, &evt_trigger_map.range);
        sim_hook_init(sram_bus.sim, &evt_ack_hook, evt_ack_cb);
        bv_set(&evt_ack_hook.signals, SIG_event_ack_o, 1);
        sim_hook_add(sram_bus.sim, &evt_ack_hook);
      }
      break;
    case CMD_ENABLE_TEST_RESULT:
      if (!enabled_test_result) {
        enabled_test_result = 1;
        mem_bus_range_add(&sram_bus, &test_result_map.range);
      }
      break;
    case CMD_ENABLE_DUMP_STACK:
      if (!enabled_dump_stack) {
        enabled_dump_stack = 1;
        mem_bus_range_add(&sram_bus, &dump_stack_map.range);
      }
      break;
    }
  }
  out:
    return -1;
}

static struct termios saved_attribs;

void reset_term_attribs(void) {
  tcsetattr(STDIN_FILENO, TCSANOW, &saved_attribs);
}

int setup_debug_term(int fd) {
  struct termios tattr;
  if (isatty(fd) == 0) {
    fprintf (stderr, "Not a terminal.\n");
    return -1;
  }
  if (tcgetattr(fd, &saved_attribs) == -1) {
    return -1;
  }

  /* shell should reset terminal, but just in case do it ourselves */
  atexit(reset_term_attribs);

  /* enable reading a single character at a time with no echo */
  memcpy(&tattr, &saved_attribs, sizeof(tattr));
  tcgetattr(fd, &tattr);
  tattr.c_lflag &= ~(ICANON|ECHO);
  tattr.c_cc[VMIN] = 1;
  tattr.c_cc[VTIME] = 0;
  return tcsetattr(fd, TCSANOW, &tattr);
}

int read_char(char *c, int block) {
  fd_set rfds;
  int r;
  struct timeval timeout;
  timeout.tv_sec = 0;
  timeout.tv_usec = 0;
  FD_ZERO(&rfds);
  FD_SET(STDIN_FILENO, &rfds);
  r = select(1, &rfds, NULL, NULL, block ? NULL : &timeout);
  if (r == 1) {
    r = read(STDIN_FILENO, c, 1);
  }
  return r;
}

struct debug_skt_cmd {
  struct debug_skt *skt;
  struct debug_command cmd;
};

static void debug_skt_cmd_done(struct debug_hw *debug, struct debug_command *cmd) {
  struct debug_skt_cmd *dsc = container_of(cmd, struct debug_skt_cmd, cmd);
  struct debug_skt *skt = dsc->skt;
  struct debug_reply reply;
  /* pass reply to the skt */
  reply.data = debug_read_data(debug);
  //printf("debug skt command done %p %08x\n", skt, reply.data);
  debug_skt_reply(skt, &reply);
}

static void process_debug_skt(struct debug_skt *skt, struct debug_hw *debug, struct debug_command *cmd) {
  struct debug_request request;
  int do_notify = 0;
  if (cmd->prev == 0) {
    /* no active command,  */
    if (debug_skt_handle(skt, &request, &do_notify) == 0) {
      /*printf("Got request cmd=%d ir=%hu data=%u d_en=%d\n",
             request.cmd, request.instr, request.data, request.data_en);*/
      debug_add_request(debug, cmd, &request);
    }
    if (do_notify && debug->is_paused) {
      debug_skt_notify_paused(skt, 0);
    }
  }
}

#define UARTLITE

#ifdef UARTLITE
#define UART_STRUCT uartlite
#define UART_INIT uartlite_init
#define UART_PTY_INIT uartlite_pty_init
#define UART_FREE uartlite_free
#else
#define UART_STRUCT uart
#define UART_INIT uart_init
#define UART_PTY_INIT uart_pty_init
#define UART_FREE uart_free
#endif

int main(int argc, char **argv) {
  struct simulator sim;
  struct simulator *s = &sim;
  int fd, i;
  struct stat sb;
  int err = 0;
  uint8_t *mmap_ptr1, *mmap_ptr2;
  struct UART_STRUCT uart;
  int debug_enabled = 0;
  struct debug_skt debug_skt;
  int arg = parse_args(&cfg, &cpu_cfg, argc, argv);
  if (arg < 0)
    return 1;

  if (sim_new(s, &cfg)) {
    fprintf(stderr, "sim_new failed\n");
    return 1;
  }
  if (delays_init_cfg(&delays, cpu_cfg.delay_file)) {
    fprintf(stderr, "failed to load delays\n");
    goto exit_sim;
  }
  if (cpu_cfg.debug_skt) {
    if (debug_skt_init(&debug_skt, &cpu_cfg.debug_skt_port)) {
      fprintf(stderr, "failed to open listening socket on port %" PRIu16 "\n", cpu_cfg.debug_skt_port);
      goto exit_sim;
    }
    printf("Debug socket listening on port %hu\n", cpu_cfg.debug_skt_port);
  }
  instr_fetch_init(&if_bus, s);
  mem_bus_init(&sram_bus, s);
  mem_bus_init(&ddr_bus, s);
  mem_bus_init(&pio_bus, s);
  mem_bus_init(&uart0_bus, s);
  /* mmap a block of memory that is exposed to the simulator. Could
     instead mmap a file. Or mmap the file in a different range. */
  fd = open(img_file_name, O_RDONLY);
  if (fd == -1) {
    perror("Failed to open img file");
    err = 1;
    goto exit_sim;
  }
  if (fstat(fd, &sb) == -1) {
    perror("Failed stat img file");
    err = 1;
    goto exit_sim;
  }
  mmap_ptr1 = mmap(NULL, sb.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
  if (mmap_ptr1 == MAP_FAILED) {
    perror("mmap");
    err = 1;
    goto exit_sim;
  }
  // 32 MB
  mmap_ptr2 = mmap(NULL, 0x1000000, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
  if (mmap_ptr2 == MAP_FAILED) {
    perror("mmap");
    err = 1;
    goto exit_sim;
  }
  parse_sim_instructions(mmap_ptr1, sb.st_size);
  mem_bus_map.ptr = mmap_ptr1;
  mem_bus_map.range.end = sb.st_size;
  if_bus_map.ptr = mmap_ptr1;
  if_bus_map.range.end = sb.st_size;
  mem_bus_stack_map.ptr = mmap_ptr2;
  mem_bus_stack_map.range.start = sb.st_size;
  if_bus_stack_map.ptr = mmap_ptr2;
  if_bus_stack_map.range.start = sb.st_size;
  printf("mapped file %s to memory [0x%08X-0x%08X)\n",
         img_file_name, mem_bus_map.range.start, mem_bus_map.range.end);
  mem_bus_range_add(&sram_bus, &mem_bus_map.range);
  mem_bus_range_add(&if_bus, &if_bus_map.range);

  mem_bus_range_add(&sram_bus, &mem_bus_stack_map.range);
  mem_bus_range_add(&if_bus, &if_bus_stack_map.range);

  mem_bus_range_add(&pio_bus, &pio_range);

  if (cpu_cfg.uart_pty) {
    char tty_name[200];
    if (UART_PTY_INIT(&uart, &uart0_bus, 0xABCD0100,
                      tty_name, 200, 1)) {
      fprintf(stderr, "uart failed to init\n");
      goto exit_sim;
    }

    printf("UART tty file: %s\n", tty_name);
  } else {
    if (UART_INIT(&uart, &uart0_bus, 0xABCD0100, -1, STDOUT_FILENO, "UART: ", 0)) {
      fprintf(stderr, "uart failed to init\n");
      goto exit_sim;
    }
  }
  mem_bus_range_add(&ddr_bus, &ddr_range);

  debug_hw_init(&debug_hw, s, CLK_PERIOD);
  if (cpu_cfg.debug_skt == 0 && setup_debug_term(STDIN_FILENO) == 0) {
    printf("Debug enabled. Press 'b' to break execution.\n");
    debug_enabled = 1;
  }

  /*sim_hook_init(s, &time_hook, time_cb);
  time_hook.time = 200000;
  sim_hook_add(s, &time_hook);*/

  /* initialize debug register values to known, unique values */
  for (i = 0; i < REG_NUM_REGS; i++) {
    cpu_state.regs[i] = i;
  }
  struct debug_command user_cmd;
  memset(&user_cmd, 0, sizeof(user_cmd));
  user_cmd.cmd = DBG_CMD_BREAK;
  user_cmd.on_done = debug_rdy;

  struct debug_skt_cmd skt_cmd;
  memset(&skt_cmd, 0, sizeof(skt_cmd));
  skt_cmd.skt = &debug_skt;
  skt_cmd.cmd.on_done = debug_skt_cmd_done;

  char c;
  uint32_t vals[6];
  vals[0] = 0x42;
  vals[1] = 0x1234;
  vals[2] = 0xABCD;
  vals[3] = 0xEF000;
  vals[4] = 0x88000;
  vals[5] = 0x22020;
  while (1) {
    /* only process debug commands on when CLK_PERIOD evenly divides
       s->picos to ensure we set the debug signals at clock edges */
    if ((s->picos % (CLK_PERIOD * 1000)) == 0) {
      if (cpu_cfg.debug_skt) {
        process_debug_skt(&debug_skt, &debug_hw, &skt_cmd.cmd);
      } else if (debug_enabled) {
        while (read_char(&c, 0 /*debug_hw.is_paused*/) == 1) {
          if (user_cmd.prev) {
            // cmd already queued
            continue;
          }

          if (c == 'b' && debug.state == DBG_STATE_RUN) {
            debug_break(&debug_hw, &user_cmd);
            debug.state = DBG_STATE_PAUSE;
          } else if (c == 'B' && debug.state == DBG_STATE_RUN) {
            trigger_event(&if_bus, &if_bus_map.range, 0, 4, 0x4000, (void *)0, (void *)0);
            printf("Trigger break event\n");
          } else if (c == 'R' && debug.state == DBG_STATE_RUN) {
            trigger_event(&if_bus, &if_bus_map.range, 0, 4, 0x5002, (void *)0, (void *)0);
            printf("Trigger Reset event\n");
          } else if (c == 'I' && debug.state == DBG_STATE_RUN) {
            trigger_event(&if_bus, &if_bus_map.range, 0, 4, 0x0000, (void *)0, (void *)0);
            printf("Trigger IRQ event\n");
          } else if (c == 'N' && debug.state == DBG_STATE_RUN) {
            trigger_event(&if_bus, &if_bus_map.range, 0, 4, 0x1010, (void *)0, (void *)0);
            printf("Trigger NMI event\n");
          } else if (debug.state == DBG_STATE_PAUSE) {
            switch (c) {
            case 'c':
              debug_continue(&debug_hw, &user_cmd);
              debug.state = DBG_STATE_RUN;
              break;
            case 's':
              debug_step(&debug_hw, &user_cmd);
              debug.state = DBG_STATE_STEP;
              break;
            case 'd':
              debug_dump_state(&debug_hw, &user_cmd);
              break;
            case 'D':
              debug_store_state(&debug_hw, &user_cmd);
              break;
            case 'r':
              debug_read_mem(&debug_hw, &user_cmd, 0x0, 10);
              break;
            case 'w':
              debug_write_mem(&debug_hw, &user_cmd, 0x10000000, vals, 6);
              break;
            case 'i':
              debug_insert(&debug_hw, &user_cmd, 0x0009);
              break;
            }
          }
        }
      }
    }
    if (debug_hw.is_paused == 0) {
      sim_wait(s);
    } else {
      /* TODO: When paused, should we throttle the speed of the
         simulation more?  */
      sim_wait_time(s, CLK_PERIOD * 1000, 1);
    }
  }

  UART_FREE(&uart);
  if (munmap(mmap_ptr1, sb.st_size)) {
    perror("munmap");
  }
  if (munmap(mmap_ptr2, 0x1000000)) {
    perror("munmap");
  }
  close(fd);
 exit_sim:
  sim_free(s);
  return err;
}
