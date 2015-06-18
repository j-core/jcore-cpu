/* Simple Xilinx uartlite compatible UART routines for GDB stub */

struct st_uartlite
{
  unsigned int rxfifo;
  unsigned int txfifo;
  unsigned int status;
  unsigned int control;
};
#define UART 0xabcd0100
#define UARTLITE (*(volatile struct st_uartlite *)UART)

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
  while (UARTLITE.status & 0x08);
  UARTLITE.txfifo = data;
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
  while (!(UARTLITE.status & 0x01));
  return (UARTLITE.rxfifo & 0xff);
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

  uart_tx(data = uart_rx());

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
  UARTLITE.control = 0x02;
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
   /* baud is fixed in a VHDL generic */
}
