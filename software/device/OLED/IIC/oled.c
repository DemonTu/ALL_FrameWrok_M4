#include "includes.h"
#include "font.h"  	 
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//Mini STM32开发板
//SSD1306 OLED 驱动IC驱动代码
//驱动方式:IIC
////////////////////////////////////////////////////////////////////////////////// 	  

#ifdef INCLUDE_OLED12832

//#define IIC_SOFTWARE
//-----------------OLED端口定义----------------  					   	
//#define OLED_RS PCout(8)

#ifdef IIC_SOFTWARE
#define I2C_SDA_GPIO		GPIOB
#define I2C_SDA_PIN			GPIO_Pin_9
#define I2C_SCL_GPIO        GPIOB
#define I2C_SCL_PIN         GPIO_Pin_8

//使用IIC串行接口时使用
#define OLED_SDA_HIGH() GPIO_WriteBit(I2C_SDA_GPIO, I2C_SDA_PIN, Bit_SET)
#define OLED_SDA_LOW()  GPIO_WriteBit(I2C_SDA_GPIO, I2C_SDA_PIN, Bit_RESET)
#define OLED_SCL_HIGH() GPIO_WriteBit(I2C_SCL_GPIO, I2C_SCL_PIN, Bit_SET)
#define OLED_SCL_LOW()  GPIO_WriteBit(I2C_SCL_GPIO, I2C_SCL_PIN, Bit_RESET)
#else

//I2C_HandleTypeDef handleIIC;

#endif
#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据
#define IIC_ADD_WC 0x78		//写命令时的地址
#define IIC_ADD_WD 0x78		//写数据时的地址
#define IIC_CONC 0x00		//控制字节指定下一个字节为命令
#define IIC_COND 0x40		//控制字节指定下一个字节为数据

#define MAX_LINE_NAME  128
#define MAX_ROW_NAME   32

void delay_us(u16 time)
{	  
//	u16 i=0;  

//	while(time--)
//	{
//	   i=12;  //自己定义
//	   while(i--) ;    
//	}
}

//向SSD1306写入一个字节。
//dat:要写入的数据/命令
//cmd:数据/命令标志 0,表示命令;1,表示数据;
	    	    
#ifdef IIC_SOFTWARE
static void IIC_WriteChar(u8 byte)
{
	u8 i,temp;
	OLED_SCL_LOW();
	temp = byte;
	for(i=0; i<8; i++)
	{
		if((temp&0x80)>>7){
			OLED_SDA_HIGH();
		}else{
			OLED_SDA_LOW();
		}
		
		OLED_SCL_HIGH();
		//delay_us(1);
		OLED_SCL_LOW();
		//delay_us(1);
		temp = temp<<1;
	}

}

static void IIC_Wait_Ack()
{
	OLED_SCL_HIGH();
	//delay_us(1);
	OLED_SCL_LOW();
}

static void IIC_Start()
{

	OLED_SCL_HIGH();
	OLED_SDA_HIGH();
	//delay_us(1);
	OLED_SDA_LOW();
	//delay_us(1);
	OLED_SCL_LOW();
	//delay_us(1);
}

static void IIC_Stop()
{
	//delay_us(1);
	OLED_SCL_HIGH();
	
	OLED_SDA_LOW();
	//delay_us(1);

	OLED_SDA_HIGH();
	//delay_us(1);
}
#endif

// 写命令
static void IIC_WriteC(u8 cmd)
{
#ifdef IIC_SOFTWARE
	IIC_Start();						//开始传输
	IIC_WriteChar(IIC_ADD_WC);			//传输地址
	IIC_Wait_Ack();						//接收1306返回的确认信号
	//delay_us(1);
	IIC_WriteChar(IIC_CONC);			//发送控制字节
	IIC_Wait_Ack();						//接收1306返回的确认信号
	//delay_us(1);
	IIC_WriteChar(cmd);					//发送命令字节
	IIC_Wait_Ack();						//接收1306返回的确认信号
	//delay_us(1);
	IIC_Stop();							//结束传输
#else
	BSP_I2cWrite(I2C_1, &cmd, IIC_ADD_WC, IIC_CONC, 1);
	while (BSP_I2cIdleState(I2C_1))
	{
		//printf("idle");
	}
	
#endif
}

// 写数据
static void IIC_WriteD(u8 data)
{
#ifdef IIC_SOFTWARE
	IIC_Start();							//开始传输
	IIC_WriteChar(IIC_ADD_WD);			//传输地址
	IIC_Wait_Ack();							//接收1306返回的确认信号
	IIC_WriteChar(IIC_COND);			//发送控制字节
	IIC_Wait_Ack();							//接收1306返回的确认信号
	//delay_us(1);
	IIC_WriteChar(data);					//发送数据字节
	IIC_Wait_Ack();							//接收1306返回的确认信号
	//delay_us(1);
	IIC_Stop();							//结束传输
#else
	BSP_I2cWrite(I2C_1, &data, IIC_ADD_WD, IIC_COND, 1);
	while (BSP_I2cIdleState(I2C_1))
	{
		
	}
#endif
}

