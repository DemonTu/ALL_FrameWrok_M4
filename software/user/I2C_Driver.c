/*
********************************************************************************
*
*                                 I2C_Driver.c
*
* File          : I2C_Driver.c
* Version       : V1.0
* Author        : Demon
* Mode          : Thumb2
* Toolchain     : 
* Description   : STM32F4xx I2C驱动程序
*                   
* History       :
* Date          : 2016.04.19
*******************************************************************************/

#include "includes.h"

static void _I2cTxIRQ(uint8_t I2C_x, I2C_PARAM_TYPE *pParam);
static void _I2cRxIRQ(uint8_t I2C_x, I2C_PARAM_TYPE *pParam);


static I2C_PARAM_TYPE I2C_PARAM[I2Cn] = {0};

static I2C_TypeDef* const I2C_NUM[I2Cn] = {
#if I2C_1_EN
    BSP_I2C1,
#endif
#if I2C_2_EN
    BSP_I2C2,
#endif
#if I2C_3_EN
    BSP_I2C3,
#endif
 }; 
static const uint32_t I2C_CLK[I2Cn] = {
#if I2C_1_EN
    BSP_I2C1_CLK,
#endif
#if I2C_2_EN
    BSP_I2C2_CLK,
#endif
#if I2C_3_EN
    BSP_I2C3_CLK,
#endif
};

static const uint32_t I2C_AF_PORT[I2Cn] = {
#if I2C_1_EN
    BSP_I2C1_AF_Port,
#endif
#if I2C_2_EN
    BSP_I2C2_AF_Port,
#endif
#if I2C_3_EN
    BSP_I2C3_AF_Port,
#endif
};
static const uint8_t I2C_SCL_AF_Source[I2Cn] = {
#if I2C_1_EN
    BSP_I2C1_SCL_AF_Source,
#endif
#if I2C_2_EN
    BSP_I2C2_SCL_AF_Source,
#endif
#if I2C_3_EN
    BSP_I2C3_SCL_AF_Source,
#endif
};
static const uint8_t I2C_SDA_AF_Source[I2Cn] = {
#if I2C_1_EN
    BSP_I2C1_SDA_AF_Source,
#endif
#if I2C_2_EN
    BSP_I2C2_SDA_AF_Source,
#endif
#if I2C_3_EN
    BSP_I2C3_SDA_AF_Source,
#endif
};

static GPIO_TypeDef* const I2C_SCL_PORT[I2Cn]  = {
#if I2C_1_EN
    BSP_I2C1_SCL_GPIO_PORT,
#endif
#if I2C_2_EN
    BSP_I2C2_SCL_GPIO_PORT,
#endif
#if I2C_3_EN
    BSP_I2C3_SCL_GPIO_PORT,
#endif
};
static const uint32_t I2C_SCL_CLK[I2Cn] = {
#if I2C_1_EN
    BSP_I2C1_SCL_GPIO_CLK,
#endif
#if I2C_2_EN
    BSP_I2C2_SCL_GPIO_CLK,
#endif
#if I2C_3_EN
    BSP_I2C3_SCL_GPIO_CLK,
#endif
};
static const uint16_t I2C_SCL_PIN[I2Cn] = {
#if I2C_1_EN
    BSP_I2C1_SCL_PIN,
#endif
#if I2C_2_EN
    BSP_I2C2_SCL_PIN,
#endif
#if I2C_3_EN
    BSP_I2C3_SCL_PIN,
#endif
};

static GPIO_TypeDef* const I2C_SDA_PORT[I2Cn]  = {
#if I2C_1_EN
    BSP_I2C1_SDA_GPIO_PORT,
#endif
#if I2C_2_EN
    BSP_I2C2_SDA_GPIO_PORT,
#endif
#if I2C_3_EN
    BSP_I2C3_SDA_GPIO_PORT,
#endif
};
static const uint32_t I2C_SDA_CLK[I2Cn] = {
#if I2C_1_EN
    BSP_I2C1_SDA_GPIO_CLK,
#endif
#if I2C_2_EN
    BSP_I2C2_SDA_GPIO_CLK,
#endif
#if I2C_3_EN
    BSP_I2C3_SDA_GPIO_CLK,
#endif
};
static const uint16_t I2C_SDA_PIN[I2Cn] = {
#if I2C_1_EN
    BSP_I2C1_SDA_PIN,
#endif
#if I2C_2_EN
    BSP_I2C2_SDA_PIN,
#endif
#if I2C_3_EN
    BSP_I2C3_SDA_PIN,
#endif
};

