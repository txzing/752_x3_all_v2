#include "../bsp.h"
#include "../bitmanip.h"


//VAR_T g_tVar;

volatile uint8_t g_adc_trigger = 0;

static unsigned char i=0;

int adc_i2c_read(i2c_no i2c, char IIC_ADDR, u16 * ret)
{
	u16 rxd;
	u8 ack=0;
	u32 status = 0;
	u8 ch;

	if(i==0)
	{
		xgpio_i2c_reg16_read(i2c,IIC_ADDR,0x1011,&ch,STRETCH_OFF);// read 0x10 write 0x08
//		status = xgpio_i2c_reg16_write(i2c, IIC_ADDR, 0x1011, 0x07,STRETCH_OFF);
		status = xgpio_i2c_reg16_write(i2c, IIC_ADDR, 0x0811, 0x04,STRETCH_OFF);
		i2c_start(i2c);
		i2c_send_byte(i2c, IIC_ADDR<<1 | 0x01);

	//  	i2c_ack(i2c);
		ack=i2c_recv_ack(i2c,STRETCH_OFF);
		if(ack)
		{
			i2c_stop(i2c);
			return XST_FAILURE;
		}
		i=1;
		xgpio_i2c_reg16_read(i2c,IIC_ADDR,0x1011,&ch,STRETCH_OFF);

		if(ch > 0)
		{
			xil_printf("\r\n");
		}
	}
	else if (i==1)
	{
		i2c_start(i2c);
		i2c_send_byte(i2c, IIC_ADDR<<1 | 0x01);

	//  	i2c_ack(i2c);
		ack=i2c_recv_ack(i2c,STRETCH_OFF);
		if(ack)
		{
			i2c_stop(i2c);
			return XST_FAILURE;
		}
//		i=2;
	}

    usleep(4);
	rxd = i2c_recv_byte(i2c)<<8;
	i2c_ack(i2c);

	rxd += i2c_recv_byte(i2c);
	i2c_nack(i2c);

//	i2c_recv_byte(i2c);
//	i2c_ack(i2c);
//	i2c_recv_byte(i2c);
//	i2c_ack(i2c);

//	i2c_scl_pos(I2C_NO_4);
//	i2c_scl_pos(I2C_NO_4);
//	i2c_scl_pos(I2C_NO_4);
//	i2c_scl_pos(I2C_NO_4);
//	i2c_scl_pos(I2C_NO_4);
//	i2c_scl_pos(I2C_NO_4);
//	i2c_scl_pos(I2C_NO_4);

	i2c_stop(i2c);

  	*ret = rxd;

  	return  XST_SUCCESS ;
}

