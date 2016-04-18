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
