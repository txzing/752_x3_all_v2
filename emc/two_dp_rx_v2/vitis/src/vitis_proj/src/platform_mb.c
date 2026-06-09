#include "bsp.h"


#if defined (__MICROBLAZE__)

#if defined (INTC_DEVICE_ID) || defined (INTC)
INTC InterruptController;
#endif

void enable_caches(void)
{
#ifdef XPAR_MICROBLAZE_USE_ICACHE
    Xil_ICacheInvalidate();
    Xil_ICacheEnable();
#endif
#ifdef XPAR_MICROBLAZE_USE_DCACHE
    Xil_DCacheInvalidate();
    Xil_DCacheEnable();
#endif
}

void disable_caches(void)
{
#ifdef __MICROBLAZE__
#ifdef XPAR_MICROBLAZE_USE_DCACHE
    Xil_DCacheDisable();
#endif
#ifdef XPAR_MICROBLAZE_USE_ICACHE
    Xil_ICacheDisable();
#endif
#endif
}

#if defined (UDP_UPDATE) || defined (TCP_UPDATE) || defined (TCP_COMMAND_SRV) || defined (UDP_COMMAND_SRV)
#if defined (__AXI_TIMER_H_) && defined (XPAR_XAXIETHERNET_NUM_INSTANCES)

#if !defined (XPAR_XAXIETHERNET_NUM_INSTANCES)
#error "No ethernet in design"
#endif

#if LWIP_DHCP==1
volatile int dhcp_timoutcntr = 24;
void dhcp_fine_tmr();
void dhcp_coarse_tmr();
#endif

volatile int TcpFastTmrFlag = 0;
volatile int TcpSlowTmrFlag = 0;

volatile u64_t tickcntr = 0;

#define PLATFORM_EMAC_BASEADDR XPAR_AXIETHERNET_0_BASEADDR

#define PLATFORM_TIMER_BASEADDR XPAR_PROCESSOR_SUBSYSTEM_AXI_TIMER_0_BASEADDR
#define PLATFORM_TIMER_INTERRUPT_INTR XPAR_PROCESSOR_SUBSYSTEM_MICROBLAZE_0_AXI_INTC_PROCESSOR_SUBSYSTEM_AXI_TIMER_0_INTERRUPT_INTR
#define PLATFORM_TIMER_INTERRUPT_MASK (1 << XPAR_PROCESSOR_SUBSYSTEM_MICROBLAZE_0_AXI_INTC_PROCESSOR_SUBSYSTEM_AXI_TIMER_0_INTERRUPT_INTR)
#define PLATFORM_TIMER_CLOCK_FREQ_HZ XPAR_PROCESSOR_SUBSYSTEM_AXI_TIMER_0_CLOCK_FREQ_HZ

void timer_callback()
{
	/* we need to call tcp_fasttmr & tcp_slowtmr at intervals specified
	 * by lwIP.
	 * It is not important that the timing is absoluetly accurate.
	 */
	static int odd = 1;
#if LWIP_DHCP==1
	static int dhcp_timer = 0;
#endif
	tickcntr++;
	if(tickcntr % 25 == 0){
		TcpFastTmrFlag = 1;

		odd = !odd;
		if (odd) {

#if LWIP_DHCP==1
			dhcp_timer++;
			dhcp_timoutcntr--;
#endif
			TcpSlowTmrFlag = 1;
#if LWIP_DHCP==1
			dhcp_fine_tmr();
			if (dhcp_timer >= 120) {
				dhcp_coarse_tmr();
				dhcp_timer = 0;
			}
#endif
		}
	}
}

void xadapter_timer_handler(void *p)
{
	timer_callback();

	/* Load timer, clear interrupt bit */
	XTmrCtr_SetControlStatusReg(PLATFORM_TIMER_BASEADDR, 0,
			XTC_CSR_INT_OCCURED_MASK
			| XTC_CSR_LOAD_MASK);

	XTmrCtr_SetControlStatusReg(PLATFORM_TIMER_BASEADDR, 0,
			XTC_CSR_ENABLE_TMR_MASK
			| XTC_CSR_ENABLE_INT_MASK
			| XTC_CSR_AUTO_RELOAD_MASK
			| XTC_CSR_DOWN_COUNT_MASK);

	XIntc_AckIntr(XPAR_INTC_0_BASEADDR, PLATFORM_TIMER_INTERRUPT_MASK);
}


#define US (PLATFORM_TIMER_CLOCK_FREQ_HZ / 1000000)
#define MS (1000*US)
#define TIMER_TLR  50*MS

