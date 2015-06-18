#include "mem_bus.h"

#include <string.h>

#include "utlist.h"

static struct mem_range *default_find_range(struct mem_bus *bus, uint32_t addr) {
  struct mem_range *r;
  DL_FOREACH(bus->ranges, r) {
    if (r->start <= addr && (r->end == 0 || addr < r->end)) {
      return r;
    }
  }
  return 0;
}

static void drop_ack(struct simulator *sim, struct mem_bus *bus) {
  //SIMLOG(sim, "drop_ack\n");
  if (sim_geti(sim, bus->sig.ack) == 1) {
    sim_seti(sim, bus->sig.ack, 0);
    sim_set_all(sim, bus->sig.dout, 'Z');
  }
}

static void schedule_ack_drop(struct simulator *sim, struct mem_bus *bus) {
  if (bus->ack_drop_delay) {
    if (bus->drop_ack_hook.time == 0) {
      //printf("will drop ack at %" PICO "\n", sim->picos + bus->ack_drop_delay * 1000);
      bus->ack_drop_op = bus->op_count;
      bus->drop_ack_hook.time = sim->picos + bus->ack_drop_delay * 1000;
      sim_hook_add(sim, &bus->drop_ack_hook);
    }
  } else {
    drop_ack(sim, bus);
  }
}

static int drop_ack_cb(struct simulator *sim, struct sig_hook *hook) {
  struct mem_bus *bus = container_of(hook, struct mem_bus, drop_ack_hook);
  if (bus->ack_drop_op == bus->op_count) {
    drop_ack(sim, bus);
  }
  return -1;
}

static int cancel_drop_cb(struct simulator *sim, struct sig_hook *hook) {
  //SIMLOG(sim, "cancel_drop_cb\n");
  struct mem_bus *bus = container_of(hook, struct mem_bus, cancel_drop_hook);
  if (bus->drop_ack_hook.time != 0) {
    //SIMLOG(sim, "actually cancel_drop_cb\n");
    sim_hook_remove(sim, &bus->drop_ack_hook);
    bus->drop_ack_hook.time = 0;
  }
  return -1;
}

static int input_changed(struct simulator *sim, struct mem_bus *bus) {
  if (sim->signals[bus->sig.en].event || sim->signals[bus->sig.a].event) {
    return 1;
  } else if (bus->sig.wr != -1) {
    return sim->signals[bus->sig.din].event || sim->signals[bus->sig.rd].event ||
      sim->signals[bus->sig.wr].event || sim->signals[bus->sig.we].event;
  }
  return 0;
}

