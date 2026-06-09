/**
 *******************************************************************************
 *
 * Copyright (C) 2022 - 2027  XXX Company. All rights reserved.
 *
 * @file xgpio_i2c.c
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
 * @endverbatim
 *******************************************************************************
 */
#include "../bsp.h"

#if defined(XPAR_XGPIO_NUM_INSTANCES)

#if defined(XPAR_XGPIO_I2C_0_AXI_GPIO_0_DEVICE_ID)
//#include "xgpio_i2c.h"
//#include "sleep.h"
//#include "../bitmanip.h"

XGpio XGpioInstI2c;

//#define  XGPIO_ID  XPAR_GPIO_0_DEVICE_ID
#define  XGPIO_ID  XPAR_XGPIO_I2C_0_AXI_GPIO_0_DEVICE_ID

XGpio_I2C_Cfg XGpio_I2C_CfgTable[I2C_NO_BUTT] =
{
#if (XIO_I2C_NUM >= 1U)
	{I2C_NO_0, XGPIO_ID, I2C_BANK_1, XGPIO_SCL0_NUM, XGPIO_SDA0_NUM},
#endif
#if (XIO_I2C_NUM >= 2U)
	{I2C_NO_1, XGPIO_ID, I2C_BANK_1, XGPIO_SCL1_NUM, XGPIO_SDA1_NUM},
#endif
#if (XIO_I2C_NUM >= 3U)
	{I2C_NO_2, XGPIO_ID, I2C_BANK_1, XGPIO_SCL2_NUM, XGPIO_SDA2_NUM},
#endif
#if (XIO_I2C_NUM >= 4U)
	{I2C_NO_3, XGPIO_ID, I2C_BANK_1, XGPIO_SCL3_NUM, XGPIO_SDA3_NUM},
#endif
#if (XIO_I2C_NUM >= 5U)
	{I2C_NO_4, XGPIO_ID, I2C_BANK_1, XGPIO_SCL4_NUM, XGPIO_SDA4_NUM},
#endif
#if (XIO_I2C_NUM >= 6U)
	{I2C_NO_5, XGPIO_ID, I2C_BANK_1, XGPIO_SCL5_NUM, XGPIO_SDA5_NUM},
#endif
#if (XIO_I2C_NUM >= 7U)
	{I2C_NO_6, XGPIO_ID, I2C_BANK_1, XGPIO_SCL6_NUM, XGPIO_SDA6_NUM},
#endif
#if (XIO_I2C_NUM >= 8U)
	{I2C_NO_7, XGPIO_ID, I2C_BANK_1, XGPIO_SCL7_NUM, XGPIO_SDA7_NUM},
#endif
#if (XIO_I2C_NUM >= 9U)
	{I2C_NO_8, XGPIO_ID, I2C_BANK_1, XGPIO_SCL8_NUM, XGPIO_SDA8_NUM},
#endif
#if (XIO_I2C_NUM >= 10U)
	{I2C_NO_9, XGPIO_ID, I2C_BANK_1, XGPIO_SCL9_NUM, XGPIO_SDA9_NUM},
#endif
#if (XIO_I2C_NUM >= 11U)
	{I2C_NO_10, XGPIO_ID, I2C_BANK_1, XGPIO_SCL10_NUM, XGPIO_SDA10_NUM},
#endif
#if (XIO_I2C_NUM >= 12U)
	{I2C_NO_11, XGPIO_ID, I2C_BANK_1, XGPIO_SCL11_NUM, XGPIO_SDA11_NUM},
#endif
#if (XIO_I2C_NUM >= 13U)
	{I2C_NO_12, XGPIO_ID, I2C_BANK_1, XGPIO_SCL12_NUM, XGPIO_SDA12_NUM},
#endif
#if (XIO_I2C_NUM >= 14U)
	{I2C_NO_13, XGPIO_ID, I2C_BANK_1, XGPIO_SCL13_NUM, XGPIO_SDA13_NUM},
#endif
#if (XIO_I2C_NUM >= 15U)
	{I2C_NO_14, XGPIO_ID, I2C_BANK_1, XGPIO_SCL14_NUM, XGPIO_SDA14_NUM},
#endif
#if (XIO_I2C_NUM >= 16U)
	{I2C_NO_15, XGPIO_ID, I2C_BANK_1, XGPIO_SCL15_NUM, XGPIO_SDA15_NUM},
#endif
};

