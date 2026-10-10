#ifndef __EEPROM_H__

#if defined (USING_EEPROM)

/*
 * PS-I2C EEPROM (slave 0x50) address map - keep regions disjoint.
 *
 *   [0x0000 .. 0x007F]    eth cfg: 1B csum + config_Settings_t (~29B today)
 *   [0x0080 .. 0x00AF]    video geometry NVM v2 VEM2 (48B = 3x16B pages)
 *   [0x00B0 .. 0x00CF]    TPG pattern NVM VTP1 (32B)
 *   [0x00D0 .. 0x00FF]    reserved gap
 *   [0x0100 .. 0x0104]    modbus slave ID (5B)
 *
 * Multi-page writes must use absolute page addresses (fixed in eeprom.c).
 * Prefer EepromWriteVerify() for any persistent region.
 *
 * Page write delay: AT24Cxx/M24Cxx tWR max is typically 5 ms. The old
 * 250 ms/page made geom+TPG save (~5 pages) exceed the host 2 s timeout.
 */
#ifndef EEPROM_PAGE_WRITE_DELAY_US
#define EEPROM_PAGE_WRITE_DELAY_US	10000U	/* 10 ms margin above tWR */
#endif

#define EEPROM_ADDR_ETH_CFG      0x0000U
#define EEPROM_ETH_CFG_MAX_BYTES 0x80U /* must not reach VIDEO_GEOM */
#define EEPROM_ADDR_VIDEO_GEOM   0x0080U
#define EEPROM_VIDEO_GEOM_BYTES  48U
#define EEPROM_ADDR_TPG_PAT      0x00B0U
#define EEPROM_TPG_PAT_BYTES     32U
#define EEPROM_ADDR_MODBUS_ID    0x0100U
#define EEPROM_MODBUS_ID_BYTES   5U

#if (EEPROM_ADDR_VIDEO_GEOM + EEPROM_VIDEO_GEOM_BYTES) > EEPROM_ADDR_TPG_PAT
#error "EEPROM: VIDEO_GEOM overlaps TPG_PAT"
#endif
#if (EEPROM_ADDR_TPG_PAT + EEPROM_TPG_PAT_BYTES) > EEPROM_ADDR_MODBUS_ID
#error "EEPROM: TPG_PAT overlaps MODBUS_ID (or reserved gap shrink)"
#endif
#if EEPROM_ETH_CFG_MAX_BYTES > EEPROM_ADDR_VIDEO_GEOM
#error "EEPROM: ETH_CFG max crosses VIDEO_GEOM"
#endif

#define EEPROM_USING_PSIIC	0
#define EEPROM_USING_XIIC	1
#define EEPROM_USING_XGPIO	2
#define EEPROM_USING_EMIO	3

#define EEPROM_INTF EEPROM_USING_PSIIC

#if EEPROM_INTF == EEPROM_USING_PSIIC
//#include "xparameters.h"
#if defined (XPAR_XIICPS_NUM_INSTANCES) && defined (__PS_I2C_H__)
#define __EEPROM_H__
#include "xil_types.h"
//#if defined (ARMR5) || (__aarch64__) || (__arm__)
#include "xiicps.h"
//#endif
//typedef u16 AddressType;

s32 EepromWriteData(XIicPs *IicInstance, u8 *BufferPtr, u16 ByteCount, u16 start_addr);
s32 EepromReadData(XIicPs *IicInstance, u8 *BufferPtr, u16 ByteCount, u16 start_addr);
/* Write then read-back compare; fails if any byte mismatches. */
s32 EepromWriteVerify(XIicPs *IicInstance, u8 *BufferPtr, u16 ByteCount, u16 start_addr);

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
// 确保有设备-且要调用xgpio_i2c.c里的初始化
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
// 确保有设备-且要调用emio_i2c.c里的初始化
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
#endif // EEPROM_USING_EMIO


#endif // USING_EEPROM
#endif // __EEPROM_H__


