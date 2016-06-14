#include "includes.h"

typedef struct
{
    uint32_t             msCountDown;        // ������ʱ������
    uint32_t             _msCount;
    uint32_t             _10msCount;
    uint32_t             _50msCount;
    uint32_t             _100msCount;
    uint32_t             _500msCount;
    uint32_t             Pollintack;
} STR_POLLINT;

static STR_POLLINT r_pollint;
extern void PollintFlagSet(void)
{
	r_pollint.Pollintack = 1;
}

// ����1-9ÿ1��ִ��һ��
static void Pollint1Sec(void)
{
    switch(r_pollint._100msCount)
    {
        case 1:
			MainQEvtPost(evtTime1s);
            break;
        case 2:
            break;
        case 3:
            break;
        default:
            break;
    }
}

// ����1-9ÿ100����ִ��һ��
static void Pollint100mSec(void)
{
    switch(r_pollint._10msCount)
    {
		case 0:
			Pollint1Sec();
			break;
        case 1:
			MainQEvtPost(evtTime100ms);
            break;
		case 2:
            if ((r_pollint._500msCount++) >= 5)
            {
                r_pollint._500msCount = 0;
                MainQEvtPost(evtTime500ms);
            }
			break;
        case 3:
			#ifdef INCLUDE_OLED12832
			OLED_Refresh_Gram();
			#endif
            break;
        case 4:
            break;
        case 5:
            break;
        case 6:
            break;
        case 7:
            break;
        case 8:
            break;
        default:
            break;
    }
}

// ����1-9ÿ10����ִ��һ��
static void Pollint10mSec(void)
{
    switch(r_pollint._msCount)
    {
		case 0:
			Pollint100mSec();
			break;
        case 1:
            break;
        case 2:
            break;
        case 3:
            #ifdef INCLUDE_BUZZER
            BuzzerProc();
            #endif
            break;
        default:
            break;
    }
}


// ÿ������������ʱ��
static void ChangeTime1mSec(void)
{
    if(r_pollint.msCountDown > 0)
    {
        r_pollint.msCountDown--;
    }
    r_pollint._msCount = (r_pollint._msCount + 1) % 10;
    if(r_pollint._msCount == 0)
    {
        r_pollint._10msCount = (r_pollint._10msCount + 1) % 10;
        if(r_pollint._10msCount == 0)
        {
            r_pollint._100msCount = (r_pollint._100msCount + 1) % 10;
        }
    }
}

// ÿ������������ʱ��,�ɹ����ķ���1
static uint32_t ChangeTimeCount_OK(void)
{
    if(r_pollint.Pollintack)
    {
        r_pollint.Pollintack = 0;
		ChangeTime1mSec();
        return (1);
    }
	return (0);
}

// ������ʱ,�������ʱ,�������κ���ʱ����ص��¼�,��׼ȷ,�����ڶ�ʱ��(���뼶��)����ʱ
extern void delayms_Lock(uint32_t value)
{
    r_pollint.msCountDown = value + 1;
    while(r_pollint.msCountDown)
    {
        ChangeTimeCount_OK();
    }
}

// ������ʱ,�������ʱ,��ʱ������ʱ����ص��¼�,��˲���̫׼ȷ,�����ڳ�ʱ��(�뼶��)����ʱ
extern void delay_NoLock(uint32_t value)
{
    r_pollint.msCountDown = value + 1;
    while(r_pollint.msCountDown)
    {
        pollint();
    }
}


// ����    ��ʱ��ѯ����
extern void pollint(void)
{
    __WFI();
    if( ChangeTimeCount_OK() )
    {
		/* û����ִ��һ�ε��¼� */
        Pollint10mSec();
		// 1ms �¼�
	    USART1_RxReaddy();
    }
    #ifdef INCLUDE_IWDG
	IWDG_ReloadCounter();
    #endif
}


