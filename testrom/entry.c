/*
  Copyright (c) 2001 by      William A. Gatliff
  All rights reserved.      bgat@billgatliff.com
  Copyright (c) 2009 by      D. Jeff Dionne
  All rights reserved.      jeff@uClinux.org

  See the file COPYING for details.

  This file is provided "as-is", and without any express
  or implied warranties, including, without limitation,
  the implied warranties of merchantability and fitness
  for a particular purpose.

  The authors welcome feedback regarding this file.

  Simple GDB ROM for SH J2 archtecture devices
*/

#include "gdb.h"
#include "sh2.h"

void uart_tx(unsigned char c);
unsigned char uart_rx();
void main_sh();
void alutest();

int gdb_putc (int c)
	{
	uart_tx(c & 0xff);
	return c & 0xff;
	}

int gdb_getc (void)
	{
	return uart_rx();
	}


void gdb_monitor_onexit (void) {}

void gdb_startup (void)
	{
	main_sh();
	}


__asm__(

".section .vect\n"
".align 2\n"
".global _vector_table\n"
"_vector_table:\n"

"  .long  _start                /*  0: power-on reset */\n"
"  .long  _stack+0x2fc\n"
"  .long  _start                 /*  2: manual reset */\n"
"  .long  _stack+0x2fc\n"
"  .long _gdb_illegalinst_isr   /*  4: general illegal instruction */\n"
"  .long _gdb_unhandled_isr     /*  5: (reserved) */\n"
"  .long _gdb_illegalinst_isr   /*  6: slot illegal instruction */\n"
"  .long _gdb_unhandled_isr     /*  7: (reserved) */\n"
"  .long _gdb_unhandled_isr     /*  8: (reserved) */\n"
"  .long _gdb_addresserr_isr    /*  9: CPU address error */\n"
"  .long _gdb_addresserr_isr    /* 10: DMAC/DTC address error */\n"

"  .long _gdb_nmi_isr     /* 11: NMI */\n"
"  .long _gdb_unhandled_isr     /* 12: UBC */\n"
"  .long _gdb_unhandled_isr     /* 13: (reserved) */\n"
"  .long _gdb_unhandled_isr     /* 14: (reserved) */\n"
"  .long _gdb_unhandled_isr     /* 15: (reserved) */\n"
#if 0	/* using sh2i.c for interrupts test */
"  .long _gdb_pit_isr     	/* 16: PIT */\n"
"  .long _gdb_ihandler_emac     /* 0x11: (EMAC interface) */\n"
"  .long _gdb_unhandled_isr     /* 0x12: (reserved) */\n"
"  .long _gdb_unhandled_isr     /* 0x13: (reserved) */\n"
"  .long _gdb_unhandled_isr     /* 0x14: (reserved) */\n"

"  .long _gdb_unhandled_isr     /* 0x15: (reserved) */\n"
"  .long _gdb_unhandled_isr     /* 0x16: (reserved) */\n"
"  .long _gdb_ihandler_uart1    /* 0x17: (UART Console) */\n"
"  .long _gdb_unhandled_isr     /* 0x18: (reserved) */\n"
#else	/* interrupts default to save space */
"  .long _gdb_unhandled_isr    	/* 16: PIT */\n"
"  .long _gdb_unhandled_isr     /* 0x11: (EMAC interface) */\n"
"  .long _gdb_unhandled_isr     /* 0x12: (reserved) */\n"
"  .long _gdb_unhandled_isr     /* 0x13: (reserved) */\n"
"  .long _gdb_unhandled_isr     /* 0x14: (reserved) */\n"

"  .long _gdb_unhandled_isr     /* 0x15: (reserved) */\n"
"  .long _gdb_unhandled_isr     /* 0x16: (reserved) */\n"
"  .long _gdb_unhandled_isr     /* 0x17: (UART Console) */\n"
"  .long _gdb_unhandled_isr     /* 0x18: (reserved) */\n"
#endif
"  .long _gdb_unhandled_isr     /* 25: (when AIC countdown reach 0) */\n"
"  .long _gdb_unhandled_isr     /* 26: (reserved) */\n"
"  .long _gdb_unhandled_isr     /* 27: (reserved) */\n"
"  .long _gdb_unhandled_isr     /* 28: (reserved) */\n"
"  .long _gdb_unhandled_isr     /* 29: (reserved) */\n"
"  .long _gdb_unhandled_isr     /* 30: (reserved) */\n"

"  .long _gdb_unhandled_isr     /* 31: (reserved) */\n"
"  .long _gdb_trapa32_isr       /* 32: trap 32 instruction */\n"
"  .long _gdb_trapa33_isr       /* 33: trap 33 instruction */\n"
"  .long _gdb_trapa34_isr       /* 34: trap 34 instruction */\n"
"  .long _gdb_unhandled_isr     /* */\n"
"  .long _gdb_unhandled_isr     /* */\n"
"  .long _gdb_unhandled_isr     /* */\n"
"  .long _gdb_unhandled_isr     /* */\n"
"  .long _gdb_unhandled_isr     /* */\n"

"  .long _gdb_unhandled_isr\n"
"  .long _gdb_unhandled_isr\n"
"  .long _gdb_unhandled_isr\n"
"  .long _gdb_unhandled_isr\n"
"  .long _gdb_unhandled_isr\n"
"  .long _gdb_unhandled_isr\n"
"  .long _gdb_unhandled_isr\n"
"  .long _gdb_unhandled_isr\n"
"  .long _gdb_unhandled_isr\n"
"  .long _gdb_unhandled_isr\n"

"  .long _gdb_unhandled_isr\n"
"  .long _gdb_unhandled_isr\n"
"  .long _gdb_unhandled_isr\n"
"  .long _gdb_unhandled_isr\n"
"  .long _gdb_unhandled_isr\n"
"  .long _gdb_unhandled_isr\n"
"  .long _gdb_unhandled_isr\n"
"  .long _gdb_unhandled_isr\n"
"  .long _gdb_unhandled_isr\n"
"  .long _gdb_unhandled_isr\n"

"  .long _gdb_unhandled_isr\n"
"  .long _gdb_unhandled_isr\n"
"  .long _gdb_unhandled_isr\n"
"  .long _gdb_unhandled_isr\n"
""
".section .text\n"
".align 2\n"
"_testjsr:\n"
"  mov.l jsr_leds, r0\n"
"  mov.l pio_addr, r1\n"
"  mov.l r0, @r1\n" 
"  rts\n"
"  nop\n"
""
".section .text\n"
".align 2\n"
".global _start\n"
".global start\n"
"start:\n"
"_start:\n"
"  nop\n"
"  mov.l start_leds, r0\n"
"  mov.l pio_addr, r1\n"
"  mov.l r0, @r1\n" 
"  mov.l testjsr_k, r0\n"
"  jsr @r0\n"
"  nop\n"
"  mov.l pio_addr, r1\n"
"  mov.l start1_leds, r0\n"
"  mov.l r0, @r1\n" 
"  mov.l testbra_k, r0\n"
"  jsr @r0\n"
"  nop\n"
"  mov.l testmov_k, r0\n"
"  jsr @r0\n"
"  nop\n"
"  mov.l testmov2_k, r0\n"
"  jsr @r0\n"
"  nop\n"
"  mov.l testalu_k, r0\n"
"  jsr @r0\n"
"  nop\n"
"  mov.l testshift_k, r0\n"
"  jsr @r0\n"
"  nop\n"
"  mov.l testmul_k, r0\n"
"  jsr @r0\n"
"  nop\n"
"  mov.l testmulu_k, r0\n"
"  jsr @r0\n"
"  nop\n"
"  mov.l testmuls_k, r0\n"
"  jsr @r0\n"
"  nop\n"
"  mov.l testmull_k, r0\n"
"  jsr @r0\n"
"  nop\n"
"  mov.l testdmulu_k, r0\n"
"  jsr @r0\n"
"  nop\n"
"  mov.l testdmuls_k, r0\n"
"  jsr @r0\n"
"  nop\n"
"  mov.l testmulconf_k, r0\n"
"  jsr @r0\n"
"  nop\n"
"  mov.l testdiv_k, r0\n"
"  jsr @r0\n"
"  nop\n"
"  mov.l testmacw_k, r0\n"
"  jsr @r0\n"
"  nop\n"
"  mov.l testmacl_k, r0\n"
"  jsr @r0\n"
"  nop\n"
"  mov   #0, r0\n"
"  mov   #1, r1\n"
"  mov   #2, r2\n"
"  mov   #3, r3\n"
"  mov   #4, r4\n"
"  mov   #5, r5\n"
"  mov   #6, r6\n"
"  mov   #7, r7\n"
"  mov   #8, r8\n"
"  mov   #9, r9\n"
"  mov   #10, r10\n"
"  mov   #11, r11\n"
"  mov   #12, r12\n"
"  mov   #13, r13\n"
"  mov   #0, r14\n"
"  ldc   r14, vbr\n"
"  ldc   r14, gbr\n"
"  ldc   r14, sr\n"
"  mov.l gdbstartup_k, r0\n"
"  jsr @r0\n"
"  nop\n"
"  trapa #32\n"
"  nop\n"
".align 2\n"
"gdbstartup_k: .long _gdb_startup\n"
"gdbmonitor_k: .long _gdb_monitor\n"
"testjsr_k:    .long _testjsr\n"
"testbra_k:    .long _testbra\n"
"testmov_k:    .long _testmov\n"
"testmov2_k:   .long _testmov2\n"
"testalu_k:    .long _testalu\n"
"testshift_k:  .long _testshift\n"
"testmul_k:    .long _testmul\n"
"testmulu_k:   .long _testmulu\n"
"testmuls_k:   .long _testmuls\n"
"testmull_k:   .long _testmull\n"
"testdmulu_k:  .long _testdmulu\n"
"testdmuls_k:  .long _testdmuls\n"
"testmulconf_k: .long _testmulconf\n"
"testdiv_k:    .long _testdiv\n"
"testmacw_k:   .long _testmacw\n"
"testmacl_k:   .long _testmacl\n"
"pio_addr:     .long 0xABCD0000\n"
"start_leds:   .long 0x000000ff\n"
"start1_leds:   .long 0x0000004f\n"
"jsr_leds:     .long 0x00000011\n"
);

