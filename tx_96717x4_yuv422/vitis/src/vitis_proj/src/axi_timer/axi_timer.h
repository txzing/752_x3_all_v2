#ifndef __AXI_TIMER_H_

#if defined (XPAR_XTMRCTR_NUM_INSTANCES)
#define __AXI_TIMER_H_
//#include "xparameters.h"
#include "xtmrctr.h"

#if (XPAR_XTMRCTR_NUM_INSTANCES >= 1U)
extern XTmrCtr axi_timer0;
#endif // (XPAR_XTMRCTR_NUM_INSTANCES >= 1U)

#if (XPAR_XTMRCTR_NUM_INSTANCES >= 2U)
extern XTmrCtr axi_timer1;
#endif // (XPAR_XTMRCTR_NUM_INSTANCES >= 2U)

#if (XPAR_XTMRCTR_NUM_INSTANCES >= 3U)
extern XTmrCtr axi_timer2;
#endif // (XPAR_XTMRCTR_NUM_INSTANCES >= 3U)

#if (XPAR_XTMRCTR_NUM_INSTANCES >= 4U)
extern XTmrCtr axi_timer3;
#endif // (XPAR_XTMRCTR_NUM_INSTANCES >= 4U)


#define TIMER_CNTR_0	 0
#define TIMER_CNTR_1	 1

extern volatile u8 timer_cnt;

void timer_intr_hander(void *InstancePtr);
int lock_timer_init(void);


#ifdef MODBUS_RTU_SLAVE
	extern XTmrCtr axi_modbus_timer;
	extern volatile uint8_t g_mods_timeout;
	extern volatile uint16_t modbus_us;
	extern volatile uint16_t modbus_ms;
	void modbus_timer_start(uint32_t _uiTimeOut);
#endif


#endif // XPAR_XTMRCTR_NUM_INSTANCES

#endif // __AXI_TIMER_H_
