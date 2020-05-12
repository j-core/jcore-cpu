/* cpu1.c */

#define ADRS_UART_LITE      0xabcd0100
#define RTC_SEC             0xabcd0224

char buf[20];

int main ( )
{
  volatile int *ptr_1, *p_read, *ptr_sec;

  int j, data_min, read_sec, read_sec_by2, read_sec_by2last ; 
  volatile int *ptr_data;
  int cpu1_print( );
  int cpu1_restart_asm( );

  p_read = ((int *)ADRS_UART_LITE) + 2;
  ptr_sec =  ((int *)RTC_SEC);

  /* check uart ready */
  while((*p_read & 0x8)!= 0) {
    for(j = 0; j < 2; j++) { }
  }
  cpu1_print("Hello world (from cpu1)");
  cpu1_print("Hello world (from cpu1)");
  ptr_data = (int *)0x8020;
  *ptr_data = 0;

  ptr_data = (int *)0x8104;
  *ptr_data = 0;
  read_sec_by2last = 0;
  while( *ptr_data == 0) {
    read_sec_by2 = read_sec_by2last;
    while(read_sec_by2 == read_sec_by2last) {
      read_sec =  *ptr_sec;
      read_sec_by2 = read_sec >> 1;
    }
    data_min = (read_sec / 60) % 60;

    buf[0] = 'c'; buf[1] = '1'; buf[2] = ':'; buf[3] = ' ';

    buf[4] = '0' + (data_min / 10);
    buf[5] = '0' + (data_min % 10);
    buf[6] = ':';
    buf[7] = '0' + ((read_sec / 10) % 6);
    buf[8] = '0' + (read_sec % 10);
    buf[9] = 0;
    cpu1_print(buf);
    read_sec_by2last = read_sec_by2;
  }
  cpu1_print("terminate cpu1 print");
  /* finish_flg */
  ptr_1 = (int *) 0x8100; *ptr_1 = 1;

  ptr_1 = (int *) 0x8020;
  while ( *ptr_1 == 0) {
  }
  cpu1_restart_asm( );
  return(0);
}

int cpu1_print( pc )
char *pc;
{
  char *pwork;
  static volatile int *p_read;
  static volatile int *p_data;
  int j;

  pwork = pc;
  p_read = ((int *)ADRS_UART_LITE) + 2;
  p_data = ((int *)ADRS_UART_LITE);

  while(*pwork != 0) {
    while((*p_read & 0x8)!= 0) {
      for(j = 0; j < 2; j++) { }
    }
    *p_data = (int)(*pwork);
    while((*p_read & 0x8)!= 0) {
      for(j = 0; j < 2; j++) { }
    }
    pwork++;
  }
  *p_data = (int)0xa;
  while((*p_read & 0x8)!= 0) {
    for(j = 0; j < 2; j++) { }
  }
  *p_data = (int)0xd;
  while((*p_read & 0x8)!= 0) {
    for(j = 0; j < 2; j++) { }
  }
  return(0);
}

