$(VHDLS) += cpu2j0_pkg.vhd
$(VHDLS) += core/components_pkg.vhd
$(VHDLS) += core/cpu.vhd
$(VHDLS) += core/mult_pkg.vhd
$(VHDLS) += core/mult.vhd
$(VHDLS) += core/datapath_pkg.vhd
$(VHDLS) += core/datapath.vhd
$(VHDLS) += core/register_file.vhd

$(VHDLS) += decode/decode_pkg.vhd
$(VHDLS) += decode/decode.vhd
$(VHDLS) += decode/decode_body.vhd
$(VHDLS) += decode/decode_table.vhd
$(VHDLS) += decode/decode_core.vhd
