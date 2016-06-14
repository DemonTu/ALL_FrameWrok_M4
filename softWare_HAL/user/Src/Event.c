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
* Description    : �¼����г�ʼ��.
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
* Description    : �¼������¼�����.
* Input          : �����¼�
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
* Description    : �����¼�.
* Input          : �¼�Timeoutʱ��(ms)
* Output         : None
* Return         : �¼�ֵ
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
