/*
  Copyright (c) 2001 by      William A. Gatliff
  All rights reserved.      bgat@billgatliff.com

  See the file COPYING for details.

  This file is provided "as-is", and without any express
  or implied warranties, including, without limitation,
  the implied warranties of merchantability and fitness
  for a particular purpose.

  The author welcomes feedback regarding this file.
*/

/* $Id$ */


/*
  This is code for the Hitachi SH-2 processor family.  Stepping is
  done via code disassembly and replacement of TRAP opcodes, which
  means that you can't step code that lives in flash.
*/

#include "gdb.h"
#include "sh2.h"


typedef enum {
  R0 = 0, R1, R2, R3, R4, R5, R6, R7, R8, R9, R10, R11, R12, R13, R14, R15,
  PC, PR, GBR, VBR, MACH, MACL, SR
} register_id_E;

typedef struct {
  unsigned long pr;
  unsigned long gbr;
  unsigned long *vbr;
  unsigned long mach;
  unsigned long macl;
  unsigned long r[16];
  unsigned long pc;
  unsigned long sr;
	} register_file_S;

static register_file_S register_file;
short gdb_sh2_stepped_opcode;


/* stuff for stepi */
#define OPCODE_BT(op)         (((op) & 0xff00) == 0x8900)
#define OPCODE_BF(op)         (((op) & 0xff00) == 0x8b00)
#define OPCODE_BTF_DISP(op) \
 (((op) & 0x80) ? (((op) | 0xffffff80) << 1) : (((op) & 0x7f ) << 1))
#define OPCODE_BFS(op)        (((op) & 0xff00) == 0x8f00)
#define OPCODE_BTS(op)        (((op) & 0xff00) == 0x8d00)
#define OPCODE_BRA(op)        (((op) & 0xf000) == 0xa000)
#define OPCODE_BRA_DISP(op) \
 (((op) & 0x800) ? (((op) | 0xfffff800) << 1) : (((op) & 0x7ff) << 1))
#define OPCODE_BRAF(op)       (((op) & 0xf0ff) == 0x0023)
#define OPCODE_BRAF_REG(op)   (((op) & 0x0f00) >> 8)
#define OPCODE_BSR(op)        (((op) & 0xf000) == 0xb000)
#define OPCODE_BSR_DISP(op) \
 (((op) & 0x800) ? (((op) | 0xfffff800) << 1) : (((op) & 0x7ff) << 1))
#define OPCODE_BSRF(op)       (((op) & 0xf0ff) == 0x0003)
#define OPCODE_BSRF_REG(op)   (((op) >> 8) & 0xf)
#define OPCODE_JMP(op)        (((op) & 0xf0ff) == 0x402b)
#define OPCODE_JMP_REG(op)    (((op) >> 8) & 0xf)
#define OPCODE_JSR(op)        (((op) & 0xf0ff) == 0x400b)
#define OPCODE_JSR_REG(op)    (((op) >> 8) & 0xf)
#define OPCODE_RTS(op)        ((op) == 0xb)
#define OPCODE_RTE(op)        ((op) == 0x2b)
#define OPCODE_TRAPA(op)      (((op) & 0xff00) == 0xc300)
#define OPCODE_TRAPA_DISP(op) ((op) & 0x00ff)


#define SR_T_BIT_MASK         0x1

#define STEP_OPCODE           0xc320


