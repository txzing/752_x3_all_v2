#include "../bsp.h"

#if defined (XPAR_XTMRCTR_NUM_INSTANCES)

#if (XPAR_XTMRCTR_NUM_INSTANCES >= 1U)
XTmrCtr axi_timer0;
#endif // (XPAR_XTMRCTR_NUM_INSTANCES >= 1U)

#if (XPAR_XTMRCTR_NUM_INSTANCES >= 2U)
XTmrCtr axi_timer1;
#endif // (XPAR_XTMRCTR_NUM_INSTANCES >= 2U)

#if (XPAR_XTMRCTR_NUM_INSTANCES >= 3U)
XTmrCtr axi_timer2;
#endif // (XPAR_XTMRCTR_NUM_INSTANCES >= 3U)

#if (XPAR_XTMRCTR_NUM_INSTANCES >= 4U)
XTmrCtr axi_timer3;
#endif // (XPAR_XTMRCTR_NUM_INSTANCES >= 4U)


volatile u8 timer_cnt;
u8 wave_flag;
#if 1
void timer_intr_hander(void *InstancePtr) //
{
    //
    if (XTmrCtr_IsExpired(&axi_timer0, TIMER_CNTR_0))
    {
//    	xil_printf("timer_intr_hander\r\n");
    	timer_cnt = timer_cnt + 1;
    }
}


#define US (XPAR_TMRCTR_0_CLOCK_FREQ_HZ / 1000000)
#define MS (1000*US)
#define TIMER_TLR  200*MS
int lock_timer_init(void)
{
	int Status;
	INTC *IntcInstPtr = &InterruptController;
	/***********************************************************/
	XTmrCtr_Initialize(&axi_timer0, XPAR_TMRCTR_0_DEVICE_ID);
	/*
	 * Perform a self-test to ensure that the hardware was built
	 * correctly, use the 1st timer in the device (0)
	 */
	Status = XTmrCtr_SelfTest(&axi_timer0, TIMER_CNTR_0);
	if (Status != XST_SUCCESS) {
		xil_printf("XTmrCtr_SelfTest TIMER_CNTR_0 FAILD\r\n");
		return XST_FAILURE;
	}
	//
	XTmrCtr_SetOptions(&axi_timer0, TIMER_CNTR_0, XTC_INT_MODE_OPTION | //
	XTC_AUTO_RELOAD_OPTION | //
	XTC_DOWN_COUNT_OPTION); //

	//
	XTmrCtr_SetResetValue(&axi_timer0, TIMER_CNTR_0, TIMER_TLR);//100Mhz,50ms
	//

#if 0
	Status = XTmrCtr_SelfTest(&axi_timer0, TIMER_CNTR_1);
	if (Status != XST_SUCCESS) {
		xil_printf("XTmrCtr_SelfTest TIMER_CNTR_1 FAILD\r\n");
		return XST_FAILURE;
	}
	//
	XTmrCtr_SetOptions(&axi_timer0, TIMER_CNTR_1, XTC_INT_MODE_OPTION | //
	XTC_AUTO_RELOAD_OPTION | //
	XTC_DOWN_COUNT_OPTION); //

	//
	XTmrCtr_SetResetValue(&axi_timer0, TIMER_CNTR_1, 5000000);//100Mhz,50ms

#endif

	XTmrCtr_SetHandler(&axi_timer0, timer_intr_hander, &axi_timer0);

#if defined (XPAR_XSCUGIC_NUM_INSTANCES)
	XScuGic_SetPriorityTriggerType(IntcInstPtr, XPAR_FABRIC_TMRCTR_0_VEC_ID, 0xA0, 0x3);
#endif
	/*
	 * Connect the interrupt handler that will be called when an
	 * interrupt occurs for the device.
	 */
	Status = INTC_CONNECT(IntcInstPtr, XPAR_FABRIC_TMRCTR_0_VEC_ID,
				 (Xil_ExceptionHandler)XTmrCtr_InterruptHandler,
				 &axi_timer0);
	if (Status != XST_SUCCESS) {
		xil_printf("ERR: lock_timer_init Timer interrupt connect failed!\n\r");
		return XST_FAILURE;
	}

	/*
	 * Enable the interrupt for the Timer device.
	 */
	INTC_CONNECT_ENABLE(IntcInstPtr, XPAR_FABRIC_TMRCTR_0_VEC_ID);


	// Don't forget start timer in proper place
	XTmrCtr_Start(&axi_timer0, TIMER_CNTR_0);
//	XTmrCtr_Start(&axi_timer0, TIMER_CNTR_1);

	return Status;
}
#endif

