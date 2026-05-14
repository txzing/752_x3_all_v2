#ifndef __BSP_H__
#define __BSP_H__

//这里引入标准库,按需要打开
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <malloc.h>
#include <assert.h>
#include <ctype.h>

/*
* PLATFORM_ZYNQ or PLATFORM_ZYNQMP defined in xparameters.h
#if defined (__arm__) && !defined (ARMR5)
#define PLATFORM_ZYNQ
#endif
#if defined (ARMR5) || (__aarch64__) || (ARMA53_32)
#define PLATFORM_ZYNQMP
#endif
*/

//引入xilinx定义的通用头文件
#include "xparameters.h"
#if defined (PLATFORM_ZYNQMP) || defined (PLATFORM_ZYNQ)
#include "xparameters_ps.h"	// defines XPAR values
#include "xttcps.h"
#endif // PLATFORM_ZYNQMP || PLATFORM_ZYNQ
#if defined (PLATFORM_ZYNQMP) || defined (PLATFORM_ZYNQ) || defined (ARMR5) || \
	defined (__aarch64__) || defined (__arm__) || defined (__PPC__)
#include "xtime_l.h"
#endif
#include "xil_types.h"
#include "xil_assert.h"
#include "xil_cache.h"
#include "xstatus.h"
#include "sleep.h"
#include "platform.h"

//自定义通用头文件
#include "config.h"
#include "dbg_trace.h"
#include "bitmanip.h"
#include "user_app.h"
#if defined (SW_VER_BY_COMPILE_TIME)
#include "version/version.h"
#else
#include "version.h"
#endif // SW_VER_BY_COMPILE_TIME

#if defined (PLATFORM_ZYNQ) || defined (PLATFORM_ZYNQMP)
#include "hot_reset/hot_reset.h"
#endif

//直接引入的外设库头文件

//简单串口打印，不支持浮点数
#ifndef __PPC__ // znyq zynqmp mb都可以用，排除PPC架构
#include "xil_printf.h"
#endif // __PPC__

//下面是串口非打印所需
#if defined (XPAR_XUARTLITE_NUM_INSTANCES)	// 一般用 uart lite，znyq zynqmp mb都可以用
#include "xuartlite_l.h"
#include "xuartlite.h"
#define XUartLite_IsTransmitEmpty(BaseAddress) \
		((XUartLite_GetStatusReg((BaseAddress)) & XUL_SR_TX_FIFO_EMPTY) == \
				XUL_SR_TX_FIFO_EMPTY)
#endif // XPAR_XUARTLITE_NUM_INSTANCES
#if defined (XPAR_XUARTPS_NUM_INSTANCES)  // ps uart
#include "xuartps.h"
#define XUartPs_IsTransmitEmpty(BaseAddress)			 \
		((Xil_In32((BaseAddress) + XUARTPS_SR_OFFSET) & 	\
				(u32)XUARTPS_SR_TXEMPTY) == (u32)XUARTPS_SR_TXEMPTY)
#endif // XPAR_XUARTPS_NUM_INSTANCES

// 一般都会设置hw的版本号
#ifdef XPAR_AXI_LITE_REG_NUM_INSTANCES
#include "AXI_LITE_REG.h"
#endif // XPAR_AXI_LITE_REG_NUM_INSTANCES

// axis stream 监测
#ifdef XPAR_AXI_PASSTHROUGH_MONITOR_NUM_INSTANCES
#include "axis_passthrough_monitor.h"
#endif // XPAR_AXI_PASSTHROUGH_MONITOR_NUM_INSTANCES

// 中断发生器
#if defined (XPAR_XSCUGIC_NUM_INSTANCES) || defined (XPAR_XINTC_NUM_INSTANCES)
#include "xil_exception.h"
#endif
#if defined (XPAR_XSCUGIC_NUM_INSTANCES)
#include "xscugic.h"
#define INTC				XScuGic
#define INTC_DEVICE_ID		XPAR_SCUGIC_SINGLE_DEVICE_ID
#define INTC_HANDLER		XScuGic_InterruptHandler
#define INTC_CONNECT		XScuGic_Connect
#define INTC_CONNECT_ENABLE	XScuGic_Enable
#define INTC_BASE_ADDR		XPAR_SCUGIC_0_CPU_BASEADDR
#define INTC_DIST_BASE_ADDR	XPAR_SCUGIC_0_DIST_BASEADDR
#elif defined (XPAR_XINTC_NUM_INSTANCES)
#include "xintc.h"
#define INTC				XIntc
#define INTC_DEVICE_ID		XPAR_INTC_0_DEVICE_ID
#define INTC_HANDLER		XIntc_InterruptHandler
#define INTC_CONNECT		XIntc_Connect
#define INTC_CONNECT_ENABLE	XIntc_Enable
#define INTC_BASE_ADDR		XPAR_INTC_0_BASEADDR
#endif
#if defined (INTC_DEVICE_ID) && defined (INTC)
extern INTC InterruptController;	/* Instance of the Interrupt Controller */
#endif

