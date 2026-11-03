#ifndef __PS_I2C_H__


//#include "xparameters.h"
#if defined (XPAR_XIICPS_NUM_INSTANCES)
#define __PS_I2C_H__
//#include "xil_types.h"
#include "xiicps.h"

#if (XPAR_XIICPS_NUM_INSTANCES >= 1U)
extern XIicPs ps_i2c_0;
#endif
#if (XPAR_XIICPS_NUM_INSTANCES >= 2U)
extern XIicPs ps_i2c_1;
#endif

int psi_2c_reg8_write(XIicPs *InstancePtr, char IIC_ADDR, char Addr, char Data);
char ps_i2c_reg8_read(XIicPs *InstancePtr, char IIC_ADDR, char Addr);
int psi_2c_reg16_write(XIicPs *InstancePtr, char IIC_ADDR, unsigned short Addr, char Data);
char ps_i2c_reg16_read(XIicPs *InstancePtr, char IIC_ADDR, unsigned short Addr);
int ps_i2c_init(XIicPs *Iic,short DeviceID ,u32 IIC_SCLK_RATE);
int ps_i2c_config(void);


#endif // XPAR_XIICPS_NUM_INSTANCES

#endif // __PS_I2C_H__

