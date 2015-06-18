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

#if !defined(GDB_H_INCLUDED)
#define GDB_H_INCLUDED


/* platform-specific stuff, in <target>[-<platform>].c */
int  gdb_putc (int c);
int  gdb_getc (void);
int  gdb_peek_register_file (int id, long *val);
int  gdb_poke_register_file (int id, long val);
void gdb_step (long addr);
void gdb_continue (long addr);
void gdb_kill (void);
void gdb_detach (void);
void gdb_return_from_exception (void);
void gdb_flush_cache (void *start, int len);
void gdb_monitor_onentry (void);
void gdb_monitor_onexit (void);
void gdb_startup (void);

/* platform-neutral stuff, in gdb.c */
void gdb_console_output (int len, const char *buf);
int gdb_file_io (int syscall, int arg1, int arg2, int arg3);
int gdb_monitor (int sigval);
void gdb_handle_exception (int sigval);


/* gdb signal values */
#define GDB_SIGHUP           1
#define GDB_SIGINT           2
#define GDB_SIGQUIT          3
#define GDB_SIGILL           4
#define GDB_SIGTRAP          5
#define GDB_SIGABRT          6
#define GDB_SIGIOT           6
#define GDB_SIGBUS           7
#define GDB_SIGFPE           8
#define GDB_SIGKILL          9
#define GDB_SIGUSR1         10
#define GDB_SIGSEGV         11
#define GDB_SIGUSR2         12
#define GDB_SIGPIPE         13
#define GDB_SIGALRM         14
#define GDB_SIGTERM         15
#define GDB_SIGSTKFLT       16
#define GDB_SIGCHLD         17
#define GDB_SIGCONT         18
#define GDB_SIGSTOP         19
#define GDB_SIGTSTP         20
#define GDB_SIGTTIN         21
#define GDB_SIGTTOU         22
#define GDB_SIGURG          23
#define GDB_SIGXCPU         24
#define GDB_SIGXFSZ         25
#define GDB_SIGVTALRM       26
#define GDB_SIGPROF         27
#define GDB_SIGWINCH        28
#define GDB_SIGIO           29



#endif /* GDB_H_INCLUDED */
