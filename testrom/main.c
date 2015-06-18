#define LEDPORT (*(volatile unsigned long  *)0xabcd0000)

extern char version_string[];

char ram0[256]; /* working ram for CPU tests */

void
putstr (char *str)
{
  while (*str)
    {
      if (*str == '\n')
	uart_tx ('\r');
      uart_tx (*(str++));
    }
}

#define DDR_BASE 0x10000000
#define MemoryRead(A) (*(volatile int*)(A))
#define MemoryWrite(A,V) *(volatile int*)(A)=(V)

//SD_A  <= address_reg(25 downto 13);  --address row
//SD_BA <= address_reg(12 downto 11);  --bank_address
//cmd   := address_reg(6 downto 4);    --bits RAS & CAS & WE
int DdrInitData[] = {
// AddressLines    Bank        Command
#ifndef LPDDR
  (0x000 << 13) | (0 << 11) | (7 << 4),	//CKE=1; NOP="111"
  (0x400 << 13) | (0 << 11) | (2 << 4),	//A10=1; PRECHARGE ALL="010"
  (0x001 << 13) | (1 << 11) | (0 << 4),	//EMR disable DLL; BA="01"; LMR="000"
#ifndef DDR_BL4
  (0x121 << 13) | (0 << 11) | (0 << 4),	//SMR reset DLL, CL=2, BL=2; LMR="000"
#else
  (0x122 << 13) | (0 << 11) | (0 << 4),	//SMR reset DLL, CL=2, BL=4; LMR="000"
#endif
  (0x400 << 13) | (0 << 11) | (2 << 4),	//A10=1; PRECHARGE ALL="010" 
  (0x000 << 13) | (0 << 11) | (1 << 4),	//AUTO REFRESH="001"
  (0x000 << 13) | (0 << 11) | (1 << 4),	//AUTO REFRESH="001
#ifndef DDR_BL4
  (0x021 << 13) | (0 << 11) | (0 << 4)	//clear DLL, CL=2, BL=2; LMR="000"
#else
  (0x022 << 13) | (0 << 11) | (0 << 4)	//clear DLL, CL=2, BL=4; LMR="000"
#endif
#else	// LPDDR
  (0x000 << 13) | (0 << 11) | (7 << 4),	//CKE=1; NOP="111"
  (0x000 << 13) | (0 << 11) | (7 << 4),	//NOP="111" after 200 uS
  (0x400 << 13) | (0 << 11) | (2 << 4),	//A10=1; PRECHARGE ALL="010"
  (0x000 << 13) | (0 << 11) | (1 << 4),	//AUTO REFRESH="001"
  (0x000 << 13) | (0 << 11) | (1 << 4),	//AUTO REFRESH="001"
  (0x021 << 13) | (0 << 11) | (0 << 4),	//SMR CL=2, BL=2; LMR="000"
  (0x000 << 13) | (1 << 11) | (0 << 4),	//EMR BA="01"; LMR="000" Full strength full array
  (0x000 << 13) | (0 << 11) | (7 << 4)	//NOP="111" after ? uS
#endif
};

int
ddr_init (void)
{
  volatile int i, j, k = 0;
  for (i = 0; i < sizeof (DdrInitData) / sizeof (int); ++i)
    {
      MemoryWrite (DDR_BASE + DdrInitData[i], 0);
      for (j = 0; j < 4; ++j)
	++k;
    }
  for (j = 0; j < 100; ++j)
    ++k;
  k += MemoryRead (DDR_BASE);	//Enable DDR
  return k;
}

void
led(int v)
{
//  LEDPORT = v;
}

void
main_sh (void)
{
  led(0x40);

  uart_set_baudrate ();
  led(0x042);

  putstr ("CPU tests passed\n");
  led(0x043);
  putstr ("DDR Init\n");
  led(0x042);
  ddr_init ();

  putstr ("GDB Stub for HS-2J0 SH2 ROM\n");
  putstr (version_string);
  led(0x50);
}
