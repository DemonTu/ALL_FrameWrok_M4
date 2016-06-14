#include "includes.h"


UART_T  usart1;
u8  usart1_tx_buf[usart1_tx_buf_Number];
u8  usart1_rx_buf[usart1_rx_buf_Number];
#define RBUF_NEXT_PT(dat, offset, size)     (((dat) + (offset)) & ((size) - 1))

/*******************************************************************************
* Function Name  : USART1_Irq_Function
* Description    : �����жϷ�����.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
extern void USART1_Irq_Function(void)
{
	static u8 temp;
	static u16 head;
//RX
	while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE))//���յ�����
	{
		USART_ClearFlag(USART1, USART_FLAG_RXNE);
		temp = USART_ReceiveData(USART1);
		head = RBUF_NEXT_PT(usart1.rx.head, 1, sizeof(usart1_rx_buf));
		if (head != usart1.rx.tail)
		{
			usart1_rx_buf[usart1.rx.head] = temp;
			usart1.rx.head = head;
            usart1.rx.timeout = RXTIMEOUT;
		}
		else
		{
			usart1.rx.error = 1;  // error
		}
	}
//TX
	if (USART_GetFlagStatus(USART1,USART_FLAG_TXE))
	{
		USART_ClearFlag(USART1, USART_FLAG_TXE);
		if (usart1.tx.head == usart1.tx.tail)
		{
			usart1.tx.busy = 0;
            USART_ITConfig(USART1,USART_IT_TXE,DISABLE);
		}
		else
		{
			USART_SendData(USART1, usart1_tx_buf[usart1.tx.tail]);
			usart1.tx.tail = RBUF_NEXT_PT(usart1.tx.tail, 1, sizeof(usart1_tx_buf));
		}
	}
#if 0
	  /* USART in Receiver mode */
  if (USART_GetITStatus(USARTx, USART_IT_RXNE) == SET)
  {
    if (ubRxIndex < BUFFERSIZE)
    {
      /* Receive Transaction data */
      aRxBuffer[ubRxIndex++] = USART_ReceiveData(USARTx);
    }
    else
    {
      /* Disable the Rx buffer not empty interrupt */
      USART_ITConfig(USARTx, USART_IT_RXNE, DISABLE);
    }
  }
  /* USART in Transmitter mode */
  if (USART_GetITStatus(USARTx, USART_IT_TXE) == SET)
  {
    if (ubTxIndex < BUFFERSIZE)
    {
      /* Send Transaction data */
      USART_SendData(USARTx, aTxBuffer[ubTxIndex++]);
    }
    else
    {
      /* Disable the Tx buffer empty interrupt */
      USART_ITConfig(USARTx, USART_IT_TXE, DISABLE);
    }
  }
 #endif 
}
/*******************************************************************************
* Function Name  : USART1_Init
* Description    : ���ڳ�ʼ������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
extern void USART1_Init(void)
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	USART_DeInit(USART1);
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
	memset((u8 *)(&usart1),0,sizeof(usart1));

	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure USART Rx as alternate function  */
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_10;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Connect PXx to USARTx_Rx*/
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);

	/* Connect PXx to USARTx_Tx*/
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);

	  /* Enable the USART OverSampling by 8 */
  	//USART_OverSampling8Cmd(USART1, ENABLE); 
	  
    USART_InitStructure.USART_BaudRate =  BaudRate_115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);

	/* Configure USART1 */
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
	/* Enable the USART1 */
	USART_Cmd(USART1, ENABLE);
}
/*******************************************************************************
* Function Name  : USART1_Tx_Start
* Description    : ���ڷ�������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void USART1_Tx_Start(void)
{
	u8 ch;

	if (usart1.tx.busy == 0)
	{
		if (usart1.tx.head != usart1.tx.tail)
		{
			usart1.tx.busy = 1;
			ch = usart1_tx_buf[usart1.tx.tail];
			usart1.tx.tail = RBUF_NEXT_PT(usart1.tx.tail, 1, sizeof(usart1_tx_buf));
			USART_SendData(USART1, ch);
            USART_ITConfig(USART1,USART_IT_TXE,ENABLE);
		}
	}
}
/*******************************************************************************
* Function Name  : USART1_Putchar
* Description    : �����ڷ���һ���ֽ�
* Input          : None
* Output         : Ҫ���͵��ֽ�
* Return         : None
*******************************************************************************/
extern int sendchar(int ch)
{
	u16 head;

	head = RBUF_NEXT_PT(usart1.tx.head, 1, sizeof(usart1_tx_buf));
	while (head == usart1.tx.tail) //��
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
* Description    : �����ڷ���һ�����ȵ�����
* Input          : Ҫ���͵����ݻ��� �� ���͵����ݳ���
* Output         : Ҫ���͵����ݻ���
* Return         : None
*******************************************************************************/
extern void USART1_Putbuf(u8 *buf, u16 len)
{
	u16 head, tail, bytes;

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
* Description    : �Ӵ���ȡһ���ַ�
* Input          : para  1 ɾ�����ַ� 0 ��ɾ��
* Output         : None
* Return         : ȡ�õ��ַ�ֵ
*******************************************************************************/
extern u8 USART1_Getchar(u8 para)
{
	u8 c;

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
* Description    : �жϴ��ڻ������ж�������
* Input          : None
* Output         : None
* Return         : �����е����ݳ���
*******************************************************************************/
extern u16 USART1_GetRxBufSize(void)
{
	u16 head;

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
* Description    : ��ȡ���ڻ����е�����
* Input          : None
* Output         : ��������
* Return         : ���������ݳ���
*******************************************************************************/
extern u16 USART1_GetRxBufDat(u8 *buf)
{
	u16 head;
    u16 len;

	head = usart1.rx.head;
	if (head < usart1.rx.tail)
	{
        u16 len1,len2;
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
* Description    : ����һ�����ݺ�֪ͨCPU
* Input          : None
* Output         : �����¼�
* Return         : None
*******************************************************************************/
extern void USART1_RxReaddy(void)
{
    if((usart1.rx.timeout)&&(--usart1.rx.timeout == 0))
    {
        MainQEvtPost(evtRxReaddy);
    }
}


