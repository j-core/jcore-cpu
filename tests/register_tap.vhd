library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use work.cpu2j0_components_pack.all;
use work.test_pkg.all;

entity register_tap is
  
end register_tap;

architecture tb  of register_tap is



  signal clk : std_logic := '0';
  signal rst : std_logic := '1';

  signal addr_ra, addr_rb, w_addr_wb, w_addr_ex : std_logic_vector(4 downto 0);
  signal dout_a, dout_b, dout_0, din_wb, din_ex : std_logic_vector(31 downto 0);
  signal we_wb, we_ex : std_logic;
  
  signal slot : std_logic;
  
  shared variable ENDSIM : boolean := false;

  
begin  -- tb
       -- 
  --clk <= '0' after 5 ns when clk = '1' else '1' after 5 ns;

  clk_gen : process
    begin
      if ENDSIM = false then
        clk <= '0';
        wait for 5 ns;
        clk <= '1';
        wait for 5 ns;
      else
        wait;
      end if;
    end process;


    u_regfile : register_file
          generic map (
            ADDR_WIDTH => 5,
            NUM_REGS => 22,
            REG_WIDTH => 32)
          port map(clk => clk, ce => slot,
                   addr_ra => addr_ra,
                   dout_a => dout_a,
                   addr_rb => addr_rb,
                   dout_b => dout_b,
                   dout_0 => dout_0,
                   we_wb => we_wb,
                   w_addr_wb => w_addr_wb,
                   din_wb => din_wb,
                   we_ex => we_ex,
                   w_addr_ex => w_addr_ex,
                   din_ex => din_ex); 
  
  process

    
    begin
   
    test_plan(11,"register file");

    addr_ra <= "00001";
    addr_rb <= "00000";
    
    w_addr_wb <= "00000";
    din_wb <= x"bbbbbbbb";
    
    w_addr_ex <= "00001";
    din_ex <= x"aaaaaaaa";

    slot <= '0';

    we_wb <= '1';
    we_ex <= '0';


    wait for 10 ns;
    rst <= '0';
    slot <= '1';
    
    wait for 5 ns;
    addr_ra <= "00000";
    din_wb <= x"cccccccc";

    wait for 5 ns; -- check out (mid CC)
    test_equal(dout_a, x"cccccccc","test wb_pipe on dout_a");
    test_equal(dout_b, x"cccccccc","test wb_pipe on dout_b");
    test_equal(dout_0, x"cccccccc","test wb_pipe on dout_0");
    wait for 10 ns;
    we_wb <= '0';
    din_wb <= x"dddddddd";
     wait for 10 ns;
    -- addr_ra <= "00001";
    wait for 20 ns;
    we_ex <= '1';
    wait for 5 ns;
    addr_ra <= "00001";
    wait for 10 ns;
    test_equal(dout_a, x"aaaaaaaa","test ex_pipe[1] on dout_a");
    
    we_ex <= '0';
    din_ex <= x"55555555";
    wait for 10 ns;
    addr_ra <= "00000";
    wait for 10 ns; -- 
    test_equal(dout_a, x"cccccccc","test RAM[0] on dout_a");
    
    wait for 10 ns; -- 
      w_addr_ex <= "00000";
     w_addr_wb <= "00001";
    
     wait for 10 ns; 
    we_ex <= '1';
    --w_addr_ex <= "00010"; -- hmmm
     wait for 10 ns; -- 
    -- w_addr_ex <= "00000";
    we_ex <= '0';
    addr_ra <= "00001";
    wait for 5 ns;
     test_equal(dout_a, x"aaaaaaaa","test RAM[1] on dout_a");
    test_equal(dout_b, x"55555555","test ex_pipe[1] on dout_b");
    test_equal(dout_0, x"55555555","test ex_pipe[1] on dout_0");
    wait for 5 ns;
    addr_ra <= "00000";
    wait for 10 ns;
     test_equal(dout_a, x"55555555","test ex_pipe[2] on dout_a");
    wait for 20 ns;
    we_wb <= '1';
    w_addr_wb <= "00010";
    wait for 10 ns;
    addr_rb <= "00010";
    w_addr_wb <= "00011";
    wait for 10 ns;
    we_wb <= '0';
    test_equal(dout_b, x"dddddddd","test RAM[2] on dout_b");
    wait for 10 ns;
    we_ex <= '1';
    --w_addr_ex <= "00011";
    din_ex <= x"ffffffff";
    wait for 10 ns;
    we_ex <= '0';
    wait for 10 ns;
    w_addr_ex <= "00100";
    din_ex <= x"22222222";
    we_ex <= '1';
    wait for 10 ns;
    we_ex <= '0';
    test_equal(dout_0, x"ffffffff","test reg_0 on dout_0");
    
    
    test_finished("done");
    
    wait for 40 ns;
    ENDSIM := true;
    
    wait;
    end process;
  

end tb ;
