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


#if !defined(SH2_H_INCLUDED)
#define SH2_H_INCLUDED

extern short gdb_sh2_stepped_opcode;

extern void gdb_unhandled_isr (void);
extern void gdb_trapa32_isr (void);
extern void gdb_trapa33_isr (void);
extern void gdb_trapa34_isr (void);
extern void gdb_illegalinst_isr (void);
extern void gdb_addresserr_isr (void);

#endif /* SH2_H_INCLUDED */

