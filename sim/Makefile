GHDL := ghdl
IVERILOG := iverilog
IVERILOG_VPI := iverilog-vpi
TOOLS_DIR := $(firstword $(wildcard ../../mcu_lib/tools) $(wildcard ../../../tools))

CONFIG_RING_BUS=0
CONFIG_PREFETCHER=0

GHDL_EXISTS := $(shell sh -c 'which $(GHDL) >/dev/null && echo true')
IVERILOG_EXISTS := $(shell sh -c 'which $(IVERILOG) >/dev/null && echo true')
CROSSCOMP_EXISTS := $(shell sh -c 'which sh2-elf-gcc >/dev/null && echo true')

UNAME := $(shell sh -c 'uname -s 2>/dev/null || echo no')
ifeq ($(UNAME),Linux)
	LDLIBS := -lutil
endif
ifeq ($(UNAME),Darwin)
	LDLIBS :=
endif

CFLAGS := -Wall -Werror -Isim -g -Wno-deprecated-declarations

VHDLS := CPU_VHDS
include ../build.mk
CPU_VHDS := $(addprefix ../,$(CPU_VHDS))

# including ring bus is optional
ifeq ($(CONFIG_RING_BUS),1)
RBUS_VHDS :=
VHDLS := RBUS_VHDS
include ../../ring_bus/build.mk
CPU_VHDS += $(addprefix ../../ring_bus/,$(RBUS_VHDS))
endif

#including instruction prefetcher is optional
ifeq ($(CONFIG_PREFETCHER),1)
PREFETCH_VHDS :=
VHDLS := PREFETCH_VHDS
include ../../prefetch/build.mk
CPU_VHDS += $(addprefix ../../prefetch/,$(PREFETCH_VHDS))
endif

VHDS := $(CPU_VHDS) bridge_pkg.vhd monitor_pkg.vhd bus_monitor.vhd timeout_cnt.vhd cpu_tb.vhh data_bus_pkg.vhd cpu_pure_tb.vhh
VHDS += mem/cpu_sram.vhd mem/asymmetric_ram.vhd
#UNISIM_VHDS := unisims/unisim_VCOMP.vhd unisims/unisim_VPKG.vhd $(wildcard unisims/primitive/*.vhd)

VHDL_TOPS := cpu_tb cpu_pure_tb
#VHDL_LIBS := unisim work
VHDL_LIBS := work

SRCS := cpu_ctb.c sh2instr.c delays.c debug.c debug_plan.c debug_skt.c recv_buf.c sim/uart.c sim/bitvec.c sim/mem_bus.c sim/packet_gen.c sim/simulator.c

all: pinst

ifeq ($(GHDL_EXISTS),true)
all: $(VHDL_TOPS) cpu_ctb cpu_tb.vhh
include $(TOOLS_DIR)/ghdl.mk
# tell simulator.c if mcode backend is being used
ifeq ($(GHDL_BACKEND_MCODE),y)
CFLAGS += -DGHDL_MCODE_BACKEND
endif
endif
ifeq ($(IVERILOG_EXISTS),true)
VPI_CFLAGS := $(shell $(IVERILOG_VPI) --cflags)
all: sim/vpibridge.vpi decodeop
endif
ifeq ($(CROSSCOMP_EXISTS),true)
all: ram.img
endif

#unisim-obj93.cf: $(UNISIM_VHDS)
work-obj93.cf: $(VHDS)

decodeop.o: decode_signals.h
decodeop: decodeop.o sim/simulator.o sim/bitvec.o sh2instr.o

pinst: pinst.o sh2instr.o
pinst.o: pinst.c sh2instr.h

sim/vpibridge.vpi: CFLAGS := $(CFLAGS) $(VPI_CFLAGS)
sim/vpibridge.vpi: sim/vpibridge.o

cpu_ctb: cpu_ctb.o sim/simulator.o sim/bitvec.o sim/mem_bus.o sim/uart.o sim/uartlite.o sh2instr.o delays.o debug.o debug_plan.o debug_skt.o recv_buf.o
cpu_pure_tb: ram.img

# compiles and creates dependency files
%.o : %.c
	$(COMPILE.c) -MD -o $@ $<
	@cp $*.d $*.P; \
	  sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
	  -e '/^$$/ d' -e 's/$$/ :/' < $*.d >> $*.P; \
	  rm -f $*.d

-include $(SRCS:.c=.P)

# Add dependencies to particular vhh files. TODO: how to avoid
# repeating this rule?
cpu_tb.vhh: cpu_tb.vhd cpu_signals.h sim/sim_macros.h
	LD_LIBRARY_PATH='' gcc -x c-header -E -P -w -nostdinc -DVHDL -DCONFIG_PREFETCHER=$(CONFIG_PREFETCHER) -DCONFIG_RING_BUS=$(CONFIG_RING_BUS) -I. -I../config -Isim $< -o $@

%.vpi:
	$(IVERILOG_VPI) --name=$* $^

%.vvp:
	$(IVERILOG) $(IVFLAGS) -o$@ $^

ifeq ($(CROSSCOMP_EXISTS),true)
ram.img: ../testrom/main.elf
	sh2-elf-objcopy -v -S -O binary --srec-forceS3 $^ $@

../testrom/main.elf:
	make -C ../testrom main.elf
endif

PURE_SIM_VHDS := $(filter-out cpu_tb.vhh,$(VHDS))
PURE_SIM_VHDS := $(filter-out bridge_pkg.vhd,$(PURE_SIM_VHDS))
PURE_SIM_VHDS := $(sort $(PURE_SIM_VHDS))

cpu_sim_pkg: $(PURE_SIM_VHDS) ram.img
	rm -rf $@
	mkdir -p $@/vhd
	cp $(PURE_SIM_VHDS) $@/vhd
	(cd $@/vhd; rename .vhh .vhd *.vhh)
	mkdir -p $@/tools
	cp $(TOOLS_DIR)/*.mk $@/tools
	cp ram.img $@/
	cp Makefile_cpu_sim_pkg $@/Makefile

clean:
	rm -f *.o *.P *.vpi *.vvp sim/*.o sim/*.P sim/*.vpi sim/*.vvp *.vhh
	rm -f *.cf $(VHDL_TOPS) cpu_ctb
	rm -f decodeop
#	rm -f ram.img

.PHONY: clean cpu_sim_pkg
