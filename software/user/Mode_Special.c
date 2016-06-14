#include "includes.h"

//==============================================================================
// 数据成员: 属性

typedef struct
{
    uint8_t sys_mode;
    uint8_t pre_sys_mode;
} MODE_STATE;
static MODE_STATE mode_state;
//==============================================================================


// 成员函数
typedef struct
{
    void (*state_init_func)(void);        // 模式的初始化函数指针
    void (*evt_pro_func)(const uint8_t);  // 模式的事件处理函数
    void (*state_exit_func)(void);        // 模式的事件处理函数
} STATE_TYPE;
//==============================================================================
// 注意:不要在模块初始化函数中调用 SysChangeMode(自己的模式); 这会造成递归，编译器无法察觉

static const STATE_TYPE  MODE_TAB[]=
{
   [MODE_IDLE] = { Mode_Idle_Init,   Mode_Idle_Proc,    NULL,  },
};
//==============================================================================
extern void Mode_Mnit(void)
{
}
//==============================================================================

extern void Mode_Event_Proc(uint8_t evt)
{
    if (mode_state.sys_mode < a_num(MODE_TAB))
    {
        (*MODE_TAB[mode_state.sys_mode].evt_pro_func)(evt);
    }
    else
    {
        // 异常
        Mode_Change(MODE_IDLE);
    }
}
//==============================================================================
// 模式切换函数
extern void Mode_Change(uint8_t mode)
{
    if (mode < a_num(MODE_TAB))
    {
        (*MODE_TAB[mode].state_init_func)();
        // 记录前一个模式
        mode_state.pre_sys_mode = mode_state.sys_mode;
        mode_state.sys_mode = mode;
    }
    else
    {
        mode_state.sys_mode = MODE_IDLE;
    }
}

/*******************************************************************************
* Function Name  : Mode_Pre_Change
* Description    : 恢复到上一次的模式
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
extern void Mode_Pre_Change(void)
{
	if (mode_state.pre_sys_mode != mode_state.sys_mode)
	{
    	Mode_Change(mode_state.pre_sys_mode);
	}
	else
	{

	}
}
//==============================================================================
extern uint8_t Is_Power_Save_Mode(void)
{
    if (mode_state.sys_mode == 1)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
//==============================================================================
extern uint8_t Get_Mode_State(void)
{
    return mode_state.sys_mode;
}
//==============================================================================

