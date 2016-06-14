#include "includes.h"

volatile u32 TimerTick;

/*******************************************************************************
* Function Name  : TimerTickNproc
* Description    : 系统1ms定时处理函数.
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
* Description    : 硬延时.
* Input          : 延时的时间(ms)
* Output         : None
* Return         : None
*******************************************************************************/
extern void SysTimeDly(u32 tm)
{
	u32 start;
	start = SysGetTimeTick();
	while ((SysGetTimeTick()-start) < (u32)tm)
	{
        #ifdef INCLUDE_IWDG
        IWDG_ReloadCounter();
        #endif
	}
}
/*******************************************************************************
* Function Name  : SysEnterSleepMode
* Description    : 系统进入睡眠模式.
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
* Description    : 取系统滴答ms积存器的值.
* Input          : None
* Output         : None
* Return         : 系统滴答ms积存器的值
*******************************************************************************/
extern u32 SysGetTimeTick(void)
{
	u32 tick;

	tick = TimerTick;
	return tick;
}
/*******************************************************************************
* Function Name  : IWDGInit
* Description    : 独立看门狗初始化开启.复位时间 280 * 4 ms
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
extern void IWDGInit(void)
{
/* IWDG timeout equal to 280 ms (the timeout may varies due to LSI frequency
     dispersion) */
   /* Enable write access to IWDG_PR and IWDG_RLR registers */
   IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

   /* IWDG counter clock: 40KHz(LSI) / 32 = 1.25 KHz */  //8MS *349 =  280ms
   IWDG_SetPrescaler(IWDG_Prescaler_32);

   /* Set counter reload value to 349 */
   IWDG_SetReload(349 * 4);

   /* Reload IWDG counter */
   IWDG_ReloadCounter();

   /* Enable IWDG (the LSI oscillator will be enabled by hardware) */
   IWDG_Enable();
}
