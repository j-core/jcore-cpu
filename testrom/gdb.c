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


/* The gdb remote communication protocol.

   A debug packet whose contents are <data>
   is encapsulated for transmission in the form:

        $ <data> # CSUM1 CSUM2

        <data> must be ASCII alphanumeric and cannot include characters
        '$' or '#'.  If <data> starts with two characters followed by
        ':', then the existing stubs interpret this as a sequence number.

        CSUM1 and CSUM2 are ascii hex representation of an 8-bit
        checksum of <data>, the most significant nibble is sent first.
        the hex digits 0-9,a-f are used.

   Receiver responds with:

        +       - if CSUM is correct and ready for next packet
        -       - if CSUM is incorrect

   <data> is as follows:
   All values are encoded in ascii hex digits.

        Request         Packet

        read registers  g
        reply           XX....X         Each byte of register data
                                        is described by two hex digits.
                                        Registers are in the internal order
                                        for GDB, and the bytes in a register
                                        are in the same order the machine uses.
                        or ENN          for an error.

        write regs      GXX..XX         Each byte of register data
                                        is described by two hex digits.
        reply           OK              for success
                        ENN             for an error

        write reg       Pn...=r...      Write register n... with value r...,
                                        which contains two hex digits for each
                                        byte in the register (target byte
                                        order).
        reply           OK              for success
                        ENN             for an error
        (not supported by all stubs).

        read mem        mAA..AA,LLLL    AA..AA is address, LLLL is length.
        reply           XX..XX          XX..XX is mem contents
                                        Can be fewer bytes than requested
                                        if able to read only part of the data.
                        or ENN          NN is errno

        write mem       MAA..AA,LLLL:XX..XX
                                        AA..AA is address,
                                        LLLL is number of bytes,
                                        XX..XX is data
        reply           OK              for success
                        ENN             for an error (this includes the case
                                        where only part of the data was
                                        written).

        write mem       XAA..AA,LLLL:XX..XX
         (binary)                       AA..AA is address,
                                        LLLL is number of bytes,
                                        XX..XX is binary data
        reply           OK              for success
                        ENN             for an error

        cont            cAA..AA         AA..AA is address to resume
                                        If AA..AA is omitted,
                                        resume at same address.

        step            sAA..AA         AA..AA is address to resume
                                        If AA..AA is omitted,
                                        resume at same address.

        last signal     ?               Reply the current reason for stopping.
                                        This is the same reply as is generated
                                        for step or cont : SAA where AA is the
                                        signal number.

        There is no immediate reply to step or cont.
        The reply comes when the machine stops.
        It is           SAA             AA is the "signal number"

        or...           TAAn...:r...;n:r...;n...:r...;
                                        AA = signal number
                                        n... = register number
                                        r... = register contents
        or...           WAA             The process exited, and AA is
                                        the exit status.  This is only
                                        applicable for certains sorts of
                                        targets.
        kill request    k

        toggle debug    d               toggle debug flag (see 386 & 68k stubs)
        reset           r               reset -- see sparc stub.
        reserved        <other>         On other requests, the stub should
                                        ignore the request and send an empty
                                        response ($#<checksum>).  This way
                                        we can extend the protocol and GDB
                                        can tell whether the stub it is
                                        talking to uses the old or the new.
        search          tAA:PP,MM       Search backwards starting at address
                                        AA for a match with pattern PP and
                                        mask MM.  PP and MM are 4 bytes.
                                        Not supported by all stubs.

        general query   qXXXX           Request info about XXXX.
        general set     QXXXX=yyyy      Set value of XXXX to yyyy.
        query sect offs qOffsets        Get section offsets.  Reply is
                                        Text=xxx;Data=yyy;Bss=zzz
        console output  Otext           Send text to stdout.  Only comes from
                                        remote target.

        Responses can be run-length encoded to save space.  A '*' means that
        the next character is an ASCII encoding giving a repeat count which
        stands for that many repititions of the character preceding the '*'.
        The encoding is n+29, yielding a printable character where n >=3
        (which is where rle starts to win).  Don't use an n > 126.

        So
        "0* " means the same as "0000".
*/