//XGPIO初始化
int xgpio_i2c_init(void)
{
	int Status ;

//	u32 ret;

	Status = XGpio_Initialize(&XGpioInstI2c, XGPIO_ID) ;
	if (Status != XST_SUCCESS)
	{
		bsp_printf(TXT_RED "In %s: XGpio_Initialize failed...\r\n" TXT_RST, __func__);
		return XST_FAILURE ;
	}

	/* set as output */
	XGpio_SetDataDirection(&XGpioInstI2c, 1, 0x0);
	XGpio_DiscreteWrite(&XGpioInstI2c, 1, 0xffffffff);
	usleep(2*1000);
	/* set as input */
	XGpio_SetDataDirection(&XGpioInstI2c, 1,0xffffffff);


	return XST_SUCCESS ;
}

//产生起始信号
void i2c_start(i2c_no i2c)
{
	u32 sda;
	u32 scl;
	u32 ret;
	sda=XGpio_I2C_CfgTable[i2c].I2C_SDA;
	scl=XGpio_I2C_CfgTable[i2c].I2C_SCL;


	ret=XGpio_GetDataDirection(&XGpioInstI2c, 1);
	CLB(ret,BIT32(sda));
	CLB(ret,BIT32(scl));
	XGpio_SetDataDirection(&XGpioInstI2c, 1, ret);
	//设置 gpio端口 为输出
	//XGpio_SetDataDirection(&XGpioInstI2c, 1, (OUTPUT<<sda|OUTPUT<<scl));

	// scl输出高， sda输出高
	ret = XGpio_DiscreteRead(&XGpioInstI2c, 1);
	STB(ret, BIT32(sda));
	STB(ret, BIT32(scl));
	XGpio_DiscreteWrite(&XGpioInstI2c, 1, ret);
	//XGpio_DiscreteWrite(&XGpioInstI2c, 1, (1<<sda|1<<scl));

	usleep(4);

	ret = XGpio_DiscreteRead(&XGpioInstI2c, 1);
	CLB(ret, BIT32(sda));
	STB(ret, BIT32(scl));
	XGpio_DiscreteWrite(&XGpioInstI2c, 1, ret);
	//XGpio_DiscreteWrite(&XGpioInstI2c, 1, (0<<sda|1<<scl));//START:when CLK is high,DATA change form high to low

 	usleep(4);

	ret = XGpio_DiscreteRead(&XGpioInstI2c, 1);
	CLB(ret, BIT32(sda));
	CLB(ret, BIT32(scl));
	XGpio_DiscreteWrite(&XGpioInstI2c, 1, ret);
 	//XGpio_DiscreteWrite(&XGpioInstI2c, 1, (0<<sda|0<<scl));//钳住I2C总线，准备发送或接收数据
}

//void i2c_scl_pos(i2c_no i2c)
//{
//	u32 sda;
//	u32 scl;
//	u32 ret;
//	sda=XGpio_I2C_CfgTable[i2c].I2C_SDA;
//	scl=XGpio_I2C_CfgTable[i2c].I2C_SCL;
//
//	ret=XGpio_GetDataDirection(&XGpioInstI2c, 1);
//	CLB(ret,BIT32(sda));
//	CLB(ret,BIT32(scl));
//	XGpio_SetDataDirection(&XGpioInstI2c, 1, ret);
//	//设置 gpio端口 为输出
//	//XGpio_SetDataDirection(&XGpioInstI2c, 1, (OUTPUT<<sda|OUTPUT<<scl));
//
//	ret = XGpio_DiscreteRead(&XGpioInstI2c, 1);
//	CLB(ret, BIT32(sda));
//	CLB(ret, BIT32(scl));
//	XGpio_DiscreteWrite(&XGpioInstI2c, 1, ret);
//	//XGpio_DiscreteWrite(&XGpioInstI2c, 1, (0<<sda|0<<scl));
//
//	usleep(4);
//
//	ret = XGpio_DiscreteRead(&XGpioInstI2c, 1);
//	STB(ret, BIT32(sda));
//	STB(ret, BIT32(scl));
//	XGpio_DiscreteWrite(&XGpioInstI2c, 1, ret);
//	//XGpio_DiscreteWrite(&XGpioInstI2c, 1, (1<<sda|1<<scl));
//
//	usleep(4);
//}