// cmd : 1 write data; 0 write command
static void OLED_WR_Byte(u8 dat, u8 cmd)
{
		u8 t_data;

		t_data = dat;
		
		if(!cmd)
		{
			IIC_WriteC(t_data);
		}
		else
		{
			IIC_WriteD(t_data);
		}

}
//OLED的显存
//存放格式如下.
//[0]0 1 2 3 ... 127	
//[1]0 1 2 3 ... 127	
//[2]0 1 2 3 ... 127	
//[3]0 1 2 3 ... 127	
//[4]0 1 2 3 ... 127	
//[5]0 1 2 3 ... 127	
//[6]0 1 2 3 ... 127	
//[7]0 1 2 3 ... 127 		   
u8 OLED_GRAM[MAX_LINE_NAME][MAX_ROW_NAME/8];	 
u8 OLED_GRAM_Temp[MAX_LINE_NAME][MAX_ROW_NAME/8];
//更新显存到LCD		 
void OLED_Refresh_Gram(void)
{
	u8 i,n;		 
	if (memcmp(OLED_GRAM_Temp, OLED_GRAM, sizeof(OLED_GRAM))){
		memcpy(OLED_GRAM_Temp, OLED_GRAM, sizeof(OLED_GRAM));	
		printf("OLED Updata\r\n");
	}else{
		// 数据一样是无需更新
		return;
	}
	for(i=0;i<(MAX_ROW_NAME/8);i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //设置页地址（0~3）
		OLED_WR_Byte (0x04,OLED_CMD);      //设置显示位置—列低地址
		OLED_WR_Byte (0x10,OLED_CMD);      //设置显示位置—列高地址   
		for(n=0;n<MAX_LINE_NAME;n++){
			OLED_WR_Byte(OLED_GRAM[n][i],OLED_DATA); 
		}
	}   
}

	  	  
//开启OLED显示    
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
	OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}
//关闭OLED显示     
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}		   			 
//清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!	  
void OLED_Clear(void)  
{  
	u8 i,n;  
	for(i=0; i<MAX_ROW_NAME/8; i++)
	{
		for(n=0; n<MAX_LINE_NAME; n++)
		{
			OLED_GRAM[n][i]=0X00;  
		}
	}
	OLED_Refresh_Gram();//更新显示
}
//画点 
//x:0~127
//y:0~31
//t:1 填充 0,清空				   
void OLED_DrawPoint(u8 x,u8 y,u8 t)
{
	u8 pos,bx,temp=0;
	if(x>127||y>31)return;//超出范围了.
	pos=3-y/8;
	bx=y%8;
	temp=1<<(7-bx);
	// 显示x轴地址是 124 125 126 127 0 1 .... 123
	if (x < 4){
		x = 124+x;
	}else{
		x = x - 4;
	}
	if(t){
		OLED_GRAM[x][pos]|=temp;
	}else{
		OLED_GRAM[x][pos]&=~temp;	    
	} 
}
//x1,y1,x2,y2 填充区域的对角坐标
//确保x1<=x2;y1<=y2 0<=x1<=127 0<=y1<=63	 	 
//dot:0,清空;1,填充	  
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot)  
{  
	u8 x,y;  
	for(x=x1;x<=x2;x++)
	{
		for(y=y1;y<=y2;y++)
		{
			OLED_DrawPoint(x,y,dot);
		}
	}													    
	OLED_Refresh_Gram();//更新显示
}
/***********************************************************************
* Funtion Name : oledShowBmp(uint8_t *bmp)
* Description  : 显示位图, 行扫描
* Input        : bmp 位图数据
* Output       : None
* return       : None
**************************************************************************/
void oledShowBmp(uint8_t *bmp)
{ 
	uint8_t i;
	uint8_t x,y;
	uint8_t temp = 0;
	
	for(y=0; y<MAX_ROW_NAME; y++){
		// 每行 16*8=128;
		for(x=0; x<16; x++){
			temp = bmp[x+y*16];
			for(i=0; i<8; i++){
				OLED_DrawPoint(x*8+i,y,temp&0x01);
				temp>>=1;
			}
		}
	}
}
  
void oledShowBmpTest(void)
{
	oledShowBmp((uint8_t *)&yanglong12832[0][0]);
}

