-- ******************************************************************
-- ******************************************************************
-- ******************************************************************
-- This file is generated. Changing this file directly is probably
-- not what you want to do. Any changes will be overwritten next time
-- the generator is run.
-- ******************************************************************
-- ******************************************************************
-- ******************************************************************
architecture rom of decode_table is
    signal line : std_logic_vector(71 downto 0);
    signal addr : std_logic_vector(7 downto 0);
    signal mac_busy : mac_busy_t;
    signal imms_12_1 : std_logic_vector(31 downto 0);
    signal imms_8_0 : std_logic_vector(31 downto 0);
    signal imms_8_1 : std_logic_vector(31 downto 0);
    type mem is array (0 to 255) of std_logic_vector(71 downto 0);
    constant microcode_rom : mem := (0 => x"024040000000040000", -- CLRT
    1 => x"024044000080802088", -- CLRMAC
    2 => x"024040000000080000", -- DIV0U
    3 => x"024040000000000000", -- NOP
    4 => x"000040008291004400", 5 => x"000000008291004401", 6 => x"000000000020000002", 7 => x"0a6000000000000000", -- RTE
    8 => x"000000000030006002", 9 => x"0a6000000000000000", -- RTS
    10 => x"0240400000000c0000", -- SETT
    11 => x"000000000000000000", 12 => x"000000000800000000", 13 => x"000000000000000000", 14 => x"024040000000000000", -- SLEEP
    15 => x"124040000000000000", -- BGND
    16 => x"0240c0010081908000", -- CMP/PL Rn
    17 => x"0240c0010081948000", -- CMP/PZ Rn
    18 => x"024140011291988000", -- DT Rn
    19 => x"02414000130200a000", -- MOVT Rn
    20 => x"0241400112c29c8000", -- ROTL Rn
    21 => x"0241c00112c29c8000", -- ROTR Rn
    22 => x"0241400112c31c8000", -- ROTCL Rn
    23 => x"0241c00112c31c8000", -- ROTCR Rn
    24 => x"0241400112c39c8000", -- SHAL Rn
    25 => x"0241c00112c41c8000", -- SHAR Rn
    26 => x"0241400112c39c8000", -- SHLL Rn
    27 => x"0241c00112c39c8000", -- SHLR Rn
    28 => x"0242400112c3808000", -- SHLL2 Rn
    29 => x"0242c00112c3808000", -- SHLR2 Rn
    30 => x"0243400112c3808000", -- SHLL8 Rn
    31 => x"0243c00112c3808000", -- SHLR8 Rn
    32 => x"0244400112c3808000", -- SHLL16 Rn
    33 => x"0244c00112c3808000", -- SHLR16 Rn
    34 => x"02404200123000a000", -- STC SR, Rn
    35 => x"02404200123000c000", -- STC GBR, Rn
    36 => x"02404200123000e000", -- STC VBR, Rn
    37 => x"024046001230010000", -- STS MACH, Rn
    38 => x"024046001230012000", -- STS MACL, Rn
    39 => x"024042001230006000", -- STS PR, Rn
    40 => x"000000410480000800", 41 => x"000000400000000000", 42 => x"4000004100d4994c00", 43 => x"024040000000000000", -- TAS.B @Rn
    44 => x"000000a11291817000", 45 => x"024042000000000000", -- STC.L SR, @-Rn
    46 => x"000000a11291819000", 47 => x"024042000000000000", -- STC.L GBR, @-Rn
    48 => x"000000a1129181b000", 49 => x"024042000000000000", -- STC.L VBR, @-Rn
    50 => x"024046a1129181d000", -- STS.L MACH, @-Rn
    51 => x"024046a1129181f000", -- STS.L MACL, @-Rn
    52 => x"024042a11291821000", -- STS.L PR, @-Rn
    53 => x"024042000030222003", -- LDC Rm, SR
    54 => x"024042002230022000", -- LDC, Rm, GBR
    55 => x"024042003230022000", -- LDC Rm, VBR
    56 => x"024046000030022110", -- LDS Rm, MACH
    57 => x"024046000030022198", -- LDS Rm, MACL
    58 => x"024042004230022000", -- LDS Rm, PR
    59 => x"0000c0010091004002", 60 => x"0a6000000000000000", -- JMP @Rm
    61 => x"0000c0014291004004", 62 => x"0a6000000000000000", -- JSR @Rm
    63 => x"000000010080000401", 64 => x"000000011291004000", 65 => x"024042000000000000", -- LDC.L @Rm+, SR
    66 => x"000000050480000400", 67 => x"000000011291004000", 68 => x"024042000000000000", -- LDC.L @Rm+, GBR
    69 => x"000000090480000400", 70 => x"000000011291004000", 71 => x"024042000000000000", -- LDC.L @Rm+, VBR
    72 => x"024056011291004620", -- LDS.L @Rm+, MACH
    73 => x"0240560112910046a8", -- LDS.L @Rm+, MACL
    74 => x"0000020d0480000400", 75 => x"024042011291004000", -- LDS.L @Rm+, PR
    76 => x"000040000191022002", 77 => x"0a6000000000000000", -- BRAF Rm
    78 => x"000040004391022004", 79 => x"0a6000000000000000", -- BSRF Rm
    80 => x"024040011291024000", -- ADD Rm, Rn
    81 => x"224040011291264000", -- ADDC Rm, Rn
    82 => x"0240400112912a4000", -- ADDV Rm, Rn
    83 => x"024040011282024000", -- AND Rm, Rn
    84 => x"024040010080ae4000", -- CMP /EQ Rm, Rn
    85 => x"024040010081b24000", -- CMP /HS Rm, Rn
    86 => x"024040010081964000", -- CMP /GE Rm, Rn
    87 => x"024040010081b64000", -- CMP /HI Rm, Rn
    88 => x"024040010081924000", -- CMP /GT Rm, Rn
    89 => x"024040010080ba4000", -- CMP /STR Rm, Rn
    90 => x"000000005230022000", 91 => x"000000518480000400", 92 => x"000000410080ae4000", 93 => x"024040618080015400", -- CAS.L Rm, Rn, @R0
    94 => x"8240400112913e4000", -- DIV1 Rm, Rn
    95 => x"024040010081424000", -- DIV0S Rm, Rn
    96 => x"024064010080024330", -- DMULS.L Rm, Rn
    97 => x"024064010080024338", -- DMULU.L Rm, Rn
    98 => x"024040001255024000", -- EXTS.B Rm, Rn
    99 => x"024040001255824000", -- EXTS.W Rm, Rn
    100 => x"024040001256024000", -- EXTU.B Rm, Rn
    101 => x"024040001256824000", -- EXTU.W Rm, Rn
    102 => x"024040001230024000", -- MOV Rm, Rn
    103 => x"024064010080024340", -- MUL.L Rm, Rn
    104 => x"024064010080024348", -- MULS.W Rm, Rn
    105 => x"024064010080024350", -- MULU.W Rm, Rn
    106 => x"0240c0001311824000", -- NEG Rm, Rn
    107 => x"2240c0001311a64000", -- NEGC Rm, Rn
    108 => x"0240c0001307024000", -- NOT Rm, Rn
    109 => x"024040011287824000", -- OR Rm, Rn
    110 => x"024040011291824000", -- SUB Rm, Rn
    111 => x"224040011291a64000", -- SUBC Rm, Rn
    112 => x"024040011291aa4000", -- SUBV Rm, Rn
    113 => x"024040001258024000", -- SWAP.B Rm, Rn
    114 => x"024040001258824000", -- SWAP.W Rm, Rn
    115 => x"0240400100822e4000", -- TST Rm, Rn
    116 => x"024040011280824000", -- XOR Rm, Rn
    117 => x"0240400112d9024000", -- XTRACT Rm, Rn
    118 => x"0240400112c4024000", -- SHAD Rm, Rn
    119 => x"0240400112c3824000", -- SHLD Rm, Rn
    120 => x"0240c0a10091026c00", -- MOV.B Rm, @Rn
    121 => x"0240c0a10091027800", -- MOV.W Rm, @Rn
    122 => x"0240c0a10091027000", -- MOV.L Rm, @Rn
    123 => x"024040900430024800", -- MOV.B @Rm, Rn
    124 => x"024040900430025c00", -- MOV.W @Rm, Rn
    125 => x"024040900430024400", -- MOV.L @Rm, Rn
    126 => x"000000011291004780", 127 => x"000038026291004458", 128 => x"024040000000000000", -- MAC.L @Rm+, @Rn+
    129 => x"000200011291005f80", 130 => x"024278026291005c60", -- MAC.W @Rm+, @Rn+
    131 => x"000100026291004000", 132 => x"024140920491804800", -- MOV.B @Rm+, Rn
    133 => x"000200026291004000", 134 => x"024240920491805c00", -- MOV.W @Rm+, Rn
    135 => x"000000026291004000", 136 => x"024040920491804400", -- MOV.L @Rm+, Rn
    137 => x"024140a11291826c00", -- MOV.B Rm,@-Rn
    138 => x"024240a11291827800", -- MOV.W Rm,@-Rn
    139 => x"024040a11291827000", -- MOV.L Rm,@-Rn
    140 => x"024040a10091028c00", -- MOV.B Rm, @(R0, Rn)
    141 => x"024040a10091029800", -- MOV.W Rm, @(R0, Rn)
    142 => x"024040a10091029000", -- MOV.L Rm, @(R0, Rn)
    143 => x"02404092049102a800", -- MOV.B @(R0, Rm), Rn
    144 => x"02404092049102bc00", -- MOV.W @(R0, Rm), Rn
    145 => x"02404092049102a400", -- MOV.L @(R0, Rm), Rn
    146 => x"024540960491004800", -- MOV.B @(disp, Rm), R0
    147 => x"0245c0960491005c00", -- MOV.W @(disp, Rm), R0
    148 => x"024540a2009102cc00", -- MOV.B R0, @(disp, Rn)
    149 => x"0245c0a2009102d800", -- MOV.W R0, @(disp, Rn)
    150 => x"024640a10091027000", -- MOV.L Rm, @(disp, Rn)
    151 => x"024640920491004400", -- MOV.L @(disp, Rm), Rn
    152 => x"0246c0a2809102cc00", -- MOV.B R0, @(disp, GBR)
    153 => x"024740a2809102d800", -- MOV.W R0, @(disp, GBR)
    154 => x"0247c0a2809102d000", -- MOV.L R0, @(disp, GBR)
    155 => x"0246c0968491004800", -- MOV.B @(disp, GBR), R0
    156 => x"024740968491005c00", -- MOV.W @(disp, GBR), R0
    157 => x"0247c0968491004400", -- MOV.L @(disp, GBR), R0
    158 => x"c247c0007391008000", -- MOVA @(disp, PC), R0
    159 => x"044840000191008005", 160 => x"006000000000000000", 161 => x"024040000000000000", -- BF label
    162 => x"048840000191008005", 163 => x"0a6000000000000000", -- BF /S label
    164 => x"064840000191008006", 165 => x"006000000000000000", 166 => x"024040000000000000", -- BT label
    167 => x"06c840000191008006", 168 => x"0a6000000000000000", -- BT /S label
    169 => x"0008c0000191008002", 170 => x"0a6000000000000000", -- BRA label
    171 => x"0008c0004391008004", 172 => x"0a6000000000000000", -- BSR label
    173 => x"024740900591009c00", -- MOV.W @(disp, PC), Rn
    174 => x"c247c0900591008400", -- MOV.L @(disp, PC), Rn
    175 => x"0000009a849102e800", 176 => x"00000002d29102e000", 177 => x"0246c1000082030c00", -- AND.B #imm, @(R0, GBR)
    178 => x"0000009a849102e800", 179 => x"00000002d29102e000", 180 => x"0246c1000087830c00", -- OR.B #imm, @(R0, GBR)
    181 => x"0000009a849102e800", 182 => x"000000000000000000", 183 => x"0246c00000822c4000", -- TST.B #imm, @(R0, GBR)
    184 => x"0000009a849102e800", 185 => x"00000002d29102e000", 186 => x"0246c1000080830c00", -- XOR.B #imm, @(R0, GBR)
    187 => x"0246c001f282008000", -- AND #imm, R0
    188 => x"024940018080ac8000", -- CMP /EQ #imm, R0
    189 => x"0246c001f287808000", -- OR #imm, R0
    190 => x"0246c00180822c8000", -- TST #imm, R0
    191 => x"0246c001f280808000", -- XOR #imm, R0
    192 => x"000000a08291817000", 193 => x"000000a08291833000", 194 => x"00078080011100e400", 195 => x"000000000000000000", 196 => x"000000000020000002", 197 => x"006000000000000000", 198 => x"024040000000000000", -- TRAPA #imm
    199 => x"024940011291008000", -- ADD #imm, Rn
    200 => x"024940001230008000", -- MOV #imm, Rn
    201 => x"000200000191804002", 202 => x"000000a08291817000", 203 => x"000000a08291833000", 204 => x"00078080011100e400", 205 => x"000000000000000000", 206 => x"000000000020000002", 207 => x"006000000000000000", 208 => x"024040000000000000", -- General Illegal
    209 => x"000080000191004002", 210 => x"000000a08291817000", 211 => x"000000a08291833000", 212 => x"00078080011100e400", 213 => x"000000000000000000", 214 => x"000000000020000002", 215 => x"006000000000000000", 216 => x"024040000000000000", -- Slot Illegal
    217 => x"000000000000000000", 218 => x"010000000000000000", 219 => x"000780805230008400", 220 => x"0000009f0491004400", 221 => x"000000000020000002", 222 => x"006000003280802000", 223 => x"024040000000000000", -- Reset CPU
    224 => x"011200000191804002", 225 => x"c0008000d291004000", 226 => x"000000a35291817000", 227 => x"000000a35291833000", 228 => x"000780838091008400", 229 => x"000000008291c44000", 230 => x"000000000020000002", 231 => x"006000008291804000", 232 => x"024040000000000000", -- Interrupt
    233 => x"011200000191804002", 234 => x"c0008000d291004000", 235 => x"000000a35291817000", 236 => x"000000a35291833000", 237 => x"000780838091008400", 238 => x"000000008291804000", 239 => x"000000000020000002", 240 => x"006000008291804000", 241 => x"024040000000000000", -- Error
    242 => x"000000000000000000", 243 => x"100200000191804002", 244 => x"006000000000000000", 245 => x"024040000000000000", -- Break
    246 => x"000000000000000000", 247 => x"000000000000000000", 248 => x"000000000000000000", 249 => x"000000000000000000", 250 => x"000000000000000000", 251 => x"000000000000000000", 252 => x"000000000000000000", 253 => x"000000000000000000", 254 => x"000000000000000000", 255 => x"000000000000000000");
