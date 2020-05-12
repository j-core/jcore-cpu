VHDS :=
VHDS += ../cpu/cpu2j0_pkg.vhd
VHDS += ../../targets/data_bus_pkg.vhd

VHDLS := VHDS
include build.mk

VHDS += dcache_tb.vhd
# VHDS += cache_pkg_tb.vhd

VHDL_TESTS :=

VHDL_TOPS := dcache_tb
VHDL_TOPS += $(VHDL_TESTS)

all: $(VHDL_TOPS)

work-obj93.cf: $(VHDS)

dcache_tb.ghw: $(VHDL_TOPS)
	./dcache_tb --wave=$@ --stop-time=4000ns

%.vhm: %.vhmh
	LD_LIBRARY_PATH='' gcc -x c-header -E -P -w -nostdinc -I. $< -o $@

TOOLS_DIR := $(firstword $(wildcard ../mcu_lib/tools) $(wildcard ../../tools))
include $(TOOLS_DIR)/ghdl.mk

clean:
	rm -f *.cf *.o $(VHDL_TOPS) *_tap *.ghw
	rm -f dcache.vhd

.PHONY: all clean