/*
  Analyzes the next instruction, to see where the program
  will go to when it runs.  Returns the destination address.
*/
static long get_stepi_dest (void)
	{
	short op = *(short*)register_file.pc;
	long addr = register_file.pc + 2;


	/* BT, BT/S (untested!), BF and BF/S (untested!)
	   TODO: test delay-slot branches */
	if (((OPCODE_BT(op) || OPCODE_BTS(op))
		 && (register_file.sr & SR_T_BIT_MASK))
		|| ((OPCODE_BF(op) || OPCODE_BFS(op))
			&& !(register_file.sr & SR_T_BIT_MASK)))
		{
    
		/* we're taking the branch */
    
		/* per 6.12 of the SH1/SH2 programming manual,
		   PC+disp is address of the second instruction
		   after the branch instruction, so we have to add 4 */
		/* TODO: spend more time understanding this magic */ 
		addr = register_file.pc + 4 + OPCODE_BTF_DISP(op);
		}
  
	/* BRA */
	else if (OPCODE_BRA(op))
		addr = register_file.pc + 4 + OPCODE_BRA_DISP(op);
  
	/* BRAF */
	else if (OPCODE_BRAF(op))
		addr = register_file.pc + 4
		  + register_file.r[OPCODE_BRAF_REG(op)];
  
	/* BSR */
	else if (OPCODE_BSR(op))
		addr = register_file.pc + 4 + OPCODE_BSR_DISP(op);
  
	/* BSRF */
	else if (OPCODE_BSRF(op))
		addr = register_file.pc + 4
		  + register_file.r[OPCODE_BSRF_REG(op)];

	/* JMP */
	else if (OPCODE_JMP(op))
		addr = register_file.r[OPCODE_JMP_REG(op)];

	/* JSR */
	else if (OPCODE_JSR(op))
		addr = register_file.r[OPCODE_JSR_REG(op)];
  
	/* RTS */
	else if (OPCODE_RTS(op))
		addr = register_file.pr;

	/* RTE */
	else if (OPCODE_RTE(op))
		addr = *(unsigned long*)(register_file.r[15]);

	/* TRAPA */
	else if (OPCODE_TRAPA(op))
		addr = register_file.vbr[OPCODE_TRAPA_DISP(op)];

	return addr;
	}


/*
  Uses a TRAP to generate an exception
  after we run the next instruction.
*/
void gdb_step (long addr)
	{
	long dest_addr;

	if (addr)
		register_file.pc = addr;

	/* determine where the step will take us */
	dest_addr = get_stepi_dest();
  
	/* save the target opcode, replace with STEP_OPCODE */
	gdb_sh2_stepped_opcode = *(short*)dest_addr;
	*(short*)dest_addr = STEP_OPCODE;

	gdb_return_from_exception();
	return;
	}


/*
  Retrieves a register value from gdb_register_file.  Returns the size
  of the register, in bytes, or zero if an invalid id is specified
  (which *will* happen--- gdb.c uses this functionality to tell how
  many registers we actually have).
*/
int gdb_peek_register_file (int id, long* val)
	{
	/* all our registers are longs */
	int retval = sizeof(long);

	switch (id)
		{
		case R0:  case R1:  case R2:  case R3:
		case R4:  case R5:  case R6:  case R7:
		case R8:  case R9:  case R10: case R11:
		case R12: case R13: case R14: case R15:
		  *val = register_file.r[id];
		  break;

		case PC: *val = register_file.pc; break;
		case PR: *val = register_file.pr; break;
		case GBR: *val = register_file.gbr; break;
		case VBR: *val = (long)register_file.vbr; break;
		case MACH: *val = register_file.mach; break;
		case MACL: *val = register_file.macl; break;
		case SR: *val = register_file.sr; break;
		default: retval = 0;
		}

	return retval;
	}

#define PORT (*(volatile unsigned long  *)0xabcd0000)

/*
  Stuffs a register value into gdb_register_file.  Returns the size of
  the register, in bytes, or zero if an invalid id is specified.
*/
int gdb_poke_register_file (int id, long val)
	{
	/* all our registers are longs */
	int retval = sizeof(long);

	switch( id )
		{
		case R0:  case R1:  case R2:  case R3:
		case R4:  case R5:  case R6:  case R7:
		case R8:  case R9:  case R10: case R11:
		case R12: case R13: case R14: case R15:
		  register_file.r[id] = val;
		  break;

		case PC: register_file.pc = val; break;
		case PR: register_file.pr = val; break;
		case GBR: register_file.gbr = val; break;
		case VBR: register_file.vbr = (void *)val; break;
		case MACH: register_file.mach = val; break;
		case MACL: register_file.macl = val; break;
		case SR: register_file.sr = val; break;
		default: retval = 0;
		}

	return retval;
	}


/*
  Releases the application to run.
*/
void gdb_continue (long addr)
	{
	if (addr) register_file.pc = addr;
	gdb_return_from_exception();
	return;
	}


/*
  The stub calls this before dropping into the monitor, to give us a
  chance to clean things like software stepping up.
*/
void gdb_monitor_onentry (void)
	{
	/* if we're stepping, then undo the step */
	if (gdb_sh2_stepped_opcode)
		{
		*(short*)register_file.pc = gdb_sh2_stepped_opcode;
		gdb_sh2_stepped_opcode = 0;
		}
	return;
	}

