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
u8 i2c_devices_list[255];


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
	usleep(50); /* > I2C Std tBUF 4.7us */
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
	u8 stretch_to = 0;
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

		/* Clock stretch wait. Cap ~32ms (GMSL DS side-channel arb max step: 2/8/32ms).
		 * Was 100ms (slow empty-addr). On timeout drive SCL high then sample SDA. */
		while(CHB(XGpio_DiscreteRead(&XGpioInstI2c, 1), BIT32(scl)) == 0)
		{
		  ucErrTime++;
		  usleep(1);
		  if(ucErrTime>32000)
		  {
			stretch_to = 1;
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

	/* If stretch wait timed out (weak pull-up / slow SCL rise), drive SCL high
	 * then sample SDA. Do not force NACK without sampling - present slaves on
	 * weak buses would otherwise always fail under STRETCH_ON. */
	if(stretch_to)
	{
		ret=XGpio_GetDataDirection(&XGpioInstI2c, 1);
		STB(ret,BIT32(sda));
		CLB(ret,BIT32(scl));
		XGpio_SetDataDirection(&XGpioInstI2c, 1, ret);
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

	/* stretch timeout: bus restored above; report TIMEOUT (not sampled NACK) */
	if (stretch_to) {
		return 2;
	}
	return check;
}

/* Read fail: fill buf with 0xFF so callers do not print stale data. */
static void i2c_read_fail(u8 *ret, u16 len)
{
	u16 i;

	if ((ret == NULL) || (len == 0U)) {
		return;
	}
	for (i = 0; i < len; i++) {
		ret[i] = 0xFF;
	}
}

/* Map i2c_recv_ack: 0=ACK 1=NACK 2=TIMEOUT. is_dev=1 for first addr byte. */
static int i2c_status_from_ack(u8 ack, int is_dev)
{
	if (ack == 0U) {
		return I2C_OK;
	}
	if (ack == 2U) {
		return I2C_ERR_TIMEOUT;
	}
	return is_dev ? I2C_ERR_NODEV : I2C_ERR_XFER;
}

int xgpio_i2c_reg8_write(i2c_no i2c, char IIC_ADDR, char Addr, char Data, stretch_mode st_mode)
{
	u8 ack=0;

	if (i2c >= I2C_NO_BUTT) {
		return I2C_ERR_PARAM;
	}

	i2c_start(i2c);

	i2c_send_byte(i2c, IIC_ADDR<<1);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return i2c_status_from_ack(ack, 1);
	}

	i2c_send_byte(i2c, Addr);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return i2c_status_from_ack(ack, 0);
	}

	i2c_send_byte(i2c, Data);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return i2c_status_from_ack(ack, 0);
	}

	i2c_stop(i2c);

	return I2C_OK;
}

/* Continuous write: each data byte followed by Slave ACK; on NACK/TIMEOUT stop and return.
 * *done = number of data bytes accepted before fail (NULL ok). */
int xgpio_i2c_reg8_continuous_write(i2c_no i2c, u8 IIC_ADDR, u8 Addr, u8 *Data,u16 len, stretch_mode st_mode, u16 *done)
{
	u8 ack=0;
	u16 i = 0;

	if (done != NULL) {
		*done = 0;
	}
	if ((i2c >= I2C_NO_BUTT) || (Data == NULL) || (len == 0U)) {
		return I2C_ERR_PARAM;
	}

	i2c_start(i2c);
	i2c_send_byte(i2c, IIC_ADDR<<1);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return i2c_status_from_ack(ack, 1);
	}

	i2c_send_byte(i2c, Addr);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return i2c_status_from_ack(ack, 0);
	}

	for(i = 0;i<len;i++)
	{
		i2c_send_byte(i2c, Data[i]);
		ack=i2c_recv_ack(i2c, st_mode);
		if(ack)
		{
			i2c_stop(i2c);
			if (done != NULL) {
				*done = i; /* data[0..i-1] ok */
			}
			return i2c_status_from_ack(ack, 0);
		}
	}

	i2c_stop(i2c);
	if (done != NULL) {
		*done = len;
	}
	return I2C_OK;
}

// 7-bit addr
int xgpio_i2c_reg8_read(i2c_no i2c, char IIC_ADDR, char Addr, u8 * ret, stretch_mode st_mode)
{
	u8 rxd;
	u8 ack=0;

	if ((i2c >= I2C_NO_BUTT) || (ret == NULL)) {
		return I2C_ERR_PARAM;
	}

	i2c_start(i2c);

	i2c_send_byte(i2c, IIC_ADDR<<1);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_read_fail(ret, 1);
		i2c_stop(i2c);
		return i2c_status_from_ack(ack, 1);
	}

	i2c_send_byte(i2c, Addr);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_read_fail(ret, 1);
		i2c_stop(i2c);
		return i2c_status_from_ack(ack, 0);
	}

	//i2c_stop(i2c);

  	i2c_start(i2c);

  	i2c_send_byte(i2c, IIC_ADDR<<1 | 0x01);
  	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_read_fail(ret, 1);
		i2c_stop(i2c);
		return i2c_status_from_ack(ack, 0);
	}

	rxd = i2c_recv_byte(i2c);
	i2c_nack(i2c);

  	i2c_stop(i2c);

  	*ret = rxd;

  	return I2C_OK;
}

