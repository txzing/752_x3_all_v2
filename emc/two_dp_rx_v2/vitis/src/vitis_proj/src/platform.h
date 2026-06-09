/******************************************************************************
*
* Copyright (C) 2008 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

#ifndef __PLATFORM_H_
#define __PLATFORM_H_

void init_platform(void);
void cleanup_platform(void);

#if defined (UDP_UPDATE) || defined (TCP_UPDATE) || defined (TCP_COMMAND_SRV) || defined (UDP_COMMAND_SRV)
#if defined (__MICROBLAZE__)
void timer_callback();
#endif // #if defined (__MICROBLAZE__)
#endif // #if defined (UDP_UPDATE) || defined (TCP_UPDATE) || defined (TCP_COMMAND_SRV) || defined (UDP_COMMAND_SRV)

#if defined(MODBUS_RTU_SLAVE)
#if defined (__MICROBLAZE__)
void Timer1Handler(void *CallBackRef, u8 TmrCtrNumber);
#endif // #if defined (__MICROBLAZE__)

#if defined (PLATFORM_ZYNQ)
void Timer0Handler(void *CallBackRef, u8 TmrCtrNumber);
#endif // #if defined (PLATFORM_ZYNQ)

#if defined (PLATFORM_ZYNQMP)
void Timer0Handler(void *CallBackRef, u8 TmrCtrNumber);
#endif // #if defined (PLATFORM_ZYNQMP)

#endif // MODBUS_RTU_SLAVE

#if defined (ARMR5) || (__aarch64__) || (__arm__)
uint64_t get_time_ms(void);
float get_time_s(void);
#endif // #if defined (ARMR5) || (__aarch64__) || (__arm__)


#if defined (INTC_DEVICE_ID) || defined (INTC)
int platform_setup_interrupts(void);
void platform_enable_interrupts(void);
#endif

#endif // __PLATFORM_H_
