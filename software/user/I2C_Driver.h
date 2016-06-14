/*
********************************************************************************
*
*                                 I2C_Driver.h
*
* File          : I2C_Driver.h
* Version       : V1.0
* Author        : Demon
* Mode          : Thumb2
* Toolchain     : 
* Description   : I2C驱动头文件
*                
* History       :
* Date          : 2016.04.19
*******************************************************************************/

#ifndef __I2C_DRIVER_H_
	#define __I2C_DRIVER_H_

//#include <stdint.h>

#define I2C_1_EN                        1   
#define I2C_2_EN                        0
#define I2C_3_EN                        0

#if !(I2C_1_EN || I2C_2_EN ||I2C_3_EN) 
#error  "请至少打开一路I2C"
#endif

typedef enum {
#if I2C_1_EN
    I2C_1,
#endif
#if I2C_2_EN
    I2C_2,
#endif
#if I2C_3_EN
    I2C_3,
#endif
    I2C_MAX
}I2C_ENUM;

#define I2Cn                            I2C_MAX
#define I2C_1_1                         //无映射:I2C_1_0，映射1:I2C_1_1
#define I2C_2_0
#define I2C_3_0                         //无映射:I2C_3_0，映射1:I2C_3_1

#define I2C_SLAVE_ADDRESS                   0x30        //本STM32芯片地址


/******************************类型声明****************************************/

typedef struct {
    volatile uint8_t idle;    //空闲标志  0)空闲   1)忙碌
    uint8_t r_w;              //读写标志  0)写     1)读
    uint8_t id;               //从机设备ID号
    uint8_t addr;             //要读写的地址
    volatile uint16_t index;  //当前缓冲区数据长度
    uint16_t bufLen;          //要发送或接收的数据长度   
    uint8_t * volatile pBuff; //缓冲区首地址    
    void (* volatile FunCallBack)(uint8_t, void *);//中断回调函数
}I2C_PARAM_TYPE;

/******************************************************************************/


/**
  * @brief  I2C1 Interface pins
  */  
#define BSP_I2C1                            I2C1
#define BSP_I2C1_CLK                        RCC_APB1Periph_I2C1
#define BSP_I2C1_AF_Port                    GPIO_AF_I2C1
#define BSP_I2C1_IRQn                       I2C1_EV_IRQn

#if defined(I2C_1_2)                        //自由组合区
#define BSP_I2C1_SCL_AF_Source              GPIO_PinSource8
#define BSP_I2C1_SCL_PIN                    GPIO_Pin_8
#define BSP_I2C1_SCL_GPIO_PORT              GPIOB 
#define BSP_I2C1_SCL_GPIO_CLK               RCC_AHB1Periph_GPIOB

#define BSP_I2C1_SDA_AF_Source              GPIO_PinSource9
#define BSP_I2C1_SDA_PIN                    GPIO_Pin_9
#define BSP_I2C1_SDA_GPIO_PORT              GPIOB
#define BSP_I2C1_SDA_GPIO_CLK               RCC_AHB1Periph_GPIOB

#elif defined(I2C_1_1)
#define BSP_I2C1_SCL_AF_Source              GPIO_PinSource8
#define BSP_I2C1_SCL_PIN                    GPIO_Pin_8
#define BSP_I2C1_SCL_GPIO_PORT              GPIOB 
#define BSP_I2C1_SCL_GPIO_CLK               RCC_AHB1Periph_GPIOB

#define BSP_I2C1_SDA_AF_Source              GPIO_PinSource9
#define BSP_I2C1_SDA_PIN                    GPIO_Pin_9
#define BSP_I2C1_SDA_GPIO_PORT              GPIOB
#define BSP_I2C1_SDA_GPIO_CLK               RCC_AHB1Periph_GPIOB

#else
#define BSP_I2C1_SCL_AF_Source              GPIO_PinSource6
#define BSP_I2C1_SCL_PIN                    GPIO_Pin_6
#define BSP_I2C1_SCL_GPIO_PORT              GPIOB 
#define BSP_I2C1_SCL_GPIO_CLK               RCC_AHB1Periph_GPIOB

#define BSP_I2C1_SDA_AF_Source              GPIO_PinSource7
#define BSP_I2C1_SDA_PIN                    GPIO_Pin_7
#define BSP_I2C1_SDA_GPIO_PORT              GPIOB
#define BSP_I2C1_SDA_GPIO_CLK               RCC_AHB1Periph_GPIOB
#endif

/**
  * @brief  I2C2 Interface pins
  */  
#define BSP_I2C2                            I2C2
#define BSP_I2C2_CLK                        RCC_APB1Periph_I2C2
#define BSP_I2C2_AF_Port                    GPIO_AF_I2C2
#define BSP_I2C2_IRQn                       I2C2_EV_IRQn

#if defined(I2C_2_3)                        //自由组合区
#define BSP_I2C2_SCL_AF_Source              GPIO_PinSource4
#define BSP_I2C2_SCL_PIN                    GPIO_Pin_4
#define BSP_I2C2_SCL_GPIO_PORT              GPIOH
#define BSP_I2C2_SCL_GPIO_CLK               RCC_AHB1Periph_GPIOH

#define BSP_I2C2_SDA_AF_Source              GPIO_PinSource5
#define BSP_I2C2_SDA_PIN                    GPIO_Pin_5
#define BSP_I2C2_SDA_GPIO_PORT              GPIOH
#define BSP_I2C2_SDA_GPIO_CLK               RCC_AHB1Periph_GPIOH