//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//mode:0,反白显示;1,正常显示				 
//size:选择字体 16/12 
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode)
{      			    
	u8 temp,t,t1;
	u8 y0=y;
	chr=chr-' ';//得到偏移后的值				   
    for(t=0;t<size;t++)
    {   
		if(size==12){
			temp=asc2_1206[chr][t];  //调用1206字体	
		}else{
			temp=asc2_1608[chr][t];		 //调用1608字体 	                          
		} 
        for(t1=0;t1<8;t1++)
		{
			if(temp&0x80){
				OLED_DrawPoint(x,y,mode);
			}else{
				OLED_DrawPoint(x,y,!mode);
			}
			temp<<=1;
			y++;
			if((y-y0)==size)
			{
				y=y0;
				x++;
				break;
			}
		}  	 
    }          
}
//m^n函数
u32 mypow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--){
		result*=m;	
	}    
	return result;
}				  
//显示2个数字
//x,y :起点坐标	 
//len :数字的位数
//size:字体大小
//mode:模式	0,填充模式;1,叠加模式
//num:数值(0~4294967295);	 		  
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size)
{         	
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/mypow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				OLED_ShowChar(x+(size/2)*t,y,' ',size,1);
				continue;
			}else enshow=1; 
		 	 
		}
	 	OLED_ShowChar(x+(size/2)*t,y,temp+'0',size,1); 
	}
} 
//显示字符串
//x,y:起点坐标  
//*p:字符串起始地址
//用16字体
void OLED_ShowString(u8 x,u8 y,const u8 *p)
{
#define MAX_CHAR_POSX 122
#define MAX_CHAR_POSY 29          
    while(*p!='\0')
    {       
        if(x>MAX_CHAR_POSX){x=0;y+=16;}
        if(y>MAX_CHAR_POSY){y=x=0;OLED_Clear();}
        OLED_ShowChar(x,y,*p,16,1);	 
        x+=8;
        p++;
    }  
}	   
/*************************************************************
* Funtion Name : OLEDIICGPIO_Init()
* Description  : IIC接口I/O初始化
* Input        : None
* Output       : None
* Return       : None
*************************************************************/
static void OLEDIICGPIO_Init(void)
{
#ifdef IIC_SOFTWARE
	GPIO_InitTypeDef GPIO_InitStructure;
	  
	// GPIO 时钟已在系统时钟初始化时使能

	GPIO_InitStructure.GPIO_Pin   = I2C_SDA_PIN | I2C_SCL_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	
    GPIO_Init(I2C_SCL_GPIO, &GPIO_InitStructure);
#else // 硬件IIC
	BSP_I2cOpen(I2C_1, 100000, I2C_Mode_SMBusHost);
#endif

}

//初始化SSD1306					    
void OLED_Init(void)
{ 	
	OLEDIICGPIO_Init();

	SysTimeDly(100);
#if 1	  
	OLED_WR_Byte(0xAE,OLED_CMD); //关闭显示
	
	OLED_WR_Byte(0xD5,OLED_CMD); //设置时钟分频因子,震荡频率
	OLED_WR_Byte(0x80,OLED_CMD);   //[3:0],分频因子;[7:4],震荡频率
	
	OLED_WR_Byte(0xA8,OLED_CMD); //设置驱动路数
	OLED_WR_Byte(0X1f,OLED_CMD); //默认0X3F(1/64) 
	
	OLED_WR_Byte(0xD3,OLED_CMD); //设置显示偏移
	OLED_WR_Byte(0X00,OLED_CMD); //默认为0

	OLED_WR_Byte(0x40,OLED_CMD); //设置显示开始行 [5:0],行数.
													    
	OLED_WR_Byte(0x8D,OLED_CMD); //电荷泵设置
	OLED_WR_Byte(0x14,OLED_CMD); //bit2，开启/关闭
	
	OLED_WR_Byte(0x20,OLED_CMD); //设置内存地址模式
	OLED_WR_Byte(0x02,OLED_CMD); //[1:0],00，列地址模式;01，行地址模式;10,页地址模式;默认10;

	//OLED_WR_Byte(0x21,OLED_CMD); //设置页面地址
	//OLED_WR_Byte(0x00,OLED_CMD); 
	
	OLED_WR_Byte(0xA1,OLED_CMD); //段重定义设置,bit0:0,0->0;1,0->127;
	OLED_WR_Byte(0xC0,OLED_CMD); //设置COM扫描方向;bit3:0,普通模式;1,重定义模式 COM[N-1]->COM0;N:驱动路数
	
	OLED_WR_Byte(0xDA,OLED_CMD); //设置COM硬件引脚配置
	OLED_WR_Byte(0x02,OLED_CMD); //[5:4]配置
		 
	OLED_WR_Byte(0x81,OLED_CMD); //对比度设置
	OLED_WR_Byte(0xEF,OLED_CMD); //1~255;默认0X7F (亮度设置,越大越亮)
	
	OLED_WR_Byte(0xD9,OLED_CMD); //设置预充电周期
	OLED_WR_Byte(0xf1,OLED_CMD); //[3:0],PHASE 1;[7:4],PHASE 2;
	
	OLED_WR_Byte(0xDB,OLED_CMD); //设置VCOMH 电压倍率
	OLED_WR_Byte(0x40,OLED_CMD); //[6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;

	OLED_WR_Byte(0xA4,OLED_CMD); //全局显示开启;bit0:1,开启;0,关闭;(白屏/黑屏)
	OLED_WR_Byte(0xA6,OLED_CMD); //设置显示方式;bit0:1,反相显示;0,正常显示	    						   

	OLED_Clear();
	OLED_WR_Byte(0xAF,OLED_CMD); //开启显示	
#endif	
}  
#endif

