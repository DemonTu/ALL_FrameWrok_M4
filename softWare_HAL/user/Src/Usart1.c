#include "includes.h"


UART_T  usart1;
uint8_t  usart1_tx_buf[usart1_tx_buf_Number];
uint8_t  usart1_rx_buf[usart1_rx_buf_Number];
#define RBUF_NEXT_PT(dat, offset, size)     (((dat) + (offset)) & ((size) - 1))


UART_HandleTypeDef handleUSART1;

/*******************************************************************************
* Function Name  : USART1_Irq_Function
* Description    : 串口中断服务函数.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
extern void USART1_Irq_Function(void)
{
	HAL_UART_IRQHandler(&handleUSART1);
}

/**
  * @brief  Rx Transfer completed callbacks.
  * @param  huart: pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	static uint16_t head;

	head = RBUF_NEXT_PT(usart1.rx.head, 1, sizeof(usart1_rx_buf));
	if (head != usart1.rx.tail)
	{
		usart1.rx.head = head;
        usart1.rx.timeout = RXTIMEOUT;
		HAL_UART_Receive_IT(huart, &usart1_rx_buf[usart1.rx.head], 1);
	}
	else
	{
		usart1.rx.error = 1;  // error
	}
	
}

/**
  * @brief  Tx Transfer completed callbacks.
  * @param  huart: pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval None
  */

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if (usart1.tx.head == usart1.tx.tail)
	{
		usart1.tx.busy = 0;
        __HAL_UART_DISABLE_IT(huart,UART_IT_TXE);
	}
	else
	{
		HAL_UART_Transmit_IT(huart, &usart1_tx_buf[usart1.tx.tail], 1);
		usart1.tx.tail = RBUF_NEXT_PT(usart1.tx.tail, 1, sizeof(usart1_tx_buf));
	}
}


/*******************************************************************************
* Function Name  : USART1_Init
* Description    : 串口初始化函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
extern void USART1_Init(void)
{
	memset((uint8_t *)(&usart1),0,sizeof(usart1));

	handleUSART1.Instance = USART1;
	handleUSART1.Init.BaudRate = BaudRate_115200;
	handleUSART1.Init.WordLength = UART_WORDLENGTH_8B;
	handleUSART1.Init.StopBits = UART_STOPBITS_1;
	handleUSART1.Init.Parity = UART_PARITY_NONE;
	handleUSART1.Init.Mode = UART_MODE_TX_RX;
	handleUSART1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	handleUSART1.Init.OverSampling = UART_OVERSAMPLING_16;

	HAL_UART_Init(&handleUSART1);
	    /* Enable the UART Data Register not empty Interrupt */
    //__HAL_UART_ENABLE_IT(&handleUSART1, UART_IT_RXNE);
	HAL_UART_Receive_IT(&handleUSART1, &usart1_rx_buf[usart1.rx.head], 1);

	//HAL_UART_Transmit_IT(&handleUSART1, "9555578", 7);
}
/*******************************************************************************
* Function Name  : USART1_Tx_Start
* Description    : 串口发送启动
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void USART1_Tx_Start(void)
{
	uint8_t ch;

	if (usart1.tx.busy == 0)
	{
		if (usart1.tx.head != usart1.tx.tail)
		{
			usart1.tx.busy = 1;
			ch = usart1_tx_buf[usart1.tx.tail];
			usart1.tx.tail = RBUF_NEXT_PT(usart1.tx.tail, 1, sizeof(usart1_tx_buf));
			HAL_UART_Transmit_IT(&handleUSART1, &ch, 1);
		}
	}
}
/*******************************************************************************
* Function Name  : USART1_Putchar
* Description    : 往串口发送一个字节
* Input          : None
* Output         : 要发送的字节
* Return         : None
*******************************************************************************/
extern int sendchar(int ch)
{
	uint16_t head;

	head = RBUF_NEXT_PT(usart1.tx.head, 1, sizeof(usart1_tx_buf));
	while (head == usart1.tx.tail) //满
	{
        #ifdef INCLUDE_IWDG
        IWDG_ReloadCounter();
        #endif
	}
	usart1_tx_buf[usart1.tx.head] = ch;
	usart1.tx.head = head;
	USART1_Tx_Start();
    return ch;
}

