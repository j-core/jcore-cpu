-- ******************************************************************
-- ******************************************************************
-- ******************************************************************
-- This file is generated. Changing this file directly is probably
-- not what you want to do. Any changes will be overwritten next time
-- the generator is run.
-- ******************************************************************
-- ******************************************************************
-- ******************************************************************
architecture reverse_logic of decode_table is
    signal mac_busy : mac_busy_t;
    signal imms_12_1 : std_logic_vector(31 downto 0);
    signal imms_8_0 : std_logic_vector(31 downto 0);
    signal imms_8_1 : std_logic_vector(31 downto 0);
    signal cond1 : std_logic_vector(2 downto 0);
    signal cond10 : std_logic_vector(1 downto 0);
    signal cond11 : std_logic_vector(2 downto 0);
    signal cond12 : std_logic_vector(6 downto 0);
    signal cond13 : std_logic_vector(2 downto 0);
    signal cond14 : std_logic_vector(4 downto 0);
    signal cond15 : std_logic_vector(6 downto 0);
    signal cond16 : std_logic_vector(2 downto 0);
    signal cond18 : std_logic_vector(1 downto 0);
    signal cond19 : std_logic_vector(1 downto 0);
    signal cond20 : std_logic_vector(6 downto 0);
    signal cond23 : std_logic_vector(6 downto 0);
    signal cond25 : std_logic_vector(2 downto 0);
    signal cond27 : std_logic_vector(1 downto 0);
    signal cond3 : std_logic_vector(6 downto 0);
    signal cond35 : std_logic_vector(2 downto 0);
    signal cond36 : std_logic_vector(5 downto 0);
    signal cond42 : std_logic_vector(17 downto 0);
    signal cond44 : std_logic_vector(4 downto 0);
    signal cond45 : std_logic_vector(2 downto 0);
    signal cond47 : std_logic_vector(1 downto 0);
    signal cond5 : std_logic_vector(1 downto 0);
    signal cond50 : std_logic_vector(4 downto 0);
    signal cond53 : std_logic_vector(6 downto 0);
    signal cond7 : std_logic_vector(2 downto 0);
    signal imp_bit_0 : std_logic;
    signal imp_bit_1 : std_logic;
    signal imp_bit_10 : std_logic;
    signal imp_bit_100 : std_logic;
    signal imp_bit_101 : std_logic;
    signal imp_bit_102 : std_logic;
    signal imp_bit_103 : std_logic;
    signal imp_bit_104 : std_logic;
    signal imp_bit_105 : std_logic;
    signal imp_bit_106 : std_logic;
    signal imp_bit_107 : std_logic;
    signal imp_bit_108 : std_logic;
    signal imp_bit_109 : std_logic;
    signal imp_bit_11 : std_logic;
    signal imp_bit_110 : std_logic;
    signal imp_bit_111 : std_logic;
    signal imp_bit_112 : std_logic;
    signal imp_bit_113 : std_logic;
    signal imp_bit_114 : std_logic;
    signal imp_bit_115 : std_logic;
    signal imp_bit_116 : std_logic;
    signal imp_bit_117 : std_logic;
    signal imp_bit_118 : std_logic;
    signal imp_bit_119 : std_logic;
    signal imp_bit_12 : std_logic;
    signal imp_bit_120 : std_logic;
    signal imp_bit_121 : std_logic;
    signal imp_bit_122 : std_logic;
    signal imp_bit_123 : std_logic;
    signal imp_bit_124 : std_logic;
    signal imp_bit_125 : std_logic;
    signal imp_bit_126 : std_logic;
    signal imp_bit_127 : std_logic;
    signal imp_bit_128 : std_logic;
    signal imp_bit_129 : std_logic;
    signal imp_bit_13 : std_logic;
    signal imp_bit_130 : std_logic;
    signal imp_bit_131 : std_logic;
    signal imp_bit_132 : std_logic;
    signal imp_bit_133 : std_logic;
    signal imp_bit_134 : std_logic;
    signal imp_bit_135 : std_logic;
    signal imp_bit_136 : std_logic;
    signal imp_bit_137 : std_logic;
    signal imp_bit_138 : std_logic;
    signal imp_bit_139 : std_logic;
    signal imp_bit_14 : std_logic;
    signal imp_bit_140 : std_logic;
    signal imp_bit_141 : std_logic;
    signal imp_bit_142 : std_logic;
    signal imp_bit_143 : std_logic;
    signal imp_bit_144 : std_logic;
    signal imp_bit_145 : std_logic;
    signal imp_bit_146 : std_logic;
    signal imp_bit_147 : std_logic;
    signal imp_bit_148 : std_logic;
    signal imp_bit_149 : std_logic;
    signal imp_bit_15 : std_logic;
    signal imp_bit_150 : std_logic;
    signal imp_bit_151 : std_logic;
    signal imp_bit_152 : std_logic;
    signal imp_bit_153 : std_logic;
    signal imp_bit_154 : std_logic;
    signal imp_bit_155 : std_logic;
    signal imp_bit_156 : std_logic;
    signal imp_bit_157 : std_logic;
    signal imp_bit_158 : std_logic;
    signal imp_bit_159 : std_logic;
    signal imp_bit_16 : std_logic;
    signal imp_bit_160 : std_logic;
    signal imp_bit_161 : std_logic;
    signal imp_bit_162 : std_logic;
    signal imp_bit_17 : std_logic;
    signal imp_bit_18 : std_logic;
    signal imp_bit_19 : std_logic;
    signal imp_bit_2 : std_logic;
    signal imp_bit_20 : std_logic;
    signal imp_bit_21 : std_logic;
    signal imp_bit_22 : std_logic;
    signal imp_bit_23 : std_logic;
    signal imp_bit_24 : std_logic;
    signal imp_bit_25 : std_logic;
    signal imp_bit_26 : std_logic;
    signal imp_bit_27 : std_logic;
    signal imp_bit_28 : std_logic;
    signal imp_bit_29 : std_logic;
    signal imp_bit_3 : std_logic;
    signal imp_bit_30 : std_logic;
    signal imp_bit_31 : std_logic;
    signal imp_bit_32 : std_logic;
    signal imp_bit_33 : std_logic;
    signal imp_bit_34 : std_logic;
    signal imp_bit_35 : std_logic;
    signal imp_bit_36 : std_logic;
    signal imp_bit_37 : std_logic;
    signal imp_bit_38 : std_logic;
    signal imp_bit_39 : std_logic;
    signal imp_bit_4 : std_logic;
    signal imp_bit_40 : std_logic;
    signal imp_bit_41 : std_logic;
    signal imp_bit_42 : std_logic;
    signal imp_bit_43 : std_logic;
    signal imp_bit_44 : std_logic;
    signal imp_bit_45 : std_logic;
    signal imp_bit_46 : std_logic;
    signal imp_bit_47 : std_logic;
    signal imp_bit_48 : std_logic;
    signal imp_bit_49 : std_logic;
    signal imp_bit_5 : std_logic;
    signal imp_bit_50 : std_logic;
    signal imp_bit_51 : std_logic;
    signal imp_bit_52 : std_logic;
    signal imp_bit_53 : std_logic;
    signal imp_bit_54 : std_logic;
    signal imp_bit_55 : std_logic;
    signal imp_bit_56 : std_logic;
    signal imp_bit_57 : std_logic;
    signal imp_bit_58 : std_logic;
    signal imp_bit_59 : std_logic;
    signal imp_bit_6 : std_logic;
    signal imp_bit_60 : std_logic;
    signal imp_bit_61 : std_logic;
    signal imp_bit_62 : std_logic;
    signal imp_bit_63 : std_logic;
    signal imp_bit_64 : std_logic;
    signal imp_bit_65 : std_logic;
    signal imp_bit_66 : std_logic;
    signal imp_bit_67 : std_logic;
    signal imp_bit_68 : std_logic;
    signal imp_bit_69 : std_logic;
    signal imp_bit_7 : std_logic;
    signal imp_bit_70 : std_logic;
    signal imp_bit_71 : std_logic;
    signal imp_bit_72 : std_logic;
    signal imp_bit_73 : std_logic;
    signal imp_bit_74 : std_logic;
    signal imp_bit_75 : std_logic;
    signal imp_bit_76 : std_logic;
    signal imp_bit_77 : std_logic;
    signal imp_bit_78 : std_logic;
    signal imp_bit_79 : std_logic;
    signal imp_bit_8 : std_logic;
    signal imp_bit_80 : std_logic;
    signal imp_bit_81 : std_logic;
    signal imp_bit_82 : std_logic;
    signal imp_bit_83 : std_logic;
    signal imp_bit_84 : std_logic;
    signal imp_bit_85 : std_logic;
    signal imp_bit_86 : std_logic;
    signal imp_bit_87 : std_logic;
    signal imp_bit_88 : std_logic;
    signal imp_bit_89 : std_logic;
    signal imp_bit_9 : std_logic;
    signal imp_bit_90 : std_logic;
    signal imp_bit_91 : std_logic;
    signal imp_bit_92 : std_logic;
    signal imp_bit_93 : std_logic;
    signal imp_bit_94 : std_logic;
    signal imp_bit_95 : std_logic;
    signal imp_bit_96 : std_logic;
    signal imp_bit_97 : std_logic;
    signal imp_bit_98 : std_logic;
    signal imp_bit_99 : std_logic;
    signal p : std_logic_vector(0 downto 0);
