#include "includes.h"

#ifdef INCLUDE_IIC_DRIVER
#if 0
#define I2C_SDA_GPIO		GPIOB
#define I2C_SDA_PIN			GPIO_Pin_9
#define I2C_SCL_GPIO        GPIOB
#define I2C_SCL_PIN         GPIO_Pin_8

//ʹ��IIC���нӿ�ʱʹ��
//#define IIC_SDA_HIGH() GPIO_WriteBit(I2C_SDA_GPIO, I2C_SDA_PIN, Bit_SET)
//#define IIC_SDA_LOW()  GPIO_WriteBit(I2C_SDA_GPIO, I2C_SDA_PIN, Bit_RESET)
#define IIC_SCL_HIGH() GPIO_WriteBit(I2C_SCL_GPIO, I2C_SCL_PIN, Bit_SET)
#define IIC_SCL_LOW()  GPIO_WriteBit(I2C_SCL_GPIO, I2C_SCL_PIN, Bit_RESET)

//#define IIC_SDA_READ() GPIO_ReadInputDataBit(I2C_SDA_GPIO, I2C_SDA_PIN)

static const u8 ACK   	=0;
static const u8 READY 	=0;
static const u8 NACK 	        =1;
static const u8 BUS_BUSY  	=2;
static const u8 BUS_ERROR 	=3;
//static const u8 TWI_RETRY_COUNT = 3; //���Դ���


void IIC_SDA_HIGH(void)//��ʱΪ��������������
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;	
	GPIO_InitStructure.GPIO_Pin = I2C_SDA_PIN;
	GPIO_Init(I2C_SDA_GPIO, &GPIO_InitStructure);
	GPIO_WriteBit(I2C_SDA_GPIO, I2C_SDA_PIN, Bit_SET);
}

void IIC_SDA_LOW(void)//��ʱΪ��������������

{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Pin = I2C_SDA_PIN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_Init(I2C_SDA_GPIO, &GPIO_InitStructure);
	GPIO_WriteBit(I2C_SDA_GPIO, I2C_SDA_PIN, Bit_RESET);
} 

uint8_t IIC_SDA_READ()//��ʱΪ�������������ݻ�ȴ�����������Ӧ���ź�
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Pin = I2C_SDA_PIN;
	GPIO_Init(I2C_SDA_GPIO, &GPIO_InitStructure);

	return GPIO_ReadInputDataBit(I2C_SDA_GPIO, I2C_SDA_PIN);
}


void IIC_DriverInit(void)
{
	// IO ����
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin   = I2C_SCL_PIN;
	GPIO_Init(I2C_SCL_GPIO, &GPIO_InitStructure);

	//GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	//GPIO_InitStructure.GPIO_Pin = I2C_SDA_PIN;
	//GPIO_Init(I2C_SDA_GPIO, &GPIO_InitStructure);
}

/*******************************************************************************
 * ��������:_NOP                                                                     
 * ��    ��:��ʱ����                                                                     
 *                                                                               
 * ��    ��:��                                                                     
 * ��    ��:��                                                                     
 * ��    ��:��                                                                     
 * ��    ��:                                                                                                                                        
 *******************************************************************************/
static void _NOP(void)
{
	uint16_t i;
	uint16_t j;
	//uint16_t sum = 0;

	i = 30;
	while(i--)
	{
		for (j=0; j<10; j++)
		{
			//sum += i; 
		}
	}
	//sum = i;
}


/*******************************************************************************
 * ��������:IIC_START                                                                     
 * ��    ��:��������                                                                     
 *                                                                               
 * ��    ��:��                                                                     
 * ��    ��:��                                                                     
 * ��    ��:����״̬                                                                     
 * ��    ��:                                                                                                                                        
 *******************************************************************************/
uint8_t IIC_Start(void)
{ 
	IIC_SDA_HIGH(); 
	_NOP();

	IIC_SCL_HIGH(); 
	_NOP();    

	if(!IIC_SDA_READ())
	{
		////DebugPrint("TWI_START:BUSY\n");
		return BUS_BUSY;
	}
	IIC_SDA_LOW();
	_NOP();

	IIC_SCL_LOW();  
	_NOP(); 

	if(IIC_SDA_READ())
	{
		////DebugPrint("TWI_START:BUS ERROR\n");
		return BUS_ERROR;
	} 

	return READY;
}