#include "gdb.h"
#include "syscalls.h"

#if !defined(GDB_RXBUFLEN)
#define GDB_RXBUFLEN 200
#endif

#define min(a,b) ((a) > (b) ? (b) : (a))
#define max(a,b) ((a) > (b) ? (a) : (b))

#define is_aligned_long(addr,len) \
  (((len) >= sizeof(long)) && ((long)(addr) % sizeof(long) == 0))

#define is_aligned_short(addr,len) \
  (((len) >= sizeof(short)) && ((long)(addr) % sizeof(short) == 0))


/* converts '[0-9,a-f,A-F]' to its integer equivalent */
static int hex_to_int (char h)
	{
	if (h >= 'a' && h <= 'f') return h - 'a' + 10;
	if (h >= '0' && h <= '9') return h - '0';
	if (h >= 'A' && h <= 'F') return h - 'A' + 10;
	return 0;
	}


/* converts the low nibble of i to its hex character equivalent */
static char lnibble_to_hex (char i)
	{
	static const char lnibble_to_hex_table[] = "0123456789abcdef";
	return lnibble_to_hex_table[i & 0xf];
	}


/* translates a delimited hex string to a long */
static const char* hargs_parse_long (const char* hargs, long* l, int delim)
	{
	*l = 0;
	while (*hargs != delim) *l = (*l << 4) + hex_to_int(*hargs++)
							  ;
	return hargs + 1;
	}


/*
  TODO: the lcbuf unions assume and depend that lbuf and sbuf start at
  the same address.  Is this always correct?  Is there a better way?
*/

/* Converts a memory region of length len bytes, starting at mem, into
   a string of hex bytes.  Returns the number of bytes placed into
   hexbuf.

   This function carefully preserves the endianness of the data,
   because that's what gdb expects.  This function also optimizes the
   read process into the largest units possible, in case we're reading
   a peripheral register that can't deal with unaligned or byte-wide
   accesses.  */
static int mem_to_hexbuf (const void* mem, char* hbuf, int len)
	{
	int i = 0;
	union
		{
		  long lbuf;
		  short sbuf;
		  char cbuf[sizeof(long)];
		} lcbuf;
	int cbuflen;
	int retval = 0;
  

	while (len > 0)
		{
		if (is_aligned_long(mem, len))
			{
			cbuflen = sizeof (long);
			lcbuf.lbuf = *(long*)mem;
			mem += sizeof (long);
			len -= sizeof (long);
			}
    
		else if (is_aligned_short(mem, len))
			{
			cbuflen = sizeof (short);
			lcbuf.sbuf = *(short*)mem;
			mem += sizeof (short);
			len -= sizeof (short);
			}

		else
			{
			cbuflen = sizeof (char);
			lcbuf.cbuf[0] = *(char*)mem;
			mem += sizeof (char);
			len -= sizeof (char);
			}

		for (i = 0; i < cbuflen; i++ )
			{
			*hbuf++ = lnibble_to_hex(lcbuf.cbuf[i] >> 4);
			*hbuf++ = lnibble_to_hex(lcbuf.cbuf[i]);
			retval += 2;
			}
		}

	return retval;
	}