/*******************************************************************************
* Function Name  : USART1_Putbuf
* Description    : 往串口发送一定长度的数据
* Input          : 要发送的数据缓冲 和 发送的数据长度
* Output         : 要发送的数据缓冲
* Return         : None
*******************************************************************************/
extern void USART1_Putbuf(uint8_t *buf, uint16_t len)
{
	uint16_t head, tail, bytes;

	head = usart1.tx.head;
	while (len)
	{
		tail = RBUF_NEXT_PT(usart1.tx.tail, -1, sizeof(usart1_tx_buf));
		if (head == tail)
		{
			usart1.tx.head = head;
			USART1_Tx_Start();
			do
			{
				tail = RBUF_NEXT_PT(usart1.tx.tail, -1, sizeof(usart1_tx_buf));
			}
			while (head == tail);
		}
		if (head > tail)
		{
			bytes = sizeof(usart1_tx_buf) - head;
		}
		else // if(head < tail)
		{
			bytes = tail - head;
		}
		if (bytes > len)
		{
			bytes = len;
		}
		memcpy(&usart1_tx_buf[head], buf, bytes);
		buf += bytes;
		len -= bytes;
		head = RBUF_NEXT_PT(head, bytes, sizeof(usart1_tx_buf));
	}
	usart1.tx.head = head;
	USART1_Tx_Start();
}
/*******************************************************************************
* Function Name  : USART1_Getchar
* Description    : 从串口取一个字符
* Input          : para  1 删除该字符 0 不删除
* Output         : None
* Return         : 取得的字符值
*******************************************************************************/
extern uint8_t USART1_Getchar(uint8_t para)
{
	uint8_t c;

	c = usart1_rx_buf[usart1.rx.tail];
    if(para)
    {
	    usart1.rx.tail = RBUF_NEXT_PT(usart1.rx.tail, 1, sizeof(usart1_rx_buf));
    }
    //usart1.rx.timeout = RXTIMEOUT;	// 10ms
	return c;
}
/*******************************************************************************
* Function Name  : USART1_GetRxBufSize
* Description    : 判断串口缓冲中有多少数据
* Input          : None
* Output         : None
* Return         : 缓冲中的数据长度
*******************************************************************************/
extern uint16_t USART1_GetRxBufSize(void)
{
	uint16_t head;

	head = usart1.rx.head;
	if (head < usart1.rx.tail)
	{
		return (sizeof(usart1_rx_buf) - (usart1.rx.tail - head));
	}
	else
	{
		return (head - usart1.rx.tail);
	}
}
/*******************************************************************************
* Function Name  : USART1_GetRxBufDat
* Description    : 读取串口缓冲中的数据
* Input          : None
* Output         : 读到数据
* Return         : 读到的数据长度
*******************************************************************************/
extern uint16_t USART1_GetRxBufDat(uint8_t *buf)
{
	uint16_t head;
    uint16_t len;

	head = usart1.rx.head;
	if (head < usart1.rx.tail)
	{
        uint16_t len1,len2;
        len = (sizeof(usart1_rx_buf) - (usart1.rx.tail - head));
        len1 = sizeof(usart1_rx_buf) - usart1.rx.tail;
        len2 = len -len1;
        memcpy(buf,&usart1_rx_buf[usart1.rx.tail],len1);
        memcpy(&buf[len1],&usart1_rx_buf[0],len2);
        usart1.rx.tail = usart1.rx.head = 0;

        return len;
    }
	else
	{
	 	len = (head - usart1.rx.tail);
        memcpy(buf,&usart1_rx_buf[usart1.rx.tail],len);
        usart1.rx.tail = usart1.rx.head = 0;

        return len;
    }
}
/*******************************************************************************
* Function Name  : USART1_RxReaddy
* Description    : 收完一串数据后通知CPU
* Input          : None
* Output         : 收完事件
* Return         : None
*******************************************************************************/
extern void USART1_RxReaddy(void)
{
    if((usart1.rx.timeout)&&(--usart1.rx.timeout == 0))
    {
        MainQEvtPost(evtRxReaddy);
    }
}