//产生停止信号
void i2c_stop(i2c_no i2c)
{
	u32 sda;
	u32 scl;
	u32 ret;
	sda=XGpio_I2C_CfgTable[i2c].I2C_SDA;
	scl=XGpio_I2C_CfgTable[i2c].I2C_SCL;

	//STOP:when CLK is high DATA change form low to high
	ret = XGpio_DiscreteRead(&XGpioInstI2c, 1);
	CLB(ret, BIT32(sda));
	CLB(ret, BIT32(scl));
	XGpio_DiscreteWrite(&XGpioInstI2c, 1, ret);
	//XGpio_DiscreteWrite(&XGpioInst, 1, (0<<sda|0<<scl));

	ret=XGpio_GetDataDirection(&XGpioInstI2c, 1);
	CLB(ret,BIT32(sda));
	CLB(ret,BIT32(scl));
	XGpio_SetDataDirection(&XGpioInstI2c, 1, ret);
//	XGpio_SetDataDirection(&XGpioInstI2c, 1, (OUTPUT<<sda|OUTPUT<<scl));

	ret = XGpio_DiscreteRead(&XGpioInstI2c, 1);
	CLB(ret, BIT32(sda));
	CLB(ret, BIT32(scl));
	XGpio_DiscreteWrite(&XGpioInstI2c, 1, ret);
	//XGpio_DiscreteWrite(&XGpioInstI2c, 1, (0<<sda|0<<scl));

 	usleep(4);

	ret = XGpio_DiscreteRead(&XGpioInstI2c, 1);
	CLB(ret, BIT32(sda));
	STB(ret, BIT32(scl));
	XGpio_DiscreteWrite(&XGpioInstI2c, 1, ret);
 	//XGpio_DiscreteWrite(&XGpioInstI2c, 1, (0<<sda|1<<scl));

	usleep(4);

	ret = XGpio_DiscreteRead(&XGpioInstI2c, 1);
	STB(ret, BIT32(sda));
	STB(ret, BIT32(scl));
	XGpio_DiscreteWrite(&XGpioInstI2c, 1, ret);
	//XGpio_DiscreteWrite(&XGpioInstI2c, 1, (1<<sda|1<<scl));

	usleep(4);
	ret=XGpio_GetDataDirection(&XGpioInstI2c, 1);
	STB(ret,BIT32(sda));
	STB(ret,BIT32(scl));
	XGpio_SetDataDirection(&XGpioInstI2c, 1, ret);
//	XGpio_SetDataDirection(&XGpioInstI2c, 1, (INPUT<<sda|INPUT<<scl));
	usleep(1000);
}

//产生ACK应答
void i2c_ack(i2c_no i2c)
{
	u32 sda;
	u32 scl;
	u32 ret;
	sda=XGpio_I2C_CfgTable[i2c].I2C_SDA;
	scl=XGpio_I2C_CfgTable[i2c].I2C_SCL;

	ret=XGpio_GetDataDirection(&XGpioInstI2c, 1);
	CLB(ret,BIT32(sda));
	CLB(ret,BIT32(scl));
	XGpio_SetDataDirection(&XGpioInstI2c, 1, ret);
	// scl, sda 都设置为输出
	//XGpio_SetDataDirection(&XGpioInstI2c, 1, (OUTPUT<<sda|OUTPUT<<scl));

	ret = XGpio_DiscreteRead(&XGpioInstI2c, 1);
	CLB(ret, BIT32(sda));
	CLB(ret, BIT32(scl));
	XGpio_DiscreteWrite(&XGpioInstI2c, 1, ret);
	//XGpio_DiscreteWrite(&XGpioInstI2c, 1, (0<<sda|0<<scl));

	usleep(4);

	ret = XGpio_DiscreteRead(&XGpioInstI2c, 1);
	CLB(ret, BIT32(sda));
	STB(ret, BIT32(scl));
	XGpio_DiscreteWrite(&XGpioInstI2c, 1, ret);
	//XGpio_DiscreteWrite(&XGpioInstI2c, 1, (0<<sda|1<<scl));

	usleep(4);

	ret = XGpio_DiscreteRead(&XGpioInstI2c, 1);
	CLB(ret, BIT32(sda));
	CLB(ret, BIT32(scl));
	XGpio_DiscreteWrite(&XGpioInstI2c, 1, ret);
	//XGpio_DiscreteWrite(&XGpioInstI2c, 1, (0<<sda|0<<scl));

	usleep(4);

	ret=XGpio_GetDataDirection(&XGpioInstI2c, 1);
	STB(ret,BIT32(sda));
	CLB(ret,BIT32(scl));
	XGpio_SetDataDirection(&XGpioInstI2c, 1, ret);
	// sda 设置为输入
	//XGpio_SetDataDirection(&XGpioInstI2c, 1, (INPUT<<sda|OUTPUT<<scl));
}