begin
    -- Read microcode line on falling edge of
    -- clock. Needs to be clocked so that xilinx
    -- uses a RAM, and needs to be falling edge to
    -- allow the ROM address to be computed.
    process(clk, op)
    begin
        if (clk = '0' and clk'event) then
            line <= microcode_rom(TO_INTEGER(unsigned(op.addr)));
        end if;
    end process;
    -- Sign extend parts of opcode
    process(op)
    begin
        -- Sign extend 8 right-most bits
        for i in 8 to 31 loop
            imms_8_0(i) <= op.code(7);
        end loop;
        imms_8_0(7 downto 0) <= op.code(7 downto 0);
        -- Sign extend 8 right-most bits shifted by 1
        for i in 9 to 31 loop
            imms_8_1(i) <= op.code(7);
        end loop;
        imms_8_1(8 downto 1) <= op.code(7 downto 0);
        imms_8_1(0) <= '0';
        -- Sign extend 12 right-most bits shifted by 1
        for i in 13 to 31 loop
            imms_12_1(i) <= op.code(11);
        end loop;
        imms_12_1(12 downto 1) <= op.code(11 downto 0);
        imms_12_1(0) <= '0';
    end process;
    with mac_busy select
        ex.mac_busy <=
            '1' when EX_BUSY,
            not next_id_stall when EX_NOT_STALL,
            '0' when others;
    with mac_busy select
        wb.mac_busy <=
            '1' when WB_BUSY,
            not next_id_stall when WB_NOT_STALL,
            '0' when others;
    with line(2 downto 0) select
        ex_stall.wrpc_z <=
            '1' when "010" | "100",
            not t_bcc when "101",
            t_bcc when "110",
            '0' when others;
    with line(2 downto 0) select
        ex_stall.wrsr_z <=
            '1' when "011",
            '0' when others;
    with line(2 downto 0) select
        wb_stall.wrsr_w <=
            '1' when "001",
            '0' when others;
    with line(2 downto 0) select
        ex_stall.wrpr_pc <=
            '1' when "100",
            '0' when others;
    with line(6 downto 3) select
        wb_stall.wrmach <=
            '1' when "0100",
            '0' when others;
    with line(6 downto 3) select
        ex_stall.wrmach <=
            '1' when "0010" | "0001",
            '0' when others;
    with line(6 downto 3) select
        ex_stall.mulcom2 <=
            MULUW when "1010",
            MULSW when "1001",
            MULL when "1000",
            DMULUL when "0111",
            DMULSL when "0110",
            NOP when others;
    with line(6 downto 3) select
        wb_stall.mulcom2 <=
            MACW when "1100",
            MACL when "1011",
            NOP when others;
    with line(6 downto 3) select
        wb_stall.macsel2 <=
            SEL_WBUS when "1100" | "1011" | "0101",
            SEL_YBUS when others;
    with line(6 downto 3) select
        ex_stall.macsel2 <=
            SEL_ZBUS when "0011" | "0001",
            SEL_YBUS when others;
    with line(9 downto 7) select
        ex_stall.mulcom1 <=
            '1' when "110",
            '0' when others;
    with line(9 downto 7) select
        wb_stall.mulcom1 <=
            '1' when "111",
            '0' when others;
    with line(9 downto 7) select
        wb_stall.macsel1 <=
            SEL_WBUS when "100" | "111",
            SEL_XBUS when others;
    with line(9 downto 7) select
        ex_stall.wrmacl <=
            '1' when "001" | "011",
            '0' when others;
    with line(9 downto 7) select
        ex_stall.macsel1 <=
            SEL_ZBUS when "001" | "010",
            SEL_XBUS when others;
    with line(9 downto 7) select
        wb_stall.wrmacl <=
            '1' when "101",
            '0' when others;
    with line(12 downto 10) select
        ex.mem_size <=
            LONG when "001" | "100" | "101",
            WORD when "110" | "111",
            BYTE when others;
    with line(12 downto 10) select
        ex_stall.ma_issue <=
            '1' when "001" | "010" | "011" | "100" | "110" | "111",
            t_bcc when "101",
            '0' when others;
    with line(12 downto 10) select
        ex.ma_wr <=
            '1' when "011" | "100" | "101" | "110",
            '0' when others;
    with line(17 downto 13) select
        ex.aluiny_sel <=
            SEL_IMM when "01110" | "01011" | "11000" | "10000" | "01101" | "01100" | "10011" | "01111" | "11001" | "00010" | "10110",
            SEL_R0 when "10100" | "10101",
            SEL_YBUS when others;
    with line(17 downto 13) select
        ex.regnum_y <=
            "10011" when "11000" | "01010",
            "10010" when "10000" | "00011",
            "10001" when "01101" | "00111",
            "10000" when "01100" | "00110",
            "10100" when "00001",
            '0' & op.code(7 downto 4) when "10011" | "10100" | "10010",
            '0' & op.code(11 downto 8) when "10001",
            "00000" when others;
    with line(17 downto 13) select
        ex.ybus_sel <=
            SEL_MACH when "01110" | "01000",
            SEL_REG when "10111" | "11000" | "10000" | "01101" | "01100" | "00001" | "10011" | "01010" | "00011" | "00111" | "00110" | "10100" | "10010" | "10110" | "10001",
            SEL_SR when "01011" | "00101",
            SEL_MACL when "01111" | "01001",
            SEL_PC when "11001",
            SEL_IMM when others;
    with line(22 downto 18) select
        ex_stall.t_sel <=
            SEL_SHIFT when "00111",
            SEL_SET when "00011",
            SEL_CARRY when "01001",
            SEL_CLEAR when others;
    with line(22 downto 18) select
        ex_stall.sr_sel <=
            SEL_DIV0U when "00010",
            SEL_SET_T when "00001" | "00111" | "00011" | "01001",
            SEL_LOGIC when "01011" | "01110",
            SEL_ZBUS when "01000",
            SEL_ARITH when "01010" | "01101" | "01100" | "00101" | "00100" | "01111" | "10000" | "00110",
            SEL_INT_MASK when "10001",
            SEL_PREV when others;
    with line(22 downto 18) select
        ex.logic_sr_func <=
            BYTE_EQ when "01110",
            ZERO when others;
    with line(22 downto 18) select
        ex.arith_sr_func <=
            OVERUNDERFLOW when "01010",
            UGRTER when "01101",
            UGRTER_EQ when "01100",
            SGRTER_EQ when "00101",
            SGRTER when "00100",
            DIV1 when "01111",
            DIV0S when "10000",
            ZERO when others;
    with line(27 downto 23) select
        ex.arith_func <=
            SUB when "00011",
            ADD when others;
    with line(27 downto 23) select
        ex.alumanip <=
            SET_BIT_7 when "01001",
            EXTEND_SBYTE when "01010",
            EXTEND_UWORD when "01101",
            EXTRACT when "10010",
            EXTEND_UBYTE when "01100",
            SWAP_WORD when "10001",
            EXTEND_SWORD when "01011",
            SWAP_BYTE when others;
    with line(27 downto 23) select
        ex_stall.shiftfunc <=
            ARITH when "01000",
            ROTC when "00110",
            ROTATE when "00101",
            LOGIC when others;
    with line(27 downto 23) select
        ex.logic_func <=
            LOGIC_OR when "01111",
            LOGIC_XOR when "00001",
            LOGIC_AND when "00100",
            LOGIC_NOT when others;
    with line(30 downto 28) select
        ex_stall.zbus_sel <=
            SEL_LOGIC when "000",
            SEL_WBUS when "010",
            SEL_YBUS when "011",
            SEL_SHIFT when "100",
            SEL_MANIP when "101",
            SEL_ARITH when others;
    with line(32 downto 31) select
        ex.xbus_sel <=
            SEL_REG when "01",
            SEL_PC when "11",
            SEL_IMM when others;
    ex_stall.wrreg_z <= line(33);
    wb_stall.wrreg_w <= line(34);
    slp <= line(35);
    with line(38 downto 36) select
        ex.regnum_z <=
            "01111" when "000",
            '0' & op.code(11 downto 8) when "001",
            "10000" when "010",
            "10001" when "011",
            "10010" when "100",
            "10011" when "101",
            '0' & op.code(7 downto 4) when "110",
            "00000" when others;
    with line(41 downto 39) select
        ex.regnum_x <=
            "10100" when "000",
            "01111" when "001",
            '0' & op.code(11 downto 8) when "010",
            '0' & op.code(7 downto 4) when "100",
            "10000" when "101",
            "10011" when "110",
            "10001" when "111",
            "00000" when others;
    with line(44 downto 42) select
        wb.regnum_w <=
            "10011" when "000",
            "10000" when "001",
            "10001" when "010",
            "10010" when "011",
            '0' & op.code(11 downto 8) when "100",
            "10100" when "110",
            "01111" when "111",
            "00000" when others;
    with line(45 downto 45) select
        ex_stall.mem_wdata_sel <=
            SEL_YBUS when "1",
            SEL_ZBUS when others;
    ex.mem_lock <= line(46);
    with line(48 downto 47) select
        ex_stall.mem_addr_sel <=
            SEL_ZBUS when "01",
            SEL_YBUS when "10",
            SEL_XBUS when others;
    maskint_next <= line(49);
    mac_stall_sense <= line(50);
    mac_s_latch <= line(51);
    with line(53 downto 52) select
        mac_busy <=
            WB_NOT_STALL when "01",
            EX_NOT_STALL when "10",
            WB_BUSY when "11",
            NOT_BUSY when others;
    id.incpc <= line(54);
    with line(59 downto 55) select
        ex.imm_val <=
            x"ffffffff" when "00011",
            x"00000001" when "00010",
            x"fffffffe" when "00101",
            x"00000002" when "00100",
            x"00000004" when "00000",
            x"fffffff8" when "00111",
            x"00000008" when "00110",
            x"fffffff0" when "01001",
            x"00000010" when "01000",
            imms_12_1 when "10001",
            x"000000" & op.code(7 downto 0) when "01101",
            "00000000000000000000000" & op.code(7 downto 0) & "0" when "01110",
            "0000000000000000000000" & op.code(7 downto 0) & "00" when "01111",
            imms_8_0 when "10010",
            imms_8_1 when "10000",
            x"0000000" & op.code(3 downto 0) when "01010",
            "000000000000000000000000000" & op.code(3 downto 0) & "0" when "01011",
            "00000000000000000000000000" & op.code(3 downto 0) & "00" when "01100",
            x"00000000" when others;
    ilevel_cap <= line(60);
    id.ifadsel <= line(61);
    with line(63 downto 62) select
        id.if_issue <=
            '1' when "01",
            t_bcc when "10",
            not t_bcc when "11",
            '0' when others;
    event_ack_0 <= line(64);
    with line(66 downto 65) select
        dispatch <=
            '1' when "01",
            t_bcc when "10",
            not t_bcc when "11",
            '0' when others;
    delay_jump <= line(67);
    debug <= line(68);
    ex.arith_ci_en <= line(69);
    with line(71 downto 70) select
        ex.aluinx_sel <=
            SEL_ZERO when "01",
            SEL_ROTCL when "10",
            SEL_FC when "11",
            SEL_XBUS when others;
end;
