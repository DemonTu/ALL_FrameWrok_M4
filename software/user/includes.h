#ifndef __INCLUDES_H__
#define __INCLUDES_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include "stm32f4xx.h"

#include "config.h"
#include "pollint.h"
#include "io.h"
#include "timer.h"
#include "usart1.h"
#include "event.h"
#include "main.h"

//设备驱动
#include "I2C_Driver.h"
#include "SmBus_Driver.h"

#ifdef INCLUDE_OLED12832
#include "oled.h"
#endif

//#include "sstflash.h"
#if defined (INCLUDE_BUZZER)
#include "buzzer.h"
#endif
#include "Mode_Special.h"


//#define  USEUSBDOWNLOAD                // 用USB下载程序 有前 12 K


typedef union
{
	u8    c[4];
	u16   i[2];
	u32   l;
#ifdef IAR_ARMCC
}LONGCHAR_T;
#else
}__attribute__ ((packed)) LONGCHAR_T;
#endif
#endif

