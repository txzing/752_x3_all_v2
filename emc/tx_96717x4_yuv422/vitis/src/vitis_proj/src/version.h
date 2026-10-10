/*
 * version.h
 *
 *  Created on: Dec 20, 2018
 *      Author: alex
 */

#ifndef __VERSION_H__
#define __VERSION_H__

//#define __SW_VER_MAJOR__		0xA000
//#define __SW_VER_MINOR__		0x0001
#define __SW_VER__				0x00000001
#if defined (XPAR_AXI_LITE_REG_NUM_INSTANCES) && (XPAR_AXI_LITE_REG_0_DEVICE_ID == 0)
extern volatile u32 __HW_VER__;
#endif

#endif // __VERSION_H__
