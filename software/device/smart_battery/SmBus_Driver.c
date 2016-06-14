#include "includes.h"
#include "I2C_SoftwareDriver.h"
/*
��������, �����ο����������Ҫ��һ��
����Ҳ��һЩ����
http://www.freedocuments.info/376948777/
�ʼǱ����оƬ������TI��BQ���� (BQxxxx)
���ַ����0x0B
Ȼ����Щ������и���׼��, ����SBS v1.1 (Smart Battery System), ͨ��ʹ��SMBus, �����I2C���е㲻һ��.
����Կ����������пɶ����Ĳ�������
������Ҫ����Ȥ����
*/

// standard I2C address for Smart Battery packs
static uint8_t deviceAddress = 11; //0x0B

#define IIC_WRITE		0xfffe
#define IIC_READ		0x0001

/**********************************************************************
* FuntionName : SMB_Init
* Description : smbus init
* Input       : None
* Output      : None
* Return      : None
***********************************************************************/
void SMB_Init(void)
{
#ifdef INCLUDE_IIC_DRIVER
	IIC_DriverInit();
#else
	BSP_I2cOpen(I2C_1, 100000, I2C_Mode_SMBusHost);
#endif
}

#ifndef INCLUDE_IIC_DRIVER
/**********************************************************************
* FuntionName : SMB_Read
* Description : smbus read data with addr
* Input       : addr (register address of slaveDevice)
* Output      : None
* Return      : None
***********************************************************************/
uint8_t SMB_Read(uint8_t addr) 
{
	uint8_t dat[8];
	BSP_I2cRead(I2C_1, dat, deviceAddress<<1, addr, 2);
	while (BSP_I2cIdleState(I2C_1))
	{
	}
	dat[7] = 0;
	printf("0x%x", dat[1]);
	return (dat[0]);
}

#else
/**********************************************************************
* FuntionName : SMB_ReadByte
* Description : smbus read one byte with command
* Input       : cmd  command
* Output      : None
* Return      : read data
***********************************************************************/
uint8_t SMB_ReadByte(uint8_t cmd)
{
	uint8_t dat;

	IIC_Start();

	if(IIC_SendByte((deviceAddress<<1) & IIC_WRITE))
	{
	   IIC_Stop();
	   return 0;
	}
	else
	{
		if(IIC_SendByte(cmd))
		{
		   IIC_Stop();
		   return 0;
		}
	}
	
	IIC_Start();
	IIC_SendByte((deviceAddress<<1) | IIC_READ);

	dat = IIC_ReadByte(1); // read & send noack
	
	return dat;
}

/**********************************************************************
* FuntionName : SMB_Read2Byte
* Description : smbus read two byte with command
* Input       : command
* Output      : None
* Return      : data
***********************************************************************/
uint16_t SMB_Read2Byte(uint8_t cmd)
{
	uint8_t dat[2];
	uint16_t temp;
#if 1
	IIC_ReadDevice((deviceAddress<<1), &cmd, 1, dat, 2);

#else
	IIC_Start();

	if(IIC_SendByte((deviceAddress<<1) & IIC_WRITE))
	{
	   IIC_Stop();
	   printf("noack\r\n");
	   return 0;
	}
	else
	{
		if(IIC_SendByte(cmd))
		{
		   printf("noack1\r\n");
		   IIC_Stop();
		   return 0;
		}
	}
	//IIC_Stop();
	
	IIC_Start();
	
	if (IIC_SendByte((deviceAddress<<1) | IIC_READ))
	{
		printf("noack2\r\n");
		IIC_Stop();
		return 0;
	}

	dat[0] = IIC_ReadByte(0); // read & send ack
	dat[1] = IIC_ReadByte(1); // read & send noack

	IIC_Stop();
#endif

	temp = (uint16_t)(dat[1]<<8)|dat[0];
	
	return temp;
	
}

/**********************************************************************
* FuntionName : SMB_ReaadString
* Description : smbus read string
* Input       : command
* Output      : string
* Return      : None
***********************************************************************/
uint8_t SMB_ReadString(uint8_t cmd, uint8_t *buf)
{
	
	unsigned char i;
	uint8_t datLen;
	
	IIC_Start();

	if(IIC_SendByte((deviceAddress<<1) & IIC_WRITE))
	{
	   IIC_Stop();
	   return 0;
	}
	else
	{
		if (IIC_SendByte(cmd))
		{
			IIC_Stop();
	   		return 0;
		}
		else
		{
			IIC_Start();
			if (IIC_SendByte((deviceAddress<<1) | IIC_READ))	// ���Ͷ���ַ
			{
				IIC_Stop();
				return 0;
			}
			else
			{
				datLen = IIC_ReadByte(0); // read & send ACK
			}
		}
	}
	
	for(i=0; i<datLen-1; i++)
	{
		*buf++ =  IIC_ReadByte(0); // read & send ACK
	}

	*buf = IIC_ReadByte(1); // read & send noack

	IIC_Stop();

	return (datLen);
}

#endif
