#include "sh2instr.h"
#include <stdio.h>
static int line0(char *str, size_t size, uint16_t instr) {
if ((instr & 0xffff) == 0x8) {
  return snprintf(str, size, "CLRT");
} else if ((instr & 0xffff) == 0x28) {
  return snprintf(str, size, "CLRMAC");
} else if ((instr & 0xffff) == 0x19) {
  return snprintf(str, size, "DIV0U");
} else if ((instr & 0xffff) == 0x9) {
  return snprintf(str, size, "NOP");
} else if ((instr & 0xffff) == 0x2b) {
  return snprintf(str, size, "RTE");
} else if ((instr & 0xffff) == 0xb) {
  return snprintf(str, size, "RTS");
} else if ((instr & 0xffff) == 0x18) {
  return snprintf(str, size, "SETT");
} else if ((instr & 0xffff) == 0x1b) {
  return snprintf(str, size, "SLEEP");
} else if ((instr & 0xffff) == 0x3b) {
  return snprintf(str, size, "BGND");
} else if ((instr & 0xf0ff) == 0x29) {
  return snprintf(str, size, "MOVT R%hu", (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf0ff) == 0x2) {
  return snprintf(str, size, "STC SR, R%hu", (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf0ff) == 0x12) {
  return snprintf(str, size, "STC GBR, R%hu", (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf0ff) == 0x22) {
  return snprintf(str, size, "STC VBR, R%hu", (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf0ff) == 0xa) {
  return snprintf(str, size, "STS MACH, R%hu", (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf0ff) == 0x1a) {
  return snprintf(str, size, "STS MACL, R%hu", (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf0ff) == 0x2a) {
  return snprintf(str, size, "STS PR, R%hu", (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf0ff) == 0x23) {
  return snprintf(str, size, "BRAF R%hu", (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf0ff) == 0x3) {
  return snprintf(str, size, "BSRF R%hu", (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf00f) == 0x7) {
  return snprintf(str, size, "MUL.L R%hu, R%hu", (uint16_t)((instr >> 4) & 0xF), (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf00f) == 0xf) {
  return snprintf(str, size, "MAC.L @R%hu+, @R%hu+", (uint16_t)((instr >> 4) & 0xF), (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf00f) == 0x4) {
  return snprintf(str, size, "MOV.B R%hu, @(R0, R%hu)", (uint16_t)((instr >> 4) & 0xF), (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf00f) == 0x5) {
  return snprintf(str, size, "MOV.W R%hu, @(R0, R%hu)", (uint16_t)((instr >> 4) & 0xF), (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf00f) == 0x6) {
  return snprintf(str, size, "MOV.L R%hu, @(R0, R%hu)", (uint16_t)((instr >> 4) & 0xF), (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf00f) == 0xc) {
  return snprintf(str, size, "MOV.B @(R0, R%hu), R%hu", (uint16_t)((instr >> 4) & 0xF), (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf00f) == 0xd) {
  return snprintf(str, size, "MOV.W @(R0, R%hu), R%hu", (uint16_t)((instr >> 4) & 0xF), (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf00f) == 0xe) {
  return snprintf(str, size, "MOV.L @(R0, R%hu), R%hu", (uint16_t)((instr >> 4) & 0xF), (uint16_t)((instr >> 8) & 0xF));
} else {
  return -1;
}
}

static int line1(char *str, size_t size, uint16_t instr) {
if ((instr & 0xf000) == 0x1000) {
  return snprintf(str, size, "MOV.L R%hu, @(disp, R%hu)", (uint16_t)((instr >> 4) & 0xF), (uint16_t)((instr >> 8) & 0xF));
} else {
  return -1;
}
}

static int line2(char *str, size_t size, uint16_t instr) {
if ((instr & 0xf00f) == 0x2009) {
  return snprintf(str, size, "AND R%hu, R%hu", (uint16_t)((instr >> 4) & 0xF), (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf00f) == 0x200c) {
  return snprintf(str, size, "CMP /STR R%hu, R%hu", (uint16_t)((instr >> 4) & 0xF), (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf00f) == 0x2003) {
  return snprintf(str, size, "CAS.L R%hu, R%hu, @R0", (uint16_t)((instr >> 4) & 0xF), (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf00f) == 0x2007) {
  return snprintf(str, size, "DIV0S R%hu, R%hu", (uint16_t)((instr >> 4) & 0xF), (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf00f) == 0x200f) {
  return snprintf(str, size, "MULS.W R%hu, R%hu", (uint16_t)((instr >> 4) & 0xF), (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf00f) == 0x200e) {
  return snprintf(str, size, "MULU.W R%hu, R%hu", (uint16_t)((instr >> 4) & 0xF), (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf00f) == 0x200b) {
  return snprintf(str, size, "OR R%hu, R%hu", (uint16_t)((instr >> 4) & 0xF), (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf00f) == 0x2008) {
  return snprintf(str, size, "TST R%hu, R%hu", (uint16_t)((instr >> 4) & 0xF), (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf00f) == 0x200a) {
  return snprintf(str, size, "XOR R%hu, R%hu", (uint16_t)((instr >> 4) & 0xF), (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf00f) == 0x200d) {
  return snprintf(str, size, "XTRACT R%hu, R%hu", (uint16_t)((instr >> 4) & 0xF), (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf00f) == 0x2000) {
  return snprintf(str, size, "MOV.B R%hu, @R%hu", (uint16_t)((instr >> 4) & 0xF), (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf00f) == 0x2001) {
  return snprintf(str, size, "MOV.W R%hu, @R%hu", (uint16_t)((instr >> 4) & 0xF), (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf00f) == 0x2002) {
  return snprintf(str, size, "MOV.L R%hu, @R%hu", (uint16_t)((instr >> 4) & 0xF), (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf00f) == 0x2004) {
  return snprintf(str, size, "MOV.B R%hu,@-R%hu", (uint16_t)((instr >> 4) & 0xF), (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf00f) == 0x2005) {
  return snprintf(str, size, "MOV.W R%hu,@-R%hu", (uint16_t)((instr >> 4) & 0xF), (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf00f) == 0x2006) {
  return snprintf(str, size, "MOV.L R%hu,@-R%hu", (uint16_t)((instr >> 4) & 0xF), (uint16_t)((instr >> 8) & 0xF));
} else {
  return -1;
}
}

static int line3(char *str, size_t size, uint16_t instr) {
if ((instr & 0xf00f) == 0x300c) {
  return snprintf(str, size, "ADD R%hu, R%hu", (uint16_t)((instr >> 4) & 0xF), (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf00f) == 0x300e) {
  return snprintf(str, size, "ADDC R%hu, R%hu", (uint16_t)((instr >> 4) & 0xF), (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf00f) == 0x300f) {
  return snprintf(str, size, "ADDV R%hu, R%hu", (uint16_t)((instr >> 4) & 0xF), (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf00f) == 0x3000) {
  return snprintf(str, size, "CMP /EQ R%hu, R%hu", (uint16_t)((instr >> 4) & 0xF), (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf00f) == 0x3002) {
  return snprintf(str, size, "CMP /HS R%hu, R%hu", (uint16_t)((instr >> 4) & 0xF), (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf00f) == 0x3003) {
  return snprintf(str, size, "CMP /GE R%hu, R%hu", (uint16_t)((instr >> 4) & 0xF), (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf00f) == 0x3006) {
  return snprintf(str, size, "CMP /HI R%hu, R%hu", (uint16_t)((instr >> 4) & 0xF), (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf00f) == 0x3007) {
  return snprintf(str, size, "CMP /GT R%hu, R%hu", (uint16_t)((instr >> 4) & 0xF), (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf00f) == 0x3004) {
  return snprintf(str, size, "DIV1 R%hu, R%hu", (uint16_t)((instr >> 4) & 0xF), (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf00f) == 0x300d) {
  return snprintf(str, size, "DMULS.L R%hu, R%hu", (uint16_t)((instr >> 4) & 0xF), (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf00f) == 0x3005) {
  return snprintf(str, size, "DMULU.L R%hu, R%hu", (uint16_t)((instr >> 4) & 0xF), (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf00f) == 0x3008) {
  return snprintf(str, size, "SUB R%hu, R%hu", (uint16_t)((instr >> 4) & 0xF), (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf00f) == 0x300a) {
  return snprintf(str, size, "SUBC R%hu, R%hu", (uint16_t)((instr >> 4) & 0xF), (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf00f) == 0x300b) {
  return snprintf(str, size, "SUBV R%hu, R%hu", (uint16_t)((instr >> 4) & 0xF), (uint16_t)((instr >> 8) & 0xF));
} else {
  return -1;
}
}

static int line4(char *str, size_t size, uint16_t instr) {
if ((instr & 0xf0ff) == 0x4015) {
  return snprintf(str, size, "CMP/PL R%hu", (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf0ff) == 0x4011) {
  return snprintf(str, size, "CMP/PZ R%hu", (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf0ff) == 0x4010) {
  return snprintf(str, size, "DT R%hu", (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf0ff) == 0x4004) {
  return snprintf(str, size, "ROTL R%hu", (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf0ff) == 0x4005) {
  return snprintf(str, size, "ROTR R%hu", (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf0ff) == 0x4024) {
  return snprintf(str, size, "ROTCL R%hu", (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf0ff) == 0x4025) {
  return snprintf(str, size, "ROTCR R%hu", (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf0ff) == 0x4020) {
  return snprintf(str, size, "SHAL R%hu", (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf0ff) == 0x4021) {
  return snprintf(str, size, "SHAR R%hu", (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf0ff) == 0x4000) {
  return snprintf(str, size, "SHLL R%hu", (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf0ff) == 0x4001) {
  return snprintf(str, size, "SHLR R%hu", (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf0ff) == 0x4008) {
  return snprintf(str, size, "SHLL2 R%hu", (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf0ff) == 0x4009) {
  return snprintf(str, size, "SHLR2 R%hu", (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf0ff) == 0x4018) {
  return snprintf(str, size, "SHLL8 R%hu", (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf0ff) == 0x4019) {
  return snprintf(str, size, "SHLR8 R%hu", (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf0ff) == 0x4028) {
  return snprintf(str, size, "SHLL16 R%hu", (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf0ff) == 0x4029) {
  return snprintf(str, size, "SHLR16 R%hu", (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf0ff) == 0x401b) {
  return snprintf(str, size, "TAS.B @R%hu", (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf0ff) == 0x4003) {
  return snprintf(str, size, "STC.L SR, @-R%hu", (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf0ff) == 0x4013) {
  return snprintf(str, size, "STC.L GBR, @-R%hu", (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf0ff) == 0x4023) {
  return snprintf(str, size, "STC.L VBR, @-R%hu", (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf0ff) == 0x4002) {
  return snprintf(str, size, "STS.L MACH, @-R%hu", (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf0ff) == 0x4012) {
  return snprintf(str, size, "STS.L MACL, @-R%hu", (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf0ff) == 0x4022) {
  return snprintf(str, size, "STS.L PR, @-R%hu", (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf0ff) == 0x400e) {
  return snprintf(str, size, "LDC R%hu, SR", (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf0ff) == 0x401e) {
  return snprintf(str, size, "LDC, R%hu, GBR", (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf0ff) == 0x402e) {
  return snprintf(str, size, "LDC R%hu, VBR", (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf0ff) == 0x400a) {
  return snprintf(str, size, "LDS R%hu, MACH", (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf0ff) == 0x401a) {
  return snprintf(str, size, "LDS R%hu, MACL", (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf0ff) == 0x402a) {
  return snprintf(str, size, "LDS R%hu, PR", (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf0ff) == 0x402b) {
  return snprintf(str, size, "JMP @R%hu", (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf0ff) == 0x400b) {
  return snprintf(str, size, "JSR @R%hu", (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf0ff) == 0x4007) {
  return snprintf(str, size, "LDC.L @R%hu+, SR", (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf0ff) == 0x4017) {
  return snprintf(str, size, "LDC.L @R%hu+, GBR", (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf0ff) == 0x4027) {
  return snprintf(str, size, "LDC.L @R%hu+, VBR", (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf0ff) == 0x4006) {
  return snprintf(str, size, "LDS.L @R%hu+, MACH", (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf0ff) == 0x4016) {
  return snprintf(str, size, "LDS.L @R%hu+, MACL", (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf0ff) == 0x4026) {
  return snprintf(str, size, "LDS.L @R%hu+, PR", (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf00f) == 0x400c) {
  return snprintf(str, size, "SHAD R%hu, R%hu", (uint16_t)((instr >> 4) & 0xF), (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf00f) == 0x400d) {
  return snprintf(str, size, "SHLD R%hu, R%hu", (uint16_t)((instr >> 4) & 0xF), (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf00f) == 0x400f) {
  return snprintf(str, size, "MAC.W @R%hu+, @R%hu+", (uint16_t)((instr >> 4) & 0xF), (uint16_t)((instr >> 8) & 0xF));
} else {
  return -1;
}
}

static int line5(char *str, size_t size, uint16_t instr) {
if ((instr & 0xf000) == 0x5000) {
  return snprintf(str, size, "MOV.L @(disp, R%hu), R%hu", (uint16_t)((instr >> 4) & 0xF), (uint16_t)((instr >> 8) & 0xF));
} else {
  return -1;
}
}

static int line6(char *str, size_t size, uint16_t instr) {
if ((instr & 0xf00f) == 0x600e) {
  return snprintf(str, size, "EXTS.B R%hu, R%hu", (uint16_t)((instr >> 4) & 0xF), (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf00f) == 0x600f) {
  return snprintf(str, size, "EXTS.W R%hu, R%hu", (uint16_t)((instr >> 4) & 0xF), (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf00f) == 0x600c) {
  return snprintf(str, size, "EXTU.B R%hu, R%hu", (uint16_t)((instr >> 4) & 0xF), (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf00f) == 0x600d) {
  return snprintf(str, size, "EXTU.W R%hu, R%hu", (uint16_t)((instr >> 4) & 0xF), (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf00f) == 0x6003) {
  return snprintf(str, size, "MOV R%hu, R%hu", (uint16_t)((instr >> 4) & 0xF), (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf00f) == 0x600b) {
  return snprintf(str, size, "NEG R%hu, R%hu", (uint16_t)((instr >> 4) & 0xF), (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf00f) == 0x600a) {
  return snprintf(str, size, "NEGC R%hu, R%hu", (uint16_t)((instr >> 4) & 0xF), (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf00f) == 0x6007) {
  return snprintf(str, size, "NOT R%hu, R%hu", (uint16_t)((instr >> 4) & 0xF), (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf00f) == 0x6008) {
  return snprintf(str, size, "SWAP.B R%hu, R%hu", (uint16_t)((instr >> 4) & 0xF), (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf00f) == 0x6009) {
  return snprintf(str, size, "SWAP.W R%hu, R%hu", (uint16_t)((instr >> 4) & 0xF), (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf00f) == 0x6000) {
  return snprintf(str, size, "MOV.B @R%hu, R%hu", (uint16_t)((instr >> 4) & 0xF), (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf00f) == 0x6001) {
  return snprintf(str, size, "MOV.W @R%hu, R%hu", (uint16_t)((instr >> 4) & 0xF), (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf00f) == 0x6002) {
  return snprintf(str, size, "MOV.L @R%hu, R%hu", (uint16_t)((instr >> 4) & 0xF), (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf00f) == 0x6004) {
  return snprintf(str, size, "MOV.B @R%hu+, R%hu", (uint16_t)((instr >> 4) & 0xF), (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf00f) == 0x6005) {
  return snprintf(str, size, "MOV.W @R%hu+, R%hu", (uint16_t)((instr >> 4) & 0xF), (uint16_t)((instr >> 8) & 0xF));
} else if ((instr & 0xf00f) == 0x6006) {
  return snprintf(str, size, "MOV.L @R%hu+, R%hu", (uint16_t)((instr >> 4) & 0xF), (uint16_t)((instr >> 8) & 0xF));
} else {
  return -1;
}
}

static int line7(char *str, size_t size, uint16_t instr) {
if ((instr & 0xf000) == 0x7000) {
  return snprintf(str, size, "ADD #imm, R%hu", (uint16_t)((instr >> 8) & 0xF));
} else {
  return -1;
}
}

static int line8(char *str, size_t size, uint16_t instr) {
if ((instr & 0xff00) == 0x8400) {
  return snprintf(str, size, "MOV.B @(disp, R%hu), R0", (uint16_t)((instr >> 4) & 0xF));
} else if ((instr & 0xff00) == 0x8500) {
  return snprintf(str, size, "MOV.W @(disp, R%hu), R0", (uint16_t)((instr >> 4) & 0xF));
} else if ((instr & 0xff00) == 0x8000) {
  return snprintf(str, size, "MOV.B R0, @(disp, R%hu)", (uint16_t)((instr >> 4) & 0xF));
} else if ((instr & 0xff00) == 0x8100) {
  return snprintf(str, size, "MOV.W R0, @(disp, R%hu)", (uint16_t)((instr >> 4) & 0xF));
} else if ((instr & 0xff00) == 0x8b00) {
  return snprintf(str, size, "BF label");
} else if ((instr & 0xff00) == 0x8f00) {
  return snprintf(str, size, "BF /S label");
} else if ((instr & 0xff00) == 0x8900) {
  return snprintf(str, size, "BT label");
} else if ((instr & 0xff00) == 0x8d00) {
  return snprintf(str, size, "BT /S label");
} else if ((instr & 0xff00) == 0x8800) {
  return snprintf(str, size, "CMP /EQ #imm, R0");
} else {
  return -1;
}
}

static int line9(char *str, size_t size, uint16_t instr) {
if ((instr & 0xf000) == 0x9000) {
  return snprintf(str, size, "MOV.W @(disp, PC), R%hu", (uint16_t)((instr >> 8) & 0xF));
} else {
  return -1;
}
}

static int line10(char *str, size_t size, uint16_t instr) {
if ((instr & 0xf000) == 0xa000) {
  return snprintf(str, size, "BRA label");
} else {
  return -1;
}
}

static int line11(char *str, size_t size, uint16_t instr) {
if ((instr & 0xf000) == 0xb000) {
  return snprintf(str, size, "BSR label");
} else {
  return -1;
}
}

static int line12(char *str, size_t size, uint16_t instr) {
if ((instr & 0xff00) == 0xc000) {
  return snprintf(str, size, "MOV.B R0, @(disp, GBR)");
} else if ((instr & 0xff00) == 0xc100) {
  return snprintf(str, size, "MOV.W R0, @(disp, GBR)");
} else if ((instr & 0xff00) == 0xc200) {
  return snprintf(str, size, "MOV.L R0, @(disp, GBR)");
} else if ((instr & 0xff00) == 0xc400) {
  return snprintf(str, size, "MOV.B @(disp, GBR), R0");
} else if ((instr & 0xff00) == 0xc500) {
  return snprintf(str, size, "MOV.W @(disp, GBR), R0");
} else if ((instr & 0xff00) == 0xc600) {
  return snprintf(str, size, "MOV.L @(disp, GBR), R0");
} else if ((instr & 0xff00) == 0xc700) {
  return snprintf(str, size, "MOVA @(disp, PC), R0");
} else if ((instr & 0xff00) == 0xcd00) {
  return snprintf(str, size, "AND.B #imm, @(R0, GBR)");
} else if ((instr & 0xff00) == 0xcf00) {
  return snprintf(str, size, "OR.B #imm, @(R0, GBR)");
} else if ((instr & 0xff00) == 0xcc00) {
  return snprintf(str, size, "TST.B #imm, @(R0, GBR)");
} else if ((instr & 0xff00) == 0xce00) {
  return snprintf(str, size, "XOR.B #imm, @(R0, GBR)");
} else if ((instr & 0xff00) == 0xc900) {
  return snprintf(str, size, "AND #imm, R0");
} else if ((instr & 0xff00) == 0xcb00) {
  return snprintf(str, size, "OR #imm, R0");
} else if ((instr & 0xff00) == 0xc800) {
  return snprintf(str, size, "TST #imm, R0");
} else if ((instr & 0xff00) == 0xca00) {
  return snprintf(str, size, "XOR #imm, R0");
} else if ((instr & 0xff00) == 0xc300) {
  return snprintf(str, size, "TRAPA #imm");
} else {
  return -1;
}
}

static int line13(char *str, size_t size, uint16_t instr) {
if ((instr & 0xf000) == 0xd000) {
  return snprintf(str, size, "MOV.L @(disp, PC), R%hu", (uint16_t)((instr >> 8) & 0xF));
} else {
  return -1;
}
}

static int line14(char *str, size_t size, uint16_t instr) {
if ((instr & 0xf000) == 0xe000) {
  return snprintf(str, size, "MOV #imm, R%hu", (uint16_t)((instr >> 8) & 0xF));
} else {
  return -1;
}
}

static int line15(char *str, size_t size, uint16_t instr) {
{
  return -1;
}
}

typedef int (*linefn)(char *, size_t, uint16_t);
linefn line_fns[] = {
  line0, line1, line2, line3, line4, line5, line6, line7, line8, line9, line10, line11, line12, line13, line14, line15
};
int op_name(char *str, size_t size, uint16_t instr) {
  snprintf(str, size, "ERR");
  linefn l = line_fns[(instr >> 12) & 0xF];
  return l(str, size, instr);
}

void print_instr(uint16_t instr) {
  char buf[256];
  op_name(buf, sizeof(buf), instr);
  printf("%s", buf);
}