//产生NACK应答
void i2c_nack(i2c_no i2c)
{
	u32 sda;
	u32 scl;
	u32 ret;
	sda=XGpio_I2C_CfgTable[i2c].I2C_SDA;
	scl=XGpio_I2C_CfgTable[i2c].I2C_SCL;

	ret=XGpio_GetDataDirection(&XGpioInstI2c, 1);
	CLB(ret,BIT32(sda));
	CLB(ret,BIT32(scl));
	XGpio_SetDataDirection(&XGpioInstI2c, 1, ret);
	// scl, sda 都设置为输出
	//XGpio_SetDataDirection(&XGpioInstI2c, 1, (OUTPUT<<sda|OUTPUT<<scl));

	ret = XGpio_DiscreteRead(&XGpioInstI2c, 1);
	STB(ret, BIT32(sda));
	CLB(ret, BIT32(scl));
	XGpio_DiscreteWrite(&XGpioInstI2c, 1, ret);
	//XGpio_DiscreteWrite(&XGpioInstI2c, 1, (1<<sda|0<<scl));

	usleep(4);

	ret = XGpio_DiscreteRead(&XGpioInstI2c, 1);
	STB(ret, BIT32(sda));
	STB(ret, BIT32(scl));
	XGpio_DiscreteWrite(&XGpioInstI2c, 1, ret);
	//XGpio_DiscreteWrite(&XGpioInstI2c, 1, (1<<sda|1<<scl));

	usleep(4);

	ret = XGpio_DiscreteRead(&XGpioInstI2c, 1);
	STB(ret, BIT32(sda));
	CLB(ret, BIT32(scl));
	XGpio_DiscreteWrite(&XGpioInstI2c, 1, ret);
	//XGpio_DiscreteWrite(&XGpioInstI2c, 1, (1<<sda|0<<scl));

	usleep(4);

	ret=XGpio_GetDataDirection(&XGpioInstI2c, 1);
	STB(ret,BIT32(sda));
	CLB(ret,BIT32(scl));
	XGpio_SetDataDirection(&XGpioInstI2c, 1, ret);
	// sda 设置为输入
	//XGpio_SetDataDirection(&XGpioInst, 1, (INPUT<<sda|OUTPUT<<scl));
}

//发送一个字节
void i2c_send_byte(i2c_no i2c, u8 txd)
{
    u8 t;
	u32 sda;
	u32 scl;
	u32 ret;
	sda=XGpio_I2C_CfgTable[i2c].I2C_SDA;
	scl=XGpio_I2C_CfgTable[i2c].I2C_SCL;

	// scl, sda 都设置为输出
	ret=XGpio_GetDataDirection(&XGpioInstI2c, 1);
	CLB(ret,BIT32(sda));
	CLB(ret,BIT32(scl));
	XGpio_SetDataDirection(&XGpioInstI2c, 1, ret);
	//XGpio_SetDataDirection(&XGpioInstI2c, 1, (OUTPUT<<sda|OUTPUT<<scl));
	ret = XGpio_DiscreteRead(&XGpioInstI2c, 1);
	XGpio_DiscreteWrite(&XGpioInstI2c, 1, (CLB(ret,BIT32(scl))));//拉低时钟开始数据传输

    for(t=0; t<8; t++)
    {
    	ret = XGpio_DiscreteRead(&XGpioInstI2c, 1);
    	if(((txd&0x80)>>7) == 0)
    	{
    		CLB(ret, BIT32(sda));
    	}
    	else
    	{
    		STB(ret, BIT32(sda));
    	}
    	CLB(ret, BIT32(scl));
    	XGpio_DiscreteWrite(&XGpioInstI2c, 1, ret);
    	//XGpio_DiscreteWrite(&XGpioInstI2c, 1, (((txd&0x80)>>7)<<sda|0<<scl));

        txd<<=1;

        usleep(2);

        ret = XGpio_DiscreteRead(&XGpioInstI2c, 1);
        XGpio_DiscreteWrite(&XGpioInstI2c, 1, (STB(ret,BIT32(scl))));

        usleep(4);

        ret = XGpio_DiscreteRead(&XGpioInstI2c, 1);
        XGpio_DiscreteWrite(&XGpioInstI2c, 1, (CLB(ret,BIT32(scl))));

        usleep(2);
    }

	ret=XGpio_GetDataDirection(&XGpioInstI2c, 1);
	STB(ret,BIT32(sda));
	CLB(ret,BIT32(scl));
	XGpio_SetDataDirection(&XGpioInstI2c, 1, ret);
    //XGpio_SetDataDirection(&XGpioInstI2c, 1, (INPUT<<sda|OUTPUT<<scl)); //SDA设置为输入
}

