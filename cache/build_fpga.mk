include $(dir $(lastword $(MAKEFILE_LIST)))build_core.mk

$(VHDLS) += cache_config_fpga.vhd
