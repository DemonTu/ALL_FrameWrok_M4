#ifndef __EVENT_H__
#define __EVENT_H__
//=============================================================================
enum
{
	evtNull = 0,
	evtTest = evtNull+1,
//°´ÏÂ
	evtNUM0,       // '0'            2
	evtNUM1,       // '1'            3
	evtNUM2,       // '2'            4
	evtNUM3,       // '3'            5
	evtNUM4,       // '4'            6
	evtNUM5,       // '5'            7
	evtNUM6,       // '6'            8
	evtNUM7,       // '7'            9
	evtNUM8,       // '8'            a
	evtNUM9,       // '9'            b

//=======================================
	evtTime100ms,
	evtTime200ms,
    evtTime300ms,
    evtTime500ms,
    evtTime1s,
    evtTime3s,
    evtRxReaddy,




	evtMax
};

//=============================================================================
extern void  MainQInit(void);
extern void  MainQEvtPost(u8 evt);
extern u8    MainQEvtPend(void);
//=============================================================================
#endif