//接收一个字节
u8  i2c_recv_byte(i2c_no i2c)
{
	unsigned char i=0 , rxd=0;
	u32 sda;
	u32 scl;
	u32 ret;
	sda=XGpio_I2C_CfgTable[i2c].I2C_SDA;
	scl=XGpio_I2C_CfgTable[i2c].I2C_SCL;

	ret=XGpio_GetDataDirection(&XGpioInstI2c, 1);
	STB(ret,BIT32(sda));
	CLB(ret,BIT32(scl));
	XGpio_SetDataDirection(&XGpioInstI2c, 1, ret);
	//XGpio_SetDataDirection(&XGpioInstI2c, 1, (INPUT<<sda|OUTPUT<<scl)); //SDA设置为输入

	ret = XGpio_DiscreteRead(&XGpioInstI2c, 1);
	XGpio_DiscreteWrite(&XGpioInstI2c, 1, (CLB(ret,BIT32(scl))));
	//XGpio_DiscreteWrite(&XGpioInstI2c, 1, (0<<scl));

	usleep(4);

	for(i=0;i<8;i++ )
	{
		ret = XGpio_DiscreteRead(&XGpioInstI2c, 1);
		XGpio_DiscreteWrite(&XGpioInstI2c, 1, (STB(ret,BIT32(scl))));
		//XGpio_DiscreteWrite(&XGpioInstI2c, 1, (1<<scl));

        usleep(2);

        rxd <<= 1;

        ret = XGpio_DiscreteRead(&XGpioInstI2c, 1);
        if(CHB(ret, BIT32(sda))){
        	rxd = rxd | 0x01;
        }
		usleep(2);

		ret = XGpio_DiscreteRead(&XGpioInstI2c, 1);
		XGpio_DiscreteWrite(&XGpioInstI2c, 1, (CLB(ret,BIT32(scl))));
		//XGpio_DiscreteWrite(&XGpioInstI2c, 1, (0<<scl));
		usleep(4);
    }

	ret=XGpio_GetDataDirection(&XGpioInstI2c, 1);
	CLB(ret,BIT32(sda));
	CLB(ret,BIT32(scl));
	XGpio_SetDataDirection(&XGpioInstI2c, 1, ret);
	//XGpio_SetDataDirection(&XGpioInstI2c, 1, (OUTPUT<<sda|OUTPUT<<scl));

    return rxd;
}

