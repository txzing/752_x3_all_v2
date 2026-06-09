#ifndef __serdes_i2c_H__

#if defined (__XGPIO_I2C_H__)
#if defined (SER_CFG) || defined (DES_CFG)
#define __SERDES_H__
//#include "xil_types.h"

struct reginfo
{
	u8	addr;
    u16 reg;
    u8 	val;
}__attribute__ ((__packed__));

#define SEQUENCE_PROPERTY    0xFFFD
#define SEQUENCE_WAIT_MS     0xFFFE
#define SEQUENCE_END	     0xFFFF

#if defined (DES_CFG)
extern struct reginfo max9296_rgb888_gmsl2[];
extern struct reginfo max96856_dual_link[];
extern struct reginfo max96772_dual_link[];
#endif // DES_CFG

#if defined (SER_CFG)
extern struct reginfo max96717_rgb888_gmsl2[];
#endif // SER_CFG

int serdes_i2c_write_8(i2c_no i2c, u8 addr, u16 reg, u8 data);
void serdes_i2c_write_array_8(i2c_no i2c, struct reginfo *regarray);

int serdes_i2c_write_16(i2c_no i2c, u8 addr, u16 reg, u8 data);
void serdes_i2c_write_array_16(i2c_no i2c, struct reginfo *regarray);


#endif // SER_CFG || DES_CFG

#endif // __XGPIO_I2C_H__

#endif // __SERDES_H__

