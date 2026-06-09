#ifndef __EEPROM_H__

#if defined (USING_EEPROM)

#define EEPROM_USING_PSIIC	0
#define EEPROM_USING_XIIC	1
#define EEPROM_USING_XGPIO	2
#define EEPROM_USING_EMIO	3

#define EEPROM_INTF EEPROM_USING_XGPIO

#if EEPROM_INTF == EEPROM_USING_PSIIC
//#include "xparameters.h"
#if defined (XPAR_XIICPS_NUM_INSTANCES) && defined (__PS_I2C_H__)
#define __EEPROM_H__
#include "xil_types.h"
//#if defined (ARMR5) || (__aarch64__) || (__arm__)
#include "xiicps.h"
//#endif
//typedef u16 AddressType;

#if 0
int I2cMux_Eeprom(void);
#endif

s32 EepromWriteData(XIicPs *IicInstance, u8 *BufferPtr, u16 ByteCount, u16 start_addr);
s32 EepromReadData(XIicPs *IicInstance, u8 *BufferPtr, u16 ByteCount, u16 start_addr);

#if 0
s32 IicPsEepromPolledExample();
#endif

#endif // XPAR_XIICPS_NUM_INSTANCES && __PS_I2C_H__

#endif // EEPROM_USING_PSIIC





#if EEPROM_INTF == EEPROM_USING_XIIC
#if defined (XPAR_XIIC_NUM_INSTANCES)

#define __EEPROM_H__
#include "xil_types.h"
#include "xiic.h"

s32 EepromWriteData(u8 *BufferPtr, u16 ByteCount, u16 start_addr);
s32 EepromReadData(u8 *BufferPtr, u16 ByteCount, u16 start_addr);

#endif // XPAR_XIIC_NUM_INSTANCES
#endif // EEPROM_USING_XIIC





#if EEPROM_INTF == EEPROM_USING_XGPIO
// 确保有设备，且要调用xgpio_i2c.c里的初始化
#if defined(XPAR_XGPIO_I2C_0_AXI_GPIO_0_DEVICE_ID) && defined (__XGPIO_I2C_H__)

#define __EEPROM_H__
#include "xil_types.h"
#include "xgpio.h"
#include "sleep.h"
#define EEPROM_XGPIO_I2C_NO	I2C_NO_0

typedef enum
{
	NO_STOP = 0,
	BUS_STOP,
} opt_mode;

s32 EepromWriteData(u8 *BufferPtr, u16 ByteCount, u16 start_addr);
s32 EepromReadData(u8 *BufferPtr, u16 ByteCount, u16 start_addr);

#endif // XPAR_XGPIO_I2C_0_AXI_GPIO_0_DEVICE_ID
#endif // EEPROM_USING_XGPIO





#if EEPROM_INTF == EEPROM_USING_EMIO
// 确保有设备，且要调用emio_i2c.c里的初始化
#if defined(XPAR_XGPIOPS_0_DEVICE_ID) && defined (__EMIO_I2C_H__)

#define __EEPROM_H__
#include "xil_types.h"
#include "xgpiops.h"
#include "sleep.h"
#define EEPROM_EMIO_I2C_NO	EMIO_I2C_NO_0

typedef enum
{
	NO_STOP = 0,
	BUS_STOP,
} opt_mode;

s32 EepromWriteData(u8 *BufferPtr, u16 ByteCount, u16 start_addr);
s32 EepromReadData(u8 *BufferPtr, u16 ByteCount, u16 start_addr);

#endif // XPAR_XGPIOPS_0_DEVICE_ID
#endif // EEPROM_USING_XGPIO


#endif // USING_EEPROM
#endif // __EEPROM_H__


