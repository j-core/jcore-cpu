include $(dir $(lastword $(MAKEFILE_LIST)))build_core.mk

$(VHDLS) += core/cpu_config.vhd
$(VHDLS) += core/register_file_flops.vhd
$(VHDLS) += core/register_file_two_bank.vhd
$(VHDLS) += decode/decode_table_simple.vhd
$(VHDLS) += decode/decode_table_simple_config.vhd
$(VHDLS) += decode/decode_table_direct.vhd
$(VHDLS) += decode/decode_table_direct_config.vhd
$(VHDLS) += decode/decode_table_rom.vhd
$(VHDLS) += decode/decode_table_rom_config.vhd
