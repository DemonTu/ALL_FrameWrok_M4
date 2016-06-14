#include "includes.h"

//static uint32_t MsTimeTick = 0;

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

	//HAL_RCC_DeInit();

	HAL_Init();

	__PWR_CLK_ENABLE();

	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.LSIState = RCC_LSI_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 7;
	RCC_OscInitStruct.PLL.PLLN = 196;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
	RCC_OscInitStruct.PLL.PLLQ = 7;
	HAL_RCC_OscConfig(&RCC_OscInitStruct);

	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
	HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2);

	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
	PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
	HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);

	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);
	//MsTimeTick = 84000;
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	/* Enable GPIOA, GPIOB, GPIOC, GPIOD, GPIOE and AFIO clocks */

	__GPIOH_CLK_ENABLE();
	__GPIOA_CLK_ENABLE();
	__GPIOB_CLK_ENABLE();
	__GPIOC_CLK_ENABLE();  
	__GPIOD_CLK_ENABLE();
	//__GPIOE_CLK_ENABLE();
	//__GPIOF_CLK_ENABLE();
	//__GPIOG_CLK_ENABLE();


}

/*******************************************************************************
* Function Name  : prvSetupHardware
* Description    : 系统时钟和IO时钟配置
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
uint8_t HSE_Or_HSI;                         // 1:外部晶震，    0:内部RC震荡
static void prvSetupHardware( void )
{
     /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	SystemClock_Config();
}


/*******************************************************************************
* Function Name  : GPIO_Configuration
* Description    : IO口功能配置
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.Pin   = GPIO_PIN_12;
    GPIO_InitStructure.Speed = GPIO_SPEED_MEDIUM;
    GPIO_InitStructure.Mode  = GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);	

	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET);
}

/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : 中断使能和优先级配置.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void NVIC_Configuration(void)
{

    /* enabling interrupt */
	HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(USART1_IRQn);
}

/*******************************************************************************
* Function Name  : SystemInit
* Description    : 系统初始化.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
extern void SystemInit2(void)
{
    prvSetupHardware();
	GPIO_Configuration();
	NVIC_Configuration();

    USART1_Init();
    MainQInit();
#if defined (INCLUDE_BUZZER)
    BuzzerInit();
    BuzzerBeep(4);
#endif
#if defined (INCLUDE_OLED12832)
	OLED_Init();
#endif
#ifdef INCLUDE_IWDG
    IWDGInit();
#endif
}
/*******************************************************************************
* Function Name  : modeCommon
* Description    : 系统公共模式.
* Input          : 事件
* Output         : None
* Return         : None
*******************************************************************************/
static void modeCommon(uint8_t evt)
{
    switch(evt)
    {
        case evtRxReaddy:
            {
                uint16_t len;
                uint8_t rxbuf[usart1_rx_buf_Number];

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
            USART1_Putchar(0X55);
				{
					static uint8_t flashFlag;
					if (flashFlag){
						flashFlag = 0;
						HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET);
					}else{
						flashFlag = 1;
						HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);
					}
				}
            break;

            default:
                break;

    }
}
/*******************************************************************************
* Function Name  : main
* Description    : 系统入口函数.
* Input          : 事件
* Output         : None
* Return         : None
*******************************************************************************/
extern int main(void)
{
    uint8_t evt;
	
	SystemInit2();
    USART1_Putchar(0XAA);
	printf("feq=%d\r\n", (HAL_RCC_GetHCLKFreq()/1000));
	OLED_ShowString(0,0, " Tag OLED Test");  
 	//OLED_ShowString(0,16,"555"); 
	//OLED_DrawPoint(0,0,1);

	//oledShowBmpTest();
    while (1)
	{
        pollint();
		evt = MainQEvtPend();
        modeCommon(evt);
        Mode_Event_Proc(evt);
    }
}
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
	uint8_t timeTemp[17];
	sprintf((char *)timeTemp, "%02d:%02d:%02d 2016/4", sysTime.Hour, sysTime.Minute, sysTime.Second);
	OLED_ShowString(0, 16 , timeTemp);
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
			sprintf((char *)timeTemp, "%02d:%02d:%02d 2016/4", sysTime.Hour, sysTime.Minute, sysTime.Second);
			OLED_ShowString(0, 16 , timeTemp);
			//oledShowBmpTest();
			break;
		default:
			
			break;
	}
}