/*
* Capability macros:
* keep hardware presence checks in one place so application code can depend on
* clear subsystem-level capabilities instead of scattered XPAR_* details.
*/
#if defined (SER_CFG)
#define BSP_CFG_SERDES_TX		1
#endif

#if defined (DES_CFG)
#define BSP_CFG_SERDES_RX		1
#endif

#if defined (SIL9136)
#define BSP_CFG_SIL9136			1
#endif

#if defined (IT6801)
#define BSP_CFG_IT6801			1
#endif

#if defined (USING_EEPROM)
#define BSP_CFG_EEPROM			1
#endif

#if defined (MODBUS_RTU_SLAVE)
#define BSP_CFG_MODBUS_RTU		1
#endif

#if defined (XPAR_XUARTLITE_NUM_INSTANCES) || defined (XPAR_XUARTPS_NUM_INSTANCES)
#define BSP_HAS_UART			1
#endif

#if defined (XPAR_XGPIOPS_NUM_INSTANCES)
#define BSP_HAS_PS_GPIO			1
#endif

#if defined (XPAR_XGPIO_NUM_INSTANCES) && defined (XPAR_XGPIO_I2C_0_AXI_GPIO_0_DEVICE_ID)
#define BSP_HAS_XGPIO_I2C		1
#endif

#if defined (XPAR_XGPIO_NUM_INSTANCES) && (XPAR_XGPIO_NUM_INSTANCES >= 2U)
#define BSP_HAS_AXI_GPIO		1
#endif

#if defined (XPAR_XIICPS_NUM_INSTANCES)
#define BSP_HAS_PS_I2C			1
#endif

#if defined (XPAR_XAXIS_SWITCH_NUM_INSTANCES)
#define BSP_HAS_AXIS_SWITCH		1
#endif

#if defined (XPAR_XCLK_WIZ_NUM_INSTANCES)
#define BSP_HAS_CLK_WIZ			1
#endif

#if defined (XPAR_XV_TPG_NUM_INSTANCES)
#define BSP_HAS_TPG			1
#endif

#if defined (XPAR_XVTC_NUM_INSTANCES)
#define BSP_HAS_VTC			1
#endif

#if defined (XPAR_XAXIVDMA_NUM_INSTANCES)
#define BSP_HAS_VDMA			1
#endif

#if defined (XPAR_XVPROCSS_NUM_INSTANCES)
#define BSP_HAS_VPSS			1
#endif

#if defined (XPAR_XV_DEMOSAIC_NUM_INSTANCES)
#define BSP_HAS_DEMOSAIC		1
#endif

#if defined (XPAR_XV_GAMMA_LUT_NUM_INSTANCES)
#define BSP_HAS_GAMMA_LUT		1
#endif

#if defined (XPAR_XCSISS_NUM_INSTANCES)
#define BSP_HAS_CSI_RX			1
#endif

#if defined (XPAR_XCSI2TX_NUM_INSTANCES)
#define BSP_HAS_CSI_TX			1
#endif

#if defined (XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES)
#define BSP_HAS_PIXEL_COMPARE		1
#endif

#if defined (XPAR_XTMRCTR_NUM_INSTANCES)
#define BSP_HAS_AXI_TIMER		1
#endif

#if defined (BSP_CFG_EEPROM) && defined (BSP_HAS_PS_I2C)
#define BSP_HAS_EEPROM			1
#endif

#if defined (BSP_HAS_NETWORK_SERVICES) && defined (BSP_CFG_VIDEO_STREAM) && defined (BSP_HAS_VDMA)
#define BSP_HAS_VIDEO_STREAM		1
#endif

#if defined (XPAR_XQSPIPSU_NUM_INSTANCES) || defined (XPAR_XQSPIPS_NUM_INSTANCES) || \
 (defined (XPAR_SPI_0_SPI_MODE) && (XPAR_SPI_0_SPI_MODE == 2U))
