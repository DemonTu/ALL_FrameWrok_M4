#include "includes.h"

//==============================================================================
// ���ݳ�Ա: ����

typedef struct
{
    uint8_t sys_mode;
    uint8_t pre_sys_mode;
} MODE_STATE;
static MODE_STATE mode_state;
//==============================================================================


// ��Ա����
typedef struct
{
    void (*state_init_func)(void);        // ģʽ�ĳ�ʼ������ָ��
    void (*evt_pro_func)(const uint8_t);  // ģʽ���¼�������
    void (*state_exit_func)(void);        // ģʽ���¼�������
} STATE_TYPE;
//==============================================================================
// ע��:��Ҫ��ģ���ʼ�������е��� SysChangeMode(�Լ���ģʽ); �����ɵݹ飬�������޷����

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
        // �쳣
        Mode_Change(MODE_IDLE);
    }
}
//==============================================================================
// ģʽ�л�����
extern void Mode_Change(uint8_t mode)
{
    if (mode < a_num(MODE_TAB))
    {
        (*MODE_TAB[mode].state_init_func)();
        // ��¼ǰһ��ģʽ
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
* Description    : �ָ�����һ�ε�ģʽ
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

