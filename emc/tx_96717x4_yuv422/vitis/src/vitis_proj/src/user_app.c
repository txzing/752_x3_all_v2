#include "bsp.h"


#if defined (PLATFORM_ZYNQMP) || defined (PLATFORM_ZYNQ)
	#define UART_BASEADDR XPAR_XUARTPS_0_BASEADDR
#else
	#define UART_BASEADDR XPAR_UARTLITE_0_BASEADDR
#endif // PLATFORM_ZYNQMP || PLATFORM_ZYNQ

#if defined (XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES)
	#define CHANNEL_NUM  XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES
#else
	#define CHANNEL_NUM  4
#endif


u32 ret32;
u8 ret8;
u8 UserInput;
u8 current_ch;
u8 switch_ch;
u8 lock_status[CHANNEL_NUM] = {0};
u8 reconfig_flag[CHANNEL_NUM] = {0};
u8 clear_flag[CHANNEL_NUM] = {0};

u16 get_volt_port1 = 0;
u16 get_volt_port2 = 0;
u16 get_volt_port3 = 0;
u16 get_volt_port4 = 0;

void app_info(void)
{
	xil_printf("----------------------\r\n");
	xil_printf("\r\n%s, UTC %s\r\n",__DATE__,__TIME__);
#if defined (UDP_UPDATE) || defined (TCP_UPDATE) || defined (TCP_COMMAND_SRV) || defined (UDP_COMMAND_SRV)
	xil_printf("Now Board ip:\r\n");
	print_config(&global_config);
#endif
	xil_printf("----------------------\r\n");
	xil_printf("UART commands:\r\n");
	xil_printf("  m/M - this menu\r\n");
	xil_printf("  d/D - CSI video detect (monitors)\r\n");
	xil_printf("  r/R - restore default 1920x1080@30 on all CSI\r\n");
#ifdef XPAR_XAXIS_SWITCH_NUM_INSTANCES
	xil_printf("s - switch video source\r\n");
#endif
	xil_printf("  k/K - test RELAY_CUTOFF\r\n");
	xil_printf("  e/E - set default eth config\r\n");
	xil_printf("  i/I - scan I2C bus (current_ch, 0x03..0x77)\r\n");
	xil_printf("  f/F - MAX96717 local reg dump (ch0..3)\r\n");
	xil_printf("  1/2/3/4 - I2C speed all buses 100/200/300/400 kHz\r\n");
	xil_printf("--- current_ch (I2C): %u ---\r\n", (unsigned)current_ch);
	xil_printf("--- eth UDP 0x50/0x51: ch=1..%u (TPG count=%u) ---\r\n",
		   (unsigned)VIDEO_SRC_COUNT,
		   (unsigned)VIDEO_SRC_COUNT);
	xil_printf("    0x51 src/chcnt/set/get/save/meas sub=0/1/2/3/4/5\r\n");
	xil_printf("--- HW: ch0->CSI0/1/2; CSI3=PathA if same WxHxFPS else PathB ---\r\n");
	xil_printf("----------------------\r\n");
	xil_printf("\r\n");
}

void video_resolution_print(char *info,u32 baseaddr)
{
	xil_printf("-%s freq: %d -\r\n", info, Xil_In32(baseaddr + 0x8));
	xil_printf("-%s res: %dx%d -\r\n", info, Xil_In32(baseaddr + 0x0), Xil_In32(baseaddr + 0x4));
}



u8 uart_RecvByte(UINTPTR BaseAddress)
{
#if defined (PLATFORM_ZYNQMP) || defined (PLATFORM_ZYNQ)
	return XUartPs_RecvByte(BaseAddress);
#else
	return XUartLite_RecvByte(BaseAddress);
#endif // PLATFORM_ZYNQMP || PLATFORM_ZYNQ
}