#define BSP_HAS_QSPI_FLASH		1
#endif

#if defined (BSP_HAS_XGPIO_I2C) && defined (BSP_CFG_SIL9136)
#define BSP_HAS_SIL9136			1
#endif

#if defined (BSP_CFG_MODBUS_RTU) && defined (BSP_HAS_PS_I2C) && defined (XPAR_XUARTLITE_NUM_INSTANCES) && \
 defined (INTC_DEVICE_ID) && defined (INTC)
#define BSP_HAS_MODBUS_RTU		1
#endif

#if defined (XPAR_XDPRXSS_NUM_INSTANCES) || defined (XPAR_XDPTXSS_NUM_INSTANCES)
#define BSP_HAS_DP			1
#endif

// 自定义外设库头文件
#if defined (BSP_HAS_XGPIO_I2C)
#include "xgpio_i2c/xgpio_i2c.h"
#endif
#if defined (BSP_HAS_CLK_WIZ)
#include "clk_wiz/clk_wiz.h"
#endif
#if defined (BSP_HAS_TPG)
#include "tpg/tpg.h"
#endif
#if defined (BSP_HAS_AXIS_SWITCH)
#include "xswitch/xswitch.h"
#endif
#if defined (BSP_HAS_VDMA)
#include "vdma/vdma.h"
#endif
#if defined (BSP_HAS_VTC)
#include "vtc/vtc.h"
#endif
#if defined (BSP_HAS_PS_GPIO)
#include "ps_gpio/ps_gpio.h"
#endif
#if defined (BSP_HAS_CSI_RX)
#include "csi_rx/csi_rx.h"
#endif
#if defined (BSP_HAS_CSI_TX)
#include "csi_tx/csi_tx.h"
#endif
#if defined (BSP_HAS_PS_I2C)
#include "ps_i2c/ps_i2c.h"
#endif
#if defined (BSP_HAS_EEPROM)
#include "eeprom/eeprom.h"
#endif

#if defined (BSP_HAS_DEMOSAIC)
#include "demosaic/demosaic.h"
#endif
#if defined (BSP_HAS_GAMMA_LUT)
#include "gamma_lut/gamma_lut.h"
#endif

#if defined (BSP_HAS_VPSS)
#include "vpss/vpss.h"
#endif
#if defined (BSP_HAS_QSPI_FLASH)
#include "qspi_flash/qspi_flash.h"
#endif
#if defined (BSP_HAS_AXI_TIMER)
#include "axi_timer/axi_timer.h"
#endif
#if defined (BSP_HAS_AXI_GPIO)
#include "xgpio/xgpio.h"
#endif
#if defined (BSP_HAS_SIL9136)
#include "sil9136/sil9136.h"
#endif
#if defined (__XGPIO_I2C_H__) && defined (ADS7128)
#include "ads7128/ads7128.h"
#endif
#if defined (__XGPIO_I2C_H__) && defined (IT6801)
#include "it6801/it6801.h"
#endif
#if defined (BSP_HAS_DP)
#include "dp/dp_app.h"
#endif
#if defined (BSP_HAS_MODBUS_RTU)
#include "modbus_slave/modbus_slave.h"
#include "modbus_slave/axi_uartlite_fifo/axi_uartlite_fifo.h"
#endif // BSP_HAS_MODBUS_RTU

// 自定义数据头文件
#if defined (SER_CFG) || defined (DES_CFG)
#include "serdes/serdes.h"
#endif

#if defined (BSP_HAS_PIXEL_COMPARE)
#include "axis_pixel_compare.h"
#include "pixel_compare/pixel_compare.h"
#endif

#if defined (UDP_UPDATE) || defined (TCP_UPDATE) || defined (TCP_COMMAND_SRV) || defined (UDP_COMMAND_SRV)
// lwip servers
#include "lwip_servers/lwip_common.h"
#include "lwip_servers/udp_cmd.h"
#include "lwip_servers/tcp_cmd.h"
// IAP methods
// udp remote update
#include "lwip_servers/udp_update.h"
// tcp remote update
#include "lwip_servers/tcp_update.h"
#include "md5_b/md5_b.h"
#include "lwip_servers/lwip_video.h"
#include "lwip_servers/settings.h"
#endif

#endif // __BSP_H__
