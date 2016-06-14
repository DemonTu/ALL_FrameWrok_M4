#include "includes.h"

typedef struct
{
    uint32_t             msCountDown;        // 毫秒延时计数器
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

// 以下1-9每1秒执行一次
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

// 以下1-9每100毫秒执行一次
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

// 以下1-9每10毫秒执行一次
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


// 每毫秒更改软件定时器
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

// 每毫秒更改软件定时器,成功更改返回1
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

// 毫秒延时,进入此延时,不处理任何与时间相关的事件,教准确,适用于短时间(毫秒级别)的延时
extern void delayms_Lock(uint32_t value)
{
    r_pollint.msCountDown = value + 1;
    while(r_pollint.msCountDown)
    {
        ChangeTimeCount_OK();
    }
}

// 毫秒延时,进入此延时,定时处理与时间相关的事件,因此不会太准确,适用于长时间(秒级别)的延时
extern void delay_NoLock(uint32_t value)
{
    r_pollint.msCountDown = value + 1;
    while(r_pollint.msCountDown)
    {
        pollint();
    }
}


// 函数    定时查询程序
extern void pollint(void)
{
    __WFI();
    if( ChangeTimeCount_OK() )
    {
		/* 没毫秒执行一次的事件 */
        Pollint10mSec();
		// 1ms 事件
	    USART1_RxReaddy();
    }
    #ifdef INCLUDE_IWDG
	IWDG_ReloadCounter();
    #endif
}