u8  i2c_recv_ack(i2c_no i2c, stretch_mode st_mode)
{
	u8 check;
	u32 ucErrTime=0;
	u32 sda;
	u32 scl;
	u32 ret;
	sda=XGpio_I2C_CfgTable[i2c].I2C_SDA;
	scl=XGpio_I2C_CfgTable[i2c].I2C_SCL;

	ret=XGpio_GetDataDirection(&XGpioInstI2c, 1);
	STB(ret,BIT32(sda));
	CLB(ret,BIT32(scl));
	XGpio_SetDataDirection(&XGpioInstI2c, 1, ret);
	//XGpio_SetDataDirection(&XGpioInstI2c, 1, (INPUT<<sda|OUTPUT<<scl)); //SDA设置为输入

	ret = XGpio_DiscreteRead(&XGpioInstI2c, 1);
	XGpio_DiscreteWrite(&XGpioInstI2c, 1, (CLB(ret,BIT32(scl))));
	//XGpio_DiscreteWrite(&XGpioInstI2c, 1, (0<<scl));

	if(st_mode)
	{
		usleep(10);
	}
	else
	{
		usleep(4);
	}

	if(st_mode)
	{
		ret=XGpio_GetDataDirection(&XGpioInstI2c, 1);
		STB(ret,BIT32(sda));
		STB(ret,BIT32(scl));
		XGpio_SetDataDirection(&XGpioInstI2c, 1, ret);
		//XGpio_SetDataDirection(&XGpioInstI2c, 1, (INPUT<<sda|INPUT<<scl)); //SCL设置为输入

		while(CHB(XGpio_DiscreteRead(&XGpioInstI2c, 1), BIT32(scl)) == 0)
		{
		  ucErrTime++;
		  usleep(1);
		  if(ucErrTime>100000) // 空设备一路就要0.5秒以上
		  {
			ucErrTime = 0;
			break;
		  }
		}
		usleep(10);
	}
	else
	{
		ret = XGpio_DiscreteRead(&XGpioInstI2c, 1);
		XGpio_DiscreteWrite(&XGpioInstI2c, 1, (STB(ret,BIT32(scl))));
		usleep(2);
	}

	check = 0;
	ret = XGpio_DiscreteRead(&XGpioInstI2c, 1);
	if(CHB(ret, BIT32(sda)))
	{
		check = 1;
	}

	if(st_mode)
	{
		ret=XGpio_GetDataDirection(&XGpioInstI2c, 1);
		STB(ret,BIT32(sda));
		CLB(ret,BIT32(scl));
		XGpio_SetDataDirection(&XGpioInstI2c, 1, ret);
		//XGpio_SetDataDirection(&XGpioInstI2c, 1, (INPUT<<sda|OUTPUT<<scl)); //SCL设置为输出
	}
	else
	{
		usleep(4);
	}
	ret = XGpio_DiscreteRead(&XGpioInstI2c, 1);
	XGpio_DiscreteWrite(&XGpioInstI2c, 1, (CLB(ret,BIT32(scl))));
	//XGpio_DiscreteWrite(&XGpioInstI2c, 1, (0<<scl));//拉低SCL
	usleep(2);

	if(st_mode)
	{
		ret = XGpio_DiscreteRead(&XGpioInstI2c, 1);
		STB(ret, BIT32(sda));
		CLB(ret, BIT32(scl));
		XGpio_DiscreteWrite(&XGpioInstI2c, 1, ret);
		//XGpio_DiscreteWrite(&XGpioInstI2c, 1, (0<<scl)|(1<<sda));
	}

	ret=XGpio_GetDataDirection(&XGpioInstI2c, 1);
	CLB(ret,BIT32(sda));
	CLB(ret,BIT32(scl));
	XGpio_SetDataDirection(&XGpioInstI2c, 1, ret);
	//XGpio_SetDataDirection(&XGpioInstI2c, 1, (OUTPUT<<sda|OUTPUT<<scl)); //SDA设置为输出

	ret = XGpio_DiscreteRead(&XGpioInstI2c, 1);
	STB(ret, BIT32(sda));
	CLB(ret, BIT32(scl));
	XGpio_DiscreteWrite(&XGpioInstI2c, 1, ret);
	//XGpio_DiscreteWrite(&XGpioInstI2c, 1, (0<<scl)|(1<<sda));

	return check;
}


int xgpio_i2c_reg8_write(i2c_no i2c, char IIC_ADDR, char Addr, char Data, stretch_mode st_mode)
{
	u8 ack=0;

	i2c_start(i2c);

	i2c_send_byte(i2c, IIC_ADDR<<1);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}

	i2c_send_byte(i2c, Addr);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}

	i2c_send_byte(i2c, Data);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}

	i2c_stop(i2c);

	return XST_SUCCESS;
}

int xgpio_i2c_reg8_continuous_write(i2c_no i2c, u8 IIC_ADDR, u8 Addr, u8 *Data,u16 len, stretch_mode st_mode)
{
	u8 ack=0;
	u8 i = 0;
	i2c_start(i2c);
	i2c_send_byte(i2c, IIC_ADDR<<1);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}

	i2c_send_byte(i2c, Addr);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}

	for(i = 0;i<len;i++)
	{
		i2c_send_byte(i2c, Data[i]);
		ack=i2c_recv_ack(i2c, st_mode);
		if(ack)
		{
			i2c_stop(i2c);
			return XST_FAILURE;
		}
	}

	i2c_stop(i2c);

	return XST_SUCCESS;
}


