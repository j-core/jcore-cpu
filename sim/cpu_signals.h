#define CLK_PERIOD 10

#define READ_ONLY_SIGNALS                                            \
  _SIG(rst, std_logic)                                               \
                                                                     \
  _SIG2(data_slaves_o(DEV_SRAM).a, db_sram_o_a, std_logic_vector(31 downto 0))      \
  _SIG2(data_slaves_o(DEV_SRAM).d, db_sram_o_d, std_logic_vector(31 downto 0))      \
  _SIG2(data_slaves_o(DEV_SRAM).we, db_sram_o_we, std_logic_vector(3 downto 0))     \
  _SIG2(data_slaves_o(DEV_SRAM).wr, db_sram_o_wr, std_logic)                        \
  _SIG2(data_slaves_o(DEV_SRAM).rd, db_sram_o_rd, std_logic)                        \
  _SIG2(data_slaves_o(DEV_SRAM).en, db_sram_o_en, std_logic)                        \
                                                                     \
  _SIG2(data_slaves_o(DEV_DDR).a, db_ddr_o_a, std_logic_vector(31 downto 0))      \
  _SIG2(data_slaves_o(DEV_DDR).d, db_ddr_o_d, std_logic_vector(31 downto 0))      \
  _SIG2(data_slaves_o(DEV_DDR).we, db_ddr_o_we, std_logic_vector(3 downto 0))     \
  _SIG2(data_slaves_o(DEV_DDR).wr, db_ddr_o_wr, std_logic)                        \
  _SIG2(data_slaves_o(DEV_DDR).rd, db_ddr_o_rd, std_logic)                        \
  _SIG2(data_slaves_o(DEV_DDR).en, db_ddr_o_en, std_logic)                        \
                                                                     \
  _SIG2(pio_data_o.a, db_pio_o_a, std_logic_vector(31 downto 0))      \
  _SIG2(pio_data_o.d, db_pio_o_d, std_logic_vector(31 downto 0))      \
  _SIG2(pio_data_o.we, db_pio_o_we, std_logic_vector(3 downto 0))     \
  _SIG2(pio_data_o.wr, db_pio_o_wr, std_logic)                        \
  _SIG2(pio_data_o.rd, db_pio_o_rd, std_logic)                        \
  _SIG2(pio_data_o.en, db_pio_o_en, std_logic)                        \
                                                                     \
  _SIG2(data_slaves_o(DEV_UART0).a, db_uart0_o_a, std_logic_vector(31 downto 0))      \
  _SIG2(data_slaves_o(DEV_UART0).d, db_uart0_o_d, std_logic_vector(31 downto 0))      \
  _SIG2(data_slaves_o(DEV_UART0).we, db_uart0_o_we, std_logic_vector(3 downto 0))     \
  _SIG2(data_slaves_o(DEV_UART0).wr, db_uart0_o_wr, std_logic)                        \
  _SIG2(data_slaves_o(DEV_UART0).rd, db_uart0_o_rd, std_logic)                        \
  _SIG2(data_slaves_o(DEV_UART0).en, db_uart0_o_en, std_logic)                        \
                                                                     \
  _SIG2(instrd_slaves_o(DEV_SRAM).en, inst_o_en, std_logic)                        \
  _SIG2(instrd_slaves_o(DEV_SRAM).a, inst_o_a, std_logic_vector(31 downto 0))      \
  _SIG2(instrd_slaves_o(DEV_SRAM).we, inst_o_we, std_logic_vector(3 downto 0))     \
                                                                     \
  _SIG2(debug_o.ack, debug_o_ack, std_logic)                         \
  _SIG2(debug_o.d, debug_o_d, std_logic_vector(31 downto 0))         \
  _SIG2(debug_o.rdy, debug_o_rdy, std_logic)                         \
                                                                     \
  _SIG(event_ack_o, std_logic_vector(2 downto 0))

#define WRITE_SIGNALS                                                \
  _SIG2(data_slaves_i(DEV_SRAM).d, db_sram_i_d, std_logic_vector(31 downto 0))      \
  _SIG2(data_slaves_i(DEV_SRAM).ack, db_sram_i_ack, std_logic)                      \
                                                                     \
  _SIG2(data_slaves_i(DEV_DDR).d, db_ddr_i_d, std_logic_vector(31 downto 0))      \
  _SIG2(data_slaves_i(DEV_DDR).ack, db_ddr_i_ack, std_logic)                      \
                                                                     \
  _SIG2(pio_data_i.d, db_pio_i_d, std_logic_vector(31 downto 0))      \
  _SIG2(pio_data_i.ack, db_pio_i_ack, std_logic)                      \
                                                                     \
  _SIG2(data_slaves_i(DEV_UART0).d, db_uart0_i_d, std_logic_vector(31 downto 0))      \
  _SIG2(data_slaves_i(DEV_UART0).ack, db_uart0_i_ack, std_logic)                      \
                                                                     \
  _SIG2(instrd_slaves_i(DEV_SRAM).d, inst_i_d, std_logic_vector(31 downto 0))      \
  _SIG2(instrd_slaves_i(DEV_SRAM).ack, inst_i_ack, std_logic)                      \
                                                                     \
  _SIG2(debug_i.en, debug_i_en, std_logic)                           \
  _SIG(debug_i_cmd, std_logic_vector(1 downto 0))                    \
  _SIG2(debug_i.d, debug_i_d, std_logic_vector(31 downto 0))         \
  _SIG2(debug_i.d_en, debug_i_d_en, std_logic)                       \
  _SIG2(debug_i.ir, debug_i_ir, std_logic_vector(15 downto 0))       \
                                                                     \
  _SIG(event_req_i, std_logic_vector(2 downto 0))                    \
  _SIG(event_info_i, std_logic_vector(11 downto 0))