/*
  Reads (len * 2) hex digits from hbuf, converts them to binary,
  writes them to mem. Returns a pointer to the first empty byte after
  the region written.

  Carefully preserves endianness, optimizes write accesses so as to be
  hardware-friendly.

*/
static char* hexbuf_to_mem (const char* hbuf, void* mem, int len)
	{
	int i = 0;
	union {
	  long lbuf;
	  short sbuf;
	  char cbuf[sizeof(long)];
		} lcbuf;
	void* cache_start = mem;
	int cache_len = len;


	while (len > 0)
		{
		if (is_aligned_long(mem, len))
			{
			for( i = 0; i < sizeof(long); i++ )
				{
				lcbuf.cbuf[i] = (hex_to_int(*hbuf++) << 4);
				lcbuf.cbuf[i] += hex_to_int(*hbuf++);
				}
			*((long*)mem) = lcbuf.lbuf;
			mem += sizeof(long);
			len -= sizeof(long);
			}

		else if (is_aligned_short(mem, len))
			{
			for( i = 0; i < sizeof(short); i++ )
				{
				lcbuf.cbuf[i] = (hex_to_int(*hbuf++) << 4);
				lcbuf.cbuf[i] += hex_to_int(*hbuf++);
				}
			*((short*)mem) = lcbuf.sbuf;
			mem += sizeof(short);
			len -= sizeof(short);
			}

		else
			{
			lcbuf.cbuf[0] = (hex_to_int(*hbuf++) << 4);
			lcbuf.cbuf[0] += hex_to_int(*hbuf++);
			*((char*)mem) = lcbuf.cbuf[0];
			mem += sizeof(char);
			len -= sizeof(char);
			}
		}
  
	gdb_flush_cache(cache_start, cache_len);

	return mem;
	}


static const void* xbin_to_bin( const void* xbin, char* bin)
	{
	if (*(char*)xbin == 0x7d)
		{
		xbin++;
		*bin = *((char*)xbin) ^ 0x20;
		xbin++;
		}
	else
		{
		*bin = *((char*)xbin);
		xbin++;
		}
	return xbin;
	}


/*
  Converts the escaped-binary ('X' packet) array pointed to by buf
  into binary, to be placed in mem. Returns a pointer to the first
  empty byte after the region written.
*/
static char* xmem_to_mem (const char* xmem, void* mem, int len)
	{
	int i = 0;
	union
		{
		  long lbuf;
		  short sbuf;
		  char cbuf[sizeof(long)];
		} lcbuf;
	void* cache_start = mem;
	int cache_len = len;

 
	while (len > 0) {

    if (is_aligned_long(mem, len))
		{
		for (i = 0; i < sizeof(long); i++) 
			xmem = xbin_to_bin(xmem, &lcbuf.cbuf[i]);
		*((long*)mem) = lcbuf.lbuf;
		mem += sizeof (long);
		len -= sizeof (long);
		}

    else if (is_aligned_short(mem, len))
		{
		for( i = 0; i < sizeof(short); i++ )
			xmem = xbin_to_bin(xmem, &lcbuf.cbuf[i]);
		*((short*)mem) = lcbuf.sbuf;
		mem += sizeof (short);
		len -= sizeof (short);
		}

    else
		{
		xmem = xbin_to_bin(xmem, &lcbuf.cbuf[0]);
		*((char*)mem) = lcbuf.cbuf[0];
		mem += sizeof (char);
		len -= sizeof (char);
		}
	}
  
	gdb_flush_cache(cache_start, cache_len);
  
	return mem;
	}


/*
   Writes a buffer of length len to gdb_putc().
   Returns the checksum of the bytes.
*/
static int putbuf (int len, const char* buf)
	{
	unsigned char sum = 0;

	while (len--)
		{
		sum += *(unsigned char*)buf;
		gdb_putc( *buf++ );
		}

	return sum;
	}


/* Sends an RSP message */
static void putmsg (char c, const char *buf, int len)
	{
	unsigned char sum;

	do
		{
		/* send the header */
		gdb_putc('$');

		/* send the message type, if specified */
		if (c) gdb_putc(c);

		/* send the data */
		sum = c + putbuf(len, buf);

		/* send the footer */
		gdb_putc('#');
		gdb_putc(lnibble_to_hex(sum >> 4));
		gdb_putc(lnibble_to_hex(sum));
		}
	while ('+' != gdb_getc());

	return;
	}


