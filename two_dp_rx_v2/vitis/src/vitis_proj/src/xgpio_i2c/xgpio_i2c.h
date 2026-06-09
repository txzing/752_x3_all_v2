/**
 *******************************************************************************
 *
 * Copyright (C) 2022 - 2027  XXX Company. All rights reserved.
 *
 * @file xgpio_i2c.h
 * @brief
 *
 * \n
 * @details
 *
 * \n
 * @version 
 * @author Andreas Zhang
 * @date 2022-01-19
 *
 * @verbatim
 * \n
 * @history
 * Ver   Who Date     Changes
 * ----- --- -------- ---------------------------------------------
 * 1.3.0 who 02/07/11 First release
 * 
 *
 * @endverbatim
 *******************************************************************************
 */
#ifndef __XGPIO_I2C_H__

//#include "xparameters.h"
#if defined(XPAR_XGPIO_NUM_INSTANCES)
#define __XGPIO_I2C_H__
#include "xgpio.h"
#include "sleep.h"

#if defined(XPAR_XGPIO_I2C_0_AXI_GPIO_0_DEVICE_ID)
extern XGpio XGpioInstI2c;

#define XIO_I2C_NUM 	(9U)	// 1~16

#define INPUT 1
#define OUTPUT 0

typedef enum xgpio_i2c_no
{
#if (XIO_I2C_NUM >= 1U)
	I2C_NO_0 = 0,
#endif
#if (XIO_I2C_NUM >= 2U)
	I2C_NO_1,
#endif
#if (XIO_I2C_NUM >= 3U)
	I2C_NO_2,
#endif
#if (XIO_I2C_NUM >= 4U)
	I2C_NO_3,
#endif
#if (XIO_I2C_NUM >= 5U)
	I2C_NO_4,
#endif
#if (XIO_I2C_NUM >= 6U)
	I2C_NO_5,
#endif
#if (XIO_I2C_NUM >= 7U)
	I2C_NO_6,
#endif
#if (XIO_I2C_NUM >= 8U)
	I2C_NO_7,
#endif
#if (XIO_I2C_NUM >= 9U)
	I2C_NO_8 ,
#endif
#if (XIO_I2C_NUM >= 10U)
	I2C_NO_9,
#endif
#if (XIO_I2C_NUM >= 11U)
	I2C_NO_10,
#endif
#if (XIO_I2C_NUM >= 12U)
	I2C_NO_11,
#endif
#if (XIO_I2C_NUM >= 13U)
	I2C_NO_12,
#endif
#if (XIO_I2C_NUM >= 14U)
	I2C_NO_13,
#endif
#if (XIO_I2C_NUM >= 15U)
	I2C_NO_14,
#endif
#if (XIO_I2C_NUM >= 16U)
	I2C_NO_15,
#endif

	I2C_NO_BUTT,
} i2c_no;

typedef enum
{
    STRETCH_OFF = 0,
	STRETCH_ON,
} stretch_mode;

typedef enum xgpio_i2c_bank_no
{
    I2C_BANK_1 = 1,
	I2C_BANK_2,

	I2C_BANK_BUTT,
} i2c_bank_no;

typedef struct {
	i2c_no I2C_ID;
	u16 DeviceId;
	i2c_bank_no I2C_BANK;
	u32 I2C_SCL;
	u32 I2C_SDA;
} XGpio_I2C_Cfg;

