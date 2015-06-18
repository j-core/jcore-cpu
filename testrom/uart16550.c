/* Simple 16550 compatible UART routines for GDB stub */

struct st_uart16550
{
  unsigned int RTX;
  unsigned int IER;
  unsigned int IIR;
  unsigned int LCR;
  unsigned int MCR;
  unsigned int LSR;
  unsigned int MSR;
  unsigned int SCR;
};
#define UART 0xabcd0100
#define UART16550 (*(volatile struct st_uart16550 *)UART)

//****************************************************
//*                                                  *
//*                UART Utilities                    *
//*                                                  *
//****************************************************

//==============================
// Send Tx
// -----------------------------
//     Input  : data = send data
//     Output : none
void
uart_tx (unsigned char data)
{
  while (!((UART16550.LSR) & 0x20));
  UART16550.RTX = data;
}

//====================================
// Receive RX
// -----------------------------------
//     Input  : none
//     Output : uart_rx = receive data
//====================================
unsigned char
uart_rx (void)
{
  while (!((UART16550.LSR) & 0x1));
  return (UART16550.RTX & 0xff);
}

//=========================================
// Receive RX with echo to TX
// ----------------------------------------
//     Input  : none
//     Output : uart_rx_echo = receive data
//=========================================
unsigned char
uart_rx_echo (void)
{
  unsigned char data;

  while (!(UART16550.LSR & 1));
  data = UART16550.RTX & 0xff;

  while (!(UART16550.LSR & 0x20));
  UART16550.RTX = data;
  return data;
}

//==================
// Flush RXD FIFO
//------------------
//     Input  : none
//     Output : none
//==================
void
uart_rx_flush (void)
{
  while (UART16550.LSR & 1) UART16550.RTX;
}

//==============================
// Set Baud Rate 115200bps
//------------------------------
//     Input  : none
//     Output : none
//==============================
void
uart_set_baudrate (void)
{
  UART16550.LCR = 0x83;
  UART16550.RTX = 0x0a;
  UART16550.IER = 0;
  UART16550.LCR = 0x3;
}