/* --------------------------------------------------------------------------*/
/** 
 * @Brief:  TWI_START_SHT 
 * 
 * @Returns:   
 */
/* --------------------------------------------------------------------------*/
uint8_t IIC_START_SHT(void)
{
    IIC_SDA_HIGH();
    IIC_SCL_LOW();
    _NOP();

    IIC_SDA_HIGH(); 
    IIC_SCL_HIGH(); 
    _NOP();

    if(!IIC_SDA_READ())
    {
	    return BUS_BUSY;
    }
    IIC_SDA_LOW();
    _NOP();

    IIC_SCL_LOW();  
    _NOP(); 

    IIC_SCL_HIGH();
    _NOP();

    IIC_SDA_HIGH();
    _NOP();

    IIC_SCL_LOW();
    _NOP();

    return READY;
}

/*******************************************************************************
 * ��������:IIC_Stop                                                                     
 * ��    ��:���ͽ���                                                                     
 *                                                                               
 * ��    ��:��                                                                     
 * ��    ��:��                                                                     
 * ��    ��:��                                                                     
 * ��    ��:                                                                                                                                        
 *******************************************************************************/
void IIC_Stop(void)
{
	IIC_SDA_LOW(); 
	_NOP();

	IIC_SCL_HIGH(); 
	_NOP();    

	IIC_SDA_HIGH();
	_NOP();
  
 //////DebugPrint("TWI_STOP\n");   
}

/* --------------------------------------------------------------------------*/
/** 
 * @Brief:  IIC_SendACK 
 */
/* --------------------------------------------------------------------------*/
void IIC_SendACK(void)
{
	IIC_SDA_LOW();
	_NOP();
	IIC_SCL_HIGH();
	_NOP();
	IIC_SCL_LOW(); 
	_NOP(); 
	IIC_SDA_HIGH();
 //////DebugPrint("TWI_SendACK\n");   
}

/* --------------------------------------------------------------------------*/
/** 
 * @Brief:  IIC_SendNACK 
 */
/* --------------------------------------------------------------------------*/
void IIC_SendNACK(void)
{
	IIC_SDA_HIGH();
	_NOP();
	IIC_SCL_HIGH();
	_NOP();
	IIC_SCL_LOW(); 
	_NOP();
 //////DebugPrint("IIC_SendNACK\n");    
}

/*******************************************************************************
 * �������� : IIC_SendByte                                                                     
 * ��    �� : ��������                                                                     
 *                                                                               
 * ��    �� : ��                                                                     
 * ��    �� : ��                                                                     
 * ��    �� : ACK                                                                                                                                                                                                         
 *******************************************************************************/
uint8_t IIC_SendByte(uint8_t Data)
{
	uint8_t i;
	
	IIC_SCL_LOW();
	for(i=0;i<8;i++)
	{  
		//---------���ݽ���----------
		if(Data&0x80)
		{
			IIC_SDA_HIGH();
		}
		else
		{
			IIC_SDA_LOW();
		} 
		Data<<=1;
		_NOP();
		//---���ݽ�������һ����ʱ----

		//----����һ��������[������] 
		IIC_SCL_HIGH();
		_NOP();
		IIC_SCL_LOW();
		_NOP();//��ʱ,��ֹSCL��û��ɵ�ʱ�ı�SDA,�Ӷ�����START/STOP�ź�
		//---------------------------   
	}
	//���մӻ���Ӧ�� 
	IIC_SDA_HIGH(); 
	_NOP();
	IIC_SCL_HIGH();
	_NOP();  
#if 0
	while (IIC_SDA_READ())
	{}
	printf("TWI_ACK!\n");
        return ACK;
#else
	if(IIC_SDA_READ())
		
	{
		IIC_SCL_LOW();
		IIC_SDA_HIGH();
		printf("TWI_NACK!\n");
		return NACK;
	}
	else
	{
		IIC_SCL_LOW();
		IIC_SDA_HIGH();
		printf("TWI_ACK!\n");
		return ACK;  
	} 
#endif	
}

