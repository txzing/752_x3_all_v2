#include "bsp.h"

#if defined (XPAR_AXI_LITE_REG_NUM_INSTANCES) && (XPAR_AXI_LITE_REG_0_DEVICE_ID == 0)
volatile u32 __HW_VER__;
#endif
extern u8 current_ch;

int main()
{
	int Status ;
	current_ch = 1;//default
	timer_cnt = 0;

    init_platform(); // include interrupts setup

    bsp_printf("\r\n\r\n***************************\n\r");
    bsp_printf("test for max96752_x3\r\n");
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


#if defined (XPAR_XGPIOPS_NUM_INSTANCES)
	PsGpioSetup(&Gpio,XPAR_PSU_GPIO_0_DEVICE_ID);
	xgpio_setup();
#endif

#if defined(XPAR_XGPIO_I2C_0_AXI_GPIO_0_DEVICE_ID)
    Status = xgpio_i2c_init();
    if (Status != XST_SUCCESS)
	{
    	bsp_printf(TXT_RED "\r\n__FILE__:%s, __LINE__:%d\r\n" TXT_RST,__FILE__, __LINE__);
		return XST_FAILURE ;
	}
#endif // XPAR_XGPIO_I2C_0_AXI_GPIO_0_DEVICE_ID

#if defined (XPAR_XAXIS_SWITCH_NUM_INSTANCES)
    Status = axis_switch_cfg();
    if (Status != XST_SUCCESS)
	{
    	bsp_printf(TXT_RED "\r\n__FILE__:%s, __LINE__:%d\r\n" TXT_RST,__FILE__, __LINE__);
		return XST_FAILURE ;
	}
#endif // XPAR_XAXIS_SWITCH_NUM_INSTANCES

#if defined (MODBUS_RTU_SLAVE)
    ps_i2c_init(&ps_i2c_0, XPAR_XIICPS_0_DEVICE_ID, 100000);
    Uart_Var_Init();
    Uart_Init_Rs485(&InterruptController, UART_DEVICE_ID);
	Status = modbus_timer_init();
	if (Status != XST_SUCCESS)
	{
		bsp_printf(TXT_RED "\r\n__FILE__:%s, __LINE__:%d\r\n" TXT_RST,__FILE__, __LINE__);
		Xil_Assert(__FILE__, __LINE__);
		return XST_FAILURE ;
	}

	read_id();
#endif // MODBUS_RTU_SLAVEs

#if defined (XPAR_XCLK_WIZ_NUM_INSTANCES)
    Status = clkwiz_vtc_cfg();
    if (Status != XST_SUCCESS)
	{
    	bsp_printf(TXT_RED "\r\n__FILE__:%s, __LINE__:%d\r\n" TXT_RST,__FILE__, __LINE__);
		return XST_FAILURE ;
	}
#endif // XPAR_XVTC_NUM_INSTANCES

#if defined (XPAR_XV_TPG_NUM_INSTANCES)
    Status = tpg_config();
    if (Status != XST_SUCCESS)
	{
    	bsp_printf(TXT_RED "\r\n__FILE__:%s, __LINE__:%d\r\n" TXT_RST,__FILE__, __LINE__);
		return XST_FAILURE ;
	}
#endif // XPAR_XV_TPG_NUM_INSTANCES

#if defined (XPAR_XAXIVDMA_NUM_INSTANCES)
    clear_display();
    vdma_config();
#if defined (UDP_UPDATE) || defined (TCP_UPDATE) || defined (TCP_COMMAND_SRV) || defined (UDP_COMMAND_SRV)
#if defined (UDP_VIDEO) || defined (TCP_VIDEO)
    clear_vdma_instance(XPAR_AXI_VDMA_LWIP_DEVICE_ID);
    vdma_udp_init();
    /* 按默认 current_ch 与 monitor/缓存分辨率对齐 S2MM，避免一直用静态初始化里的 1920x1080 */
    vdma_lwip_apply_channel_geometry(current_ch);
#endif
#endif
#endif // XPAR_XAXIVDMA_NUM_INSTANCES


#if defined(SIL9136)
    sil9136_config();
#endif


#if defined (XPAR_XVPROCSS_NUM_INSTANCES)

	Status = vpss_config_color_Space_Conversion(XPAR_XVPROCSS_0_DEVICE_ID);
	if (Status != XST_SUCCESS)
	{
		bsp_printf(TXT_RED "\r\n__FILE__:%s, __LINE__:%d\r\n" TXT_RST,__FILE__, __LINE__);
		return XST_FAILURE ;
	}

#endif // XPAR_XVPROCSS_NUM_INSTANCES

#if defined (XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES)
	PixelCompare_init();
#endif

	lock_timer_init();

#if defined (SER_CFG) || defined (DES_CFG)
	usleep(10*1000);

	serdes_i2c_write_array_16(I2C_NO_1, max96752_oldi);
	serdes_i2c_write_array_16(I2C_NO_2, max96752_oldi);
	serdes_i2c_write_array_16(I2C_NO_3, max96752_oldi);

	xgpio_i2c_reg16_write(I2C_NO_2, 0x90>>1, 0x0050, 0x01, STRETCH_ON);


#endif // SER_CFG || DES_CFG

#if defined (UDP_UPDATE) || defined (TCP_UPDATE) || defined (TCP_COMMAND_SRV) || defined (UDP_COMMAND_SRV)
    init_default_config();
    ps_i2c_config();
    loadconfig(&global_config);
	lwip_common_init(&server_netif);
#endif // #if defined (UDP_UPDATE) || defined (TCP_UPDATE) || defined (TCP_COMMAND_SRV) || defined (UDP_COMMAND_SRV)

#if defined (INTC_DEVICE_ID) || defined (INTC)
	platform_enable_interrupts();
#endif //#if defined (INTC_DEVICE_ID) || defined (INTC)

	app_info();
	xil_printf("\r\nstart\r\n");
	XGpio_DiscreteWrite(&XGpioOutput_oldi, 1, 0x7);
	usleep(10*1000);
	XGpio_DiscreteWrite(&XGpioOutput_oldi, 1, 0x0);

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

#if defined (BSP_HAS_VDMA) && defined (XPAR_AXI_PASSTHROUGH_MONITOR_NUM_INSTANCES) \
	&& defined (XPAR_XAXIS_SWITCH_NUM_INSTANCES)
    	switch_run();
#endif
    }

	// never reached
    cleanup_platform();
    return 0;
}