/* Reads a message */
static int getmsg (char *rxbuf)
	{
	char c;
	unsigned char sum;
	unsigned char rx_sum;
	char *buf;


 get_msg:

	/* wait around for start character, ignore all others */
	while (gdb_getc() != '$');

	/* start counting bytes */
	buf = rxbuf;
	sum = 0;

	/* read until we see the '#' at the end of the packet */
	do
		{
		*buf++ = c = gdb_getc();
		if (c != '#') sum += c;

		/* since the buffer is ascii, may as well terminate it */
		*buf = 0;

		}
	while (c != '#');

	/* receive checksum */
	rx_sum = hex_to_int(gdb_getc());
	rx_sum = (rx_sum << 4) + hex_to_int(gdb_getc());

	/* if computed checksum doesn't match received checksum, then reject */
	if (sum != rx_sum)
		{
		gdb_putc('-');
		goto get_msg;
		}

	/* got the message ok */
	else gdb_putc('+');

	return 1;
	}


/*
  "last signal" message
  "Sxx", where:
  xx is the signal number
*/
static void last_signal (int sigval)
	{
	char tx_buf[2];

	tx_buf[0] = lnibble_to_hex(sigval >> 4);
	tx_buf[1] = lnibble_to_hex(sigval);
	putmsg('S', tx_buf, 2);
	return;
	}


/*
  "expedited response" message
  "Txx..........."
*/
static void expedited (int sigval)
	{
	long val;
	int id = 0;
	int reglen;
	int sum;


	do
		{
		/* send header */
		gdb_putc('$');
		sum = gdb_putc('T');

		/* signal number */
		sum += gdb_putc(lnibble_to_hex(sigval >> 4));
		sum += gdb_putc(lnibble_to_hex(sigval));

		/* register values */
		id = 0;
		while ((reglen = gdb_peek_register_file(id, &val)) != 0)
			{
			/* register id */
			sum += gdb_putc(lnibble_to_hex(id >> 4));
			sum += gdb_putc(lnibble_to_hex(id));
			sum += gdb_putc(':');

			/* register value */
			switch(reglen)
				{
				case 4:
				  sum += gdb_putc(lnibble_to_hex(val >> 28));
				  sum += gdb_putc(lnibble_to_hex(val >> 24));
				case 3:
				  sum += gdb_putc(lnibble_to_hex(val >> 20));
				  sum += gdb_putc(lnibble_to_hex(val >> 16));
				case 2:
				  sum += gdb_putc(lnibble_to_hex(val >> 12));
				  sum += gdb_putc(lnibble_to_hex(val >> 8));
				case 1:
				  sum += gdb_putc(lnibble_to_hex(val >> 4));
				  sum += gdb_putc(lnibble_to_hex(val));
				  break;
				}

			sum += gdb_putc(';');

			/* try the next register */
			id++;
			}

		/* send the message footer */
		gdb_putc('#');
		gdb_putc(lnibble_to_hex(sum >> 4));
		gdb_putc(lnibble_to_hex(sum));
		}
	while ('+' != gdb_getc());

	return;
	}


static void read_memory (const char *hargs)
	{
	char tx_buf[sizeof(long) * 2];
	long addr = 0, orig_addr = 0;
	long len = 0, orig_len = 0;
	int tx;
	unsigned char sum = 0;

	/* parse address, length */
	hargs = hargs_parse_long(hargs, &addr, ',');
	hargs = hargs_parse_long(hargs, &len, '#');

	orig_addr = addr;
	orig_len = len;

	do
		{
		addr = orig_addr;
		len = orig_len;

		gdb_putc('$');
    
		/* send the message a piece at a time, so we don't need much memory */
		while (len)
			{
			tx = mem_to_hexbuf((void*)addr, tx_buf, min(len, sizeof(long)));
			sum += putbuf(tx, tx_buf);
			addr += tx / 2;
			len -= min(tx / 2, len);
			}
    
		gdb_putc('#');
		gdb_putc(lnibble_to_hex(sum >> 4));
		gdb_putc(lnibble_to_hex(sum));
		} while (gdb_getc() != '+');

	return;
	}