/*******************************************************************************
 * �������� : IIC_ReceiveByte                                                                     
 * ��    �� : ��������                                                                     
 *                                                                               
 * ��    �� : ��                                                                     
 * ��    �� : ��                                                                     
 * ��    �� : ���� dat                                                                                                                                                                                                         
 *******************************************************************************/
uint8_t IIC_ReceiveByte(void)
{
	uint8_t i;
	uint8_t Dat;
	
	IIC_SDA_HIGH();
	IIC_SCL_LOW(); 
	Dat=0;
	for(i=0;i<8;i++)
	{
		IIC_SCL_HIGH();//����ʱ��������[������],�ôӻ�׼�������� 
		_NOP(); 
		Dat<<=1;
		if(IIC_SDA_READ()) //������״̬
		{
			Dat|=0x01; 
		}   
		IIC_SCL_LOW();//׼�����ٴν�������  
		_NOP();//�ȴ�����׼����         
	}
	//////DebugPrint("TWI_Dat:%x\n",Dat);
	return Dat;
}
#else

void delay_us(unsigned char us)   //10us��ʱ
{	
#if 0
	
#else
	uint16_t j;
	uint16_t i;

	i = us*20;
	
	while(i--)
	{
		for (j=0; j<12; j++)
		{
		}
	}
#endif	
}

#define IIC_SDA0    GPIOB->BSRRH=GPIO_Pin_9   //sda=0
#define IIC_SDA1    GPIOB->BSRRL=GPIO_Pin_9   //sda=1
#define IIC_SCL0    GPIOB->BSRRH=GPIO_Pin_8
#define IIC_SCL1    GPIOB->BSRRL=GPIO_Pin_8

#define SDA_IN()	GPIOB->MODER|=GPIO_Mode_IN<<18  //sda����
#define SDA_OUT()   GPIOB->MODER|=GPIO_Mode_OUT<<18//sda���

#define IN            1
#define OUT           0


#define  SetIicScl(x) do{if(x){IIC_SCL1;}\
						 else {IIC_SCL0;}\
						 	}while(0)

#define  SetIicSda(x) do{if(x){IIC_SDA1;}\
						 else {IIC_SDA0;}\
						 	}while(0)
						
//����SDA IO�ڷ���
#if 1
#define  SetIicSdaDir(x) do{if(x){SDA_IN();}\
	                        else {SDA_OUT();}\
								}while(0)
#else
void SetIicSdaDir(uint8_t flag)
{
	GPIO_InitTypeDef  GPIO_InitStructure;//GPIO��ʼ���ṹ��
  	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//����ʱ��ʹ��
  	
	if (IN == flag)
	{
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	}
	else
	{  	
	  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//״̬Ϊ���
		GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;//
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//��������	
	}
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;//��ӦGPIO����
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//IO�ٶ�Ϊ100MHZ
	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIO
	SoftDelay(10);
}

#endif
#define  SoftDelay(i)  delay_us(i)
#define  ReadIicSda()  ((GPIOB->IDR&GPIO_Pin_9)!=0)?1:0  //�жϽ��յ���bit

/***********************************************************************

 * Name: ReadIicSda()

 * Note: Read SDA state;

 * Para: None

 * Return : SDA state

*/
void IIC_DriverInit(void)
{	
  	GPIO_InitTypeDef  GPIO_InitStructure;//GPIO��ʼ���ṹ��
  	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//����ʱ��ʹ��
  	
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 |GPIO_Pin_9;//��ӦGPIO����
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//״̬Ϊ���
  	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//IO�ٶ�Ϊ100MHZ
  	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//��������
  	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIO
  	
	IIC_SDA1;
	IIC_SCL1;				     
}

/***********************************************************************

 * Name: IIC_Start()

 * Note: Start I2C Bus, SDA change to low when SCL is hight

 * Para: None

 * Return : None

*/
void IIC_Start(void)
{
	SetIicSda(1);
  	SetIicScl(1);
	SoftDelay(1);
	
	SetIicSda(0);
	SoftDelay(0);
	
	SetIicScl(0);

}

