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
    bsp_printf("test for emc\r\n");
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

	Status = serdes_i2c_write_array_16(I2C_NO_1, max96856_dual_link);
	if (Status != I2C_OK) {
		xil_printf("max96856 array cfg fail st=%d\r\n", Status);
	}
	Status = serdes_i2c_write_array_16(I2C_NO_2, max96772_dual_link);
	if (Status != I2C_OK) {
		xil_printf("max96772 array cfg fail st=%d\r\n", Status);
	}

	xgpio_i2c_reg16_write(I2C_NO_1, 0x90>>1, 0x0116, 0xA0, STRETCH_ON);//Control channel enabled to remote serializer
//	xgpio_i2c_reg16_write(I2C_NO_1, 0x90>>1, 0x0116, 0xe0, STRETCH_ON);//Control channel disabled to remote serializer

	/* ===== MAX96856：开 PT1，挂 SIOA，I2C ===== */
	/* PT1_EN=1, PT1_I2CSEL=1, PT1_LINK_SEL=0(SIOA) */
	xgpio_i2c_reg16_write(I2C_NO_1, 0x90>>1, 0x011E, 0xA0, STRETCH_ON);
	/* PT1 速率默认即可：SLV/MST ≈397kHz */
	xgpio_i2c_reg16_write(I2C_NO_1, 0x90>>1, 0x0122, 0x2D, STRETCH_ON);

	//GPIO23
	xgpio_i2c_reg16_write(I2C_NO_1, 0x90>>1, 0x018B, 0xFA, STRETCH_ON);
	xgpio_i2c_reg16_write(I2C_NO_1, 0x90>>1, 0x018C, 0x62, STRETCH_ON);
	xgpio_i2c_reg16_write(I2C_NO_1, 0x90>>1, 0x018D, 0x1A, STRETCH_ON);

//772
	/* DEV_772: 7-bit，你们现在是 0x90>>1 */
	xgpio_i2c_reg16_write(I2C_NO_2, 0x90>>1, 0x0170, 0x00, STRETCH_ON); /* 关 SPI，GPIO5 与 SCLK 复用 */
	xgpio_i2c_reg16_write(I2C_NO_2, 0x90>>1, 0x0003, 0x00, STRETCH_ON); /* 关 UART1，建议 RMW */

	/* 释放 GPIO3(SDA1) / GPIO5(SCL1) */
	xgpio_i2c_reg16_write(I2C_NO_2, 0x90>>1, 0x02B9, 0x01, STRETCH_ON);
	xgpio_i2c_reg16_write(I2C_NO_2, 0x90>>1, 0x02BF, 0x01, STRETCH_ON);

	/* 不开 XOVER（地址是 0x048A，不是 0x004E） */
	xgpio_i2c_reg16_write(I2C_NO_2, 0x90>>1, 0x048A, 0x00, STRETCH_ON);

	/* 远端 master 时序（772 在 PT1 脚上再生 I2C） */
	xgpio_i2c_reg16_write(I2C_NO_2, 0x90>>1, 0x0480, 0x06, STRETCH_ON); /* I2C_PT_0 */
	//不用配
//	xgpio_i2c_reg16_write(I2C_NO_2, 0x90>>1, 0x0481, 0x26, STRETCH_ON); /* I2C_PT_1，上拉弱可再降速 */

	xgpio_i2c_reg16_write(I2C_NO_2, 0x90>>1, 0x0001, 0x42, STRETCH_ON); /* 仅当当前就是 0x02 时直写 */
	//GPIO4
	xgpio_i2c_reg16_write(I2C_NO_2, 0x90>>1, 0x02bc, 0x43, STRETCH_ON);
	xgpio_i2c_reg16_write(I2C_NO_2, 0x90>>1, 0x02bd, 0x44, STRETCH_ON);
	xgpio_i2c_reg16_write(I2C_NO_2, 0x90>>1, 0x02be, 0x40, STRETCH_ON);

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
