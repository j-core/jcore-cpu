#ifdef VHDL

/* Included by VHDL code to create processes used for communicating
   with the C side. */

#ifndef NO_CLOCK
#ifndef CLK_SIGNAL
#define CLK_SIGNAL clk
#endif
#endif

#ifndef READ_ONLY_SIGNALS
#define READ_ONLY_SIGNALS
#endif

#ifndef WRITE_SIGNALS
#define WRITE_SIGNALS
#endif

#ifndef WRITE_ARRAY_SIGNALS
#define WRITE_ARRAY_SIGNALS
#endif

#define _SIG(name, type) _SIG2(name, name, type)
#define _SIG2(name, _, type)                                            \
  process (name)                                                        \
  variable l : line;                                                    \
  begin                                                                 \
  write(l, string'("v ")); write(l, string'(#name));                    \
  write(l, string'(" ")); write(l, now); write(l, string'(" "));        \
  write(l, name);                                                       \
  writeline(output, l);                                                 \
  end process;
  READ_ONLY_SIGNALS
  WRITE_SIGNALS
#undef _SIG2

  process
    variable l : line;
    variable good : boolean;
    variable parts : lines_ptr;
    variable i : integer;
    variable t : time;
#define _SIG2(_, name, type) variable v_##name : type;
    WRITE_SIGNALS
#undef _SIG2
  begin
    read_loop: while not endfile(input) loop
      readline(input, l);
      split_string(l, parts);
      deallocate(l);

      next read_loop when parts'length = 0; -- '
      if parts(1).all = "." then
-- echo the line
        wait for 0 ns;
        for i in 1 to parts'length loop -- '
          write(l, parts(i).all);
          if i < parts'length then -- '
            write(l, string'(" ")); -- '
          end if;
        end loop;
--        write(l, parts(parts'length).all); -- '
        writeline(output, l);
        deallocate(l);
      elsif parts(1).all = "r" then
--        report "READ COMMAND";
        if parts'length = 2 then -- '
          write(l, string'("r ")); -- '
          if false then

#define _SIG2(name, _, _2) elsif parts(2).all = #name then              \
       write(l, parts(2).all); write(l, string'(" ")); write(l, string'(" ")); \
       write(l, name); writeline(output, l);
            READ_ONLY_SIGNALS
            WRITE_SIGNALS
#undef _SIG2

          else
            report "read: unknown signal";
          end if;
        else
          report "read: invalid args";
        end if;
      elsif parts(1).all = "t" then
        write(l, string'("t ")); -- '
        write(l, now);
        writeline(output, l);
      elsif parts(1).all = "w" then
        --report "WRITE COMMAND";
        --write(l, string'("WRITING ")); -- '
        --write(l, parts(2).all);
        --writeline(output, l);

        if parts'length = 3 then -- '
          if false then

#define _SIG2(name1, name2, _)                     \
          elsif parts(2).all = #name1 then         \
            read(parts(3), v_##name2, good);       \
            if (good) then                         \
              name1 <= v_##name2;                  \
            else                                   \
              report "write: invalid value";        \
            end if;
            WRITE_SIGNALS
#undef _SIG2

          else
            report "write: unknown signal";
          end if;
        else
          report "write: invalid args";
        end if;
      elsif parts(1).all = "wait" then
--        report "WAIT COMMAND";

        if parts'length = 1 then -- '
          wait on dummy
#define _SIG2(name, _, _2) , name
            READ_ONLY_SIGNALS WRITE_SIGNALS;
#undef _SIG2
        elsif parts'length = 2 or parts'length = 3 then
          write(l, parts(2).all);
          write(l, string'(" ")); -- '
          if parts'length = 3 then -- '
            write(l, parts(3).all);
          else
            -- assume ns
            write(l, string'("ns")); -- '
          end if;
          read(l, t, good);
          if (good) then
            wait on dummy
#define _SIG2(name, _, _2) , name
              READ_ONLY_SIGNALS WRITE_SIGNALS for t;
#undef _SIG2
          else
            report "wait: invalid time";
          end if;
        else
          report "wait: invalid args";
        end if;
        write(l, string'("t ")); -- '
        write(l, now);
        writeline(output, l);
      elsif parts(1).all = "hwait" then
--        report "HWAIT COMMAND";
        if parts'length = 2 or parts'length = 3 then
          write(l, parts(2).all);
          write(l, string'(" ")); -- '
          if parts'length = 3 then -- '
            write(l, parts(3).all);
          else
            -- assume ns
            write(l, string'("ns")); -- '
          end if;
          read(l, t, good);
          if (good) then
            wait for t;
          else
            report "hwait: invalid time";
          end if;
        else
          report "hwait: invalid args";
        end if;
        write(l, string'("t ")); -- '
        write(l, now);
        writeline(output, l);

#ifndef NO_CLOCK
      elsif parts(1).all = "clkwait" then
--        report "clkwait COMMAND";
        if parts'length = 1 then -- '
            wait on CLK_SIGNAL;
        elsif parts'length = 2 then -- '
          if parts(2).all = "r" then
            wait until rising_edge(CLK_SIGNAL);
          elsif parts(2).all = "f" then
            wait until falling_edge(CLK_SIGNAL);
          else
            report "clkwait: invalid arg";
          end if;
        else
          report "clkwait: invalid arg number";
        end if;
        write(l, string'("t ")); -- '
        write(l, now);
        writeline(output, l);
#endif
      else
        report "UNKNOWN COMMAND";
      end if;
    end loop;
    wait;
  end process;

#undef _SIG

#else /* VHDL not defined */

/* Included by C code to create the signals array. */

#ifndef SIGENUM
#define SIGENUM(name) SIG_ ## name
#endif

#define _SIG(name, type) _SIG2(name, name, type)
enum {
#define _SIG2(_, name, _2) SIGENUM(name),
  READ_ONLY_SIGNALS
  WRITE_SIGNALS
#undef _SIG2
  SIGENUM(NUM_SIGNALS)
};

#ifndef SIG_ARRAY
#define SIG_ARRAY signals
#endif

static struct signal SIG_ARRAY[] = {
#define _SIG2(name1, name2, typename)                                \
  {.index = SIGENUM(name2), .name = #name1, .type = #typename, .read_only = 1},
  READ_ONLY_SIGNALS
#undef _SIG2
#define _SIG2(name1, name2, typename) \
  {.index = SIGENUM(name2), .name = #name1, .type = #typename, .read_only = 0},
  WRITE_SIGNALS
#undef _SIG2
  {.name = NULL}
};

#undef _SIG
#undef SIG_ARRAY
#undef SIGENUM

#endif