#if (XIO_I2C_NUM >= 1U)
#define XGPIO_SDA0_NUM 0
#define XGPIO_SCL0_NUM 1
#endif
#if (XIO_I2C_NUM >= 2U)
#define XGPIO_SDA1_NUM 2
#define XGPIO_SCL1_NUM 3
#endif
#if (XIO_I2C_NUM >= 3U)
#define XGPIO_SDA2_NUM 4
#define XGPIO_SCL2_NUM 5
#endif
#if (XIO_I2C_NUM >= 4U)
#define XGPIO_SDA3_NUM 6
#define XGPIO_SCL3_NUM 7
#endif
#if (XIO_I2C_NUM >= 5U)
#define XGPIO_SDA4_NUM 8
#define XGPIO_SCL4_NUM 9
#endif
#if (XIO_I2C_NUM >= 6U)
#define XGPIO_SDA5_NUM 10
#define XGPIO_SCL5_NUM 11
#endif
#if (XIO_I2C_NUM >= 7U)
#define XGPIO_SDA6_NUM 12
#define XGPIO_SCL6_NUM 13
#endif
#if (XIO_I2C_NUM >= 8U)
#define XGPIO_SDA7_NUM 14
#define XGPIO_SCL7_NUM 15
#endif
#if (XIO_I2C_NUM >= 9U)
#define XGPIO_SDA8_NUM 16
#define XGPIO_SCL8_NUM 17
#endif
#if (XIO_I2C_NUM >= 10U)
#define XGPIO_SDA9_NUM 18
#define XGPIO_SCL9_NUM 19
#endif
#if (XIO_I2C_NUM >= 11U)
#define XGPIO_SDA10_NUM 20
#define XGPIO_SCL10_NUM 21
#endif
#if (XIO_I2C_NUM >= 12U)
#define XGPIO_SDA11_NUM 22
#define XGPIO_SCL11_NUM 23
#endif
#if (XIO_I2C_NUM >= 13U)
#define XGPIO_SDA12_NUM 24
#define XGPIO_SCL12_NUM 25
#endif
#if (XIO_I2C_NUM >= 14U)
#define XGPIO_SDA13_NUM 26
#define XGPIO_SCL13_NUM 27
#endif
#if (XIO_I2C_NUM >= 15U)
#define XGPIO_SDA14_NUM 28
#define XGPIO_SCL14_NUM 29
#endif
#if (XIO_I2C_NUM >= 16U)
#define XGPIO_SDA15_NUM 30
#define XGPIO_SCL15_NUM 31
#endif


int xgpio_i2c_init(void);
//void i2c_scl_pos(i2c_no i2c);
void i2c_start(i2c_no i2c);
void i2c_stop(i2c_no i2c);
void i2c_ack(i2c_no i2c);
void i2c_nack(i2c_no i2c);
void i2c_send_byte(i2c_no i2c, u8 txd);
u8  i2c_recv_byte(i2c_no i2c);
u8  i2c_recv_ack(i2c_no i2c, stretch_mode st_mode);
int xgpio_i2c_reg8_write(i2c_no i2c, char IIC_ADDR, char Addr, char Data, stretch_mode st_mode);
int xgpio_i2c_reg8_read(i2c_no i2c, char IIC_ADDR, char Addr, u8 * ret, stretch_mode st_mode);
int xgpio_i2c_reg16_write(i2c_no i2c, char IIC_ADDR, unsigned short Addr, char Data, stretch_mode st_mode);
int xgpio_i2c_reg16_read(i2c_no i2c, char IIC_ADDR, unsigned short Addr, u8 * ret, stretch_mode st_mode);

int xgpio_i2c_reg8_continuous_write(i2c_no i2c, u8 IIC_ADDR, u8 Addr, u8 *Data,u16 len, stretch_mode st_mode);
int xgpio_i2c_reg8_continuous_read(i2c_no i2c, u8 IIC_ADDR, u8 Addr, u8 * ret, u16 len, stretch_mode st_mode);
int xgpio_i2c_reg16_continuous_write(i2c_no i2c, u8 IIC_ADDR, u16 Addr, u8 *Data,u16 len, stretch_mode st_mode);
int xgpio_i2c_reg16_continuous_read(i2c_no i2c, u8 IIC_ADDR, u16 Addr, u8 * ret,u16 len, stretch_mode st_mode);


//int xgpio_i2c_32b32_write(i2c_no i2c, char IIC_ADDR, unsigned int Addr, unsigned int Data, stretch_mode st_mode);
//int xgpio_i2c_32b32_read(i2c_no i2c, char IIC_ADDR, unsigned int Addr, unsigned int * ret, stretch_mode st_mode);

#endif // XPAR_XGPIO_I2C_0_AXI_GPIO_0_DEVICE_ID


#endif // XPAR_XGPIO_NUM_INSTANCES

#endif // __XGPIO_I2C_H__