void platform_setup_timer()
{
	/* set the number of cycles the timer counts before interrupting */
	/* 100 Mhz clock => .01us for 1 clk tick. For 100ms, 10000000 clk ticks need to elapse  */
	XTmrCtr_SetLoadReg(PLATFORM_TIMER_BASEADDR, 0, TIMER_TLR);

	/* reset the timers, and clear interrupts */
	XTmrCtr_SetControlStatusReg(PLATFORM_TIMER_BASEADDR, 0, XTC_CSR_INT_OCCURED_MASK | XTC_CSR_LOAD_MASK );

	/* start the timers */
	XTmrCtr_SetControlStatusReg(PLATFORM_TIMER_BASEADDR, 0,
			XTC_CSR_ENABLE_TMR_MASK | XTC_CSR_ENABLE_INT_MASK
			| XTC_CSR_AUTO_RELOAD_MASK | XTC_CSR_DOWN_COUNT_MASK);

	/* Register Timer handler */
	XIntc_RegisterHandler(XPAR_INTC_0_BASEADDR,
			PLATFORM_TIMER_INTERRUPT_INTR,
			(XInterruptHandler)xadapter_timer_handler,
			0);
}

#endif // #if defined (__AXI_TIMER_H_) && defined (XPAR_XAXIETHERNET_NUM_INSTANCES)
#endif // #if defined (UDP_UPDATE) || defined (TCP_UPDATE) || defined (TCP_COMMAND_SRV) || defined (UDP_COMMAND_SRV)



#if defined (INTC_DEVICE_ID) || defined (INTC)

int platform_setup_interrupts(void)
{
	int Status;

	Status = XIntc_Initialize(&InterruptController, XPAR_INTC_0_DEVICE_ID);
	if (Status != XST_SUCCESS)
	{
		bsp_printf(TXT_RED "XIntc_Initialize Failed\r\n" TXT_RST);
		return XST_FAILURE;
	}
	/* Start the interrupt controller */
	XIntc_MasterEnable(INTC_BASE_ADDR);

#ifdef __MICROBLAZE__
	microblaze_register_handler((XInterruptHandler)XIntc_InterruptHandler, &InterruptController);
#endif

#if defined (UDP_UPDATE) || defined (TCP_UPDATE) || defined (TCP_COMMAND_SRV) || defined (UDP_COMMAND_SRV)
#if defined (__AXI_TIMER_H_) && defined (XPAR_XAXIETHERNET_NUM_INSTANCES)
	platform_setup_timer();
#endif // #if defined (__AXI_TIMER_H_) && defined (XPAR_ETHERNET_SUBSYSTEM_AXI_TIMER_0_DEVICE_ID)
#endif // #if defined (UDP_UPDATE) || defined (TCP_UPDATE) || defined (TCP_COMMAND_SRV) || defined (UDP_COMMAND_SRV)


#ifdef XPAR_ETHERNET_MAC_IP2INTC_IRPT_MASK
	/* Enable timer and EMAC interrupts in the interrupt controller */
	XIntc_EnableIntr(XPAR_INTC_0_BASEADDR,
#ifdef __MICROBLAZE__
			PLATFORM_TIMER_INTERRUPT_MASK |
#endif
			XPAR_ETHERNET_MAC_IP2INTC_IRPT_MASK);
#endif


#ifdef XPAR_INTC_0_LLTEMAC_0_VEC_ID
#ifdef __MICROBLAZE__
	XIntc_Enable(&InterruptController, PLATFORM_TIMER_INTERRUPT_INTR);
#endif
	XIntc_Enable(&InterruptController, XPAR_INTC_0_LLTEMAC_0_VEC_ID);
#endif

#if defined (UDP_UPDATE) || defined (TCP_UPDATE) || defined (TCP_COMMAND_SRV) || defined (UDP_COMMAND_SRV)
#ifdef XPAR_INTC_0_AXIETHERNET_0_VEC_ID
	XIntc_Enable(&InterruptController, PLATFORM_TIMER_INTERRUPT_INTR);
	XIntc_Enable(&InterruptController, XPAR_INTC_0_AXIETHERNET_0_VEC_ID);
#endif
#endif

#ifdef XPAR_INTC_0_EMACLITE_0_VEC_ID
#ifdef __MICROBLAZE__
	XIntc_Enable(&InterruptController, PLATFORM_TIMER_INTERRUPT_INTR);
#endif
	XIntc_Enable(&InterruptController, XPAR_INTC_0_EMACLITE_0_VEC_ID);
#endif

	return Status;
}

void platform_enable_interrupts()
{
	int Status ;
	Status = XIntc_Start(&InterruptController, XIN_REAL_MODE);
	if (Status != XST_SUCCESS)
	{
		bsp_printf(TXT_RED "XIntc_Start Failed\r\n" TXT_RST);
		return XST_FAILURE;
	}

	Xil_ExceptionInit();
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT, (XExceptionHandler)INTC_HANDLER, &InterruptController);
	Xil_ExceptionEnable();
//	microblaze_enable_interrupts();
}

#endif // #if defined (INTC_DEVICE_ID) || defined (INTC)

void init_platform(void)
{
    enable_caches();
#if defined (INTC_DEVICE_ID) || defined (INTC)
	platform_setup_interrupts();
#endif // #if defined (INTC_DEVICE_ID) || defined (INTC)

}

void cleanup_platform(void)
{
    disable_caches();
}

#if defined (UDP_UPDATE) || defined (TCP_UPDATE) || defined (TCP_COMMAND_SRV) || defined (UDP_COMMAND_SRV)
u64_t get_time_ms()
{
	return tickcntr * 10;
}
#endif

#endif // __MICROBLAZE__