static int read_write_cb(struct simulator *sim, struct sig_hook *hook) {
  struct mem_bus *bus = container_of(hook, struct mem_bus, rw_hook);

#define BUSLOG(fmt, args...) SIMLOG(sim, "%s: " fmt, bus->name, ##args)
#define BUSERR(fmt, args...) SIMERR(sim, "%s: " fmt, bus->name, ##args)

  uint32_t addr;
  uint32_t alignment;
  uint32_t data;
  uint32_t rd, wr, we, en;
  struct mem_range *range;
  int is_read = 1;
  int num_bytes;

  if (sim_geti_check(sim, bus->sig.en, &en)) {
    BUSERR("invalid en=%s\n", sim_get(sim, bus->sig.en));
    goto bus_exception;
  }
  if (bus->sig.wr != -1) {
    if (sim_geti_check(sim, bus->sig.wr, &wr) || sim_geti_check(sim, bus->sig.rd, &rd)) {
      BUSERR("invalid rd=%s or wr=%s\n", sim_get(sim, bus->sig.rd), sim_get(sim, bus->sig.wr));
      goto bus_exception;
    }
    if (wr) {
      is_read = 0;
    } else if (!rd && en) {
      BUSERR("Unknown bus operation. rd=wr=0\n");
      goto bus_exception;
    }
  }

  if (input_changed(sim, bus)) {
    //SIMLOG(sim, "read_write_cb EN: %s A: %s\n", sim_get(sim, bus->sig.en), sim_get(sim, bus->sig.a));
    if (en) {
      if (bus->ack_pending) {
        BUSERR("bus inputs changed mid-operation\n");
        bus->ack_pending = 0;
      } else if (sim_geti(sim, bus->sig.ack)) {
        //BUSLOG("Start new operation with ACK high, need to schedule ACK drop\n");
        schedule_ack_drop(sim, bus);
      }

      /* start a new operation */
      if (sim_geti_check(sim, bus->sig.a, &addr)) {
        BUSERR("invalid %s addr: %s\n", is_read ? "read" : "write", sim_get(sim, bus->sig.a));
        goto bus_exception;
      }
      if (sim_geti_check(sim, bus->sig.we, &we)) {
        BUSERR("invalid write_enable: %s\n", sim_get(sim, bus->sig.we));
        goto bus_exception;
      }
      range = bus->range_fn(bus, addr);
      if (is_read) {
        if (we != 0) {
          BUSERR("non_zero write_enable during read: %s\n", sim_get(sim, bus->sig.we));
          goto bus_exception;
        }
        if (!range || !range->read_fn) {
          BUSERR("invalid read addr: 0x%X in range: %p\n", addr, range);
          goto bus_exception;
        }
        /* use alignment of address to determine how many bytes to read */
        switch (addr & 0x3) {
        case 0:
          num_bytes = 4;
          break;
        case 1:
          num_bytes = 1;
          break;
        case 2:
          num_bytes = 2;
          break;
        case 3:
          num_bytes = 1;
          break;
        }
        bus->ack_delay = bus->delays.ack_read;
        bus->ack_drop_delay = bus->delays.drop_ack;
        if (range->read_fn(bus, range, addr, num_bytes, &bus->read_val, &bus->ack_delay, &bus->ack_drop_delay)) {
          BUSERR("read failed addr: 0x%X\n", addr);
          goto bus_exception;
        }
        BUSLOG("read addr: 0x%X val = 0x%X we: 0x%02X\n", addr, bus->read_val, we);

        /* the last byte read by a read_fn is always in the least
           significant byte. Shift the value to be where the CPU expects
           based on the address alignement. */
        switch (addr & 0x3) {
        case 1:
          bus->read_val = (bus->read_val & 0xFF) << 16;
          break;
        case 2:
          bus->read_val = bus->read_val & 0xFFFF;
          break;
        case 3:
          bus->read_val = bus->read_val & 0xFF;
          break;
        default:
          break;
        }
      } else {
        switch (we) {
        case 1:
          alignment = 3;
          break;
        case 2:
        case 3:
          alignment = 2;
          break;
        case 4:
          alignment = 1;
          break;
        case 8:
        case 0xC:
        case 0xF:
          alignment = 0;
          break;
        default:
          BUSERR("invalid write_enable: %s\n", sim_get(sim, bus->sig.we));
          goto bus_exception;
        }
        if ((addr & 0x3) != alignment) {
          BUSERR("misaligned memory write addr=0x%X we=0x%X\n", addr, we);
          goto bus_exception;
        }
        switch (we) {
        case 1:
        case 2:
        case 4:
        case 8:
          num_bytes = 1;
          break;
        case 3:
        case 0xC:
          num_bytes = 2;
          break;
        case 0xF:
          num_bytes = 4;
          break;
        }
        if (sim_geti_check(sim, bus->sig.din, &data)) {
          BUSERR("invalid write data: %s\n", sim_get(sim, bus->sig.din));
          goto bus_exception;
        }
        if (!range || !range->write_fn) {
          BUSERR("invalid write addr: 0x%X in range: %p data: 0x%X we: 0x%02X\n",
                 addr, range, data, we);
          goto bus_exception;
        }
        bus->ack_delay = bus->delays.ack_write;
        bus->ack_drop_delay = bus->delays.drop_ack;
        /* Zero out duplicated write bits */
        switch (num_bytes) {
        case 1:
          data &= 0xFF;
          break;
        case 2:
          data &= 0xFFFF;
          break;
        }
        if (range->write_fn(bus, range, addr, num_bytes, data, &bus->ack_delay, &bus->ack_drop_delay)) {
          BUSERR("write failed addr: 0x%X data: 0x%X we: 0x%02X\n", addr, data, we);
          goto bus_exception;
        }
        BUSLOG("write addr: 0x%X data: 0x%X we: 0x%02X\n", addr, data, we);
      }

      /* no data delay, set data immediately */
      if (bus->ack_delay) {
        bus->ack_pending = 1;
        hook->time = sim->picos + bus->ack_delay * 1000;
        //printf("schedule set ACK at %lu\n", hook->time);
      } else {
        /* no ack delay, set ack immediately */
        bus->ack_pending = 0;
        if (is_read)
          sim_seti(sim, bus->sig.dout, bus->read_val);
        bus->op_count++;
        sim_seti(sim, bus->sig.ack, 1);
        sim_immediate_hook_add(sim, &bus->cancel_drop_hook);
        hook->time = 0;
        //printf("ACK set at %lu\n", hook->time);
      }
    } else if (bus->ack_pending) {
      BUSERR("EN low during an operation\n");
      goto reset;
    } else if (sim_geti(sim, bus->sig.ack)) {
      //SIMLOG(sim, "EN low so schedule ack drop\n");
      schedule_ack_drop(sim, bus);
    }
  } else if (hook->time == sim->picos) {
    /* a timer expired */
    //printf("Timer expired\n");
    if (bus->ack_pending) {
      bus->ack_pending = 0;
      if (is_read)
        sim_seti(sim, bus->sig.dout, bus->read_val);
      bus->op_count++;
      sim_seti(sim, bus->sig.ack, 1);
      sim_immediate_hook_add(sim, &bus->cancel_drop_hook);
      //printf("set ACK, schedule cancel drop hook\n");
    }
  }

  return 0;
 bus_exception:
  /* TODO: assert bus exception event? */
  BUSERR("Bus exception\n");
 reset:
  bus->ack_pending = 0;
  sim_immediate_hook_add(sim, &bus->cancel_drop_hook);
  drop_ack(sim, bus);
  hook->time = 0;
  return 0;

#undef BUSLOG
#undef BUSERR
}