// 7-bit addr
int xgpio_i2c_reg8_read(i2c_no i2c, char IIC_ADDR, char Addr, u8 * ret, stretch_mode st_mode)
{
	u8 rxd;
	u8 ack=0;

	i2c_start(i2c);

	i2c_send_byte(i2c, IIC_ADDR<<1);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}

	i2c_send_byte(i2c, Addr);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}

	//i2c_stop(i2c);

  	i2c_start(i2c);

  	i2c_send_byte(i2c, IIC_ADDR<<1 | 0x01);
  	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}

	rxd = i2c_recv_byte(i2c);
	i2c_nack(i2c);

  	i2c_stop(i2c);

  	*ret = rxd;

  	return XST_SUCCESS;
}

int xgpio_i2c_reg8_continuous_read(i2c_no i2c, u8 IIC_ADDR, u8 Addr, u8 * ret, u16 len, stretch_mode st_mode)
{
	u8 rxd;
	u8 ack=0;
	u8 i = 0;

	i2c_start(i2c);

	i2c_send_byte(i2c, IIC_ADDR<<1);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}

	i2c_send_byte(i2c, Addr);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}

	//i2c_stop(i2c);

  	i2c_start(i2c);

  	i2c_send_byte(i2c, IIC_ADDR<<1 | 0x01);
  	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}
	usleep(50);
	for(i=0;i<len;i++)
	{
		rxd = i2c_recv_byte(i2c);
		*(ret+i) = rxd;
		if(i<len-1)
			i2c_ack(i2c);
		else
			i2c_nack(i2c);
		usleep(20);
	}

  	i2c_stop(i2c);

  	return XST_SUCCESS;
}


int xgpio_i2c_reg16_write(i2c_no i2c, char IIC_ADDR, unsigned short Addr, char Data, stretch_mode st_mode)
{
	u8 ack=0;

	i2c_start(i2c);

	i2c_send_byte(i2c, IIC_ADDR<<1);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}

	i2c_send_byte(i2c, Addr >> 8);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}

	i2c_send_byte(i2c, Addr & 0x00FF);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}

	i2c_send_byte(i2c, Data);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}

	i2c_stop(i2c);

	return XST_SUCCESS;
}

int xgpio_i2c_reg16_continuous_write(i2c_no i2c, u8 IIC_ADDR, u16 Addr, u8 *Data,u16 len, stretch_mode st_mode)
{
	u8 ack=0;
	u8 i = 0;

	i2c_start(i2c);

	i2c_send_byte(i2c, IIC_ADDR<<1);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}

	i2c_send_byte(i2c, Addr >> 8);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}

	i2c_send_byte(i2c, Addr & 0x00FF);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}
	for(i = 0;i<len;i++)
	{
		i2c_send_byte(i2c, Data[i]);
		ack=i2c_recv_ack(i2c, st_mode);
		if(ack)
		{
			i2c_stop(i2c);
			return XST_FAILURE;
		}
	}
	return XST_SUCCESS;
}


int xgpio_i2c_reg16_read(i2c_no i2c, char IIC_ADDR, unsigned short Addr, u8 * ret, stretch_mode st_mode)
{
	u8 rxd;
	u8 ack=0;

	i2c_start(i2c);

	i2c_send_byte(i2c, IIC_ADDR<<1);
//	i2c_ack(i2c);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}

	i2c_send_byte(i2c, Addr >> 8);
//	i2c_ack(i2c);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}

	i2c_send_byte(i2c, Addr & 0x00FF);
//	i2c_ack(i2c);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}

	//i2c_stop(i2c);

  	i2c_start(i2c);

  	i2c_send_byte(i2c, IIC_ADDR<<1 | 0x01);
//  	i2c_ack(i2c);
  	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}

	rxd = i2c_recv_byte(i2c);
	i2c_nack(i2c);
//	i2c_ack(i2c);

  	i2c_stop(i2c);

  	*ret = rxd;

  	return  XST_SUCCESS ;
}

