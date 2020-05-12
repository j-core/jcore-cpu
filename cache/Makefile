TOOLS_DIR := ../../tools
include $(TOOLS_DIR)/mk_utils.mk

VHDS :=
VHDS += ../cpu/cpu2j0_pkg.vhd
VHDS += ../../targets/data_bus_pkg.vhd
# VHDS += ../../lib/reg_file_struct/bist_pkg.vhd

VHDS += dcache_tb.vhd
# VHDS += cache_pkg_tb.vhd

VHDS += $(call include_vhdl,../../lib/reg_file_struct)
VHDS += $(call include_vhdl,../../lib/memory_tech_lib)
VHDS += $(call include_vhdl,../../lib/memory_tech_lib/tech/sim)
VHDS += $(call include_vhdl,../../lib/hwutils)

VHDLS := VHDS
include build.mk

VHDL_TESTS :=

VHDL_TOPS := dcache_tb
VHDL_TOPS += $(VHDL_TESTS)

all: $(VHDL_TOPS)

work-obj93.cf: $(VHDS)

dcache_tb.ghw: $(VHDL_TOPS)
	./dcache_tb --wave=$@ --stop-time=32000ns

%.vhm: %.vhmh
	LD_LIBRARY_PATH='' gcc -x c-header -E -P -w -nostdinc -I. $< -o $@

include $(TOOLS_DIR)/ghdl.mk

clean:
	rm -f *.cf *.o $(VHDL_TOPS) *_tap *.ghw

.PHONY: all clean