begin
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
    -- Mac busy muxes
    with mac_busy select
        ex.mac_busy <=
            not next_id_stall when EX_NOT_STALL,
            '1' when EX_BUSY,
            '0' when others;
    with mac_busy select
        wb.mac_busy <=
            not next_id_stall when WB_NOT_STALL,
            '1' when WB_BUSY,
            '0' when others;
    p <= "0" when op.plane = NORMAL_INSTR else "1";
    imp_bit_0 <= (not p(0) and (op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(11) and not op.code(8)));
    imp_bit_1 <= (p(0) and (not op.code(10) and not op.code(9)) and (not op.addr(3) and not op.addr(2) and not op.addr(1) and op.addr(0)));
    imp_bit_2 <= ((not op.addr(3) and op.addr(2) and not op.addr(1) and not op.addr(0)) and p(0) and (not op.code(10) and not op.code(9)));
    imp_bit_3 <= (op.addr(0) and not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and op.code(5) and not op.code(4) and not op.code(3) and op.code(2) and op.code(1) and not op.code(0)));
    imp_bit_4 <= (not op.addr(0) and not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(5) and op.code(4) and op.code(3) and not op.code(2) and op.code(1) and op.code(0)));
    imp_bit_5 <= (not p(0) and (not op.code(15) and not op.code(14) and op.code(13) and not op.code(12) and not op.code(3) and op.code(2) and not op.code(0)));
    imp_bit_6 <= (not p(0) and (not op.code(15) and not op.code(14) and op.code(13) and op.code(12) and op.code(2) and not op.code(1) and op.code(0)));
    imp_bit_7 <= (not p(0) and (not op.code(15) and not op.code(14) and not op.code(13) and not op.code(12) and op.code(3) and op.code(2) and not op.code(0)));
    imp_bit_8 <= (not p(0) and (not op.code(15) and op.code(14) and op.code(13) and not op.code(12) and op.code(3)));
    imp_bit_9 <= (not p(0) and (not op.code(15) and op.code(14) and op.code(13) and not op.code(12) and not op.code(2)));
    imp_bit_10 <= (not p(0) and (op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and op.code(11) and not op.code(10)));
    imp_bit_11 <= (not p(0) and not op.addr(1) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(5) and op.code(4) and op.code(3) and not op.code(2) and op.code(1) and op.code(0)));
    imp_bit_12 <= (not op.addr(0) and not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(5) and op.code(4) and not op.code(3) and not op.code(2) and op.code(1) and op.code(0)));
    imp_bit_13 <= (not p(0) and (not op.code(15) and not op.code(14) and not op.code(13) and not op.code(12) and op.code(3) and op.code(2) and not op.code(1)));
    imp_bit_14 <= ((op.addr(1) and not op.addr(0)) and not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(5) and op.code(4) and op.code(3) and not op.code(2) and op.code(1) and op.code(0)));
    imp_bit_15 <= ((not op.addr(1) and not op.addr(0)) and not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(5) and op.code(4) and op.code(3) and not op.code(2) and op.code(1) and op.code(0)));
    imp_bit_16 <= ((not op.addr(1) and not op.addr(0)) and not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and op.code(5) and not op.code(4) and not op.code(3) and op.code(2) and op.code(1) and op.code(0)));
    imp_bit_17 <= (not op.addr(0) and not p(0) and (not op.code(15) and op.code(14) and op.code(13) and not op.code(12) and not op.code(3) and op.code(2) and not op.code(0)));
    imp_bit_18 <= (not p(0) and (op.code(15) and not op.code(13) and not op.code(12) and not op.code(11) and not op.code(9)));
    imp_bit_19 <= (op.addr(0) and not p(0) and (not op.code(15) and op.code(14) and op.code(13) and not op.code(12) and not op.code(3) and op.code(2) and not op.code(0)));
    imp_bit_20 <= (not op.addr(0) and not p(0) and (not op.code(15) and op.code(14) and op.code(13) and not op.code(12) and not op.code(3) and op.code(2) and not op.code(1)));
    imp_bit_21 <= ((not op.addr(1) and not op.addr(0)) and not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(5) and op.code(4) and not op.code(3) and op.code(2) and op.code(1) and op.code(0)));
    imp_bit_22 <= (op.addr(0) and not p(0) and (not op.code(15) and op.code(14) and op.code(13) and not op.code(12) and not op.code(3) and op.code(2) and not op.code(1)));
    imp_bit_23 <= (not p(0) and not op.addr(1) and (op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and op.code(11) and op.code(10)));
    imp_bit_24 <= ((op.addr(1) and not op.addr(0)) and not p(0) and (op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and op.code(11) and op.code(10)));
    imp_bit_25 <= ((not op.addr(1) and not op.addr(0)) and not p(0) and (op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and op.code(11) and op.code(10)));
    imp_bit_26 <= (not p(0) and (not op.code(15) and op.code(14) and not op.code(12) and op.code(3) and op.code(2) and not op.code(1)));
    imp_bit_27 <= (not op.addr(0) and not p(0) and (not op.code(15) and not op.code(14) and not op.code(13) and not op.code(12) and not op.code(11) and not op.code(10) and not op.code(9) and not op.code(8) and not op.code(7) and not op.code(6) and not op.code(4) and op.code(3) and not op.code(2) and op.code(1) and op.code(0)));
    imp_bit_28 <= (not op.addr(0) and not p(0) and (not op.code(15) and not op.code(14) and not op.code(13) and not op.code(12) and not op.code(11) and not op.code(10) and not op.code(9) and not op.code(8) and not op.code(7) and not op.code(6) and not op.code(5) and op.code(3) and not op.code(2) and op.code(1) and op.code(0)));
    imp_bit_29 <= (not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(4) and not op.code(3) and not op.code(2) and op.code(1) and not op.code(0)));
    imp_bit_30 <= (p(0) and (not op.addr(1) and op.addr(0)) and (not op.code(10) and op.code(9) and not op.code(8)));
    imp_bit_31 <= (not op.addr(0) and not p(0) and (not op.code(15) and not op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(4) and not op.code(3) and not op.code(2) and op.code(1) and op.code(0)));
    imp_bit_32 <= (not op.addr(0) and not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(4) and op.code(3) and not op.code(2) and op.code(1) and op.code(0)));
    imp_bit_33 <= (not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(5) and not op.code(3) and op.code(2) and op.code(1) and not op.code(0)));
    imp_bit_34 <= (not op.addr(0) and not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(4) and not op.code(3) and not op.code(2) and op.code(1) and op.code(0)));
    imp_bit_35 <= (op.addr(0) and not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(4) and op.code(3) and not op.code(2) and op.code(1) and op.code(0)));
    imp_bit_36 <= (op.addr(0) and not p(0) and (not op.code(15) and not op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(4) and not op.code(3) and not op.code(2) and op.code(1) and op.code(0)));
    imp_bit_37 <= (not p(0) and (not op.code(15) and op.code(14) and op.code(13) and not op.code(12) and op.code(3) and not op.code(2) and op.code(1) and not op.code(0)));
    imp_bit_38 <= (not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(4) and not op.code(3) and not op.code(1)));
    imp_bit_39 <= (not op.addr(0) and not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(5) and not op.code(3) and not op.code(2) and op.code(1) and op.code(0)));
    imp_bit_40 <= (not p(0) and (not op.code(15) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and op.code(5) and not op.code(4) and not op.code(2) and op.code(1) and not op.code(0)));
    imp_bit_41 <= (not p(0) and (not op.code(15) and op.code(14) and op.code(13) and not op.code(12) and op.code(1) and op.code(0)));
    imp_bit_42 <= ((not op.addr(2) and op.addr(1) and not op.addr(0)) and p(0) and (not op.code(10) and op.code(9) and op.code(8)));
    imp_bit_43 <= (not p(0) and not op.addr(1) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(4) and not op.code(3) and op.code(2) and op.code(1) and op.code(0)));
    imp_bit_44 <= (not p(0) and (not op.code(15) and not op.code(14) and op.code(13) and not op.code(12) and not op.code(3) and not op.code(0)));
    imp_bit_45 <= (not p(0) and (op.code(15) and op.code(14) and op.code(13) and not op.code(12)));
    imp_bit_46 <= ((not op.addr(2) and op.addr(1) and op.addr(0)) and p(0) and (not op.code(10) and op.code(9) and op.code(8)));
    imp_bit_47 <= ((op.addr(1) and not op.addr(0)) and not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(4) and not op.code(3) and op.code(2) and op.code(1) and op.code(0)));
    imp_bit_48 <= (not op.addr(0) and not p(0) and (not op.code(15) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(4) and not op.code(3) and not op.code(2) and op.code(1) and op.code(0)));
    imp_bit_49 <= (not p(0) and not op.addr(1) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(5) and not op.code(3) and op.code(2) and op.code(1) and op.code(0)));
    imp_bit_50 <= (not op.addr(0) and not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(5) and op.code(4) and not op.code(2) and op.code(1) and op.code(0)));
    imp_bit_51 <= (not op.addr(0) and not p(0) and (not op.code(15) and not op.code(14) and op.code(13) and not op.code(12) and not op.code(3) and not op.code(2) and op.code(1) and op.code(0)));
    imp_bit_52 <= (not p(0) and op.addr(1) and (not op.code(15) and not op.code(14) and op.code(13) and not op.code(12) and not op.code(3) and not op.code(2) and op.code(1) and op.code(0)));
    imp_bit_53 <= (not op.addr(0) and not p(0) and (op.code(15) and not op.code(14) and not op.code(13) and not op.code(12) and op.code(11) and op.code(10) and op.code(9) and op.code(8)));
    imp_bit_54 <= (not op.addr(0) and p(0) and (op.code(9) and not op.code(8)));
    imp_bit_55 <= (not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(5) and op.code(4) and not op.code(3) and not op.code(1) and op.code(0)));
    imp_bit_56 <= (not p(0) and (not op.code(15) and not op.code(14) and op.code(13) and op.code(12) and not op.code(3) and op.code(2) and not op.code(1) and not op.code(0)));
    imp_bit_57 <= (not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and op.code(3) and op.code(2) and op.code(1) and op.code(0)));
    imp_bit_58 <= (not p(0) and (not op.code(15) and not op.code(14) and not op.code(13) and not op.code(12) and not op.code(3) and op.code(2) and op.code(1) and op.code(0)));
    imp_bit_59 <= (not p(0) and (not op.code(15) and op.code(14) and op.code(13) and not op.code(12) and not op.code(3) and op.code(2) and op.code(1) and op.code(0)));
    imp_bit_60 <= (not p(0) and (op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(11) and op.code(10) and op.code(9) and op.code(8)));
    imp_bit_61 <= (not p(0) and (not op.code(15) and not op.code(14) and op.code(13) and op.code(12) and not op.code(3) and op.code(1)));
    imp_bit_62 <= (not p(0) and (not op.code(15) and not op.code(14) and not op.code(13) and not op.code(12) and op.code(2) and not op.code(0)));
    imp_bit_63 <= (not p(0) and (not op.code(15) and not op.code(14) and op.code(13) and not op.code(12) and not op.code(3) and not op.code(1)));
    imp_bit_64 <= (not p(0) and (op.code(15) and not op.code(14) and not op.code(13) and op.code(12)));
    imp_bit_65 <= ((op.addr(2) and not op.addr(1) and not op.addr(0)) and p(0) and (not op.code(10) and op.code(9) and op.code(8)));
    imp_bit_66 <= ((op.addr(1) and not op.addr(0)) and not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(5) and not op.code(3) and op.code(2) and op.code(1) and op.code(0)));
    imp_bit_67 <= ((not op.addr(1) and not op.addr(0)) and not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(4) and not op.code(3) and op.code(2) and op.code(1) and op.code(0)));
    imp_bit_68 <= (not p(0) and not op.addr(1) and (not op.code(15) and not op.code(14) and op.code(13) and not op.code(12) and not op.code(3) and not op.code(2) and op.code(1) and op.code(0)));
    imp_bit_69 <= (op.addr(0) and not p(0) and (not op.code(15) and not op.code(14) and op.code(13) and not op.code(12) and not op.code(3) and not op.code(2) and op.code(1) and op.code(0)));
    imp_bit_70 <= (not op.addr(0) and not p(0) and (op.code(15) and not op.code(14) and not op.code(13) and not op.code(12) and op.code(11) and op.code(10) and not op.code(9) and op.code(8)));
    imp_bit_71 <= (not p(0) and (op.code(15) and not op.code(13) and not op.code(12) and op.code(11) and not op.code(10) and not op.code(9) and not op.code(8)));
    imp_bit_72 <= (not p(0) and (not op.code(15) and not op.code(14) and not op.code(13) and not op.code(12) and op.code(2) and not op.code(1)));
    imp_bit_73 <= (not p(0) and (op.code(15) and op.code(14) and not op.code(13) and op.code(12)));
    imp_bit_74 <= (not p(0) and (not op.code(15) and not op.code(14) and not op.code(13) and op.code(12)));
    imp_bit_75 <= (not p(0) and (not op.code(15) and op.code(14) and op.code(13) and op.code(12)));
    imp_bit_76 <= (p(0) and (not op.code(10) and op.code(9) and op.code(8)) and (op.addr(2) and not op.addr(1) and op.addr(0)));
    imp_bit_77 <= ((not op.addr(1) and op.addr(0)) and not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(4) and not op.code(3) and op.code(2) and op.code(1) and op.code(0)));
    imp_bit_78 <= ((not op.addr(1) and not op.addr(0)) and not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(5) and not op.code(3) and op.code(2) and op.code(1) and op.code(0)));
    imp_bit_79 <= ((not op.addr(1) and not op.addr(0)) and not p(0) and (op.code(15) and not op.code(14) and not op.code(13) and not op.code(12) and op.code(11) and not op.code(10) and op.code(9) and op.code(8)));
    imp_bit_80 <= (not p(0) and not op.addr(1) and (not op.code(15) and not op.code(14) and not op.code(13) and not op.code(12) and op.code(3) and op.code(2) and op.code(1) and op.code(0)));
    imp_bit_81 <= (not op.addr(0) and not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and op.code(3) and op.code(2) and op.code(1) and op.code(0)));
    imp_bit_82 <= (not p(0) and not op.addr(1) and (op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(11) and not op.code(10) and op.code(9) and op.code(8)));
    imp_bit_83 <= (not p(0) and (op.code(15) and not op.code(14) and op.code(13) and op.code(12)) and not op.addr(0));
    imp_bit_84 <= ((not op.addr(1) and op.addr(0)) and not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(5) and not op.code(3) and op.code(2) and op.code(1) and op.code(0)));
    imp_bit_85 <= ((not op.addr(1) and not op.addr(0)) and not p(0) and (not op.code(15) and not op.code(14) and op.code(13) and not op.code(12) and not op.code(3) and not op.code(2) and op.code(1) and op.code(0)));
    imp_bit_86 <= ((op.addr(1) and op.addr(0)) and not p(0) and (not op.code(15) and not op.code(14) and op.code(13) and not op.code(12) and not op.code(3) and not op.code(2) and op.code(1) and op.code(0)));
    imp_bit_87 <= ((not op.addr(1) and not op.addr(0)) and not p(0) and (op.code(15) and not op.code(14) and not op.code(13) and not op.code(12) and op.code(11) and not op.code(10) and not op.code(9) and op.code(8)));
    imp_bit_88 <= (op.addr(0) and not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and op.code(3) and op.code(2) and op.code(1) and op.code(0)));
    imp_bit_89 <= (not p(0) and not op.addr(2) and (op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(11) and not op.code(10) and op.code(9) and op.code(8)));
    imp_bit_90 <= (p(0) and (op.code(9) and op.code(8)) and not op.addr(2));
    imp_bit_91 <= (not op.addr(0) and p(0) and (op.code(10) and op.code(9)));
    imp_bit_92 <= ((not op.addr(1) and not op.addr(0)) and not p(0) and (not op.code(15) and not op.code(14) and not op.code(13) and not op.code(12) and op.code(3) and op.code(2) and op.code(1) and op.code(0)));
    imp_bit_93 <= ((not op.addr(1) and op.addr(0)) and not p(0) and (not op.code(15) and not op.code(14) and op.code(13) and not op.code(12) and not op.code(3) and not op.code(2) and op.code(1) and op.code(0)));
    imp_bit_94 <= (not p(0) and (op.code(15) and not op.code(13) and op.code(12)));
    imp_bit_95 <= ((not op.addr(1) and op.addr(0)) and not p(0) and (not op.code(15) and not op.code(14) and not op.code(13) and not op.code(12) and op.code(3) and op.code(2) and op.code(1) and op.code(0)));
    imp_bit_96 <= (p(0) and (op.code(10) and op.code(9)) and not op.addr(2));
    imp_bit_97 <= (not p(0) and (not op.code(15) and not op.code(13) and op.code(12)));
    imp_bit_98 <= (not p(0) and (not op.addr(2) and not op.addr(1)) and (op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(11) and not op.code(10) and op.code(9) and op.code(8)));
    imp_bit_99 <= (p(0) and (not op.code(10) and not op.code(9)) and not op.addr(3));
    imp_bit_100 <= ((not op.addr(2) and op.addr(1) and not op.addr(0)) and not p(0) and (op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(11) and not op.code(10) and op.code(9) and op.code(8)));
    imp_bit_101 <= (not op.addr(0) and not p(0) and (op.code(15) and not op.code(14) and op.code(13)));
    imp_bit_102 <= (not p(0) and (op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and op.code(11) and not op.code(10) and op.code(9)));
    imp_bit_103 <= (not p(0) and (op.code(15) and not op.code(14) and op.code(13)) and op.addr(0));
    imp_bit_104 <= (not p(0) and (not op.code(15) and not op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(4) and not op.code(2) and op.code(1) and not op.code(0)));
    imp_bit_105 <= (not p(0) and (op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(11) and not op.code(10) and not op.code(8)));
    imp_bit_106 <= (not p(0) and (op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and op.code(11) and not op.code(10) and op.code(8)));
    imp_bit_107 <= ((op.addr(2) and not op.addr(1) and not op.addr(0)) and not p(0) and (op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(11) and not op.code(10) and op.code(9) and op.code(8)));
    imp_bit_108 <= (p(0) and (not op.addr(2) and op.addr(1) and not op.addr(0)) and (op.code(10) and op.code(9)));
    imp_bit_109 <= ((not op.addr(3) and op.addr(0)) and p(0) and (not op.code(10) and not op.code(9)));
    imp_bit_110 <= (not p(0) and (not op.code(15) and not op.code(14) and not op.code(13) and not op.code(12) and not op.code(11) and not op.code(10) and not op.code(9) and not op.code(8) and not op.code(7) and not op.code(6) and op.code(5) and not op.code(4) and op.code(3) and not op.code(2) and not op.code(1) and not op.code(0)));
    imp_bit_111 <= (not p(0) and (not op.code(15) and not op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(5) and not op.code(2) and op.code(1) and not op.code(0)));
    imp_bit_112 <= (not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(4) and op.code(3) and op.code(1) and not op.code(0)));
    imp_bit_113 <= (not p(0) and (op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(11) and op.code(10) and not op.code(8)));
    imp_bit_114 <= ((not op.addr(2) and not op.addr(1) and not op.addr(0)) and p(0) and (op.code(10) and op.code(9)));
    imp_bit_115 <= (p(0) and (not op.addr(2) and op.addr(1) and op.addr(0)) and (op.code(10) and op.code(9)));
    imp_bit_116 <= (p(0) and (not op.code(10) and not op.code(9)) and (not op.addr(3) and not op.addr(2)));
    imp_bit_117 <= (not op.addr(0) and not p(0) and (op.code(15) and not op.code(14) and not op.code(13) and not op.code(12) and op.code(11) and op.code(10) and op.code(8)));
    imp_bit_118 <= (not op.addr(0) and not p(0) and (op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and op.code(11) and op.code(10) and op.code(9)));
    imp_bit_119 <= (not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(5) and op.code(3) and op.code(1) and not op.code(0)));
    imp_bit_120 <= (p(0) and (not op.addr(2) and not op.addr(1) and op.addr(0)) and (op.code(10) and op.code(9)));
    imp_bit_121 <= (not p(0) and op.addr(1) and (not op.code(15) and not op.code(14) and not op.code(13) and not op.code(12) and not op.code(11) and not op.code(10) and not op.code(9) and not op.code(8) and not op.code(7) and not op.code(6) and op.code(5) and not op.code(4) and op.code(3) and not op.code(2) and op.code(1) and op.code(0)));
    imp_bit_122 <= (op.addr(0) and not p(0) and (op.code(15) and not op.code(14) and not op.code(13) and not op.code(12) and op.code(11) and op.code(10) and op.code(8)));
    imp_bit_123 <= (not p(0) and not op.addr(1) and (op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and op.code(11) and op.code(10) and op.code(9)));
    imp_bit_124 <= (not op.addr(0) and not p(0) and (op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and op.code(11) and op.code(10) and op.code(8)));
    imp_bit_125 <= (p(0) and not op.addr(1) and op.code(9));
    imp_bit_126 <= (not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(5) and not op.code(3) and op.code(1) and not op.code(0)));
    imp_bit_127 <= (p(0) and (not op.addr(3) and not op.addr(2) and op.addr(1)) and (not op.code(10) and not op.code(9)));
    imp_bit_128 <= ((op.addr(2) and not op.addr(1) and op.addr(0)) and p(0) and (op.code(10) and op.code(9)));
    imp_bit_129 <= ((not op.addr(1) and not op.addr(0)) and not p(0) and (op.code(15) and not op.code(14) and not op.code(13) and not op.code(12) and op.code(11) and not op.code(10) and op.code(8)));
    imp_bit_130 <= ((op.addr(1) and not op.addr(0)) and not p(0) and (op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and op.code(11) and op.code(10) and op.code(9)));
    imp_bit_131 <= (not p(0) and not op.addr(1) and (not op.code(15) and not op.code(14) and not op.code(13) and not op.code(12) and not op.code(11) and not op.code(10) and not op.code(9) and not op.code(8) and not op.code(7) and not op.code(6) and op.code(5) and not op.code(4) and op.code(3) and not op.code(2) and op.code(1) and op.code(0)));
    imp_bit_132 <= (not op.addr(0) and not p(0) and (not op.code(15) and not op.code(14) and not op.code(13) and not op.code(12) and not op.code(11) and not op.code(10) and not op.code(9) and not op.code(8) and not op.code(7) and not op.code(6) and not op.code(5) and not op.code(4) and op.code(3) and not op.code(2) and op.code(1) and op.code(0)));
    imp_bit_133 <= (not op.addr(0) and not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(4) and not op.code(2) and op.code(1) and op.code(0)));
    imp_bit_134 <= (not p(0) and not op.addr(1) and (op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and op.code(11) and op.code(10) and op.code(8)));
    imp_bit_135 <= (not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(5) and not op.code(4) and op.code(3) and not op.code(2) and op.code(1) and not op.code(0)));
    imp_bit_136 <= (not p(0) and (not op.code(15) and not op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and op.code(5) and not op.code(4) and op.code(3) and not op.code(2) and not op.code(1) and op.code(0)));
    imp_bit_137 <= (not p(0) and (op.code(15) and not op.code(13) and not op.code(12) and not op.code(11) and not op.code(10) and not op.code(9)));
    imp_bit_138 <= ((op.addr(1) and not op.addr(0)) and not p(0) and (not op.code(15) and not op.code(14) and not op.code(13) and not op.code(12) and not op.code(11) and not op.code(10) and not op.code(9) and not op.code(8) and not op.code(7) and not op.code(6) and op.code(5) and not op.code(4) and op.code(3) and not op.code(2) and op.code(1) and op.code(0)));
    imp_bit_139 <= ((not op.addr(1) and op.addr(0)) and not p(0) and (op.code(15) and not op.code(14) and not op.code(13) and not op.code(12) and op.code(11) and not op.code(10) and op.code(8)));
    imp_bit_140 <= ((op.addr(1) and not op.addr(0)) and not p(0) and (op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and op.code(11) and op.code(10) and op.code(8)));
    imp_bit_141 <= (op.addr(0) and not p(0) and (not op.code(15) and not op.code(14) and not op.code(13) and not op.code(12) and not op.code(11) and not op.code(10) and not op.code(9) and not op.code(8) and not op.code(7) and not op.code(6) and not op.code(5) and not op.code(4) and op.code(3) and not op.code(2) and op.code(1) and op.code(0)));
    imp_bit_142 <= (not op.addr(0) and not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(5) and not op.code(2) and op.code(1) and op.code(0)));
    imp_bit_143 <= (not p(0) and (not op.code(15) and not op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(5) and op.code(4) and not op.code(3) and not op.code(2) and op.code(1) and not op.code(0)));
    imp_bit_144 <= (not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(5) and not op.code(4) and op.code(3) and op.code(2) and op.code(1) and not op.code(0)));
    imp_bit_145 <= (not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(5) and op.code(4) and op.code(3) and not op.code(2) and op.code(1) and not op.code(0)));
    imp_bit_146 <= (not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(5) and op.code(3) and not op.code(2) and not op.code(1)));
    imp_bit_147 <= (not p(0) and (not op.code(15) and not op.code(14) and op.code(13) and op.code(12) and op.code(3) and op.code(1) and not op.code(0)));
    imp_bit_148 <= (not p(0) and (op.code(15) and not op.code(13) and not op.code(12) and not op.code(11) and op.code(10) and not op.code(9)));
    imp_bit_149 <= (p(0) and (not op.addr(3) and op.addr(2) and op.addr(0)) and (not op.code(10) and not op.code(9)));
    imp_bit_150 <= ((op.addr(1) and op.addr(0)) and not p(0) and (not op.code(15) and not op.code(14) and not op.code(13) and not op.code(12) and not op.code(11) and not op.code(10) and not op.code(9) and not op.code(8) and not op.code(7) and not op.code(6) and op.code(5) and not op.code(4) and op.code(3) and not op.code(2) and op.code(1) and op.code(0)));
    imp_bit_151 <= ((not op.addr(1) and op.addr(0)) and not p(0) and (op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and op.code(11) and op.code(10) and op.code(9)));
    imp_bit_152 <= (not p(0) and not op.addr(1) and (not op.code(15) and not op.code(14) and not op.code(13) and not op.code(12) and not op.code(11) and not op.code(10) and not op.code(9) and not op.code(8) and not op.code(7) and not op.code(6) and not op.code(5) and op.code(4) and op.code(3) and not op.code(2) and op.code(1) and op.code(0)));
    imp_bit_153 <= (not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(5) and not op.code(4) and not op.code(3) and op.code(2) and op.code(1) and not op.code(0)));
    imp_bit_154 <= ((not op.addr(3) and not op.addr(2) and not op.addr(1) and not op.addr(0)) and p(0) and (not op.code(10) and not op.code(9)));
    imp_bit_155 <= (p(0) and (not op.code(10) and not op.code(9)) and (not op.addr(3) and op.addr(2) and op.addr(1) and not op.addr(0)));
    imp_bit_156 <= ((not op.addr(1) and op.addr(0)) and not p(0) and (op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and op.code(11) and op.code(10) and op.code(8)));
    imp_bit_157 <= (not op.addr(0) and not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and op.code(5) and not op.code(4) and not op.code(3) and op.code(2) and op.code(1) and not op.code(0)));
    imp_bit_158 <= (not op.addr(0) and not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(5) and not op.code(4) and op.code(3) and not op.code(2) and op.code(1) and op.code(0)));
    imp_bit_159 <= (not op.addr(0) and not p(0) and (not op.code(15) and not op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(5) and not op.code(4) and not op.code(3) and not op.code(2) and op.code(1) and op.code(0)));
    imp_bit_160 <= (not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(5) and op.code(4) and not op.code(3) and op.code(2) and op.code(1) and not op.code(0)));
    imp_bit_161 <= (not p(0) and (not op.code(15) and op.code(14) and op.code(13) and not op.code(12) and op.code(3) and not op.code(2) and op.code(1)));
    imp_bit_162 <= (not p(0) and (not op.code(15) and not op.code(14) and op.code(13) and not op.code(12) and op.code(3) and op.code(2) and op.code(1)));
    ex.arith_func <= SUB when (imp_bit_55 or imp_bit_39 or imp_bit_34 or imp_bit_29 or imp_bit_61 or imp_bit_161 or (not p(0) and (not op.code(15) and not op.code(14) and op.code(13) and op.code(12) and op.code(3) and not op.code(2) and not op.code(0))) or (not p(0) and (not op.code(15) and not op.code(14) and op.code(13) and op.code(12) and not op.code(2) and op.code(1))) or imp_bit_22 or imp_bit_19 or (not p(0) and (not op.code(15) and not op.code(14) and op.code(13) and not op.code(12) and not op.code(3) and op.code(2) and not op.code(1))) or imp_bit_98 or imp_bit_120 or imp_bit_108 or (p(0) and (not op.addr(3) and not op.addr(2) and not op.addr(0)) and (not op.code(10) and not op.code(9))) or imp_bit_149 or imp_bit_30 or (not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(5) and op.code(4) and not op.code(3) and not op.code(2) and not op.code(0))) or imp_bit_127 or imp_bit_5 or (p(0) and (not op.addr(2) and not op.addr(1)) and (op.code(10) and op.code(9) and op.code(8)))) = '1' else ADD;
    cond1 <= imp_bit_56 & imp_bit_14 & (imp_bit_60 or imp_bit_73 or imp_bit_1);
    with cond1 select
        ex.aluinx_sel <=
            SEL_ROTCL when "100",
            SEL_ZERO when "010",
            SEL_FC when "001",
            SEL_XBUS when others;
    wb_stall.wrmach <= imp_bit_153;
    cond3 <= (not p(0) and (not op.code(15) and op.code(14) and op.code(13) and not op.code(12) and op.code(3) and op.code(2) and op.code(1) and op.code(0))) & (not p(0) and (not op.code(15) and op.code(14) and op.code(13) and not op.code(12) and op.code(3) and not op.code(2) and not op.code(1) and op.code(0))) & (not p(0) and (not op.code(15) and op.code(14) and op.code(13) and not op.code(12) and op.code(3) and op.code(2) and not op.code(1) and not op.code(0))) & (not p(0) and (not op.code(15) and not op.code(14) and op.code(13) and not op.code(12) and op.code(3) and op.code(2) and not op.code(1) and op.code(0))) & (not p(0) and (not op.code(15) and op.code(14) and op.code(13) and not op.code(12) and op.code(3) and op.code(2) and not op.code(1) and op.code(0))) & (not p(0) and (not op.code(15) and op.code(14) and op.code(13) and not op.code(12) and op.code(3) and op.code(2) and op.code(1) and not op.code(0))) & imp_bit_14;
    with cond3 select
        ex.alumanip <=
            EXTEND_SWORD when "1000000",
            SWAP_WORD when "0100000",
            EXTEND_UBYTE when "0010000",
            EXTRACT when "0001000",
            EXTEND_UWORD when "0000100",
            EXTEND_SBYTE when "0000010",
            SET_BIT_7 when "0000001",
            SWAP_BYTE when others;
    ilevel_cap <= imp_bit_154;
    cond5 <= (imp_bit_72 or imp_bit_62) & (imp_bit_131 or imp_bit_39 or imp_bit_133 or imp_bit_126 or imp_bit_29 or imp_bit_84 or imp_bit_77 or imp_bit_3 or imp_bit_63 or imp_bit_44 or imp_bit_80 or imp_bit_57 or (not p(0) and (not op.code(15) and op.code(13) and not op.code(12) and not op.code(3) and op.code(2) and not op.code(1))) or (not p(0) and (not op.code(15) and op.code(13) and not op.code(12) and not op.code(3) and op.code(2) and not op.code(0))) or imp_bit_18 or imp_bit_97 or imp_bit_0 or imp_bit_24 or imp_bit_98 or (p(0) and (not op.addr(2) and not op.addr(1)) and (op.code(10) and op.code(9))) or ((not op.addr(2) and not op.addr(0)) and p(0) and (op.code(10) and op.code(9))) or imp_bit_46 or imp_bit_116 or imp_bit_109 or imp_bit_30);
    with cond5 select
        ex.aluiny_sel <=
            SEL_R0 when "10",
            SEL_IMM when "01",
            SEL_YBUS when others;
    mac_stall_sense <= (imp_bit_110 or (not p(0) and (not op.code(15) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(5) and op.code(3) and not op.code(2) and op.code(1) and not op.code(0))) or imp_bit_126 or imp_bit_6 or imp_bit_58 or imp_bit_162);
    cond7 <= imp_bit_53 & imp_bit_70 & (imp_bit_131 or imp_bit_27 or imp_bit_152 or imp_bit_28 or imp_bit_11 or imp_bit_142 or imp_bit_133 or imp_bit_49 or imp_bit_43 or imp_bit_157 or imp_bit_48 or imp_bit_68 or imp_bit_51 or imp_bit_80 or imp_bit_81 or imp_bit_20 or imp_bit_17 or imp_bit_101 or imp_bit_23 or imp_bit_89 or ((not op.addr(1) and not op.addr(0)) and not p(0) and (op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(11) and not op.code(10) and op.code(9) and op.code(8))) or (p(0) and not op.addr(1) and (op.code(10) and op.code(9))) or imp_bit_96 or imp_bit_90 or ((not op.addr(1) and not op.addr(0)) and p(0) and op.code(9)) or imp_bit_116 or (p(0) and (not op.addr(3) and not op.addr(1)) and (not op.code(10) and not op.code(9))) or ((not op.addr(3) and not op.addr(0)) and p(0) and (not op.code(10) and not op.code(9))) or (p(0) and not op.addr(1) and (op.code(9) and not op.code(8))));
    with cond7 select
        id.if_issue <=
            t_bcc when "100",
            not t_bcc when "010",
            '0' when "001",
            '1' when others;
    ex_stall.wrmach <= (imp_bit_110 or imp_bit_135);
    ex.arith_ci_en <= (imp_bit_147 or imp_bit_37);
    cond10 <= (imp_bit_31 or imp_bit_60 or imp_bit_129 or imp_bit_117 or imp_bit_101 or imp_bit_94 or imp_bit_114 or imp_bit_154 or imp_bit_30) & ((not p(0) and (not op.code(15) and not op.code(14) and not op.code(13) and not op.code(12) and not op.code(11) and not op.code(10) and not op.code(9) and not op.code(8) and not op.code(7) and not op.code(6) and not op.code(5) and op.code(3) and not op.code(2) and not op.code(1))) or imp_bit_121 or (not p(0) and (not op.code(15) and not op.code(14) and not op.code(13) and not op.code(12) and not op.code(11) and not op.code(10) and not op.code(9) and not op.code(8) and not op.code(7) and not op.code(6) and not op.code(5) and op.code(3) and not op.code(2) and op.code(0))) or (not p(0) and (not op.code(15) and not op.code(14) and not op.code(13) and not op.code(12) and not op.code(11) and not op.code(10) and not op.code(9) and not op.code(8) and not op.code(7) and not op.code(6) and op.code(4) and op.code(3) and not op.code(2) and op.code(1) and op.code(0))) or imp_bit_136 or imp_bit_111 or imp_bit_104 or (op.addr(0) and not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(5) and not op.code(2) and op.code(1) and op.code(0))) or imp_bit_119 or imp_bit_112 or (op.addr(0) and not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(4) and not op.code(2) and op.code(1) and op.code(0))) or imp_bit_66 or imp_bit_47 or (op.addr(0) and not p(0) and (not op.code(15) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(4) and not op.code(3) and not op.code(2) and op.code(1) and op.code(0))) or imp_bit_85 or imp_bit_8 or imp_bit_41 or imp_bit_9 or ((op.addr(1) and not op.addr(0)) and not p(0) and (not op.code(15) and not op.code(14) and not op.code(13) and not op.code(12) and op.code(3) and op.code(2) and op.code(1) and op.code(0))) or imp_bit_139 or ((op.addr(1) and not op.addr(0)) and not p(0) and (op.code(15) and not op.code(14) and not op.code(13) and not op.code(12) and op.code(11) and not op.code(10) and op.code(8))) or imp_bit_122 or imp_bit_103 or ((not op.addr(1) and op.addr(0)) and not p(0) and (op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and op.code(11) and op.code(10) and not op.code(9) and not op.code(8))) or (not p(0) and (not op.addr(2) and op.addr(1)) and (op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(11) and not op.code(10) and op.code(9) and op.code(8))) or (not p(0) and (op.addr(2) and not op.addr(1)) and (op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(11) and not op.code(10) and op.code(9) and op.code(8))) or imp_bit_45 or (p(0) and (op.code(10) and op.code(9)) and (op.addr(1) and op.addr(0))) or (p(0) and (op.code(10) and op.code(9)) and op.addr(2)) or (p(0) and (not op.code(10) and op.code(9) and op.code(8)) and (not op.addr(2) and not op.addr(1))) or (p(0) and (not op.code(10) and op.code(9)) and not op.addr(0)) or imp_bit_155 or (p(0) and (not op.code(10) and not op.code(9)) and (op.addr(3) and not op.addr(2) and not op.addr(1) and not op.addr(0))) or ((op.addr(1) and not op.addr(0)) and not p(0) and (op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(11) and not op.code(10) and op.code(9) and op.code(8))) or (p(0) and (not op.code(10) and op.code(9) and not op.code(8)) and op.addr(1)));
    with cond10 select
        ex.xbus_sel <=
            SEL_PC when "10",
            SEL_IMM when "01",
            SEL_REG when others;
    cond11 <= (imp_bit_87 or imp_bit_70) & (imp_bit_79 or imp_bit_53) & (imp_bit_138 or imp_bit_132 or imp_bit_32 or imp_bit_31 or imp_bit_101 or imp_bit_107 or imp_bit_114 or imp_bit_128 or imp_bit_65 or imp_bit_154 or imp_bit_155 or imp_bit_30);
    with cond11 select
        ex_stall.wrpc_z <=
            t_bcc when "100",
            not t_bcc when "010",
            '1' when "001",
            '0' when others;
    cond12 <= imp_bit_46 & imp_bit_25 & imp_bit_157 & (imp_bit_148 or imp_bit_113) & imp_bit_15 & imp_bit_16 & imp_bit_21;
    with cond12 select
        wb.regnum_w <=
            "01111" when "1000000",
            "10100" when "0100000",
            "10010" when "0010000",
            "00000" when "0001000",
            "10011" when "0000100",
            "10001" when "0000010",
            "10000" when "0000001",
            '0' & op.code(11 downto 8) when others;
    cond13 <= (not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and op.code(5) and not op.code(4) and not op.code(3) and op.code(2) and not op.code(1))) & (not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(5) and not op.code(4) and not op.code(3) and op.code(2) and not op.code(1))) & ((not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and op.code(5) and not op.code(4) and not op.code(3) and not op.code(2) and not op.code(1) and op.code(0))) or (not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and op.code(3) and op.code(2) and not op.code(1) and not op.code(0))));
    with cond13 select
        ex_stall.shiftfunc <=
            ROTC when "100",
            ROTATE when "010",
            ARITH when "001",
            LOGIC when others;
    cond14 <= (not p(0) and (not op.code(15) and not op.code(14) and op.code(13) and op.code(12) and not op.code(3) and op.code(2) and not op.code(1) and op.code(0))) & (not p(0) and (not op.code(15) and not op.code(14) and op.code(13) and op.code(12) and op.code(3) and op.code(2) and not op.code(1) and op.code(0))) & (not p(0) and (not op.code(15) and not op.code(14) and op.code(13) and not op.code(12) and op.code(3) and op.code(2) and op.code(1) and not op.code(0))) & imp_bit_58 & (not p(0) and (not op.code(15) and not op.code(14) and op.code(13) and not op.code(12) and op.code(3) and op.code(2) and op.code(1) and op.code(0)));
    with cond14 select
        ex_stall.mulcom2 <=
            DMULUL when "10000",
            DMULSL when "01000",
            MULUW when "00100",
            MULL when "00010",
            MULSW when "00001",
            NOP when others;
    cond15 <= imp_bit_2 & (imp_bit_46 or imp_bit_127) & (imp_bit_69 or imp_bit_10 or imp_bit_71) & (imp_bit_110 or imp_bit_24 or imp_bit_76) & ((not p(0) and (op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(11) and not op.code(9))) or imp_bit_0 or imp_bit_134 or imp_bit_25 or imp_bit_123) & (imp_bit_131 or imp_bit_98 or imp_bit_120 or imp_bit_108 or ((not op.addr(3) and not op.addr(1) and op.addr(0)) and p(0) and (not op.code(10) and not op.code(9))) or imp_bit_149) & (imp_bit_95 or imp_bit_88 or (not p(0) and (not op.code(15) and op.code(14) and op.code(13) and not op.code(12) and not op.code(3) and op.code(2) and not op.code(1))) or (not p(0) and (not op.code(15) and op.code(14) and op.code(13) and not op.code(12) and not op.code(3) and op.code(2) and not op.code(0))) or imp_bit_13 or imp_bit_7 or (not p(0) and (op.code(15) and not op.code(14) and not op.code(13) and not op.code(12) and not op.code(11) and not op.code(9))) or (not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and op.code(12))));
    with cond15 select
        ex.regnum_x <=
            "10001" when "1000000",
            "10011" when "0100000",
            "00000" when "0010000",
            "10100" when "0001000",
            "10000" when "0000100",
            "01111" when "0000010",
            '0' & op.code(7 downto 4) when "0000001",
            '0' & op.code(11 downto 8) when others;
    cond16 <= (imp_bit_79 or imp_bit_53) & (imp_bit_87 or imp_bit_70) & (imp_bit_131 or imp_bit_27 or imp_bit_152 or imp_bit_28 or imp_bit_11 or imp_bit_142 or imp_bit_133 or imp_bit_49 or imp_bit_43 or imp_bit_157 or imp_bit_48 or imp_bit_68 or imp_bit_51 or imp_bit_80 or imp_bit_81 or imp_bit_20 or imp_bit_17 or imp_bit_139 or imp_bit_101 or imp_bit_23 or imp_bit_89 or imp_bit_82 or imp_bit_125 or imp_bit_91 or imp_bit_96 or imp_bit_90 or imp_bit_99 or imp_bit_54);
    with cond16 select
        dispatch <=
            t_bcc when "100",
            not t_bcc when "010",
            '0' when "001",
            '1' when others;
    ex_stall.mulcom1 <= (imp_bit_6 or imp_bit_58 or imp_bit_162);
    cond18 <= imp_bit_88 & imp_bit_95;
    with cond18 select
        wb_stall.mulcom2 <=
            MACW when "10",
            MACL when "01",
            NOP when others;
    cond19 <= ((not p(0) and (not op.code(15) and op.code(13) and not op.code(12) and not op.code(3) and not op.code(2) and not op.code(1) and op.code(0))) or imp_bit_57 or (op.addr(0) and not p(0) and (not op.code(15) and op.code(14) and op.code(13) and not op.code(12) and not op.code(3) and op.code(2) and not op.code(1) and op.code(0))) or (not p(0) and (not op.code(15) and not op.code(14) and not op.code(13) and not op.code(12) and op.code(2) and not op.code(1) and op.code(0))) or (not p(0) and (op.code(15) and not op.code(13) and not op.code(12) and not op.code(11) and not op.code(9) and op.code(8))) or imp_bit_64 or (not p(0) and (not op.code(15) and not op.code(14) and op.code(13) and not op.code(12) and not op.code(3) and not op.code(1) and op.code(0)))) & (imp_bit_4 or (not p(0) and (not op.code(15) and op.code(13) and not op.code(12) and not op.code(3) and not op.code(2) and not op.code(1) and not op.code(0))) or (op.addr(0) and not p(0) and (not op.code(15) and op.code(14) and op.code(13) and not op.code(12) and not op.code(3) and op.code(2) and not op.code(1) and not op.code(0))) or (not p(0) and (not op.code(15) and not op.code(14) and not op.code(13) and not op.code(12) and op.code(2) and not op.code(1) and not op.code(0))) or (not p(0) and (op.code(15) and not op.code(13) and not op.code(12) and not op.code(11) and not op.code(9) and not op.code(8))) or imp_bit_124 or imp_bit_25 or imp_bit_118 or (not p(0) and (not op.code(15) and not op.code(14) and op.code(13) and not op.code(12) and not op.code(3) and not op.code(1) and not op.code(0))));
    with cond19 select
        ex.mem_size <=
            WORD when "10",
            BYTE when "01",
            LONG when others;
    cond20 <= (imp_bit_110 or imp_bit_76) & (imp_bit_143 or imp_bit_12) & (imp_bit_137 or imp_bit_105 or imp_bit_134 or imp_bit_25 or imp_bit_123) & (imp_bit_132 or (not p(0) and (not op.code(15) and not op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and op.code(5) and not op.code(4) and op.code(3) and not op.code(2) and op.code(1) and not op.code(0))) or (not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and op.code(5) and not op.code(4) and not op.code(3) and not op.code(2) and op.code(1) and not op.code(0)))) & ((not p(0) and (not op.code(15) and not op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and op.code(5) and not op.code(4) and not op.code(3) and not op.code(2) and op.code(1) and not op.code(0))) or (not op.addr(0) and not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and op.code(5) and not op.code(4) and not op.code(3) and not op.code(2) and op.code(1) and op.code(0))) or imp_bit_100 or imp_bit_115) & (imp_bit_119 or imp_bit_112 or imp_bit_31 or imp_bit_85) & (imp_bit_14 or imp_bit_86 or imp_bit_140 or imp_bit_130);
    with cond20 select
        ex.regnum_y <=
            "10100" when "1000000",
            "10000" when "0100000",
            "00000" when "0010000",
            "10010" when "0001000",
            "10001" when "0000100",
            '0' & op.code(11 downto 8) when "0000010",
            "10011" when "0000001",
            '0' & op.code(7 downto 4) when others;
    event_ack_0 <= (((not op.addr(2) and not op.addr(1) and op.addr(0)) and p(0) and (not op.code(10) and op.code(9) and op.code(8))) or imp_bit_154);
    wb_stall.mulcom1 <= (imp_bit_92 or imp_bit_81);
    cond23 <= (not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(5) and op.code(4) and op.code(3) and op.code(2) and op.code(1) and not op.code(0))) & ((not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and op.code(5) and not op.code(4) and op.code(3) and op.code(2) and op.code(1) and not op.code(0))) or imp_bit_76) & (imp_bit_60 or imp_bit_106 or imp_bit_102) & (imp_bit_95 or imp_bit_88 or imp_bit_20 or imp_bit_17) & ((not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and op.code(5) and not op.code(4) and op.code(3) and not op.code(2) and op.code(1) and not op.code(0))) or imp_bit_158 or imp_bit_159 or imp_bit_83) & (imp_bit_131 or imp_bit_98 or imp_bit_120 or imp_bit_108 or imp_bit_149) & (imp_bit_85 or imp_bit_156 or imp_bit_151 or imp_bit_42 or ((not op.addr(3) and not op.addr(2) and op.addr(0)) and p(0) and (not op.code(10) and not op.code(9))) or imp_bit_127);
    with cond23 select
        ex.regnum_z <=
            "10000" when "1000000",
            "10001" when "0100000",
            "00000" when "0010000",
            '0' & op.code(7 downto 4) when "0001000",
            "10010" when "0000100",
            "01111" when "0000010",
            "10011" when "0000001",
            '0' & op.code(11 downto 8) when others;
    ex_stall.mem_wdata_sel <= SEL_ZBUS when (imp_bit_14 or imp_bit_140 or imp_bit_130) = '1' else SEL_YBUS;
    cond25 <= imp_bit_33 & (imp_bit_95 or imp_bit_88) & (imp_bit_6 or imp_bit_58 or imp_bit_162);
    with cond25 select
        mac_busy <=
            WB_NOT_STALL when "100",
            WB_BUSY when "010",
            EX_NOT_STALL when "001",
            NOT_BUSY when others;
    slp <= ((not op.addr(1) and op.addr(0)) and not p(0) and (not op.code(15) and not op.code(14) and not op.code(13) and not op.code(12) and not op.code(11) and not op.code(10) and not op.code(9) and not op.code(8) and not op.code(7) and not op.code(6) and not op.code(5) and op.code(4) and op.code(3) and not op.code(2) and op.code(1) and op.code(0)));
    cond27 <= imp_bit_86 & (imp_bit_131 or imp_bit_50 or imp_bit_34 or imp_bit_126 or imp_bit_29 or imp_bit_78 or imp_bit_67 or imp_bit_157 or imp_bit_93 or (not p(0) and (not op.code(15) and op.code(13) and not op.code(12) and not op.code(3) and not op.code(2) and not op.code(1))) or (not p(0) and (not op.code(15) and op.code(13) and not op.code(12) and not op.code(3) and not op.code(2) and not op.code(0))) or imp_bit_80 or imp_bit_57 or imp_bit_22 or imp_bit_19 or imp_bit_72 or imp_bit_62 or imp_bit_18 or (not p(0) and (not op.code(13) and op.code(12))) or imp_bit_0 or imp_bit_124 or imp_bit_25 or imp_bit_118 or imp_bit_98 or ((not op.addr(2) and not op.addr(0)) and not p(0) and (op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(11) and not op.code(10) and op.code(9) and op.code(8))) or ((not op.addr(2) and op.addr(0)) and p(0) and (op.code(10) and op.code(9))) or (p(0) and (not op.addr(2) and op.addr(1)) and (op.code(10) and op.code(9))) or (p(0) and (not op.addr(2) and op.addr(1)) and (op.code(9) and op.code(8))) or imp_bit_127 or imp_bit_2 or imp_bit_63 or imp_bit_44);
    with cond27 select
        ex_stall.ma_issue <=
            t_bcc when "10",
            '1' when "01",
            '0' when others;
    ex.ma_wr <= (imp_bit_14 or imp_bit_39 or imp_bit_34 or (not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(5) and not op.code(3) and not op.code(2) and op.code(1) and not op.code(0))) or imp_bit_29 or imp_bit_86 or imp_bit_63 or imp_bit_44 or (not p(0) and (not op.code(15) and not op.code(14) and not op.code(12) and not op.code(3) and op.code(2) and not op.code(1))) or (not p(0) and (not op.code(15) and not op.code(14) and not op.code(12) and not op.code(3) and op.code(2) and not op.code(0))) or imp_bit_137 or imp_bit_74 or imp_bit_105 or imp_bit_140 or imp_bit_130 or imp_bit_98 or imp_bit_120 or imp_bit_108 or imp_bit_127);
    id.ifadsel <= (imp_bit_150 or imp_bit_141 or imp_bit_35 or imp_bit_36 or imp_bit_139 or imp_bit_122 or imp_bit_103 or ((op.addr(2) and not op.addr(1) and op.addr(0)) and not p(0) and (op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(11) and not op.code(10) and op.code(9) and op.code(8))) or ((op.addr(2) and op.addr(1) and not op.addr(0)) and p(0) and (op.code(10) and op.code(9))) or imp_bit_76 or ((not op.addr(3) and op.addr(2) and op.addr(1) and op.addr(0)) and p(0) and (not op.code(10) and not op.code(9))) or ((op.addr(1) and not op.addr(0)) and p(0) and (not op.code(10) and op.code(9) and not op.code(8))));
    ex_stall.wrsr_z <= imp_bit_144;
    wb_stall.macsel1 <= SEL_WBUS when (imp_bit_153 or imp_bit_92 or imp_bit_81) = '1' else SEL_XBUS;
    wb_stall.wrsr_w <= (((not op.addr(1) and op.addr(0)) and not p(0) and (not op.code(15) and not op.code(14) and not op.code(13) and not op.code(12) and not op.code(11) and not op.code(10) and not op.code(9) and not op.code(8) and not op.code(7) and not op.code(6) and op.code(5) and not op.code(4) and op.code(3) and not op.code(2) and op.code(1) and op.code(0))) or ((not op.addr(1) and not op.addr(0)) and not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(5) and not op.code(4) and not op.code(3) and op.code(2) and op.code(1) and op.code(0))));
    ex_stall.wrmacl <= (imp_bit_110 or imp_bit_145);
    wb_stall.macsel2 <= SEL_WBUS when (imp_bit_160 or imp_bit_95 or imp_bit_88) = '1' else SEL_YBUS;
    cond35 <= imp_bit_38 & (not p(0) and (not op.code(15) and not op.code(14) and not op.code(13) and not op.code(12) and not op.code(11) and not op.code(10) and not op.code(9) and not op.code(8) and not op.code(7) and not op.code(6) and not op.code(5) and op.code(4) and op.code(3) and not op.code(2) and not op.code(1) and not op.code(0))) & (imp_bit_147 or imp_bit_37);
    with cond35 select
        ex_stall.t_sel <=
            SEL_SHIFT when "100",
            SEL_SET when "010",
            SEL_CARRY when "001",
            SEL_CLEAR when others;
    cond36 <= ((not op.addr(3) and op.addr(2) and not op.addr(1) and op.addr(0)) and p(0) and (not op.code(10) and not op.code(9) and not op.code(8))) & imp_bit_144 & (not p(0) and (not op.code(15) and not op.code(14) and not op.code(13) and not op.code(12) and not op.code(11) and not op.code(10) and not op.code(9) and not op.code(8) and not op.code(7) and not op.code(6) and not op.code(5) and op.code(4) and op.code(3) and not op.code(2) and not op.code(1) and op.code(0))) & ((not p(0) and (not op.code(15) and not op.code(14) and not op.code(13) and not op.code(12) and not op.code(11) and not op.code(10) and not op.code(9) and not op.code(8) and not op.code(7) and not op.code(6) and not op.code(5) and op.code(3) and not op.code(2) and not op.code(1) and not op.code(0))) or imp_bit_38 or imp_bit_147 or imp_bit_37) & ((not p(0) and (not op.code(15) and not op.code(14) and op.code(13) and op.code(12) and not op.code(3) and not op.code(2) and not op.code(1) and not op.code(0))) or (not p(0) and (not op.code(15) and not op.code(14) and op.code(13) and not op.code(12) and op.code(3) and not op.code(1) and not op.code(0))) or ((op.addr(1) and not op.addr(0)) and not p(0) and (not op.code(15) and not op.code(14) and op.code(13) and not op.code(12) and not op.code(3) and not op.code(2) and op.code(1) and op.code(0))) or ((op.addr(1) and not op.addr(0)) and not p(0) and (op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and op.code(11) and op.code(10) and not op.code(9) and not op.code(8))) or imp_bit_71) & (imp_bit_55 or (not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(5) and op.code(4) and not op.code(3) and not op.code(2) and not op.code(1))) or imp_bit_14 or (not p(0) and (not op.code(15) and not op.code(14) and op.code(13) and op.code(12) and op.code(1) and op.code(0))) or imp_bit_61 or (not p(0) and (not op.code(15) and not op.code(14) and op.code(13) and op.code(12) and not op.code(3) and op.code(2) and not op.code(0))) or (not p(0) and (not op.code(15) and not op.code(14) and op.code(13) and not op.code(3) and op.code(2) and op.code(1) and op.code(0))));
    with cond36 select
        ex_stall.sr_sel <=
            SEL_INT_MASK when "100000",
            SEL_ZBUS when "010000",
            SEL_DIV0U when "001000",
            SEL_SET_T when "000100",
            SEL_LOGIC when "000010",
            SEL_ARITH when "000001",
            SEL_PREV when others;
    mac_s_latch <= (imp_bit_95 or imp_bit_88);
    wb_stall.wrreg_w <= (imp_bit_15 or imp_bit_21 or imp_bit_16 or imp_bit_157 or imp_bit_93 or (not p(0) and (not op.code(15) and op.code(14) and op.code(13) and not op.code(12) and not op.code(3) and not op.code(2) and not op.code(1))) or (not p(0) and (not op.code(15) and op.code(14) and op.code(13) and not op.code(12) and not op.code(3) and not op.code(2) and not op.code(0))) or imp_bit_22 or imp_bit_19 or imp_bit_13 or imp_bit_7 or imp_bit_148 or (not p(0) and (op.code(14) and not op.code(13) and op.code(12))) or imp_bit_113 or imp_bit_94 or imp_bit_25 or imp_bit_46);
    ex.logic_sr_func <= BYTE_EQ when (not p(0) and (not op.code(15) and not op.code(14) and op.code(13) and not op.code(12) and op.code(3) and op.code(2) and not op.code(1) and not op.code(0))) = '1' else ZERO;
    ex_stall.wrreg_z <= (imp_bit_131 or (not p(0) and (not op.code(15) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and op.code(5) and not op.code(4) and op.code(3) and not op.code(2) and not op.code(1) and op.code(0))) or imp_bit_38 or imp_bit_146 or imp_bit_111 or imp_bit_39 or imp_bit_34 or (not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(5) and op.code(4) and op.code(2) and op.code(1) and not op.code(0))) or (not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and op.code(5) and not op.code(4) and op.code(3) and op.code(1) and not op.code(0))) or (not op.addr(0) and not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(5) and not op.code(4) and not op.code(2) and op.code(1) and op.code(0))) or imp_bit_84 or imp_bit_77 or imp_bit_3 or (not op.addr(0) and not p(0) and (not op.code(15) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(5) and not op.code(4) and not op.code(3) and not op.code(2) and op.code(1) and op.code(0))) or (not p(0) and (not op.code(15) and not op.code(14) and op.code(13) and op.code(12) and op.code(3) and op.code(1))) or imp_bit_85 or imp_bit_8 or imp_bit_41 or (not p(0) and (not op.code(15) and not op.code(14) and op.code(13) and op.code(12) and op.code(3) and not op.code(0))) or imp_bit_26 or imp_bit_80 or (not p(0) and (not op.code(15) and op.code(14) and not op.code(12) and op.code(3) and op.code(2) and op.code(0))) or imp_bit_20 or imp_bit_17 or imp_bit_5 or imp_bit_60 or imp_bit_83 or imp_bit_156 or imp_bit_151 or imp_bit_106 or imp_bit_102 or imp_bit_98 or imp_bit_75 or imp_bit_45 or imp_bit_120 or imp_bit_108 or ((not op.addr(2) and op.addr(1) and not op.addr(0)) and p(0) and (op.code(9) and op.code(8))) or imp_bit_76 or imp_bit_109 or imp_bit_127 or (not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(5) and not op.code(3) and not op.code(2) and not op.code(0))) or imp_bit_40 or (not p(0) and (not op.code(15) and not op.code(14) and op.code(13) and not op.code(12) and op.code(2) and not op.code(1) and op.code(0))) or (not p(0) and (not op.code(15) and op.code(13) and not op.code(12) and op.code(3) and not op.code(2) and op.code(0))) or (not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(5) and not op.code(4) and not op.code(3) and not op.code(0))) or (not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and op.code(5) and not op.code(4) and not op.code(2) and not op.code(0))) or (not p(0) and (not op.code(15) and not op.code(14) and op.code(13) and not op.code(3) and op.code(2) and not op.code(1) and not op.code(0))) or (not p(0) and (not op.code(15) and op.code(13) and not op.code(12) and op.code(3) and not op.code(2) and op.code(1))));
    id.incpc <= not ((op.addr(0) and not p(0) and (not op.code(15) and not op.code(14) and not op.code(13) and not op.code(12) and not op.code(11) and not op.code(10) and not op.code(9) and not op.code(8) and not op.code(7) and not op.code(6) and not op.code(4) and op.code(3) and not op.code(2) and op.code(1) and op.code(0))) or imp_bit_121 or imp_bit_152 or imp_bit_28 or imp_bit_11 or imp_bit_50 or imp_bit_34 or imp_bit_35 or imp_bit_49 or imp_bit_43 or imp_bit_157 or imp_bit_36 or imp_bit_68 or imp_bit_51 or imp_bit_80 or imp_bit_81 or imp_bit_20 or imp_bit_17 or imp_bit_139 or imp_bit_122 or imp_bit_103 or imp_bit_23 or imp_bit_89 or imp_bit_82 or imp_bit_125 or imp_bit_91 or imp_bit_96 or imp_bit_90 or imp_bit_99 or imp_bit_54);
    cond42 <= imp_bit_97 & imp_bit_101 & (not p(0) and (op.code(15) and not op.code(14) and not op.code(13) and not op.code(12) and not op.code(11) and not op.code(9) and not op.code(8))) & (not p(0) and (op.code(15) and not op.code(14) and not op.code(13) and not op.code(12) and not op.code(11) and not op.code(9) and op.code(8))) & (not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(4) and not op.code(3) and not op.code(1) and op.code(0))) & (not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(5) and not op.code(4) and op.code(3) and not op.code(2) and not op.code(1) and op.code(0))) & (not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(5) and op.code(4) and op.code(3) and not op.code(2) and not op.code(1) and op.code(0))) & (not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(5) and op.code(4) and op.code(3) and not op.code(2) and not op.code(1) and not op.code(0))) & (not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and op.code(5) and not op.code(4) and op.code(3) and not op.code(2) and not op.code(1) and op.code(0))) & (not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and op.code(5) and not op.code(4) and op.code(3) and not op.code(2) and not op.code(1) and not op.code(0))) & ((not p(0) and (op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(11) and not op.code(9) and op.code(8))) or imp_bit_64) & ((not p(0) and (op.code(15) and not op.code(14) and not op.code(13) and not op.code(12) and op.code(11) and not op.code(10) and not op.code(9) and not op.code(8))) or imp_bit_75 or imp_bit_45) & (imp_bit_129 or imp_bit_117) & ((not p(0) and (op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(11) and not op.code(9) and not op.code(8))) or imp_bit_24 or imp_bit_10) & ((not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(5) and not op.code(3) and not op.code(2) and not op.code(1) and not op.code(0))) or imp_bit_136 or (not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(4) and not op.code(3) and not op.code(1) and not op.code(0))) or (not p(0) and (not op.code(15) and op.code(13) and not op.code(12) and not op.code(3) and op.code(2) and not op.code(1) and not op.code(0)))) & ((not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(5) and not op.code(4) and op.code(3) and not op.code(2) and not op.code(1) and not op.code(0))) or imp_bit_57 or (not p(0) and (not op.code(15) and op.code(13) and not op.code(12) and not op.code(3) and op.code(2) and not op.code(1) and op.code(0))) or (p(0) and (not op.addr(2) and not op.addr(1) and not op.addr(0)) and (op.code(10) and op.code(9) and op.code(8))) or imp_bit_154 or imp_bit_30) & ((not p(0) and (op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(11) and op.code(9) and not op.code(8))) or (not p(0) and (op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(11) and op.code(10) and op.code(9))) or imp_bit_73 or imp_bit_100 or imp_bit_115 or imp_bit_42 or imp_bit_2) & (imp_bit_55 or imp_bit_32 or imp_bit_161 or imp_bit_59 or (not p(0) and (not op.code(15) and not op.code(14) and op.code(13) and not op.code(12) and not op.code(3) and not op.code(2) and not op.code(1))) or (not p(0) and (not op.code(15) and not op.code(14) and op.code(13) and not op.code(12) and not op.code(3) and not op.code(2) and not op.code(0))) or (p(0) and (not op.addr(2) and not op.addr(1) and not op.addr(0)) and (op.code(10) and op.code(9) and not op.code(8))) or imp_bit_1);
    with cond42 select
        ex.imm_val <=
            "00000000000000000000000000" & op.code(3 downto 0) & "00" when "100000000000000000",
            imms_12_1 when "010000000000000000",
            x"0000000" & op.code(3 downto 0) when "001000000000000000",
            "000000000000000000000000000" & op.code(3 downto 0) & "0" when "000100000000000000",
            x"ffffffff" when "000010000000000000",
            x"fffffffe" when "000001000000000000",
            x"fffffff8" when "000000100000000000",
            x"00000008" when "000000010000000000",
            x"fffffff0" when "000000001000000000",
            x"00000010" when "000000000100000000",
            "00000000000000000000000" & op.code(7 downto 0) & "0" when "000000000010000000",
            imms_8_0 when "000000000001000000",
            imms_8_1 when "000000000000100000",
            x"000000" & op.code(7 downto 0) when "000000000000010000",
            x"00000001" when "000000000000001000",
            x"00000002" when "000000000000000100",
            "0000000000000000000000" & op.code(7 downto 0) & "00" when "000000000000000010",
            x"00000000" when "000000000000000001",
            x"00000004" when others;
    debug <= ((not p(0) and (not op.code(15) and not op.code(14) and not op.code(13) and not op.code(12) and not op.code(11) and not op.code(10) and not op.code(9) and not op.code(8) and not op.code(7) and not op.code(6) and op.code(5) and op.code(4) and op.code(3) and not op.code(2) and op.code(1) and op.code(0))) or imp_bit_30);
    cond44 <= (imp_bit_14 or (not p(0) and (not op.code(15) and op.code(14) and op.code(13) and not op.code(12) and op.code(3) and op.code(2))) or (not p(0) and (not op.code(15) and op.code(14) and op.code(13) and not op.code(12) and op.code(3) and not op.code(1))) or (not p(0) and (not op.code(15) and op.code(13) and not op.code(12) and op.code(3) and op.code(2) and not op.code(1) and op.code(0)))) & (imp_bit_38 or imp_bit_146 or (not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(4) and not op.code(2) and not op.code(1))) or (not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and op.code(3) and op.code(2) and not op.code(1)))) & (imp_bit_138 or imp_bit_107 or imp_bit_128 or imp_bit_65 or imp_bit_155) & (imp_bit_132 or imp_bit_111 or imp_bit_104 or imp_bit_119 or imp_bit_112 or imp_bit_85 or (not p(0) and (not op.code(15) and op.code(14) and op.code(13) and not op.code(12) and not op.code(3) and not op.code(2))) or imp_bit_45 or imp_bit_42) & (imp_bit_110 or imp_bit_136 or (not p(0) and (not op.code(15) and not op.code(14) and op.code(13) and not op.code(12) and op.code(3) and not op.code(2) and op.code(0))) or imp_bit_59 or (not p(0) and (not op.code(15) and not op.code(14) and op.code(13) and not op.code(12) and op.code(3) and not op.code(2) and op.code(1))) or imp_bit_140 or imp_bit_130 or imp_bit_106 or imp_bit_102 or imp_bit_76);
    with cond44 select
        ex_stall.zbus_sel <=
            SEL_MANIP when "10000",
            SEL_SHIFT when "01000",
            SEL_WBUS when "00100",
            SEL_YBUS when "00010",
            SEL_LOGIC when "00001",
            SEL_ARITH when others;
    cond45 <= imp_bit_59 & ((not p(0) and (not op.code(15) and not op.code(14) and op.code(13) and not op.code(12) and op.code(3) and not op.code(2) and op.code(1) and op.code(0))) or ((op.addr(1) and not op.addr(0)) and not p(0) and (op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and op.code(11) and op.code(10) and op.code(9) and op.code(8))) or (not p(0) and (op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and op.code(11) and not op.code(10) and op.code(9) and op.code(8)))) & (imp_bit_136 or (not p(0) and (not op.code(15) and not op.code(14) and op.code(13) and not op.code(12) and op.code(3) and not op.code(2) and not op.code(1))) or ((op.addr(1) and not op.addr(0)) and not p(0) and (op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and op.code(11) and op.code(10) and not op.code(9))) or (not p(0) and (op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and op.code(11) and not op.code(10) and not op.code(9))));
    with cond45 select
        ex.logic_func <=
            LOGIC_NOT when "100",
            LOGIC_OR when "010",
            LOGIC_AND when "001",
            LOGIC_XOR when others;
    delay_jump <= (imp_bit_150 or imp_bit_141 or imp_bit_35 or imp_bit_36 or imp_bit_122 or imp_bit_103);
    cond47 <= (imp_bit_140 or imp_bit_130) & (imp_bit_131 or imp_bit_4 or imp_bit_78 or imp_bit_67 or imp_bit_33 or imp_bit_157 or imp_bit_69 or imp_bit_80 or imp_bit_57);
    with cond47 select
        ex_stall.mem_addr_sel <=
            SEL_YBUS when "10",
            SEL_XBUS when "01",
            SEL_ZBUS when others;
    ex_stall.macsel1 <= SEL_ZBUS when (imp_bit_110 or imp_bit_135) = '1' else SEL_XBUS;
    ex_stall.macsel2 <= SEL_ZBUS when (imp_bit_110 or imp_bit_145) = '1' else SEL_YBUS;
    cond50 <= (((not op.addr(2) and not op.addr(1) and op.addr(0)) and not p(0) and (op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(11) and not op.code(10) and op.code(9) and op.code(8))) or imp_bit_108 or ((not op.addr(3) and not op.addr(2) and op.addr(1) and op.addr(0)) and p(0) and (not op.code(10) and not op.code(9)))) & ((not p(0) and (not op.code(15) and not op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(5) and not op.code(4) and op.code(3) and not op.code(2) and op.code(1) and not op.code(0))) or (not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(5) and not op.code(4) and not op.code(3) and not op.code(2) and op.code(1) and not op.code(0)))) & ((not p(0) and (not op.code(15) and not op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(5) and op.code(4) and op.code(3) and not op.code(2) and op.code(1) and not op.code(0))) or (not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(5) and op.code(4) and not op.code(3) and not op.code(2) and op.code(1) and not op.code(0)))) & (imp_bit_136 or (not p(0) and (not op.code(15) and not op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(5) and not op.code(4) and not op.code(3) and not op.code(2) and op.code(1) and not op.code(0))) or (not op.addr(0) and not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(5) and not op.code(4) and not op.code(3) and not op.code(2) and op.code(1) and op.code(0))) or ((not op.addr(2) and not op.addr(1) and not op.addr(0)) and not p(0) and (op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(11) and not op.code(10) and op.code(9) and op.code(8))) or imp_bit_120 or (p(0) and (not op.addr(3) and not op.addr(2) and op.addr(1) and not op.addr(0)) and (not op.code(10) and not op.code(9)))) & (imp_bit_110 or imp_bit_132 or imp_bit_143 or imp_bit_40 or imp_bit_14 or imp_bit_12 or (not op.addr(0) and not p(0) and (not op.code(15) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and op.code(5) and not op.code(4) and not op.code(3) and not op.code(2) and op.code(1) and op.code(0))) or imp_bit_119 or imp_bit_112 or imp_bit_31 or (not p(0) and (not op.code(15) and not op.code(14) and op.code(13) and not op.code(0))) or (not p(0) and (not op.code(15) and not op.code(14) and op.code(13) and op.code(12) and op.code(1))) or imp_bit_51 or imp_bit_52 or (not p(0) and (not op.code(15) and not op.code(14) and op.code(13) and op.code(2))) or (not p(0) and (not op.code(15) and op.code(13) and not op.code(12) and op.code(3))) or (not p(0) and (not op.code(15) and not op.code(14) and not op.code(12) and not op.code(3) and op.code(2))) or imp_bit_26 or imp_bit_137 or imp_bit_74 or imp_bit_105 or imp_bit_134 or imp_bit_124 or imp_bit_25 or imp_bit_123 or imp_bit_118 or imp_bit_100 or imp_bit_115 or imp_bit_76 or imp_bit_9 or (not p(0) and (not op.code(15) and op.code(13) and not op.code(12) and op.code(2) and op.code(1) and op.code(0))) or (not p(0) and (not op.code(15) and op.code(13) and not op.code(12) and not op.code(2) and not op.code(1))));
    with cond50 select
        ex.ybus_sel <=
            SEL_PC when "10000",
            SEL_MACH when "01000",
            SEL_MACL when "00100",
            SEL_SR when "00010",
            SEL_REG when "00001",
            SEL_IMM when others;
    ex.mem_lock <= (imp_bit_11 or imp_bit_4 or imp_bit_69 or imp_bit_52);
    ex_stall.wrpr_pc <= (imp_bit_158 or imp_bit_159 or imp_bit_83);
    cond53 <= (not p(0) and (not op.code(15) and not op.code(14) and op.code(13) and op.code(12) and op.code(3) and op.code(1) and op.code(0))) & (not p(0) and (not op.code(15) and not op.code(14) and op.code(13) and op.code(12) and not op.code(3) and not op.code(2) and op.code(1) and not op.code(0))) & (not p(0) and (not op.code(15) and not op.code(14) and op.code(13) and op.code(12) and not op.code(3) and op.code(2) and op.code(1) and not op.code(0))) & (not p(0) and (not op.code(15) and not op.code(14) and op.code(13) and not op.code(12) and not op.code(3) and op.code(2) and op.code(1) and op.code(0))) & imp_bit_56 & ((not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(5) and op.code(4) and not op.code(3) and op.code(2) and not op.code(1) and op.code(0))) or (not p(0) and (not op.code(15) and not op.code(14) and op.code(13) and op.code(12) and not op.code(3) and op.code(2) and op.code(1) and op.code(0)))) & ((not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(5) and op.code(4) and not op.code(3) and not op.code(2) and not op.code(1) and op.code(0))) or (not p(0) and (not op.code(15) and not op.code(14) and op.code(13) and op.code(12) and not op.code(3) and not op.code(2) and op.code(1) and op.code(0))));
    with cond53 select
        ex.arith_sr_func <=
            OVERUNDERFLOW when "1000000",
            UGRTER_EQ when "0100000",
            UGRTER when "0010000",
            DIV0S when "0001000",
            DIV1 when "0000100",
            SGRTER when "0000010",
            SGRTER_EQ when "0000001",
            ZERO when others;
    wb_stall.wrmacl <= imp_bit_160;
    maskint_next <= ((not p(0) and (not op.code(15) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(5) and not op.code(2) and op.code(1) and not op.code(0))) or (not p(0) and (not op.code(15) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(4) and not op.code(2) and op.code(1) and not op.code(0))) or (op.addr(0) and not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(5) and not op.code(3) and not op.code(2) and op.code(1) and op.code(0))) or (op.addr(0) and not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(4) and not op.code(3) and not op.code(2) and op.code(1) and op.code(0))) or (not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(5) and op.code(1) and not op.code(0))) or (not p(0) and (not op.code(15) and op.code(14) and not op.code(13) and not op.code(12) and not op.code(7) and not op.code(6) and not op.code(4) and op.code(1) and not op.code(0))) or imp_bit_66 or imp_bit_47);
end;
