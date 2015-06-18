#ifndef DECODE_SIGNALS_H
#define DECODE_SIGNALS_H


#define ALUFUNCS \
  ALU(NOP, "Z=0")      \
  ALU(THRUX, "Z=X")          \
  ALU(SPARE2, "previously Z=W by way of the ALU")                       \
  ALU(THRUY, "Z=Y")                                                     \
  ALU(ADD, "Z=X+Y")                                                     \
  ALU(ADDC, "Z=X+Y, T<-Carry")                                          \
  ALU(ADDV, "Z=X+Y, T<-Overflow")                                       \
  ALU(INCX, "Z=X+1")                                                    \
  ALU(INCX2, "Z=X+2")                                                   \
  ALU(INCX4, "Z=X+4")                                                   \
  ALU(ADDCN, "Z=X+CONST")                                               \
  ALU(ADDR0, "Z=X+R0(if necessary, forwarding from WBUS)")              \
  ALU(ADDXFC, "Z=(X & FC) + Y")                                         \
  ALU(SWAPW, "Z=SWAPW(Y)")                                              \
  ALU(SWAPB, "Z=SWAPB(Y)")                                              \
  ALU(SPARE, "unused")                                                  \
  ALU(EXTUB, "Z=EXTUB(Y)")                                              \
  ALU(EXTUW, "Z=EXTUW(Y)")                                              \
  ALU(EXTSB, "Z=EXTSB(Y)")                                              \
  ALU(EXTSW, "Z=EXTSW(Y)")                                              \
  ALU(SUB, "Z=X-Y")                                                     \
  ALU(SUBC, "Z=X-Y, T<-Carry")                                          \
  ALU(SUBV, "Z=X-Y, T<-Overflow")                                       \
  ALU(DECX, "Z=X-1")                                                    \
  ALU(DECX2, "Z=X-2")                                                   \
  ALU(DECX4, "Z=X-4")                                                   \
  ALU(TAS, "Z=X | 32'h00000080")                                        \
  ALU(XTRCT, "Z=XTRCT(X,Y)")                                            \
  ALU(AND, "Z=X&Y")                                                     \
  ALU(XOR, "Z=X^Y")                                                     \
  ALU(OR, "Z=Z|Y")                                                      \
  ALU(NOT, "Z=~Y")


#define SIGNALS                                 \
SIG(CLK) \
SIG(RST) \
SIG(SLOT) \
SIG(ID_IF_ISSUE) \
SIG(ID_IF_JP) \
SIG(IF_DR) \
SIG(IF_STALL) \
SIG(EX_MA_ISSUE) \
SIG(EX_KEEP_CYC) \
SIG(EX_MA_WR) \
SIG(EX_MA_SZ) \
SIG(EX_MULCOM1) \
SIG(EX_MULCOM2) \
SIG(WB_MULCOM1) \
SIG(WB_MULCOM2) \
SIG(EX_WRMACH) \
SIG(EX_WRMACL) \
SIG(WB_WRMACH) \
SIG(WB_WRMACL) \
SIG(WB_MAC_BUSY) \
SIG(WB1_MULCOM1) \
SIG(WB1_MULCOM2) \
SIG(WB1_WRMACH) \
SIG(WB1_WRMACL) \
SIG(WB1_MAC_BUSY) \
SIG(WB2_MULCOM1) \
SIG(WB2_MULCOM2) \
SIG(WB2_WRMACH) \
SIG(WB2_WRMACL) \
SIG(WB2_MAC_BUSY) \
SIG(WB3_MAC_BUSY) \
SIG(EX_MAC_BUSY) \
SIG(EX1_MAC_BUSY) \
SIG(MAC_BUSY) \
SIG(MAC_STALL_SENSE)                           \
SIG(EX_RDREG_X) \
SIG(EX_RDREG_Y) \
SIG(EX_WRREG_Z) \
SIG(WB_WRREG_W) \
SIG(EX_REGNUM_X) \
SIG(EX_REGNUM_Y) \
SIG(EX_REGNUM_Z) \
SIG(WB_REGNUM_W) \
SIG(EX_ALUFUNC) \
SIG(EX_WRMAAD_Z) \
SIG(EX_WRMADW_X) \
SIG(EX_WRMADW_Y) \
SIG(EX_MACSEL1) \
SIG(EX_MACSEL2) \
SIG(WB_MACSEL1) \
SIG(WB_MACSEL2) \
SIG(EX_RDMACH_X) \
SIG(EX_RDMACL_X) \
SIG(EX_RDMACH_Y) \
SIG(EX_RDMACL_Y) \
SIG(EX_RDSR_X) \
SIG(EX_RDSR_Y) \
SIG(EX_WRSR_Z) \
SIG(WB_WRSR_W) \
SIG(MAC_S_LATCH) \
SIG(EX_RDGBR_X) \
SIG(EX_RDGBR_Y) \
SIG(EX_WRGBR_Z) \
SIG(WB_WRGBR_W) \
SIG(EX_RDVBR_X) \
SIG(EX_RDVBR_Y) \
SIG(EX_WRVBR_Z) \
SIG(WB_WRVBR_W) \
SIG(EX_RDPR_X) \
SIG(EX_RDPR_Y) \
SIG(EX_WRPR_Z) \
SIG(WB_WRPR_W) \
SIG(EX_WRPR_PC) \
SIG(EX_RDPC_X) \
SIG(EX_RDPC_Y) \
SIG(EX_WRPC_Z) \
SIG(EX_WRPC_W) \
SIG(ID_INCPC) \
SIG(ID_IFADSEL) \
SIG(EX_CONST_ZERO4) \
SIG(EX_CONST_ZERO42) \
SIG(EX_CONST_ZERO44) \
SIG(EX_CONST_ZERO8) \
SIG(EX_CONST_ZERO82) \
SIG(EX_CONST_ZERO84) \
SIG(EX_CONST_SIGN8) \
SIG(EX_CONST_SIGN82) \
SIG(EX_CONST_SIGN122) \
SIG(EX_RDCONST_X) \
SIG(EX_RDCONST_Y) \
SIG(EX_CMPCOM) \
SIG(EX_SFTFUNC) \
SIG(EX_RDSFT_Z) \
SIG(EX_RDDIV_Z) \
SIG(T_BCC) \
SIG(EX_T_CMPSET) \
SIG(EX_T_CRYSET) \
SIG(EX_T_TSTSET) \
SIG(EX_T_SFTSET) \
SIG(EX_QT_DV1SET) \
SIG(EX_MQT_DV0SET) \
SIG(EX_T_CLR) \
SIG(EX_T_SET) \
SIG(EX_MQ_CLR) \
SIG(EX_RDTEMP_X) \
SIG(EX_WRTEMP_Z) \
SIG(EX_WRMAAD_TEMP) \
SIG(EVENT_REQ) \
SIG(EVENT_ACK) \
SIG(EVENT_ACK_0) \
SIG(EVENT_INFO) \
SIG(RST_SR) \
SIG(IBIT) \
SIG(ILEVEL) \
SIG(ILEVEL_CAP) \
SIG(WR_IBIT) \
SIG(SLP)\
SIG(EX_FWD_W2X) \
SIG(DELAY_JUMP) \
SIG(DISPATCH) \
SIG(INSTR_SEQ) \
SIG(INSTR_STATE) \
SIG(NEXT_ID_STALL) \
SIG(MASKINT_NEXT) \
SIG(IF_ISSUE) \
SIG(RDREG_X) \
SIG(REGNUM_X) \
SIG(RDREG_Y)
#endif
