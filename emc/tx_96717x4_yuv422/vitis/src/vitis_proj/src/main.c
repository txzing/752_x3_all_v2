#include "bsp.h"

#if defined (XPAR_AXI_LITE_REG_NUM_INSTANCES) && (XPAR_AXI_LITE_REG_0_DEVICE_ID == 0)
volatile u32 __HW_VER__;
#endif


int main()
{
	int Status ;
	current_ch = 1;//default
	timer_cnt = 0;

    init_platform(); // include interrupts setup

    bsp_printf("\r\n\r\n***************************\n\r");
    bsp_printf("test for max96717f_x4\r\n");
    bsp_printf("for emc board\r\n");
    xil_printf("---yuv422 ---\r\n");
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
#elif defined (USING_EEPROM) && defined (XPAR_XIICPS_NUM_INSTANCES)
	/* Geometry NVM needs PS I2C before video_inject_apply_default(). */
	Status = ps_i2c_config();
	if (Status != XST_SUCCESS)
	{
		bsp_printf(TXT_RED "\r\n__FILE__:%s, __LINE__:%d\r\n" TXT_RST,__FILE__, __LINE__);
		return XST_FAILURE ;
	}
#endif // MODBUS_RTU_SLAVE / USING_EEPROM


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
    clear_vdma_0();
    vdma_udp_init();
#endif
#endif
#endif // XPAR_XAXIVDMA_NUM_INSTANCES

#if defined (XPAR_XCSI2TX_NUM_INSTANCES)

	Csi2TxSs_Init_0(XPAR_CSI2TXSS_0_DEVICE_ID);
	Csi2TxSs_Init_1(XPAR_CSI2TXSS_1_DEVICE_ID);
	Csi2TxSs_Init_2(XPAR_CSI2TXSS_2_DEVICE_ID);
	Csi2TxSs_Init_3(XPAR_CSI2TXSS_3_DEVICE_ID);

#endif

	/* Resolve geom (EEPROM or factory) and start TPG/VTC/CSI once. */
	Status = video_inject_apply_default();
	if (Status != XST_SUCCESS)
	{
		bsp_printf(TXT_RED "\r\n__FILE__:%s, __LINE__:%d\r\n" TXT_RST,__FILE__, __LINE__);
		return XST_FAILURE ;
	}

    adc_config();

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
	usleep(10*1000);

	serdes_i2c_write_array_16(I2C_NO_0, max96717_yuv422_gmsl2);
	serdes_i2c_write_array_16(I2C_NO_1, max96717_yuv422_gmsl2);
	serdes_i2c_write_array_16(I2C_NO_2, max96717_yuv422_gmsl2);
	serdes_i2c_write_array_16(I2C_NO_3, max96717_yuv422_gmsl2);

	xgpio_i2c_reg16_write(I2C_NO_0, 0x80>>1, 0x0000, 0x20, STRETCH_ON);
	xgpio_i2c_reg16_write(I2C_NO_1, 0x80>>1, 0x0000, 0x20, STRETCH_ON);
	xgpio_i2c_reg16_write(I2C_NO_2, 0x80>>1, 0x0000, 0x20, STRETCH_ON);
	xgpio_i2c_reg16_write(I2C_NO_3, 0x80>>1, 0x0000, 0x20, STRETCH_ON);


#endif // SER_CFG || DES_CFG

#if defined (UDP_UPDATE) || defined (TCP_UPDATE) || defined (TCP_COMMAND_SRV) || defined (UDP_COMMAND_SRV)
    init_default_config();
    ps_i2c_config();
    loadconfig(&global_config);
	lwip_common_init(&server_netif);
#endif // #if defined (UDP_UPDATE) || defined (TCP_UPDATE) || defined (TCP_COMMAND_SRV) || defined (UDP_COMMAND_SRV)

#if (defined (INTC_DEVICE_ID) || defined (INTC))
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

    }

	// never reached
    cleanup_platform();
    return 0;
}
