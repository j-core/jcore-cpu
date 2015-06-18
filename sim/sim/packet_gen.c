#include <stdio.h>
#include <stdlib.h>

#define IDLE_CNT 5

#define K_28_1 0x3c
#define K_28_5 0xbc
#define K_28_7 0xfc
#define D_10_2 0x4a

/* Packet Type field definitions */
#define PT_INIT ( 1 << 7 )
#define PT_RPLY ( 0 << 7 )
#define PT_DATA ( 1 << 6 )
#define PT_CTRL ( 0 << 6 )
#define PT_EVNT ( 1 << 5 )
#define PT_ISOC ( 0 << 5 )

#define PT_INTR ( 0 << 4 )
#define PT_IOOP ( 1 << 4 )

#define PT_EXPT ( 1 << 3 )

#define PT_STAT ( 0 << 2 )
#define PT_TRIG ( 1 << 2 )

/* Packet field masks */
#define PT_MASK_ISO_T ( 0x1F )
#define PT_MASK_DAT_T ( 0x1F )
#define PT_MASK_EVT_T ( 0x03 )
#define PT_MASK_CAP_T ( 0x03 )
#define PT_MASK_PRI_T ( 0x07 )
#define PT_MASK_IOF_T ( 0x0F )

/* Canned Frame Types */
#define PT_TIME_F (PT_CTRL | PT_ISOC | 0x01)

static unsigned int parity = 0;
static unsigned int tm = 1;

unsigned int
dump_word(int k, unsigned long v)
{
   int b;

   if (k < 0) v = rand();

   /* 8 bits data, K, Valid, disp=0, Error */
   printf("   \"");
   for (b=7; b>=0; b--) {
      printf("%d", v & (1<<b) ? 1 : 0);
   }
   printf("%d%d0%d", k ? 1 : 0, k>=0 ? 1 : 0, k<-1 ? 1 : 0);
   printf("\" after %d ns,\n", (tm++) * 80);

   if (k) parity = 0;
   else parity ^= v;

   return parity & 0xff;
}

void
dump_packet(int type, unsigned int seq, unsigned int off, unsigned char * data, int len)
{
   int w;
   unsigned int p;

   dump_word(1, K_28_5);
   dump_word(0, type);
   dump_word(0, seq);
   dump_word(0, off>>16);
   dump_word(0, off>> 8);
   dump_word(0, off>> 0);

   for (w=0; w<len; w++) {
      p = dump_word(0, data[w]);
   }

   dump_word(0, p);
   dump_word(1, K_28_1);
}

void
dump_idle(int cd, int num)
{
   int i;

   for (i=0; i<num; i++) dump_word(cd, cd ? K_28_1 : D_10_2);
}

void
dump_header()
{
   printf("library ieee;\n");
   printf("use ieee.std_logic_1164.all;\n");
   printf("use std.textio.all;\n");
   printf("\n");
   printf("use work.serdes_pack.all;\n");
   printf("\n");
   printf("entity deframe_tb is\n");
   printf("end deframe_tb;\n");
   printf("\n");
   printf("architecture tb of deframe_tb is\n");
   printf("\n");
   printf("   signal rom : std_logic_vector(11 downto 0);\n");
   printf("   signal dfi : frame_dec_i_t;\n");
   printf("   signal dfo : frame_dec_o_t;\n");
   printf("   signal tmi : timeoffset_i_t;\n");
   printf("   signal tmo : timeoffset_o_t;\n");
   printf("\n");
   printf("   signal clk : std_logic;\n");
   printf("   signal rst : std_logic;\n");
   printf("   signal g   : std_logic;\n");
   printf("begin\n");
   printf("\n");
   printf("   dfi.d.d    <= rom(11 downto 4);\n");
   printf("   dfi.d.k    <= rom(3);\n");
   printf("   dfi.d.v    <= rom(2) and g;\n");
   printf("   dfi.d.disp <= rom(1);\n");
   printf("   dfi.d.err  <= rom(0);\n");
   printf("\n");
   printf("   rst   <= '1', '0' after 15 ns;\n");
   printf("   clk <= '0' after 4 ns when clk = '1' else '1' after 4 ns;\n");
   printf("   g   <= '1', '0' after 8 ns when g = '1' else '1' after 72 ns;\n");
   printf("\n");
   printf("   df0 : frame_dec port map(rst => rst, clk => clk, a => dfi, y => dfo);\n");
   printf("\n");
   printf("   tmi.d   <= dfo;\n");
   printf("   tmi.phs <= (others => '0');\n");
   printf("\n");
   printf("   tm0 : timecnt port map(rst => rst, clk => clk, a => tmi, y => tmo);\n");
   printf("\n");
   printf("   rom <= \"000000000000\",\n");
}

void
dump_footer()
{
   printf("   \"000000000000\" after %d ns;\n", tm * 80);
   printf("end tb;\n");
}

int
main(int argc, char *argv[])
{
   unsigned char ts0[] = { 1,2,3,4,5,6,7,8,9,0 };
   unsigned char ts1[] = { 0,1,2,3,4,5,6,7,8,9 };
   unsigned char ts2[] = { 9,0,1,2,3,4,5,6,7,8 };

   dump_header();

   dump_idle(-2, 10);
   dump_idle(-1, 10);
   dump_idle(0, IDLE_CNT);
   dump_idle(1, IDLE_CNT);
   dump_packet(PT_TIME_F | PT_INIT, 5, 0x111111, ts0, 10);
   dump_idle(1, IDLE_CNT);
   dump_packet(PT_TIME_F | PT_INIT, 6, 0x122222, ts1, 10);
   dump_idle(1, IDLE_CNT);
   dump_packet(PT_TIME_F | PT_INIT, 7, 0x133333, ts2, 10);
   dump_idle(1, IDLE_CNT);
   dump_word(0, 5);
   dump_word(0, 2);
   dump_word(1, K_28_5);
   dump_word(0, 3);
   dump_idle(1, IDLE_CNT);
   dump_word(1, K_28_5);
   dump_word(1, K_28_5);
   dump_idle(1, IDLE_CNT);

   dump_footer();
   return 0;
}
