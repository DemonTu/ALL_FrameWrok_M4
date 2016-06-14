#include "includes.h"

volatile uint32_t TimerTick;

/*******************************************************************************
* Function Name  : TimerTickNproc
* Description    : ϵͳ1ms��ʱ������.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
extern void TimerTickNproc(void)
{
	TimerTick++;
	PollintFlagSet();
}
/*******************************************************************************
* Function Name  : SysTimeDly
* Description    : Ӳ��ʱ.
* Input          : ��ʱ��ʱ��(ms)
* Output         : None
* Return         : None
*******************************************************************************/
extern void SysTimeDly(uint32_t tm)
{
	uint32_t start;
	start = SysGetTimeTick();
	while ((SysGetTimeTick()-start) < (uint32_t)tm)
	{
        #ifdef INCLUDE_IWDG
        IWDG_ReloadCounter();
        #endif
	}
}
/*******************************************************************************
* Function Name  : SysEnterSleepMode
* Description    : ϵͳ����˯��ģʽ.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
extern void SysEnterSleepMode(void)
{
    /* Config the EXTI to wake up from SLEEP Mode */
    /* Mode: SLEEP + Entry with WFI*/
    __WFI();
}
/*******************************************************************************
* Function Name  : SysGetTimeTick
* Description    : ȡϵͳ�δ�ms��������ֵ.
* Input          : None
* Output         : None
* Return         : ϵͳ�δ�ms��������ֵ
*******************************************************************************/
extern uint32_t SysGetTimeTick(void)
{
	uint32_t tick;

	tick = TimerTick;
	return tick;
}

static IWDG_HandleTypeDef handleIWDG;

/*******************************************************************************
* Function Name  : IWDGInit
* Description    : �������Ź���ʼ������.��λʱ�� 280 * 4 ms
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
extern void IWDGInit(void)
{
/* IWDG timeout equal to 280 ms (the timeout may varies due to LSI frequency
     dispersion) */

	handleIWDG.Instance = IWDG;
	 /* IWDG counter clock: 40KHz(LSI) / 32 = 1.25 KHz */  //8MS *349 =  280ms
	handleIWDG.Init.Prescaler = IWDG_PRESCALER_32;
	 /* Set counter reload value to 349 */
	handleIWDG.Init.Reload = 349*4;
	 /* Enable write access to IWDG_PR and IWDG_RLR registers */
	HAL_IWDG_Init(&handleIWDG);
	
	/* Reload IWDG counter */
	/* Enable IWDG (the LSI oscillator will be enabled by hardware) */
	HAL_IWDG_Start(&handleIWDG);
}
/**************************************************************************
* Funtion Name : IWDG_ReloadCounter
* Description  : ι��
* Input		   : None
* Output       : None
* Return       : None
***************************************************************************/
extern void IWDG_ReloadCounter(void)
{
	/* Reload IWDG counter with value defined in the RLR register */
	__HAL_IWDG_RELOAD_COUNTER(&handleIWDG);
}