/***********************************************************************

 * Name: IIC_Stop()

 * Note: Stop I2C Bus, SCL change to hight when SDA is hight

 * Para: None

 * Return : None

*/
void IIC_Stop(void)
{
	SetIicScl(1);
	
	SetIicSda(0);
	SoftDelay(1);
	
	SetIicSda(1);
}

/***********************************************************************

 * Name: IIC_SendByte(dat)

 * Note: Write 8bite data and get ack;

 * Para: dat -> the data which will be send out

 * Return : ack -> ack signal

*/
unsigned char IIC_SendByte(unsigned char dat)
{
    unsigned char i;
	unsigned char ack;

	for(i=0; i<8; i++)
	{
	    if(dat & 0x80)
		{
		    SetIicSda(1);
		}
		else
		{

		    SetIicSda(0);
		}
		SoftDelay(1);
		
		SetIicScl(1);
		SoftDelay(1);
		dat <<= 1;
		SetIicScl(0);
	}

	SetIicSdaDir(IN);
	SoftDelay(1);
	
	SetIicScl(1);
	SoftDelay(10);	// ����smbus ʱ������Ҫ��
	ack = ReadIicSda();
	SetIicScl(0);
	
	SetIicSdaDir(OUT);
	
	return ack;

}




/***********************************************************************

 * Name: IIC_ReadByte

 * Note: Read 8bite data and Send  ack;

 * Para: ack=0 -> Set ack, ack=1 -> Set noack

 * Return : read data

*/
unsigned char IIC_ReadByte(unsigned char ack)
{
    unsigned char i;
	unsigned char dat=0;

	SetIicSdaDir(IN);
	
	//SetIicScl(0);
	for(i=0; i<8; i++)
	{
	    dat <<= 1;
		SetIicScl(1);
		SoftDelay(1);
		if(ReadIicSda())
		{
		    dat |= 1;
		}
		SetIicScl(0);
		SoftDelay(2);
	}
	
	SetIicSdaDir(OUT);

	SetIicSda(ack);	   // ack = 0; ask, ack = 1,stop
	SoftDelay(1);
	
	SetIicScl(1);
	SoftDelay(1);
	SetIicScl(0);

	return dat;
}




/***********************************************************************

 * Name: IicWriteDevice

 * Note: Write data to device

 * Para: device -> device address, buf->subaddress + data

 * Return : how many bytes have been write

*/

unsigned char IIC_WriteDevice(unsigned char device, unsigned char *buf, unsigned char cnt)
{

    unsigned char i;

    IIC_Start();

	if(IIC_SendByte(device))
	{
	    IIC_Stop();
		return 0;
	}

	for(i=0; i<cnt; i++)
	{
	    if(IIC_SendByte(*buf++))
	    {
	        IIC_Stop();
			break;
	    }
    }

	IIC_Stop();

	return i;

}




/***********************************************************************

 * Name: IIC_ReadDevice

 * Note: Read data from device

 * Para: device->device address, subaddr->subaddress, acnt->subaddress lengh

 *       buf->read out data space, bcnt->read out data lengh

 * Return : write subaddress lengh

*/

unsigned char IIC_ReadDevice(unsigned char device, unsigned char *subaddr, unsigned char acnt, unsigned char *buf, unsigned char bcnt)
{
    unsigned char i;
	unsigned char wlen;

    IIC_Start();

	if(IIC_SendByte(device))
	{
		printf("ok2\r\n");
	    IIC_Stop();
	    return 0;
	}

	for (i=0; i<acnt; i++)
	{
		if(IIC_SendByte(*subaddr++))
		{
			printf("ok1\r\n");
			IIC_Stop();

			return i;
		}
	}
	wlen = i;
	
	IIC_Start();
	if (IIC_SendByte(device+0x01))
	{
		printf("error\r\n");
	}

	for(i=0; i<bcnt-1; i++)
	{
	    *buf++ =  IIC_ReadByte(0); // read & send ACK
	}

	*buf = IIC_ReadByte(1); // read & send noack

	IIC_Stop();
	printf("ok\r\n");
	return wlen;

}

#endif
#endif