static const uint32_t I2C_IRQn[I2Cn] = {
#if I2C_1_EN
    BSP_I2C1_IRQn,
#endif
#if I2C_2_EN
    BSP_I2C2_IRQn,
#endif
#if I2C_3_EN
    BSP_I2C3_IRQn,
#endif
};


/*******************************************************************************
* Function Name : void BSP_I2cOpen(uint8_t I2C_x, uint32_t clockSpeed, uint16_t mode)
* Description   : 打开I2C口  
* Input         :   I2C_x:      I2C_1, I2C_2
                    clockSpeed: 时钟线频率
                    mode      : I2C工作模式 I2C_Mode_I2C、I2C_Mode_SMBusDevice、I2C_Mode_SMBusHost          
* Output        :  
* Other         :   
*******************************************************************************/
void BSP_I2cOpen(uint8_t I2C_x, uint32_t clockSpeed, uint16_t mode)
{
    I2C_InitTypeDef  I2C_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Enable peripheral clocks ----------------------------------------------*/
    /* Enable I2C clock */
    RCC_APB1PeriphClockCmd(I2C_CLK[I2C_x], ENABLE);
    /* Enable GPIOB clock */
    RCC_AHB1PeriphClockCmd(I2C_SCL_CLK[I2C_x] | I2C_SDA_CLK[I2C_x], ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    I2C_Cmd(I2C_NUM[I2C_x], DISABLE);
    I2C_DeInit(I2C_NUM[I2C_x]);   

    /* Connect I2C_SCL*/    
    GPIO_PinAFConfig(I2C_SCL_PORT[I2C_x], I2C_SCL_AF_Source[I2C_x], I2C_AF_PORT[I2C_x]);
    /* Connect I2C_SDA*/
    GPIO_PinAFConfig(I2C_SDA_PORT[I2C_x], I2C_SDA_AF_Source[I2C_x], I2C_AF_PORT[I2C_x]);

    /* Configure I2C pins: SCL and SDA ---------------------------------------*/
    GPIO_InitStructure.GPIO_Pin = I2C_SCL_PIN[I2C_x];
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_Init(I2C_SCL_PORT[I2C_x], &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = I2C_SDA_PIN[I2C_x];
    GPIO_Init(I2C_SDA_PORT[I2C_x], &GPIO_InitStructure);

    /* DISABLE I2C event and buffer interrupt */
    I2C_ITConfig(I2C_NUM[I2C_x], I2C_IT_EVT | I2C_IT_BUF, DISABLE);

    /* I2C configuration -----------------------------------------------------*/
    I2C_InitStructure.I2C_Mode = mode;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_16_9;
    I2C_InitStructure.I2C_OwnAddress1 = I2C_SLAVE_ADDRESS;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed = clockSpeed;
    I2C_Init(I2C_NUM[I2C_x], &I2C_InitStructure);

    memset (&I2C_PARAM[I2C_x], 0, sizeof(I2C_PARAM_TYPE));

    /* Configure and enable I2C interrupt ------------------------------------*/
    NVIC_InitStructure.NVIC_IRQChannel = I2C_IRQn[I2C_x];
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);   

    /* Enable I2C ------------------------------------------------------------*/    
    I2C_Cmd(I2C_NUM[I2C_x], ENABLE);
}

/*******************************************************************************
* Function Name : void _I2CDelay(volatile uint32_t count)
* Description   : 延迟程序
* Input         : 
* Output        : 
* Other         : 
*******************************************************************************/
void _I2CDelay(volatile uint32_t count)
{
    for (; count > 0; count--);
}

/*******************************************************************************
* Function Name : void BSP_I2cClose(uint8_t I2C_x)
* Description   : 关闭I2C口 并释放总线
* Input         : 
* Output        : 
* Other         : 
*******************************************************************************/
void BSP_I2cClose(uint8_t I2C_x)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    uint16_t i = 0;

    I2C_ITConfig(I2C_NUM[I2C_x], I2C_IT_EVT | I2C_IT_BUF, DISABLE);
    RCC_APB1PeriphClockCmd(I2C_CLK[I2C_x], DISABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, DISABLE);

    I2C_Cmd(I2C_NUM[I2C_x], DISABLE);
    I2C_DeInit(I2C_NUM[I2C_x]);

    /* Configure I2C pins: SCL and SDA ---------------------------------------*/
    GPIO_InitStructure.GPIO_Pin = I2C_SCL_PIN[I2C_x];
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_Init(I2C_SCL_PORT[I2C_x], &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = I2C_SDA_PIN[I2C_x];
    GPIO_Init(I2C_SDA_PORT[I2C_x], &GPIO_InitStructure);

    _I2CDelay(100); 
    for (i = 16; i > 0; i--) //16个时钟 脉冲 释放I2C总线
    {
        GPIO_ResetBits(I2C_SCL_PORT[I2C_x], I2C_SCL_PIN[I2C_x]);
        _I2CDelay(100);  
        GPIO_SetBits(I2C_SCL_PORT[I2C_x], I2C_SCL_PIN[I2C_x]);
        _I2CDelay(100);        
    }

    GPIO_InitStructure.GPIO_Pin = I2C_SCL_PIN[I2C_x];
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_Init(I2C_SCL_PORT[I2C_x], &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = I2C_SDA_PIN[I2C_x];
    GPIO_Init(I2C_SDA_PORT[I2C_x], &GPIO_InitStructure);

    memset (&I2C_PARAM[I2C_x], 0, sizeof(I2C_PARAM_TYPE));
}

/*******************************************************************************
* Function Name : uint32_t BSP_I2cWrite(uint8_t I2C_x, uint8_t *buff, uint16_t i2cSaleAddress, uint8_t writeAddress, uint16_t writeLen)
* Description   : I2C向从机发送数据
* Input         :   I2C_x:          I2C_1,  I2C_2
                    buff:           要发送的数据
                    i2cSaleAddress: 从机ID号
                    writeAddress:   写入的地址
                    writeLen:       要写入的数据长度
* Output        : 
* Other         : 本函数为非阻塞式 执行完后调用BSP_I2cIdleState 是否执行完毕
*******************************************************************************/
uint32_t BSP_I2cWrite(uint8_t I2C_x, uint8_t *buff, uint16_t i2cSaleAddress, uint8_t writeAddress, uint16_t writeLen)
{
    if (I2C_x >= I2C_MAX)
        return 0;

    if (NULL == buff)
        return 0;

    if (0 == writeLen)
        return 0;

    if (0 != I2C_PARAM[I2C_x].idle)
        return 0;

    I2C_PARAM[I2C_x].idle       = 1;
    I2C_PARAM[I2C_x].id         = i2cSaleAddress;
    I2C_PARAM[I2C_x].addr       = writeAddress;
    I2C_PARAM[I2C_x].index      = 0;
    I2C_PARAM[I2C_x].r_w        = 0;
    I2C_PARAM[I2C_x].bufLen     = writeLen;
    I2C_PARAM[I2C_x].pBuff      = buff;
    I2C_PARAM[I2C_x].FunCallBack = (void (*)(uint8_t, void *))_I2cTxIRQ;

    I2C_ITConfig(I2C_NUM[I2C_x], I2C_IT_EVT | I2C_IT_BUF, ENABLE);
    I2C_AcknowledgeConfig(I2C_NUM[I2C_x], ENABLE);
    /* Send I2C START condition */
    I2C_GenerateSTART(I2C_NUM[I2C_x], ENABLE);
    return writeLen;
}

/*******************************************************************************
* Function Name : uint32_t BSP_I2cRead(uint8_t I2C_x, uint8_t *buff, uint16_t i2cSaleAddress, uint8_t readAddress, uint16_t readLen)
* Description   : I2C 读取数据
* Input         :   I2C_x:          I2C_1,  I2C_2
                    buff:           读数缓冲区
                    i2cSaleAddress: 从机ID号
                    readAddress:    读取的地址
                    readLen:        要读取的数据长度
* Output        : 
* Other         : 本函数为非阻塞式 执行完后调用BSP_I2cIdleState 是否执行完毕
*******************************************************************************/
uint32_t BSP_I2cRead(uint8_t I2C_x, uint8_t *buff, uint16_t i2cSaleAddress, uint8_t readAddress, uint16_t readLen)
{
    if (I2C_x >= I2C_MAX)
        return 0;

    if (NULL == buff)
        return 0;

    if (0 == readLen)
        return 0;

    if (0 != I2C_PARAM[I2C_x].idle)
        return 0;
        
    I2C_PARAM[I2C_x].idle       = 1;
    I2C_PARAM[I2C_x].id         = i2cSaleAddress;
    I2C_PARAM[I2C_x].addr       = readAddress;
    I2C_PARAM[I2C_x].index      = 0;
    I2C_PARAM[I2C_x].r_w        = 1;
    I2C_PARAM[I2C_x].bufLen     = readLen;
    I2C_PARAM[I2C_x].pBuff      = buff;
    I2C_PARAM[I2C_x].FunCallBack = (void (*)(uint8_t, void *))_I2cTxIRQ;

    I2C_ITConfig(I2C_NUM[I2C_x], I2C_IT_EVT | I2C_IT_BUF, ENABLE);
    I2C_AcknowledgeConfig(I2C_NUM[I2C_x], ENABLE);
    /* Send I2C START condition */
    I2C_GenerateSTART(I2C_NUM[I2C_x], ENABLE);
    return readLen;
}

/*******************************************************************************
* Function Name : uint32_t BSP_I2cIdleState(uint8_t I2C_x)
* Description   : 查询是否总线空闲 如果为空闲则读取参数
* Input         :   I2C_x:      I2C_1,      I2C_2
* Output        :   return:     0)空闲      1)忙碌
* Other         : 
*******************************************************************************/
uint32_t BSP_I2cIdleState(uint8_t I2C_x)
{
    return (I2C_PARAM[I2C_x].idle || I2C_GetFlagStatus(I2C_NUM[I2C_x], I2C_FLAG_BUSY));
}

/*******************************************************************************
* Function Name : static void _I2cTxIRQ(uint8_t I2C_x, I2C_PARAM_TYPE *pParam)
* Description   : 发送数据中断函数
* Input         : 
* Output        : 
*******************************************************************************/
static void _I2cTxIRQ(uint8_t I2C_x, I2C_PARAM_TYPE *pParam)
{ 
    switch (I2C_GetLastEvent(I2C_NUM[I2C_x]))
    {
		/* Test on I2Cx EV5 and clear it */
		case I2C_EVENT_MASTER_MODE_SELECT:
			//printf("start\r\n");
		    I2C_Send7bitAddress(I2C_NUM[I2C_x], pParam->id, I2C_Direction_Transmitter);
		    break;

		/* Test on I2Cx EV6 and first EV8 and clear them */
		case I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED:
		    /* Send the first data */
			//printf("addr\r\n");
		    I2C_SendData(I2C_NUM[I2C_x], pParam->addr);  /* EV8 just after EV6 */
		    break;

		case I2C_EVENT_MASTER_BYTE_TRANSMITTING:
			
		    if((pParam->index < pParam->bufLen) && (pParam->r_w == 0))
		    {
		        /* Transmit buffer data */
		        I2C_SendData(I2C_NUM[I2C_x], pParam->pBuff[pParam->index++]);
				//printf("data\r\n");
		    }
		    else
		    {    
		        I2C_ITConfig(I2C_NUM[I2C_x], I2C_IT_BUF, DISABLE);
				//printf("data1\r\n");
		    }
		    break;

		/* Test on I2Cx EV8 and clear it */
		case I2C_EVENT_MASTER_BYTE_TRANSMITTED:     
			
		    if (pParam->r_w != 0)
		    {
		        pParam->FunCallBack = (void (*)(uint8_t, void *))_I2cRxIRQ;            
		        I2C_ITConfig(I2C_NUM[I2C_x], I2C_IT_BUF, ENABLE);
		        I2C_GenerateSTART(I2C_NUM[I2C_x], ENABLE);
				//printf("end\r\n");
		    }
		    else
		    {
		        I2C_ITConfig(I2C_NUM[I2C_x], I2C_IT_EVT | I2C_IT_BUF, DISABLE);
		        I2C_AcknowledgeConfig(I2C_NUM[I2C_x], DISABLE);
		        I2C_GenerateSTOP(I2C_NUM[I2C_x], ENABLE);            
		        pParam->idle = 0;            //接收结束标志
		        
		    }
		    break;
		default:
			//printf("test it\r\n");
			break;
    }
}

/*******************************************************************************
* Function Name : static void _I2cRxIRQ(uint8_t I2C_x, I2C_PARAM_TYPE *pParam)
* Description   : 接收数据中断函数
* Input         : 
* Output        : 
* Other         : 
*******************************************************************************/
static void _I2cRxIRQ(uint8_t I2C_x, I2C_PARAM_TYPE *pParam)
{
    switch (I2C_GetLastEvent(I2C_NUM[I2C_x]))
    {
    /* Test on I2Cx EV5 and clear it */
    case I2C_EVENT_MASTER_MODE_SELECT:  
		//printf("start\r\n");
        /* Send I2Cx slave Address for write */
        I2C_Send7bitAddress(I2C_NUM[I2C_x], pParam->id, I2C_Direction_Receiver);
        break;

    /* Test on I2Cx EV6 and first EV8 and clear them */
    case I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED: 		 
		if (pParam->index == (pParam->bufLen - 1))
		{
			printf("test\r\n");
		    I2C_AcknowledgeConfig(I2C_NUM[I2C_x], DISABLE);
		    I2C_GenerateSTOP(I2C_NUM[I2C_x], ENABLE);
		}
		break;

    /* Test on I2Cx EV2 and clear it */
    case I2C_EVENT_MASTER_BYTE_RECEIVED:
        pParam->pBuff[pParam->index++] = I2C_ReceiveData(I2C_NUM[I2C_x]);
        //printf("end1\r\n");    
        if (pParam->index == (pParam->bufLen - 1))
        {
            I2C_AcknowledgeConfig(I2C_NUM[I2C_x], DISABLE);
            I2C_GenerateSTOP(I2C_NUM[I2C_x], ENABLE);
        }
        else if (pParam->index >= pParam->bufLen)
        {
            pParam->FunCallBack = (void (*)(uint8_t, void *))_I2cTxIRQ;    //默认进接收中断       
            I2C_ITConfig(I2C_NUM[I2C_x], I2C_IT_EVT | I2C_IT_BUF, DISABLE);
            pParam->idle = 0;            
        }
        break; 
    }
}

#if I2C_1_EN
/*******************************************************************************
* Function Name : void I2C1_EV_IRQHandler(void)
* Description   : I2C1中断函数
* Input         : None
* Output        : None
* Other         : None
*******************************************************************************/
void I2C1_EV_IRQHandler(void)
{
    if (I2C_PARAM[I2C_1].FunCallBack)
    {
        I2C_PARAM[I2C_1].FunCallBack(I2C_1, &I2C_PARAM[I2C_1]);
    }
}
#endif

#if I2C_2_EN
/*******************************************************************************
* Function Name : void I2C2_EV_IRQHandler(void)
* Description   : I2C2中断函数
* Input         : 
* Output        : 
* Other         : 
*******************************************************************************/
void I2C2_EV_IRQHandler(void)
{
    if (I2C_PARAM[I2C_2].FunCallBack)
    {
        I2C_PARAM[I2C_2].FunCallBack(I2C_2, &I2C_PARAM[I2C_2]);
    }
}
#endif

#if I2C_3_EN
/*******************************************************************************
* Function Name : void I2C3_EV_IRQHandler(void)
* Description   : I2C3中断函数
* Input         : 
* Output        : 
* Other         : 
*******************************************************************************/
void I2C3_EV_IRQHandler(void)
{
    if (I2C_PARAM[I2C_3].FunCallBack)
    {
        I2C_PARAM[I2C_3].FunCallBack(I2C_3, &I2C_PARAM[I2C_3]);
    }
}
#endif

