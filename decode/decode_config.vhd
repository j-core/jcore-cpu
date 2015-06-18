-- Copyright (c) 2015, Smart Energy Instruments Inc.
-- All rights reserved.  For details, see COPYING in the top level directory.

-- Configurations for choosing the type of CPU instruction decoder to use

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use work.decode_pack.all;

configuration cpu_decode_simple of decode is
  for arch
    for core : decode_core
      use entity work.decode_core(arch)
        generic map (
          decode_type => SIMPLE,
          reset_vector => DEC_CORE_RESET);
    end for;
    for table : decode_table
      use entity work.decode_table(simple_logic);
    end for;
  end for;
end configuration;

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use work.decode_pack.all;

configuration cpu_decode_reverse of decode is
  for arch
    for core : decode_core
      use entity work.decode_core(arch)
        generic map (
          decode_type => REVERSE,
          reset_vector => DEC_CORE_RESET);
    end for;
    for table : decode_table
      use entity work.decode_table(reverse_logic);
    end for;
  end for;
end configuration;

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use work.decode_pack.all;

configuration cpu_decode_microcode of decode is
  for arch
    for core : decode_core
      use entity work.decode_core(arch)
        generic map (
          decode_type => MICROCODE,
          reset_vector => DEC_CORE_ROM_RESET);
    end for;
    for table : decode_table
      use entity work.decode_table(rom);
    end for;
  end for;
end configuration;
