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
	u16 head;
	volatile u16 tail;
	volatile u8 busy;
#ifdef IAR_ARMCC
}UART_TX_T;
#else
}__attribute__ ((packed))UART_TX_T;
#endif
typedef struct
{
	volatile u16 head;
	u16 tail;
	volatile u8 timeout;
	volatile u8 error;
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
extern  u8  usart1_tx_buf[usart1_tx_buf_Number];
extern  u8  usart1_rx_buf[usart1_rx_buf_Number];
extern  UART_T  usart1;

//=============================================================================
extern void USART1_Init(void);
extern int sendchar(int ch);
extern void USART1_Irq_Function(void);
extern void USART1_Putchar(u8 ch);
extern void USART1_Putbuf( u8 *buf,u16 len);
extern u8   USART1_Getchar(u8 para);
extern u16  USART1_GetRxBufSize(void);
extern u16  USART1_GetRxBufDat(u8 *buf);
extern void USART1_RxReaddy(void);

#define     USART1_Putchar(ch)           sendchar(ch)

#endif
