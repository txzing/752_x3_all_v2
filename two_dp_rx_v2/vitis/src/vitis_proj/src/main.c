#include "bsp.h"

#if defined (XPAR_AXI_LITE_REG_NUM_INSTANCES) && (XPAR_AXI_LITE_REG_0_DEVICE_ID == 0)
volatile u32 __HW_VER__;
#endif

int main()
{
	int Status ;
	current_ch = 1;//default
	switch_ch = current_ch;
	timer_cnt = 0;

    init_platform(); // include interrupts setup

    bsp_printf("\r\n\r\n***************************\n\r");
    bsp_printf("test for dual_link--max96856&&max96772\r\n");
    bsp_printf("test for rgb\r\n");
    bsp_printf("\r\n%s, UTC %s\r\n",__DATE__,__TIME__);
    bsp_printf(TXT_RED "\r\n__FILE__:%s, __LINE__:%d\r\n" TXT_RST,__FILE__, __LINE__);
#if defined (XPAR_AXI_LITE_REG_NUM_INSTANCES) && (XPAR_AXI_LITE_REG_0_DEVICE_ID == 0)
	__HW_VER__ = AXI_LITE_REG_mReadReg(XPAR_PROCESSOR_SUBSYSTEM_AXI_LITE_REG_0_S00_AXI_BASEADDR, AXI_LITE_REG_S00_AXI_SLV_REG0_OFFSET);
//	__HW_VER__ = AXI_LITE_REG_mReadReg(XPAR_AXI_LITE_REG_0_S00_AXI_BASEADDR, AXI_LITE_REG_S00_AXI_SLV_REG0_OFFSET);
	bsp_printf(TXT_GREEN "hardware ver = 0x%08x\n\r" TXT_RST, __HW_VER__);
#endif // XPAR_AXI_LITE_REG_NUM_INSTANCES
#ifdef SW_VER_BY_COMPILE_TIME
    __SW_VER__ = GetSoftWareVersion();
    bsp_printf("software ver = 0x%08x\n\r", __SW_VER__);
    bsp_printf("***************************\n\r");

#elif defined (__SW_VER__)
//    bsp_printf("software ver = 0x%08x\n\r", __SW_VER__);
    bsp_printf(TXT_GREEN "software ver = 0x%08x\n\r" TXT_RST, __SW_VER__);
    bsp_printf("***************************\n\r");
#endif // __SW_VER__ || SW_VER_BY_COMPILE_TIME

#if (XPAR_XGPIO_NUM_INSTANCES >= 2U)
    Status = xgpio_setup();
    if (Status != XST_SUCCESS)
	{
    	bsp_printf(TXT_RED "\r\n__FILE__:%s, __LINE__:%d\r\n" TXT_RST,__FILE__, __LINE__);
		Xil_Assert(__FILE__, __LINE__);
		return XST_FAILURE ;
	}
#endif

#if defined(XPAR_XGPIO_I2C_0_AXI_GPIO_0_DEVICE_ID)
    Status = xgpio_i2c_init();
    if (Status != XST_SUCCESS)
	{
    	bsp_printf(TXT_RED "\r\n__FILE__:%s, __LINE__:%d\r\n" TXT_RST,__FILE__, __LINE__);
		Xil_Assert(__FILE__, __LINE__);
		return XST_FAILURE ;
	}
#endif // XPAR_XGPIO_I2C_0_AXI_GPIO_0_DEVICE_ID


#if defined (XPAR_XAXIS_SWITCH_NUM_INSTANCES)
    Status = axis_switch_cfg();
    if (Status != XST_SUCCESS)
	{
    	bsp_printf(TXT_RED "\r\n__FILE__:%s, __LINE__:%d\r\n" TXT_RST,__FILE__, __LINE__);
		Xil_Assert(__FILE__, __LINE__);
		return XST_FAILURE ;
	}
#endif // XPAR_XAXIS_SWITCH_NUM_INSTANCES

#if defined (XPAR_XCLK_WIZ_NUM_INSTANCES)
    Status = clkwiz_config();
    if (Status != XST_SUCCESS)
	{
    	bsp_printf(TXT_RED "\r\n__FILE__:%s, __LINE__:%d\r\n" TXT_RST,__FILE__, __LINE__);
		Xil_Assert(__FILE__, __LINE__);
		return XST_FAILURE ;
	}
#endif // XPAR_XCLK_WIZ_NUM_INSTANCES

#if defined (XPAR_XV_TPG_NUM_INSTANCES)
    Status = tpg_config();
    if (Status != XST_SUCCESS)
	{
    	bsp_printf(TXT_RED "\r\n__FILE__:%s, __LINE__:%d\r\n" TXT_RST,__FILE__, __LINE__);
		Xil_Assert(__FILE__, __LINE__);
		return XST_FAILURE ;
	}
#endif // XPAR_XV_TPG_NUM_INSTANCES

#if defined (XPAR_XAXIVDMA_NUM_INSTANCES)
    clear_display();
    vdma_config();
#if defined (UDP_UPDATE) || defined (TCP_UPDATE) || defined (TCP_COMMAND_SRV) || defined (UDP_COMMAND_SRV)
#if defined (UDP_VIDEO) || defined (TCP_VIDEO)
    clear_vdma_0();
    vdma_udp_init();
#endif
#endif
#endif // XPAR_XAXIVDMA_NUM_INSTANCES

#if defined (XPAR_XVPROCSS_NUM_INSTANCES)

	Status = vpss_config_scaler_0(XPAR_XVPROCSS_0_DEVICE_ID);
	if (Status != XST_SUCCESS)
	{
		bsp_printf(TXT_RED "\r\n__FILE__:%s, __LINE__:%d\r\n" TXT_RST,__FILE__, __LINE__);
		return XST_FAILURE ;
	}

#endif // XPAR_XVPROCSS_NUM_INSTANCES

#if defined (XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES)
	PixelCompare_init();
#endif

#if defined (XPAR_XTMRCTR_NUM_INSTANCES)
	Status = lock_timer_init();
	if (Status != XST_SUCCESS)
	{
		bsp_printf(TXT_RED "\r\n__FILE__:%s, __LINE__:%d\r\n" TXT_RST,__FILE__, __LINE__);
		Xil_Assert(__FILE__, __LINE__);
		return XST_FAILURE ;
	}
#endif // XPAR_XVPROCSS_NUM_INSTANCES


#if defined (SER_CFG) || defined (DES_CFG)

	serdes_i2c_write_array_16(I2C_NO_1, max96856_dual_link);
	serdes_i2c_write_array_16(I2C_NO_2, max96772_dual_link);

#endif // SER_CFG || DES_CFG

#if defined (UDP_UPDATE) || defined (TCP_UPDATE) || defined (TCP_COMMAND_SRV) || defined (UDP_COMMAND_SRV)
    init_default_config();
    loadconfig(&global_config);
	lwip_common_init(&server_netif);
#endif // #if defined (UDP_UPDATE) || defined (TCP_UPDATE) || defined (TCP_COMMAND_SRV) || defined (UDP_COMMAND_SRV)

#if defined (XPAR_XDPRXSS_NUM_INSTANCES) || defined (XPAR_XDPTXSS_NUM_INSTANCES)
	dp_init();
#endif // XPAR_XVPROCSS_NUM_INSTANCES

#if defined (INTC_DEVICE_ID) || defined (INTC)
	platform_enable_interrupts();
#endif //#if defined (INTC_DEVICE_ID) || defined (INTC)

	app_info();
	xil_printf("\r\nstart\r\n");


    while(1)
    {
#if defined (UDP_UPDATE) || defined (TCP_UPDATE) || defined (TCP_COMMAND_SRV) || defined (UDP_COMMAND_SRV)
    	transfer_data(&server_netif);
#if defined (UDP_VIDEO) || defined (TCP_VIDEO)
    	lwip_video_transfer();
#if defined (XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES)
    	pixel_err_handel();
#endif
#endif
#endif // #if defined (UDP_UPDATE) || defined (TCP_UPDATE) || defined (TCP_COMMAND_SRV) || defined (UDP_COMMAND_SRV)

#if	defined (MODBUS_RTU_SLAVE)
		MODS_Poll();
//		MODS_Switch();
#endif

    	uart_receive_process();

    	display_fresh();

    	switch_run();
    }

	// never reached
    cleanup_platform();
    return 0;
}