/* Continuous read: Master ACK for bytes [0..len-2], NACK on last byte, then STOP.
 * Setup-phase fail => *done=0; success => *done=len. */
int xgpio_i2c_reg8_continuous_read(i2c_no i2c, u8 IIC_ADDR, u8 Addr, u8 * ret, u16 len, stretch_mode st_mode, u16 *done)
{
	u8 rxd;
	u8 ack=0;
	u16 i = 0;

	if (done != NULL) {
		*done = 0;
	}
	if ((i2c >= I2C_NO_BUTT) || (ret == NULL) || (len == 0U)) {
		return I2C_ERR_PARAM;
	}

	i2c_start(i2c);

	i2c_send_byte(i2c, IIC_ADDR<<1);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_read_fail(ret, len);
		i2c_stop(i2c);
		return i2c_status_from_ack(ack, 1);
	}

	i2c_send_byte(i2c, Addr);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_read_fail(ret, len);
		i2c_stop(i2c);
		return i2c_status_from_ack(ack, 0);
	}

  	i2c_start(i2c);

  	i2c_send_byte(i2c, IIC_ADDR<<1 | 0x01);
  	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_read_fail(ret, len);
		i2c_stop(i2c);
		return i2c_status_from_ack(ack, 0);
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
	if (done != NULL) {
		*done = len;
	}
  	return I2C_OK;
}

int xgpio_i2c_reg16_write(i2c_no i2c, char IIC_ADDR, unsigned short Addr, char Data, stretch_mode st_mode)
{
	u8 ack=0;

	if (i2c >= I2C_NO_BUTT) {
		return I2C_ERR_PARAM;
	}

	i2c_start(i2c);

	i2c_send_byte(i2c, IIC_ADDR<<1);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return i2c_status_from_ack(ack, 1);
	}

	i2c_send_byte(i2c, Addr >> 8);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return i2c_status_from_ack(ack, 0);
	}

	i2c_send_byte(i2c, Addr & 0x00FF);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return i2c_status_from_ack(ack, 0);
	}

	i2c_send_byte(i2c, Data);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return i2c_status_from_ack(ack, 0);
	}

	i2c_stop(i2c);

	return I2C_OK;
}

/* Same ACK rules as reg8 continuous write (Slave ACK each data byte). */
int xgpio_i2c_reg16_continuous_write(i2c_no i2c, u8 IIC_ADDR, u16 Addr, u8 *Data,u16 len, stretch_mode st_mode, u16 *done)
{
	u8 ack=0;
	u16 i = 0;

	if (done != NULL) {
		*done = 0;
	}
	if ((i2c >= I2C_NO_BUTT) || (Data == NULL) || (len == 0U)) {
		return I2C_ERR_PARAM;
	}

	i2c_start(i2c);

	i2c_send_byte(i2c, IIC_ADDR<<1);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return i2c_status_from_ack(ack, 1);
	}

	i2c_send_byte(i2c, Addr >> 8);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return i2c_status_from_ack(ack, 0);
	}

	i2c_send_byte(i2c, Addr & 0x00FF);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_stop(i2c);
		return i2c_status_from_ack(ack, 0);
	}
	for(i = 0;i<len;i++)
	{
		i2c_send_byte(i2c, Data[i]);
		ack=i2c_recv_ack(i2c, st_mode);
		if(ack)
		{
			i2c_stop(i2c);
			if (done != NULL) {
				*done = i;
			}
			return i2c_status_from_ack(ack, 0);
		}
	}
	i2c_stop(i2c);
	if (done != NULL) {
		*done = len;
	}
	return I2C_OK;
}

int xgpio_i2c_reg16_read(i2c_no i2c, char IIC_ADDR, unsigned short Addr, u8 * ret, stretch_mode st_mode)
{
	u8 rxd;
	u8 ack=0;

	if ((i2c >= I2C_NO_BUTT) || (ret == NULL)) {
		return I2C_ERR_PARAM;
	}

	i2c_start(i2c);

	i2c_send_byte(i2c, IIC_ADDR<<1);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_read_fail(ret, 1);
		i2c_stop(i2c);
		return i2c_status_from_ack(ack, 1);
	}

	i2c_send_byte(i2c, Addr >> 8);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_read_fail(ret, 1);
		i2c_stop(i2c);
		return i2c_status_from_ack(ack, 0);
	}

	i2c_send_byte(i2c, Addr & 0x00FF);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_read_fail(ret, 1);
		i2c_stop(i2c);
		return i2c_status_from_ack(ack, 0);
	}

  	i2c_start(i2c);

  	i2c_send_byte(i2c, IIC_ADDR<<1 | 0x01);
  	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_read_fail(ret, 1);
		i2c_stop(i2c);
		return i2c_status_from_ack(ack, 0);
	}

	rxd = i2c_recv_byte(i2c);
	i2c_nack(i2c);

  	i2c_stop(i2c);

  	*ret = rxd;

  	return I2C_OK;
}

