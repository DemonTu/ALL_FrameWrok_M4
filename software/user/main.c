#include "includes.h"

//static uint32_t MsTimeTick = 0;
RCC_ClocksTypeDef RCC_Clocks;


/*******************************************************************************
* Function Name  : prvSetupHardware
* Description    : ϵͳʱ�Ӻ�IOʱ������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
uint8_t HSE_Or_HSI;                         // 1:�ⲿ����    0:�ڲ�RC��
static void prvSetupHardware( void )
{
#if 1
     ErrorStatus HSEStartUpStatus;

    /* RCC system reset(for debug purpose) */
    RCC_DeInit();                                                               // ������RCC�Ĵ�������Ϊȱʡֵ

    /* Enable HSE */
    RCC_HSEConfig(RCC_HSE_ON);                                                  // �����ⲿ���پ���HSE��

    /* Wait till HSE is ready */
    HSEStartUpStatus = RCC_WaitForHSEStartUp();                                 // �ȴ�HSE���񣬸ú������ȴ�ֱ��HSE�����������ڳ�ʱ��������˳�

    /* Enable Prefetch Buffer */
    FLASH_PrefetchBufferCmd(ENABLE);  					                   // ʹ��Ԥȡָ����   
    FLASH_InstructionCacheCmd(ENABLE);
	FLASH_DataCacheCmd(ENABLE);
	
    /* Flash 2 wait state */
    FLASH_SetLatency(FLASH_Latency_2);                                      // ���ô�����ʱֵ

    /* HCLK = SYSCLK */
    RCC_HCLKConfig(RCC_SYSCLK_Div1);                                        // ����AHBʱ�ӣ�HCLK��

    /* PCLK2 = HCLK */
    RCC_PCLK2Config(RCC_HCLK_Div1);                                         // ���ø���AHBʱ�ӣ�PCLK2��

    /* PCLK1 = HCLK/2 */
    RCC_PCLK1Config(RCC_HCLK_Div2);                                         // ���õ���AHBʱ�ӣ�PCLK1��

    /* ADCCLK = PCLK2/4 */
    //RCC_ADCCLKConfig(RCC_PCLK2_Div4);                                       // ����ADCʱ�ӣ�ADCCLK��

    if(HSEStartUpStatus == SUCCESS)
    {
        /* PLLCLK = ((12MHz/12)*336)/4 = 84 MHz */
        RCC_PLLConfig(RCC_PLLCFGR_PLLSRC_HSE, 12, 336, 4, 7);                    // ����PLLʱ��Դ����Ƶϵ��
		HSE_Or_HSI = 1;
        //MsTimeTick = 84000;
    }
    else
    {
        /* PLLCLK = ((4MHz/7)*336)/4 = 48 MHz */
        RCC_PLLConfig(RCC_PLLCFGR_PLLSRC_HSI, 7, 336, 4, 4);                // ����PLLʱ��Դ����Ƶϵ��
        HSE_Or_HSI = 0;
        //MsTimeTick = 48000;
    }

    /* Enable PLL */
    RCC_PLLCmd(ENABLE);                                                     // ʹ��PLL

    /* Wait till PLL is ready */
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)                      // ���ָ����RCC��־λ�������
    {
    }

    /* Select PLL as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);                              // ����ϵͳʱ�ӣ�SYSCLK��

    /* Wait till PLL is used as system clock source */
    while(RCC_GetSYSCLKSource() != 0x08)                                    // ��������ϵͳʱ�ӵ�ʱ��Դ
    {
    }

	/* Enable GPIOA, GPIOB, GPIOC, GPIOD, GPIOE and AFIO clocks */
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_ADC1 , ENABLE );

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC
	                        | RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE,ENABLE);

	/* Configure HCLK clock as SysTick clock source. */
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK );
#endif
	RCC_GetClocksFreq(&RCC_Clocks);
	SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000);
}


