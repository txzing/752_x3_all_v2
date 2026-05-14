#ifndef __ADS7128_H__
#define __ADS7128_H__

#include "../xgpio_i2c/xgpio_i2c.h"


int adc_config(void);
int adc_i2c_read(i2c_no i2c, char IIC_ADDR, u16 * ret);
int adc_read(u16 * val);
int adc_read_poll(u8 ch,u16 * val);
void get_adc_voltage(u8 ch,u16 * val);

#endif //__ADS7128_H_
