#ifndef __MODE_SPECIAL_H
    #define __MODE_SPECIAL_H


// �����Ϊ���¼���ģʽ
enum
{
    MODE_IDLE = 0,        // ����

    MODE_MAX              //
};


#define a_num(a)   			(sizeof(a)/sizeof((a)[0]))

extern void Mode_Init(void);
// ����ģʽ�¼�����
extern void Mode_Event_Proc(uint8_t evt);
// ģʽ�л�
extern void Mode_Change(uint8_t mode);
// �ص�ǰһ��״̬
extern void Mode_Pre_Change(void);
// ��ǰ״̬
extern uint8_t Get_Mode_State(void);
//==============================================================================
extern uint8_t Is_Power_Save_Mode(void);
//==============================================================================
#endif