void uart_receive_process(void)
{

#if defined (PLATFORM_ZYNQMP) || defined (PLATFORM_ZYNQ)
	while(XUartPs_IsReceiveData(UART_BASEADDR))
	{
#else
	if (!XUartLite_IsReceiveEmpty(UART_BASEADDR))
	{
#endif // PLATFORM_ZYNQMP || PLATFORM_ZYNQ

		// Read data from uart
		UserInput = uart_RecvByte(UART_BASEADDR);
		if((UserInput == 'm') || (UserInput == 'M'))
		{
			app_info();
		}
		else if((UserInput == 'd') || (UserInput == 'D'))
		{
			const video_src_cfg_t *s0 = video_src_cfg_get(VIDEO_SRC_0);
			const video_src_cfg_t *s1 = video_src_cfg_get(VIDEO_SRC_1);

			xil_printf("\r\n!!!!!!!!!!!!!!!!!!!!!\r\n");
			xil_printf("------------------------\r\n");
			video_resolution_print("ch0 tpg",XPAR_TPG_0_AXIS_PASSTHROUGH_MON_0_S00_AXI_BASEADDR);
			video_resolution_print("ch0 st",XPAR_ST_0_AXIS_PASSTHROUGH_MON_0_S00_AXI_BASEADDR);
			video_resolution_print("ch1 tpg",XPAR_TPG_1_AXIS_PASSTHROUGH_MON_0_S00_AXI_BASEADDR);
			video_resolution_print("ch1 st",XPAR_ST_1_AXIS_PASSTHROUGH_MON_0_S00_AXI_BASEADDR);
			if ((s0 != NULL) && (s1 != NULL))
			{
				xil_printf("cfg ch0=%ux%u@%u  ch1=%ux%u@%u\r\n",
					   (unsigned)s0->width, (unsigned)s0->height, (unsigned)s0->fps,
					   (unsigned)s1->width, (unsigned)s1->height, (unsigned)s1->fps);
			}
			xil_printf("------------------------\r\n");
		}
		else if ((UserInput == 'r') || (UserInput == 'R'))
		{
			/* Factory geometry + persist so next boot stays default. */
			if (video_inject_factory_default() != XST_SUCCESS)
			{
				xil_printf("video factory default failed\r\n");
			}
		}
		else if((UserInput == 'k') || (UserInput == 'K'))
		{

			xil_printf("------------test RELAY_CUTOFF------------\r\n");
			usleep(100*1000);
			XGpioPs_WritePin(&Gpio, RELAY_CUTOFF, 1) ; // RELAY_CUTOFF: 0-keep; 1-cutoff
		}
		else if((UserInput == 'e') || (UserInput == 'E'))
		{
			xil_printf("------------now set default eth config ------------\r\n");

			ret32 = saveconfig(&default_config);
			if (ret32 != XST_SUCCESS)
			{
				xil_printf("--------saveconfig fail!---------\r\n");
			}
			else
			{
				xil_printf("--------saveconfig ok!---------\r\n");
			}
		}
		else if(UserInput == 'c')
		{


		}
		else if((UserInput == 'v') || UserInput == 'V')
		{
			xil_printf("\r\n------------------------\r\n");
			xil_printf("\r\nDetect port poc Voltage\r\n");
			get_volt_port1 = 0;
			get_volt_port2 = 0;
			get_volt_port3 = 0;
			get_volt_port4 = 0;
			get_adc_voltage(0,&get_volt_port1);
			xil_printf("\r\nPort1 poc Voltage = %d\r\n",get_volt_port1);

			get_adc_voltage(1,&get_volt_port2);
			xil_printf("\r\nPort2 poc Voltage = %d\r\n",get_volt_port2);

			get_adc_voltage(2,&get_volt_port3);
			xil_printf("\r\nPort3 poc Voltage = %d\r\n",get_volt_port3);

			get_adc_voltage(3,&get_volt_port4);
			xil_printf("\r\nPort4 poc Voltage = %d\r\n",get_volt_port4);
			xil_printf("\r\n------------------------\r\n");
		}

		else if((UserInput == 'f') || (UserInput == 'F'))
		{
			xil_printf("------------detect info------------\r\n");
			ret8 = 0;
			for(u8 ch = 0;ch<4;ch++)
			{

				/* bus scan (weak pull-up: STRETCH_ON) */
				u8 found[16];
				u8 fi;
				int nfound;
				xil_printf("\r\n");
				xil_printf("*** I2C scan I2C_NO_%d ***\r\n",ch);
				nfound = xgpio_i2c_scan(ch, STRETCH_ON, found, (u8)(sizeof(found)));
				xil_printf("found %d device(s):", nfound);
				for (fi = 0; fi < (u8)nfound && fi < (u8)(sizeof(found)); fi++)
				{
					xil_printf(" 0x%02x", found[fi]);
				}
				xil_printf("\r\n");

				xil_printf("***********717 ch %d***********\r\n",ch);
				xgpio_i2c_reg16_read(ch, 0x20>>1, 0x0013, &ret8, STRETCH_ON);
				xil_printf("0x0013 == 0x%02x\r\n", ret8);
				xgpio_i2c_reg16_read(ch, 0x20>>1, 0x0102, &ret8, STRETCH_ON);
				xil_printf("0x0102 == 0x%02x\r\n", ret8);
				xgpio_i2c_reg16_read(ch, 0x20>>1, 0x010A, &ret8, STRETCH_ON);
				xil_printf("0x010A == 0x%02x\r\n", ret8);
				xgpio_i2c_reg16_read(ch, 0x20>>1, 0x0112, &ret8, STRETCH_ON);
				xil_printf("0x0112 == 0x%02x\r\n", ret8);
				xgpio_i2c_reg16_read(ch, 0x20>>1, 0x011A, &ret8, STRETCH_ON);
				xil_printf("0x011A == 0x%02x\r\n", ret8);
			}

		}
/****************************************************************************/
/****************************************************************************/

	}
}

void display_fresh(void)
{
	if(timer_cnt >= 1)//200ms
	{
		timer_cnt = 0;

	}

}


