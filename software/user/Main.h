#ifndef __MAIN_H__
#define __MAIN_H__

extern uint8_t HSE_Or_HSI;   // 1:外部晶震，    0:内部RC震荡

extern void Mode_Idle_Init(void);

extern void Mode_Idle_Proc(uint8_t evt);

#endif