static void write_memory (const char *hargs)
	{
	long addr = 0;
	long len = 0;

	/* parse address, length */
	hargs = hargs_parse_long(hargs, &addr, ',');
	hargs = hargs_parse_long(hargs, &len, ':' );

	/* write all requested bytes */
	hexbuf_to_mem(hargs, (void*)addr, len);

	putmsg(0, "OK", 2);
	return;
	}

static void write_xbin_memory (const char *hargs)
	{
	long addr = 0;
	long len = 0;

	/* parse address, length */
	hargs = hargs_parse_long(hargs, &addr, ',');
	hargs = hargs_parse_long(hargs, &len, ':' );

	/* write all requested bytes */
	xmem_to_mem(hargs, (void*)addr, len);

	putmsg(0, "OK", 2);
	return;
	}


static void write_registers (char *hargs)
	{
	int id = 0;
	long val;
	int reglen;

	while (*hargs != '#')
		{
		/* how big is this register? */
		reglen = gdb_peek_register_file(id, &val);

		if(reglen)
			{
			/* extract the register's value */
			hexbuf_to_mem(hargs, &val, reglen);
			hargs += sizeof(long) * 2;
			
			/* stuff it into the register file */
			gdb_poke_register_file(id++, val);
			}

		else break;
		}

	putmsg(0, "OK", 2);

	return;
	}


static void read_registers (void)
	{
	char tx_buf[sizeof(long) * 2];
	long val;
	int id = 0;
	int reglen;
	unsigned char sum;


	do
		{
		gdb_putc('$');
		sum = 0;
		
		/* send register values */
		id = 0;
		while((reglen = gdb_peek_register_file(id++, &val)) != 0)
			sum += putbuf(mem_to_hexbuf(&val, tx_buf, reglen), tx_buf);
		
		/* send the message footer */
		gdb_putc('#');
		gdb_putc(lnibble_to_hex(sum >> 4));
		gdb_putc(lnibble_to_hex(sum));
		}
	while ('+' != gdb_getc());

	return;
	}


static void write_register (char *hargs)
	{
	long id = 0;
	long val = 0;
	int reglen;

	
	while (*hargs != '=') id = (id << 4) + hex_to_int(*hargs++)
							;

	hargs++;

	reglen = gdb_peek_register_file(id, &val);
	hexbuf_to_mem(hargs, &val, reglen);
	gdb_poke_register_file(id, val);
	putmsg(0, "OK", 2);

	return;
	}


void gdb_console_output (int len, const char *buf)
	{
	char tx_buf[2];
	unsigned char sum;


	gdb_putc('$');
	sum = putbuf(1, "O");

	while (len--)
		{
		tx_buf[0] = lnibble_to_hex(*buf >> 4);
		tx_buf[1] = lnibble_to_hex(*buf++);
		sum += putbuf(2, tx_buf);
		}

	/* send the message footer */
	gdb_putc('#');
	gdb_putc(lnibble_to_hex(sum >> 4));
	gdb_putc(lnibble_to_hex(sum));

	/* DON'T wait for response; we don't want to get hung
	   up here and halt the application if gdb has gone away! */

	return;
	}

static char * syscall_name[] = { "Fopen,", "Fclose,", "Fread,", "Fwrite,", "Flseek," };
static int syscall_namelen[] = {        6,         7,        6,         7,         7 };

static int strl(char *buf)
	{
	int i;

	for (i=0; buf[i]; i++) {}
	return i;
	}