static void common_init(struct mem_bus *bus, struct simulator *sim) {
  bus->sim = sim;
  bus->op_count = 0;
  bus->ack_drop_op = 0;
  if (!bus->range_fn)
    bus->range_fn = default_find_range;
  sim_hook_init(sim, &bus->rw_hook, read_write_cb);
  sim_hook_init(sim, &bus->drop_ack_hook, drop_ack_cb);
  sim_hook_init(sim, &bus->cancel_drop_hook, cancel_drop_cb);
}

void mem_bus_init(struct mem_bus *bus, struct simulator *sim) {
  common_init(bus, sim);
  bv_set(&bus->rw_hook.signals, bus->sig.en, 1);
  bv_set(&bus->rw_hook.signals, bus->sig.a, 1);
  bv_set(&bus->rw_hook.signals, bus->sig.din, 1);
  bv_set(&bus->rw_hook.signals, bus->sig.rd, 1);
  bv_set(&bus->rw_hook.signals, bus->sig.wr, 1);
  bv_set(&bus->rw_hook.signals, bus->sig.we, 1);
  sim_hook_add(sim, &bus->rw_hook);
  sim_seti(sim, bus->sig.ack, 0);
  sim_set_all(sim, bus->sig.dout, 'Z');
}

void instr_fetch_init(struct mem_bus *bus, struct simulator *sim) {
  bus->sig.wr = -1;
  bus->sig.rd = -1;
  common_init(bus, sim);
  bv_set(&bus->rw_hook.signals, bus->sig.en, 1);
  bv_set(&bus->rw_hook.signals, bus->sig.a, 1);
  sim_hook_add(sim, &bus->rw_hook);
  sim_seti(sim, bus->sig.ack, 0);
  sim_set_all(sim, bus->sig.dout, 'Z');
}

void mem_bus_range_add(struct mem_bus *bus, struct mem_range *range) {
  DL_APPEND(bus->ranges, range);
}

void mem_bus_range_remove(struct mem_bus *bus, struct mem_range *range) {
  DL_DELETE(bus->ranges, range);
}
