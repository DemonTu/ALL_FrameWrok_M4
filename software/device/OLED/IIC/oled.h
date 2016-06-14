#ifndef __OLED_H
#define __OLED_H

//SSD1306 OLED 驱动IC驱动代码
//驱动方式:8080并口/4线串口
//修改日期:2016/64/11
//版本：V1.0
//版权所有，盗版必究。
//All rights reserved
////////////////////////////////////////////////////////////////////////////////// 	  

#if defined (INCLUDE_OLED12832)

#define OLED_MODE 0
		    						
//OLED控制用函数
//void OLED_WR_Byte(u8 dat,u8 cmd);	    
void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Refresh_Gram(void);		   
							   		    
void OLED_Init(void);
void OLED_Clear(void);
void OLED_DrawPoint(u8 x,u8 y,u8 t);
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot);
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode);
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size);
void OLED_ShowString(u8 x,u8 y,const u8 *p);	
//void IIC_WriteC(u8 cmd);
//void IIC_writeD(u8 data);

void oledShowBmp(uint8_t *bmp);
void oledShowBmpTest(void);


#endif
#endif