#ifdef MODBUS_RTU_SLAVE

#define MODBUS_TIMER_ID 				XPAR_PROCESSOR_SUBSYSTEM_RS485_HIER_AXI_TIMER_0_DEVICE_ID
#define MODBUS_TIMER_INTERRUPT_VEC_ID 	XPAR_FABRIC_PROCESSOR_SUBSYSTEM_RS485_HIER_AXI_TIMER_0_INTERRUPT_INTR
#define MODBUS_US 						(XPAR_PROCESSOR_SUBSYSTEM_RS485_HIER_AXI_TIMER_0_CLOCK_FREQ_HZ / 1000000)
#define MODBUS_MS (1000*MODBUS_US)

XTmrCtr axi_modbus_timer;
volatile uint8_t g_mods_timeout;
volatile uint16_t modbus_us;
volatile uint16_t modbus_ms;

void modbus_intr_hander(void *InstancePtr) //
{
    //
    if (XTmrCtr_IsExpired(&axi_modbus_timer, TIMER_CNTR_0))
    {
//    	xil_printf("modbus_intr_hander\r\n");
    	g_mods_timeout = 1;
    }
}

int modbus_timer_init(void)
{
	int Status;
	modbus_us = MODBUS_US;
	modbus_ms = MODBUS_MS;
	INTC *IntcInstPtr = &InterruptController;
	/***********************************************************/
	XTmrCtr_Initialize(&axi_modbus_timer, MODBUS_TIMER_ID);
	/*
	 * Perform a self-test to ensure that the hardware was built
	 * correctly, use the 1st timer in the device (0)
	 */
	Status = XTmrCtr_SelfTest(&axi_modbus_timer, TIMER_CNTR_0);
	if (Status != XST_SUCCESS) {
		xil_printf("XTmrCtr_SelfTest TIMER_CNTR_0 FAILD\r\n");
		return XST_FAILURE;
	}
	//
	XTmrCtr_SetOptions(&axi_modbus_timer, TIMER_CNTR_0, XTC_INT_MODE_OPTION | //
	XTC_AUTO_RELOAD_OPTION | //
	XTC_DOWN_COUNT_OPTION); //

	//
	XTmrCtr_SetResetValue(&axi_modbus_timer, TIMER_CNTR_0, 20*modbus_ms);//
	//

#if 0
	Status = XTmrCtr_SelfTest(&axi_modbus_timer, TIMER_CNTR_1);
	if (Status != XST_SUCCESS) {
		xil_printf("XTmrCtr_SelfTest TIMER_CNTR_1 FAILD\r\n");
		return XST_FAILURE;
	}
	//
	XTmrCtr_SetOptions(&axi_modbus_timer, TIMER_CNTR_1, XTC_INT_MODE_OPTION | //
	XTC_AUTO_RELOAD_OPTION | //
	XTC_DOWN_COUNT_OPTION); //

	//
	XTmrCtr_SetResetValue(&axi_modbus_timer, TIMER_CNTR_1, 5000000);

#endif

	XTmrCtr_SetHandler(&axi_modbus_timer, modbus_intr_hander, &axi_modbus_timer);

#if defined (XPAR_XSCUGIC_NUM_INSTANCES)
	XScuGic_SetPriorityTriggerType(IntcInstPtr, MODBUS_TIMER_INTERRUPT_VEC_ID, 0xA8, 0x3);
#endif

	Status = INTC_CONNECT(IntcInstPtr, MODBUS_TIMER_INTERRUPT_VEC_ID,
				(XInterruptHandler)XTmrCtr_InterruptHandler,
				&axi_modbus_timer);
	if (Status != XST_SUCCESS)
	{
		xil_printf("ERR: modbus_timer_init Timer interrupt connect failed!\n\r");
		return XST_FAILURE;
	}
	INTC_CONNECT_ENABLE(IntcInstPtr, MODBUS_TIMER_INTERRUPT_VEC_ID);

	// Don't forget start timer in proper place
	XTmrCtr_Start(&axi_modbus_timer, TIMER_CNTR_0);
//	XTmrCtr_Start(&axi_modbus_timer, TIMER_CNTR_1);

	return Status;
}

void modbus_timer_start(uint32_t _uiTimeOut)
{
	XTmrCtr_SetResetValue(&axi_modbus_timer, TIMER_CNTR_0, _uiTimeOut);//
	XTmrCtr_Start(&axi_modbus_timer, TIMER_CNTR_0);
}



#endif //#ifdef MODBUS_RTU_SLAVE



#endif //#if defined (XPAR_XTMRCTR_NUM_INSTANCES)