int gdb_file_io (int syscall, int arg1, int arg2, int arg3)
	{
	char tx_buf[sizeof(long)*2];
	int len;
	unsigned char sum;

	if (!syscall || syscall > __NR_syscalls) return -1;

	gdb_putc('$');
	sum =  putbuf(syscall_namelen[syscall-1], syscall_name[syscall-1]);

	sum += putbuf(mem_to_hexbuf(&arg1, tx_buf, sizeof(int)), tx_buf);
	/* Who thought this was a good idea?  If it's open, we need a file name length */
	if (syscall == __NR_open) {
		sum += putbuf(1, "/");
		len = strl((char *)arg1) + 1;
		sum += putbuf(mem_to_hexbuf(&len, tx_buf, sizeof(int)), tx_buf);
	}
	if (syscall != __NR_close) { /* close has only 1 arg */
		sum += putbuf(1, ",");
		sum += putbuf(mem_to_hexbuf(&arg2, tx_buf, sizeof(int)), tx_buf);
		sum += putbuf(1, ",");
		sum += putbuf(mem_to_hexbuf(&arg3, tx_buf, sizeof(int)), tx_buf);
	}
	/* send the message footer */
	gdb_putc('#');
	gdb_putc(lnibble_to_hex(sum >> 4));
	gdb_putc(lnibble_to_hex(sum));

	return gdb_monitor(0); /* handle requests until the system call returns */
	}

static int _strcmp(const char *a, const char *b) {
  /* walk through strings while they're equal and haven't hit
     terminating 0 */
  while ((*a && *b) && (*a == *b)) {
    a++;
    b++;
  }
  return *a - *b;
}

/*
  The gdb command processor.
*/
int gdb_monitor (int sigval)
	{
	char rxbuf[GDB_RXBUFLEN];
	char *hargs;
	long addr;

	gdb_monitor_onentry();

	while (1)
		{
		getmsg(rxbuf);
		hargs = rxbuf;
		switch (*hargs++)
			{
			case '?':
			  last_signal(sigval);
			  break;

			case 'c':
			  /* this call probably doesn't return */
			  hargs_parse_long(hargs, &addr, '#');
			  gdb_continue(addr);
			  /* if it does, exit back to interrupted code */
			  return 0;

			case 'D':
			  /* detach from target, gdb is going away */
			  putmsg(0, "OK", 2);
			  gdb_detach();
			  break;

			case 'F':
			  /* File syscall result.  Return value syscall
			   * FIXME: this doesn't completely parse the reply */
			  if (*hargs=='-') return -1;
			  hargs_parse_long(hargs, &addr, '#');
			  putmsg(0, "OK", 2);
			  return addr;

			case 'g': read_registers(); break;
			case 'G': write_registers(hargs); break;

			case 'H':
			  /* set thread--- unimplemented, but gdb likes it */
			  putmsg(0, "OK", 2);
			  break;

			case 'k':
			  /* kill program */
			  putmsg(0, "OK", 2);
			  gdb_kill();
			  break;

			case 'm': read_memory(hargs); break;
			case 'M': write_memory(hargs); break;
			case 'P': write_register(hargs); break;

			case 'q':
			  /* query */
			  /* TODO: finish query command in gdb_handle_exception. */
                          if (_strcmp(hargs, "Offsets") == 0) {
                            /* for now, only respond to "Offsets" query */
                            putmsg(0, "Text=0;Data=0;Bss=0", 19);
                          } else {
                            putmsg(0, "", 0);
                          }
			  break;

			case 's':
			  /* step (address optional) */
			  hargs_parse_long(hargs, &addr, '#');
			  gdb_step(addr);
			  /* exit back to interrupted code */
			  return 0;

			case 'X':
			  /* write to memory (source in escaped-binary format) */
			  write_xbin_memory(hargs);
			  break;

			default :
			  /* received a command we don't recognize---
				 send empty response per gdb spec */
			  putmsg(0, "", 0);
			}
		}

	return 0;
	}


void gdb_handle_exception (int sigval)
	{
#if 1
	/* for some reason, this seems to confuse gdb-5.0 */

	/* tell the host why we're here */
	expedited(sigval);
#else
	last_signal(sigval);
#endif

	/* ask gdb what to do next */
	gdb_monitor(sigval);

	/* return to the interrupted code */
	gdb_return_from_exception();

	return;
	}
