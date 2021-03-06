#include "includes.h"
//=============================================================================
typedef struct
{
	uint8_t head;
	uint8_t tail;
	uint8_t q[256];
#ifdef IAR_ARMCC
}MAINQ_T;
#else
} __attribute__ ((packed)) MAINQ_T;
#endif

static MAINQ_T  MainQ;

/*******************************************************************************
* Function Name  : MainQInit
* Description    : 事件队列初始化.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
extern void MainQInit(void)
{
	MainQ.head = MainQ.tail = 0;
}
/*******************************************************************************
* Function Name  : MainQEvtPost
* Description    : 事件传入事件队列.
* Input          : 传入事件
* Output         : None
* Return         : None
*******************************************************************************/
extern void MainQEvtPost(uint8_t evt)
{
	uint8_t tmpHead;

	tmpHead = MainQ.head;
	MainQ.head = (tmpHead + 1) & (sizeof(MainQ.q) - 1);
	MainQ.q[tmpHead] = evt;
}
/*******************************************************************************
* Function Name  : MainQEvtPend
* Description    : 读出事件.
* Input          : 事件Timeout时间(ms)
* Output         : None
* Return         : 事件值
*******************************************************************************/
extern uint8_t MainQEvtPend(void)
{
	uint8_t evt;

	if (MainQ.head == MainQ.tail)
	{
        return evtNull;
	}
	evt = MainQ.q[MainQ.tail];
	MainQ.tail = (MainQ.tail+1) & (sizeof(MainQ.q) - 1);
	return evt;
}
//=============================================================================
