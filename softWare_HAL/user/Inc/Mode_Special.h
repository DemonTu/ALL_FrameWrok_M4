#ifndef __MODE_SPECIAL_H
    #define __MODE_SPECIAL_H


// 软件分为以下几个模式
enum
{
    MODE_IDLE = 0,        // 空闲

    MODE_MAX              //
};


#define a_num(a)   			(sizeof(a)/sizeof((a)[0]))

extern void Mode_Init(void);
// 各个模式事件处理
extern void Mode_Event_Proc(uint8_t evt);
// 模式切换
extern void Mode_Change(uint8_t mode);
// 回到前一个状态
extern void Mode_Pre_Change(void);
// 当前状态
extern uint8_t Get_Mode_State(void);
//==============================================================================
extern uint8_t Is_Power_Save_Mode(void);
//==============================================================================
#endif
