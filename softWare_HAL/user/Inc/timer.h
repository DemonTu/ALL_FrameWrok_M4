#ifndef __TIMER_H__
#define __TIMER_H__


extern void IWDGInit(void);
extern void TimerTickNproc(void);
extern uint32_t  SysGetTimeTick(void);
extern void SysEnterSleepMode(void);
extern void SysTimeDly(uint32_t tm);
extern void IWDG_ReloadCounter(void);
#endif

