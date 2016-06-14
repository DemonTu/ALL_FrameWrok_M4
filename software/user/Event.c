#include "includes.h"
//=============================================================================
typedef struct
{
	u8 head;
	u8 tail;
	u8 q[256];
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
extern void MainQEvtPost(u8 evt)
{
	u8 tmpHead;

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
extern u8 MainQEvtPend(void)
{
	u8 evt;

	if (MainQ.head == MainQ.tail)
	{
        return evtNull;
	}
	evt = MainQ.q[MainQ.tail];
	MainQ.tail = (MainQ.tail+1) & (sizeof(MainQ.q) - 1);
	return evt;
}
//=============================================================================
