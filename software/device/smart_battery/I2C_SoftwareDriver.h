#ifndef __IIC_SOFTWAREDRIVER_H
	#define __IIC_SOFTWAREDRIVER_H
	
#ifdef INCLUDE_IIC_DRIVER
extern void    IIC_DriverInit(void);
extern void    IIC_Start(void);
extern uint8_t IIC_START_SHT(void);
extern void    IIC_Stop(void);
extern uint8_t IIC_SendByte(uint8_t Data);
extern uint8_t IIC_ReadByte(uint8_t dat);
unsigned char IIC_ReadDevice(unsigned char device, unsigned char *subaddr, unsigned char acnt, unsigned char *buf, unsigned char bcnt);

#endif
#endif	