/*
   Catches TRAPA #34 calls from newlib and other runtime library
   stubs.  Currently only handles SYS_write.
   TODO: fix magic numbers.
*/
int gdb_trapa34 (int syscall, int arg1, int arg2, int arg3)
	{
	return gdb_file_io(syscall, arg1, arg2, arg3);
	}


static int i_cnt = 50; /* toggle LED 2Hz */
static int led = 0;
void gdb_pit ()
{
        if (!(i_cnt--)) {
                i_cnt = 50;
                if (!led) PORT = led = 0x088;
                else      PORT = led = 0x000;
        }
}

void gdb_flush_cache (void *start, int len) { return; }


__asm__(

".section .text\n"
"save_registers_handle_exception:\n"

/*
  Generic code to save processor context.
  Assumes the stack looks like this:

  sigval<-r15
  r1
  r0
  pc
  sr
*/

  /* find end of register_file */
"  mov.l register_file_end, r0\n"

  /* copy sr to register file */
"  mov.l @(16, r15), r1\n"
"  mov.l r1, @r0\n"

  /* copy pc to register file */
"  mov.l @(12, r15), r1\n"
"  mov.l r1, @-r0\n"

  /* sigval, r1, r0, pc, sr are already on the stack, */
  /* so r15 isn't the same as it was immediately before */
  /* we took the current exception.  We have to adjust */
  /* r15 in the register file so that gdb gets the right */
  /* stack pointer value */
"  mov r15, r1\n"
"  add #20, r1\n"
"  mov.l r1, @-r0\n"

  /* save r14-r2 */
"  mov.l r14, @-r0\n"
"  mov.l r13, @-r0\n"
"  mov.l r12, @-r0\n"
"  mov.l r11, @-r0\n"
"  mov.l r10, @-r0\n"
"  mov.l r9, @-r0\n"
"  mov.l r8, @-r0\n"
"  mov.l r7, @-r0\n"
"  mov.l r6, @-r0\n"
"  mov.l r5, @-r0\n"
"  mov.l r4, @-r0\n"
"  mov.l r3, @-r0\n"
"  mov.l r2, @-r0\n"

  /* copy r1 to register file */
"  mov.l @(4, r15), r1\n"
"  mov.l r1, @-r0\n"

  /* copy r0 to register file */
"  mov.l @(8, r15), r1\n"
"  mov.l r1, @-r0\n"

  /* save macl, mach, vbr, gbr, pr in register file */
"  sts.l macl, @-r0\n"
"  sts.l mach, @-r0\n"
"  stc.l vbr, @-r0\n"
"  stc.l gbr, @-r0\n"
"  sts.l pr, @-r0\n"

  /* call gdb_handle_exception */
"  mov.l handle_exception, r0\n"
"  mov.l @r15, r4\n"
"  jsr @r0\n"
"  nop\n"

"  .align 2\n"
"  handle_exception: .long _gdb_handle_exception\n"
"  register_file_end: .long _register_file+88\n"


/*
  TRAPA #32 (breakpoint) isr.
  Sends a SIGTRAP to gdb_handle_exception().

  Because we always subtract 2 from the pc
  stacked during exception processing, this
  function won't permit compiled-in breakpoints.
  If you compile a TRAPA #32 into the code, we'll
  loop on it indefinitely.  Use TRAPA #33 instead.
*/
".section .text\n"
".global _gdb_trapa32_isr\n"
"_gdb_trapa32_isr:\n"

  /* put r0, r1 on the stack */
"  mov.l r0, @-r15\n"
"  mov.l r1, @-r15\n"

  /* disable interrupts */
"  mov   #0xf0, r0\n"
"  ldc   r0, sr\n"

  /* put SIGTRAP on stack */
"  mov #5, r0\n"
"  mov.l r0, @-r15\n"

  /* fudge pc, so we re-execute the instruction replaced
     by the trap; this breaks compiled-in breakpoints! */
"  mov.l @(12, r15), r0\n"
"  add #-2, r0\n"
"  mov.l r0, @(12, r15)\n"

  /* save registers, call gdb_handle_exception */
"  bra save_registers_handle_exception\n"
"  nop\n"


".section .text\n"
".global _gdb_trapa33_isr\n"
"_gdb_trapa33_isr:\n"
"  mov.l r0, @-r15\n"
"  mov   #0xf0, r0\n"
"  ldc   r0, sr\n"
"  mov.l r1, @-r15\n"
"  mov #5, r0\n"
"  mov.l r0, @-r15\n"
"  bra save_registers_handle_exception\n"
"  nop\n"

/*
   PIT
*/
".section .text\n"
".global _gdb_my_isr\n"
"_gdb_my_isr:\n"
"  sts.l pr,@-r15\n"
"  bsr _gdb_pit\n"
"  nop\n"
"  lds.l @r15+, pr\n"
"  rte\n"
"  nop\n"

/*
   TRAPA #34 handler.  Used by newlib et al for system calls.  We
   include it here so that printf() and family get automagically bound
   to gdb_console_write().
*/
".section .text\n"
".global _gdb_trapa34_isr\n"
"_gdb_trapa34_isr:\n"
"  sts.l pr,@-r15\n"
"  bsr _gdb_trapa34\n"
"  nop\n"
"  lds.l @r15+, pr\n"
"  rte\n"
"  nop\n"

".section .text\n"
".global _gdb_unhandled_isr\n"
"_gdb_unhandled_isr:\n"
"  mov.l r0, @-r15\n"
"  mov   #0xf0, r0\n"
"  ldc   r0, sr\n"
"  mov.l r1, @-r15\n"
"  mov #30, r0\n"
"  mov.l r0, @-r15\n"
"  bra save_registers_handle_exception\n"
"  nop\n"

".section .text\n"
".global _gdb_nmi_isr\n"
"_gdb_nmi_isr:\n"
"  mov.l r0, @-r15\n"
"  mov   #0xf0, r0\n"
"  ldc   r0, sr\n"
"  mov.l r1, @-r15\n"
"  mov #2, r0\n"
"  mov.l r0, @-r15\n"
"  bra save_registers_handle_exception\n"
"  nop\n"

".section .text\n"
".global _gdb_illegalinst_isr\n"
"_gdb_illegalinst_isr:\n"
"  mov.l r0, @-r15\n"
"  mov   #0xf0, r0\n"
"  ldc   r0, sr\n"
"  mov.l r1, @-r15\n"
"  mov #4, r0\n"
"  mov.l r0, @-r15\n"
"  bra save_registers_handle_exception\n"
"  nop\n"

".section .text\n"
".global _gdb_addresserr_isr\n"
"_gdb_addresserr_isr:\n"
"  mov.l r0, @-r15\n"
"  mov   #0xf0, r0\n"
"  ldc   r0, sr\n"
"  mov.l r1, @-r15\n"
"  mov #11, r0\n"
"  mov.l r0, @-r15\n"
"  bra save_registers_handle_exception\n"
"  nop\n"


/* Restores registers to the values specified in register_file.  */
".section .text\n"
".global _gdb_return_from_exception\n"
"_gdb_return_from_exception:\n"

  /* find register_file */
"  mov.l register_file, r0\n"
"  lds.l @r0+, pr\n"
"  ldc.l @r0+, gbr\n"
"  ldc.l @r0+, vbr\n"
"  lds.l @r0+, mach\n"
"  lds.l @r0+, macl\n"

  /* skip r0 and r1 for now,
     since we're using them */
"  add #8, r0\n"

"  mov.l @r0+, r2\n"
"  mov.l @r0+, r3\n"
"  mov.l @r0+, r4\n"
"  mov.l @r0+, r5\n"
"  mov.l @r0+, r6\n"
"  mov.l @r0+, r7\n"
"  mov.l @r0+, r8\n"
"  mov.l @r0+, r9\n"
"  mov.l @r0+, r10\n"
"  mov.l @r0+, r11\n"
"  mov.l @r0+, r12\n"
"  mov.l @r0+, r13\n"
"  mov.l @r0+, r14\n"
"  mov.l @r0+, r15\n"

  /* put sr onto stack */
"  mov.l @(4,r0), r1\n"
"  mov.l r1, @-r15\n"

  /* put pc onto stack */
"  mov.l @r0, r1\n"
"  mov.l r1, @-r15\n"

  /* restore r1, r0 */
"  add #-64, r0\n"
"  mov.l @(4,r0), r1\n"
"  mov.l @r0, r0\n"

"  rte\n"
"  nop\n"
".align 2\n"
"  register_file: .long _register_file\n"


/* "kill" and "detach" try to simulate a reset */
".section .text\n"
".global _gdb_kill\n"
".global _gdb_detach\n"
"_gdb_kill:\n"
"_gdb_detach:\n"
"   mov #4, r15\n"
"   mov.l @r15, r15\n"
"   mov #0, r0\n"
"   mov.l @r0, r0\n"
"   jmp @r0\n"
"   nop\n"

);