/* Same ACK rules as reg8 continuous read (Master NACK on last data byte). */
int xgpio_i2c_reg16_continuous_read(i2c_no i2c, u8 IIC_ADDR, u16 Addr, u8 * ret,u16 len, stretch_mode st_mode, u16 *done)
{
	u8 rxd;
	u8 ack=0;
	u16 i = 0;

	if (done != NULL) {
		*done = 0;
	}
	if ((i2c >= I2C_NO_BUTT) || (ret == NULL) || (len == 0U)) {
		return I2C_ERR_PARAM;
	}

	i2c_start(i2c);

	i2c_send_byte(i2c, IIC_ADDR<<1);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_read_fail(ret, len);
		i2c_stop(i2c);
		return i2c_status_from_ack(ack, 1);
	}

	i2c_send_byte(i2c, Addr >> 8);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_read_fail(ret, len);
		i2c_stop(i2c);
		return i2c_status_from_ack(ack, 0);
	}

	i2c_send_byte(i2c, Addr & 0x00FF);
	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_read_fail(ret, len);
		i2c_stop(i2c);
		return i2c_status_from_ack(ack, 0);
	}

  	i2c_start(i2c);

  	i2c_send_byte(i2c, IIC_ADDR<<1 | 0x01);
  	ack=i2c_recv_ack(i2c, st_mode);
	if(ack)
	{
		i2c_read_fail(ret, len);
		i2c_stop(i2c);
		return i2c_status_from_ack(ack, 0);
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
	if (done != NULL) {
		*done = len;
	}
  	return I2C_OK;
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

/*
 * 读 8-bit 寄存器并打印; 必须查返回值.
 * 失败时驱动会把 ret8 写成 0xFF, 打印 FAIL, 避免沿用上一次成功读的旧值.
 */
void dump_i2c_reg8_read(i2c_no bus, u8 addr7, u8 reg)
{
	ret8 = 0xFF;
	ret32 = (u32)xgpio_i2c_reg8_read(bus, (char)addr7, reg, &ret8, STRETCH_ON);
	if (ret32 != I2C_OK) {
		xil_printf("0x%02x == FAIL st=%d (0x%02x)\r\n", reg, (int)ret32, ret8);
	} else {
		xil_printf("0x%02x == 0x%02x\r\n", reg, ret8);
	}
}
/*
 * 读 16-bit 寄存器并打印; 必须查返回值.
 * 失败时驱动会把 ret8 写成 0xFF, 打印 FAIL, 避免沿用上一次成功读的旧值.
 */
void dump_i2c_reg16_read(i2c_no bus, u8 addr7, u16 reg)
{
	ret8 = 0xFF;
	ret32 = (u32)xgpio_i2c_reg16_read(bus, (char)addr7, reg, &ret8, STRETCH_ON);
	if (ret32 != I2C_OK) {
		xil_printf("0x%04x == FAIL st=%d (0x%02x)\r\n", reg, (int)ret32, ret8);
	} else {
		xil_printf("0x%04x == 0x%02x\r\n", reg, ret8);
	}
}

/* 探测单个 7-bit 地址: 收到 ACK 返回 XST_SUCCESS */
int xgpio_i2c_probe(i2c_no i2c, u8 addr7, stretch_mode st_mode)
{
	u8 ack;

	if ((i2c >= I2C_NO_BUTT) || (addr7 > 0x7F)) {
		return I2C_ERR_PARAM;
	}

	i2c_start(i2c);
	i2c_send_byte(i2c, (u8)(addr7 << 1));
	ack = i2c_recv_ack(i2c, st_mode);
	i2c_stop(i2c);

	return i2c_status_from_ack(ack, 1);
}

/*
 * 扫描 7-bit 地址 0x03..0x77 (风格类似 Linux i2cdetect).
 * found_addrs: 可选, 写入发现的地址; found_max 为缓冲容量.
 * 返回发现数量. 弱上拉总线请用 STRETCH_OFF.
 */
int xgpio_i2c_scan(i2c_no i2c, stretch_mode st_mode, u8 *found_addrs, u8 found_max)
{
	u8 addr;
	u8 found = 0;

	xil_printf("i2c-%d scan:\r\n", (int)i2c);
	xil_printf("     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f\r\n");

	for (addr = 0; addr < 0x80; addr++) {
		if ((addr & 0x0FU) == 0U) {
			xil_printf("%02x:", addr);
		}

		if ((addr < 0x03U) || (addr > 0x77U)) {
			xil_printf("   ");
		} else {
			if (xgpio_i2c_probe(i2c, addr, st_mode) == I2C_OK) {
				xil_printf(" %02x", addr);
				if ((found_addrs != NULL) && (found < found_max)) {
					found_addrs[found] = addr;
				}
				found++;
			} else {
				xil_printf(" --");
			}
		}

		if ((addr & 0x0FU) == 0x0FU) {
			xil_printf("\r\n");
		}
	}

	xil_printf("found %d device(s)\r\n", found);
	return (int)found;
}

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