int adc_config(void)
{
	int status;
	u8 ret;

    status = xgpio_i2c_reg16_read(I2C_NO_4, 0x10, 0x1000, &ret,STRETCH_OFF);
    if(status!= XST_SUCCESS)
	{
		xil_printf("xgipo_i2c_%d_read_id failed\r\n", I2C_NO_4);
	}
    else
    {
    	xil_printf("xgipo_i2c_%d_read_id = 0x%02x\r\n", I2C_NO_4, ret);
    }
//	if(ret==0x81)
//	{
//    	xgpio_i2c_reg16_write(I2C_NO_4, 0x10, 0x1005, 0xFE,STRETCH_OFF);
//    	xgpio_i2c_reg16_write(I2C_NO_4, 0x10, 0x100F, 0x32,STRETCH_OFF);
//		xgpio_i2c_reg16_write(I2C_NO_4, 0x10, 0x10C3, 0x00,STRETCH_OFF);
//		xgpio_i2c_reg16_write(I2C_NO_4, 0x10, 0x10C5, 0x00,STRETCH_OFF);
//		xgpio_i2c_reg16_write(I2C_NO_4, 0x10, 0x10C7, 0x00,STRETCH_OFF);
//		xgpio_i2c_reg16_write(I2C_NO_4, 0x10, 0x10C9, 0x00,STRETCH_OFF);
//		xgpio_i2c_reg16_write(I2C_NO_4, 0x10, 0x10CB, 0x00,STRETCH_OFF);
//		xgpio_i2c_reg16_write(I2C_NO_4, 0x10, 0x10CD, 0x00,STRETCH_OFF);
//		xgpio_i2c_reg16_write(I2C_NO_4, 0x10, 0x10CF, 0x00,STRETCH_OFF);
//		xgpio_i2c_reg16_write(I2C_NO_4, 0x10, 0x10D1, 0x00,STRETCH_OFF);
//	}
//
//	xgpio_i2c_reg16_write(I2C_NO_4, 0x10, 0x1011, 0x00,STRETCH_OFF);
//	adc_i2c_read_start(I2C_NO_4, 0x10);

	status = xgpio_i2c_reg16_write(I2C_NO_4, 0x10, 0x0804, 0x00,STRETCH_OFF);
	if (status != XST_SUCCESS)
	{
		xil_printf("\r\nadc config faild 0!\r\n");
		return XST_FAILURE;
	}
	status = xgpio_i2c_reg16_write(I2C_NO_4, 0x10, 0x0810, 0x00,STRETCH_OFF);
	if (status != XST_SUCCESS)
	{
		xil_printf("\r\nadc config faild 1!\r\n");
		return XST_FAILURE;
	}
	status = xgpio_i2c_reg16_write(I2C_NO_4, 0x10, 0x0805, 0x00,STRETCH_OFF);
	if (status != XST_SUCCESS)
	{
		xil_printf("\r\nadc config faild 2!\r\n");
		return XST_FAILURE;
	}
	status = xgpio_i2c_reg16_write(I2C_NO_4, 0x10, 0x080F, 0x32,STRETCH_OFF);
	if (status != XST_SUCCESS)
	{
		xil_printf("\r\nadc config faild 3!\r\n");
		return XST_FAILURE;
	}
	status = xgpio_i2c_reg16_write(I2C_NO_4, 0x10, 0x08C3, 0x00,STRETCH_OFF);
	if (status != XST_SUCCESS)
	{
		xil_printf("\r\nadc config faild 4!\r\n");
		return XST_FAILURE;
	}
	status = xgpio_i2c_reg16_write(I2C_NO_4, 0x10, 0x08C5, 0x00,STRETCH_OFF);
	if (status != XST_SUCCESS)
	{
		xil_printf("\r\nadc config faild 5!\r\n");
		return XST_FAILURE;
	}
	status = xgpio_i2c_reg16_write(I2C_NO_4, 0x10, 0x08C7, 0x00,STRETCH_OFF);
	if (status != XST_SUCCESS)
	{
		xil_printf("\r\nadc config faild 6!\r\n");
		return XST_FAILURE;
	}
	status = xgpio_i2c_reg16_write(I2C_NO_4, 0x10, 0x08C9, 0x00,STRETCH_OFF);
	if (status != XST_SUCCESS)
	{
		xil_printf("\r\nadc config faild 7!\r\n");
		return XST_FAILURE;
	}
	status = xgpio_i2c_reg16_write(I2C_NO_4, 0x10, 0x08CB, 0x00,STRETCH_OFF);
	if (status != XST_SUCCESS)
	{
		xil_printf("\r\nadc config faild 8!\r\n");
		return XST_FAILURE;
	}
	status = xgpio_i2c_reg16_write(I2C_NO_4, 0x10, 0x08CD, 0x00,STRETCH_OFF);
	if (status != XST_SUCCESS)
	{
		xil_printf("\r\nadc config faild 9!\r\n");
		return XST_FAILURE;
	}
	status = xgpio_i2c_reg16_write(I2C_NO_4, 0x10, 0x08CF, 0x00,STRETCH_OFF);
	if (status != XST_SUCCESS)
	{
		xil_printf("\r\nadc config faild 10!\r\n");
		return XST_FAILURE;
	}
	status = xgpio_i2c_reg16_write(I2C_NO_4, 0x10, 0x08D1, 0x00,STRETCH_OFF);
	if (status != XST_SUCCESS)
	{
		xil_printf("\r\nadc config faild 11!\r\n");
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}


int adc_read(u16 * val)
{
	int status;
	u16 ret;

    status = adc_i2c_read(I2C_NO_4, 0x10, &ret);
//    if(status!= XST_SUCCESS)
//	{
//		//xil_printf("xgipo_i2c_%d_read_id failed\r\n", I2C_NO_4);
//    	asm("nop");
//	}
//    else
//    {
////    	//xil_printf("xgipo_i2c_%d_read_id = 0x%02x\r\n", I2C_NO_4, id);
////    	i2c_scl_pos(I2C_NO_4);
////    	i2c_scl_pos(I2C_NO_4);
////    	i2c_scl_pos(I2C_NO_4);
////    	i2c_scl_pos(I2C_NO_4);
////    	i2c_scl_pos(I2C_NO_4);
////    	i2c_scl_pos(I2C_NO_4);
////    	i2c_scl_pos(I2C_NO_4);
//    	asm("nop");
//    }
    *val = ret>>4;
    return status;
}

int adc_read_poll(u8 ch,u16 * val)
{
	int status;
	u8 ack = 0;
	u16 rxd;


	status = xgpio_i2c_reg16_write(I2C_NO_4, 0x10, 0x0811, ch,STRETCH_OFF);
	usleep(10);
	i2c_start(I2C_NO_4);
	i2c_send_byte(I2C_NO_4, 0x10<<1 | 0x01);

	ack=i2c_recv_ack(I2C_NO_4,STRETCH_OFF);
	if(ack)
	{
		i2c_stop(I2C_NO_4);
	}
	rxd = i2c_recv_byte(I2C_NO_4)<<8;
	i2c_ack(I2C_NO_4);

	rxd += i2c_recv_byte(I2C_NO_4);
	i2c_nack(I2C_NO_4);

	i2c_stop(I2C_NO_4);

	*val = rxd>>4;

}

void get_adc_voltage(u8 ch,u16 * val)
{
	int status;
	u8 ack = 0;
	u16 rxd;
	u16 ret;


	status = xgpio_i2c_reg16_write(I2C_NO_4, 0x10, 0x0811, ch,STRETCH_OFF);
	usleep(10);
	i2c_start(I2C_NO_4);
	i2c_send_byte(I2C_NO_4, 0x10<<1 | 0x01);

	ack=i2c_recv_ack(I2C_NO_4,STRETCH_OFF);
	if(ack)
	{
		i2c_stop(I2C_NO_4);
	}
	rxd = i2c_recv_byte(I2C_NO_4)<<8;
	i2c_ack(I2C_NO_4);

	rxd += i2c_recv_byte(I2C_NO_4);
	i2c_nack(I2C_NO_4);

	i2c_stop(I2C_NO_4);


	ret = rxd>>4;

     *val = (u16)(((float)ret/4096)*3300);

}





