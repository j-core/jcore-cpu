include $(dir $(lastword $(MAKEFILE_LIST)))build_core.mk

$(VHDLS) += core/cpu_config.vhd
$(VHDLS) += decode/decode_table_simple.vhd
$(VHDLS) += decode/decode_table_simple_config.vhd
$(VHDLS) += decode/decode_table_reverse.vhd
$(VHDLS) += decode/decode_table_reverse_config.vhd
$(VHDLS) += decode/decode_table_rom.vhd
$(VHDLS) += decode/decode_table_rom_config.vhd
