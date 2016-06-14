#ifndef __USART1_H
#define __USART1_H

#define   RXTIMEOUT    10                      // 接收超时时间,如10mSec内未接收任何数据,当作一个接收包

#define   usart1_tx_buf_Number  0x100          // 发送缓冲区大小(4K)
#define   usart1_rx_buf_Number  0x100          // 接接缓冲区大小
//=============================================================================
#define   BaudRate_9600         9600
#define   BaudRate_19200        19200
#define   BaudRate_38400        38400
#define   BaudRate_57600        57600
#define   BaudRate_115200       115200
#define   BaudRate_230400       230400
//===============================================================================
typedef struct
{
	uint16_t head;
	volatile uint16_t tail;
	volatile uint8_t busy;
#ifdef IAR_ARMCC
}UART_TX_T;
#else
}__attribute__ ((packed))UART_TX_T;
#endif
typedef struct
{
	volatile uint16_t head;
	uint16_t tail;
	volatile uint8_t timeout;
	volatile uint8_t error;
#ifdef IAR_ARMCC
}UART_RX_T;
#else
}__attribute__ ((packed))UART_RX_T;
#endif
typedef struct
{
	UART_TX_T tx;
	UART_RX_T rx;
#ifdef IAR_ARMCC
}UART_T;
#else
}__attribute__ ((packed))UART_T;
#endif

//=============================================================================
extern  uint8_t  usart1_tx_buf[usart1_tx_buf_Number];
extern  uint8_t  usart1_rx_buf[usart1_rx_buf_Number];
extern  UART_T  usart1;

//=============================================================================
extern void USART1_Init(void);
extern int sendchar(int ch);
extern void USART1_Irq_Function(void);
extern void USART1_Putchar(uint8_t ch);
extern void USART1_Putbuf( uint8_t *buf,uint16_t len);
extern uint8_t   USART1_Getchar(uint8_t para);
extern uint16_t  USART1_GetRxBufSize(void);
extern uint16_t  USART1_GetRxBufDat(uint8_t *buf);
extern void USART1_RxReaddy(void);

#define     USART1_Putchar(ch)           sendchar(ch)

#endif
