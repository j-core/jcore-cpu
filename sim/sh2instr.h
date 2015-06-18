#ifndef SH2_INSTR
#define SH2_INSTR

#include <stdint.h>
#include <stdio.h>

void print_instr(uint16_t instr);
int op_name(char *str, size_t size, uint16_t instr);

#endif