int xgpio_i2c_reg16_continuous_read(i2c_no i2c, u8 IIC_ADDR, u16 Addr, u8 * ret,u16 len, stretch_mode st_mode)
{
	u8 rxd;
	u8 ack=0;
	u8 i = 0;

	i2c_start(i2c);

	i2c_send_byte(i2c, IIC_ADDR<<1);
//	i2c_ack(i2c);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}

	i2c_send_byte(i2c, Addr >> 8);
//	i2c_ack(i2c);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}

	i2c_send_byte(i2c, Addr & 0x00FF);
//	i2c_ack(i2c);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}

	//i2c_stop(i2c);

  	i2c_start(i2c);

  	i2c_send_byte(i2c, IIC_ADDR<<1 | 0x01);
//  	i2c_ack(i2c);
  	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}
	for(i=0;i<len;i++)
	{
		rxd = i2c_recv_byte(i2c);
		*(ret+i) = rxd;
		if(i<len-1)
			i2c_ack(i2c);
		else
			i2c_nack(i2c);
	}

  	i2c_stop(i2c);

//  	*ret = rxd;

  	return  XST_SUCCESS ;
}


#if 0
int xgpio_i2c_32b32_write(i2c_no i2c, char IIC_ADDR, unsigned int Addr, unsigned int Data, stretch_mode st_mode)
{
	u8 ack=0;

	i2c_start(i2c);

	i2c_send_byte(i2c, IIC_ADDR<<1);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}

	i2c_send_byte(i2c, Addr >> 24);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}
	i2c_send_byte(i2c, Addr >> 16);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}
	i2c_send_byte(i2c, Addr >> 8);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}

	i2c_send_byte(i2c, Addr & 0x00FF);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}

	i2c_send_byte(i2c, Data>>24);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}

	i2c_send_byte(i2c, Data>>16);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}

	i2c_send_byte(i2c, Data>>8);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}

	i2c_send_byte(i2c, Data);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}

	i2c_stop(i2c);

	return XST_SUCCESS;
}

int xgpio_i2c_32b32_read(i2c_no i2c, char IIC_ADDR, unsigned int Addr, unsigned int * ret, stretch_mode st_mode)
{
	u32 rxd;
	u8 ack=0;

	i2c_start(i2c);

	i2c_send_byte(i2c, IIC_ADDR<<1);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}

	i2c_send_byte(i2c, Addr >> 24);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}
	i2c_send_byte(i2c, Addr >> 16);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}
	i2c_send_byte(i2c, Addr >> 8);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}

	i2c_send_byte(i2c, Addr & 0x00FF);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}

	//i2c_stop(i2c);

  	i2c_start(i2c);

  	i2c_send_byte(i2c, IIC_ADDR<<1 | 0x01);
//  	i2c_ack(i2c);
  	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}

	rxd = i2c_recv_byte(i2c);
//	i2c_nack(i2c);
	i2c_ack(i2c);
	usleep(110);

	rxd += i2c_recv_byte(i2c)<<8;
//	i2c_nack(i2c);
	i2c_ack(i2c);
	usleep(110);

	rxd += i2c_recv_byte(i2c)<<16;
//	i2c_nack(i2c);
	i2c_ack(i2c);
	usleep(110);

	rxd += i2c_recv_byte(i2c)<<24;
	i2c_nack(i2c);
//	i2c_ack(i2c);

  	i2c_stop(i2c);

  	*ret = rxd;

  	return  XST_SUCCESS ;
}
#endif

#endif // XPAR_XGPIO_I2C_0_AXI_GPIO_0_DEVICE_ID

#endif // XPAR_XGPIO_NUM_INSTANCES

/*
usage:
assume you have a xgpio_i2c_0 heir
call follows before main_loop

#if defined(XPAR_XGPIO_I2C_0_AXI_GPIO_0_DEVICE_ID)
    Status = xgpio_i2c_init();
    if (Status != XST_SUCCESS)
	{
		Xil_Assert(__FILE__, __LINE__);
		return XST_FAILURE ;
	}
//#elif defined (XPAR_XGPIO_NUM_INSTANCES)
//    Status = xgpio_setup(&XGpioInst, XPAR_AXI_GPIO_0_DEVICE_ID, 0, 0);
//    if (Status != XST_SUCCESS)
//	{
//		Xil_Assert(__FILE__, __LINE__);
//		return XST_FAILURE ;
//	}
#endif // XPAR_XGPIO_I2C_0_AXI_GPIO_0_DEVICE_ID

then, you can access i2c devices like:
```
	Status = xgpio_i2c_reg16_read(I2C_NO_3, 0x90>>1, 0x0000, &ret8, STRETCH_ON);
    Status = xgpio_i2c_reg16_read(I2C_NO_3, 0x90>>1, 0x0001, &ret8, STRETCH_ON);
```
*/
