#ifndef __TIMER_H__
#define __TIMER_H__


extern void IWDGInit(void);
extern void TimerTickNproc(void);
extern u32  SysGetTimeTick(void);
extern void SysEnterSleepMode(void);
extern void SysTimeDly(u32 tm);
#endif