/*******************************************************************************
* Function Name  : GPIO_Configuration
* Description    : IO�ڹ�������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);	

	GPIO_ResetBits(GPIOA, GPIO_Pin_8);
}

/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : �ж�ʹ�ܺ����ȼ�����.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

	/* Set the Vector Table base location at 0x08000000 */
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0000);

    /* Configure the Priority Group to 2 bits */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    /* enabling interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/*******************************************************************************
* Function Name  : SystemInit
* Description    : ϵͳ��ʼ��.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
extern void SystemInit2(void)
{
    prvSetupHardware();
	GPIO_Configuration();
	NVIC_Configuration();
    //SysTick_Config(MsTimeTick);

    USART1_Init();
    MainQInit();
#if defined (INCLUDE_BUZZER)
    BuzzerInit();
    BuzzerBeep(4);
#endif
#ifdef INCLUDE_OLED12832
	OLED_Init();
#else
	SMB_Init();
#endif
#ifdef INCLUDE_IWDG
    IWDGInit();
#endif
}
/*******************************************************************************
* Function Name  : modeCommon
* Description    : ϵͳ����ģʽ.
* Input          : �¼�
* Output         : None
* Return         : None
*******************************************************************************/
static void modeCommon(u8 evt)
{
    switch(evt)
    {
        case evtRxReaddy:
            {
                u16 len;
                u8 rxbuf[usart1_rx_buf_Number];

                len = USART1_GetRxBufDat(rxbuf);
                if(len)
                {
                    #ifdef INCLUDE_BUZZER
                    BuzzerBeep(4);
                    #endif
                    USART1_Putbuf(rxbuf,len);
                }
            }
            break;
        case evtTime1s:
			#ifndef INCLUDE_IIC_DRIVER
			printf("%02x\r\n", SMB_Read(VOLTAGE));
			printf("%02x\r\n", SMB_Read(DESIGN_CAPACITY+1));
			printf("%02x\r\n", SMB_Read(DESIGN_CAPACITY+2));
			printf("%02x\r\n", SMB_Read(DESIGN_CAPACITY+3));
			printf("%02x\r\n", SMB_Read(TEMPERATURE));
			
			#else
			printf("0x%x\r\n", SMB_Read2Byte(DESIGN_CAPACITY+2));
			#endif
            //USART1_Putchar(0X55);
			{
				static uint8_t flashFlag;
				if (flashFlag){
					flashFlag = 0;
					GPIO_SetBits(GPIOA, GPIO_Pin_8);
				}else{
					flashFlag = 1;
					GPIO_ResetBits(GPIOA, GPIO_Pin_8);
				}
			}
            break;

            default:
                break;

    }
}
/*******************************************************************************
* Function Name  : main
* Description    : ϵͳ��ں���.
* Input          : �¼�
* Output         : None
* Return         : None
*******************************************************************************/
extern int main(void)
{
    u8 evt;

	SystemInit2();
    USART1_Putchar(0XAA);
	printf("feq=%d\r\n", (RCC_Clocks.HCLK_Frequency/1000));
    while (1)
	{
        pollint();
		evt = MainQEvtPend();
        modeCommon(evt);
        Mode_Event_Proc(evt);
    }
}
//===============================================================

//===============================================================
typedef struct
{
	uint8_t Hour;
	uint8_t Minute;
	uint8_t Second;
}SYSTIMES_STR;
static SYSTIMES_STR sysTime;
extern void Mode_Idle_Init(void)
{
#ifdef INCLUDE_OLED12832
	uint8_t timeTemp[17];
	sprintf((char *)timeTemp, "%02d:%02d:%02d 2016/4", sysTime.Hour, sysTime.Minute, sysTime.Second);
	OLED_ShowString(0, 0, "Tag Test");  
	OLED_ShowString(0, 16, timeTemp);
#endif	
}

extern void Mode_Idle_Proc(uint8_t evt)
{
	uint8_t timeTemp[17];
	
	switch(evt){
		case evtTime1s:
			if (++sysTime.Second >= 60){
				sysTime.Second = 0;
				if (++sysTime.Minute >= 60){
					sysTime.Minute = 0;
					if (++sysTime.Hour >= 24){
						sysTime.Hour = 0;
					}
				}
			}
			#ifdef INCLUDE_OLED12832
			sprintf((char *)timeTemp, "%02d:%02d:%02d 2016/4", sysTime.Hour, sysTime.Minute, sysTime.Second);
			OLED_ShowString(0, 16 , timeTemp);
			//oledShowBmpTest();
			#endif	
			break;
		default:
			
			break;
	}

}