#elif defined(I2C_2_2)
#define BSP_I2C2_SCL_AF_Source              GPIO_PinSource4
#define BSP_I2C2_SCL_PIN                    GPIO_Pin_4
#define BSP_I2C2_SCL_GPIO_PORT              GPIOH
#define BSP_I2C2_SCL_GPIO_CLK               RCC_AHB1Periph_GPIOH

#define BSP_I2C2_SDA_AF_Source              GPIO_PinSource5
#define BSP_I2C2_SDA_PIN                    GPIO_Pin_5
#define BSP_I2C2_SDA_GPIO_PORT              GPIOH
#define BSP_I2C2_SDA_GPIO_CLK               RCC_AHB1Periph_GPIOH

#elif defined(I2C_2_1)
#define BSP_I2C2_SCL_AF_Source              GPIO_PinSource1
#define BSP_I2C2_SCL_PIN                    GPIO_Pin_1
#define BSP_I2C2_SCL_GPIO_PORT              GPIOF
#define BSP_I2C2_SCL_GPIO_CLK               RCC_AHB1Periph_GPIOF

#define BSP_I2C2_SDA_AF_Source              GPIO_PinSource0
#define BSP_I2C2_SDA_PIN                    GPIO_Pin_0
#define BSP_I2C2_SDA_GPIO_PORT              GPIOF
#define BSP_I2C2_SDA_GPIO_CLK               RCC_AHB1Periph_GPIOF

#else
#define BSP_I2C2_SCL_AF_Source              GPIO_PinSource10
#define BSP_I2C2_SCL_PIN                    GPIO_Pin_10
#define BSP_I2C2_SCL_GPIO_PORT              GPIOB 
#define BSP_I2C2_SCL_GPIO_CLK               RCC_AHB1Periph_GPIOB

#define BSP_I2C2_SDA_AF_Source              GPIO_PinSource11
#define BSP_I2C2_SDA_PIN                    GPIO_Pin_11
#define BSP_I2C2_SDA_GPIO_PORT              GPIOB
#define BSP_I2C2_SDA_GPIO_CLK               RCC_AHB1Periph_GPIOB
#endif

/**
  * @brief  I2C3 Interface pins
  */  
#define BSP_I2C3                            I2C3
#define BSP_I2C3_CLK                        RCC_APB1Periph_I2C3
#define BSP_I2C3_AF_Port                    GPIO_AF_I2C3
#define BSP_I2C3_IRQn                       I2C3_EV_IRQn

#if defined(I2C_3_2)                        //自由组合区
#define BSP_I2C3_SCL_AF_Source              GPIO_PinSource8
#define BSP_I2C3_SCL_PIN                    GPIO_Pin_8
#define BSP_I2C3_SCL_GPIO_PORT              GPIOA
#define BSP_I2C3_SCL_GPIO_CLK               RCC_AHB1Periph_GPIOA

#define BSP_I2C3_SDA_AF_Source              GPIO_PinSource8
#define BSP_I2C3_SDA_PIN                    GPIO_Pin_8
#define BSP_I2C3_SDA_GPIO_PORT              GPIOH
#define BSP_I2C3_SDA_GPIO_CLK               RCC_AHB1Periph_GPIOH

#elif defined(I2C_3_1)
#define BSP_I2C3_SCL_AF_Source              GPIO_PinSource8
#define BSP_I2C3_SCL_PIN                    GPIO_Pin_8
#define BSP_I2C3_SCL_GPIO_PORT              GPIOA
#define BSP_I2C3_SCL_GPIO_CLK               RCC_AHB1Periph_GPIOA

#define BSP_I2C3_SDA_AF_Source              GPIO_PinSource9
#define BSP_I2C3_SDA_PIN                    GPIO_Pin_9
#define BSP_I2C3_SDA_GPIO_PORT              GPIOC
#define BSP_I2C3_SDA_GPIO_CLK               RCC_AHB1Periph_GPIOC

#else
#define BSP_I2C3_SCL_AF_Source              GPIO_PinSource7
#define BSP_I2C3_SCL_PIN                    GPIO_Pin_7
#define BSP_I2C3_SCL_GPIO_PORT              GPIOH
#define BSP_I2C3_SCL_GPIO_CLK               RCC_AHB1Periph_GPIOH

#define BSP_I2C3_SDA_AF_Source              GPIO_PinSource8
#define BSP_I2C3_SDA_PIN                    GPIO_Pin_8
#define BSP_I2C3_SDA_GPIO_PORT              GPIOH
#define BSP_I2C3_SDA_GPIO_CLK               RCC_AHB1Periph_GPIOH
#endif



/******************************函数声明****************************************/
void BSP_I2cOpen(uint8_t I2C_x, uint32_t clockSpeed, uint16_t mode);
void BSP_I2cClose(uint8_t I2C_x);
uint32_t BSP_I2cWrite(uint8_t I2C_x, uint8_t *buff, uint16_t i2cSaleAddress, uint8_t writeAddress, uint16_t writeLen);
uint32_t BSP_I2cRead(uint8_t I2C_x, uint8_t *buff, uint16_t i2cSaleAddress, uint8_t readAddress, uint16_t readLen);
uint32_t BSP_I2cIdleState(uint8_t I2C_x);



#endif
