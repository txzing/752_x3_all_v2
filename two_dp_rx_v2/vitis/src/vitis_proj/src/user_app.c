#include "bsp.h"

#define UARTLITE_BASEADDR XPAR_PROCESSOR_SUBSYSTEM_AXI_UARTLITE_0_BASEADDR


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
u8 stream_id[CHANNEL_NUM] = {0};

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
	xil_printf("  d/D - video path detect (monitors + pixel compare)\r\n");
#if defined (XPAR_XV_TPG_NUM_INSTANCES)
	xil_printf("  t   - TPG motion speed 0\r\n");
	xil_printf("  y   - TPG box pattern\r\n");
#endif
	xil_printf("  k/K - test RELAY_CUTOFF\r\n");
	xil_printf("  e/E - set default eth config\r\n");
	xil_printf("  z   - config Max\r\n");
	xil_printf("  a   - (reserved)\r\n");
#if defined (XPAR_XAXIS_SWITCH_NUM_INSTANCES) && (XPAR_XAXIS_SWITCH_NUM_INSTANCES >= 1U)
	xil_printf("  s   - switch video source, then");
	xil_printf(" 1 / 2\r\n");
#endif
	xil_printf("  7/8 - axis pixel compare ON / OFF\r\n");
	xil_printf("  c   - clear display + VDMA re-init\r\n");
	xil_printf("  f/F - SerDes / pipeline status (current_ch)\r\n");
	xil_printf("--- current_ch (I2C): %u ---\r\n", (unsigned)current_ch);
	xil_printf("----------------------\r\n");
	xil_printf("\r\n");
}


void video_resolution_print(char *info,u32 baseaddr)
{
	xil_printf("-%s freq: %d -\r\n", info, AXI_PASSTHROUGH_MONITOR_REG_mReadReg(baseaddr, 0x8));
	xil_printf("-%s res: %dx%d -\r\n", info, AXI_PASSTHROUGH_MONITOR_REG_mReadReg(baseaddr, 0x0),\
												AXI_PASSTHROUGH_MONITOR_REG_mReadReg(baseaddr, 0x4));
}
u8 uart_RecvByte(UINTPTR BaseAddress)
{
#if defined (PLATFORM_ZYNQMP) || defined (PLATFORM_ZYNQ)
	return XUartPs_RecvByte(BaseAddress);
#else
	return XUartLite_RecvByte(BaseAddress);
#endif // PLATFORM_ZYNQMP || PLATFORM_ZYNQ
}

#if defined (XPAR_AXI_PASSTHROUGH_MONITOR_NUM_INSTANCES)
/* 串口 d/D：按 BSP 中 passthrough monitor 实例数循环打印（基址由 vdma.c 统一映射） */
static void uart_debug_dump_passthrough_monitors(void)
{
	u8 i;
	u8 n = (u8)XPAR_AXI_PASSTHROUGH_MONITOR_NUM_INSTANCES;
	char label[20];

	for (i = 0U; i < n; i++)
	{
		u32 ba = vdma_passthrough_mon_base_lvds(i);

		if (ba == 0U)
		{
			continue;
		}
		(void)snprintf(label, sizeof label, "LVDS_in_%u", (unsigned)i);
		video_resolution_print(label, ba);
	}
}
#endif

#if defined (XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES)
/* 串口 d/D：按 pixel compare 表项数循环打印分辨率与 STATUS */
static void uart_debug_dump_pixel_compare(void)
{
	u8 i;
	u8 n = (u8)XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES;
	char label[32];

	for (i = 0U; i < n; i++)
	{
		UINTPTR ba = XAxisPixelCompare_ConfigTable[i].S00_axi_BaseAddr;

		(void)snprintf(label, sizeof label, "pc_out_%u", i);
		video_resolution_print(label, (u32)ba);
		{
			const u32 st = Xil_In32(ba + STATUS);
			xil_printf("-STATUS[%u]: %u (en=%u err=%u strm=%u) -\r\n", i,
				   st,
				   (st & 1U),
				   ((st >> 1) & 1U),
				   ((st >> 2) & 1U));
		}
	}
}
#endif

void uart_receive_process(void)
{
	if (!XUartLite_IsReceiveEmpty(UARTLITE_BASEADDR))
	{
		// Read data from uart
		UserInput = XUartLite_RecvByte(UARTLITE_BASEADDR);
		if((UserInput == 'm') || (UserInput == 'M'))
		{

			app_info();
		}
		else if((UserInput == 'd') || (UserInput == 'D'))
		{
			xil_printf("--- current_ch (I2C): %u ---\r\n", (u8)current_ch);
			xil_printf("\r\n=== UART d/D: video path dump ===\r\n");
			xil_printf("------------------------\r\n");
#if defined (XPAR_AXI_PASSTHROUGH_MONITOR_NUM_INSTANCES)
			uart_debug_dump_passthrough_monitors();
#else
			xil_printf("(passthrough monitors not in BSP)\r\n");
#endif
			xil_printf("------------------------\r\n");
#if defined (XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES)
			uart_debug_dump_pixel_compare();
#else
			xil_printf("(pixel compare not in BSP)\r\n");
#endif
			xil_printf("------------------------\r\n");
		}
		else if((UserInput == 'k') || (UserInput == 'K'))
		{

			xil_printf("------------test RELAY_CUTOFF------------\r\n");
			usleep(100*1000);
		    ret32 = XGpio_DiscreteRead(&Axi_Gpio, 1);
		    STB(ret32, BIT32(0));//RELAY_CUTOFF: 0-keep; 1-cutoff // RELAY_CUTOFF: 0-keep; 1-cutoff
			XGpio_DiscreteWrite(&Axi_Gpio, 1, ret32);
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
			xil_printf("------------clear display------------\r\n");
			XGpio_DiscreteWrite(&XGpioOutput_oldi, 1, 0x3);
		    clear_display();
		    vdma_config();
			XGpio_DiscreteWrite(&XGpioOutput_oldi, 1, 0x0);
		}
		else if (UserInput == 'a')
		{

		    xil_printf("--------------------------------------------\r\n");
		}
		else if(UserInput == 's')
		{

			xil_printf("------------swictch video source------------\r\n");
			UserInput = XUartLite_RecvByte(UARTLITE_BASEADDR);
			if(UserInput == '1')
			{
				switch_ch = 1;
			}
			else if(UserInput == '2')
			{
				switch_ch = 2;
			}
		}
		else if((UserInput == 'z') || (UserInput == 'Z'))
		{

			if(current_ch == 1)
			{
				xil_printf("-------config max96856------\r\n");
//				serdes_i2c_write_array_16(I2C_NO_1, max96856_dual_link);
				xgpio_i2c_reg16_write(I2C_NO_1, 0x90 >> 1, 0x01E7, 0x05, STRETCH_ON);
			}
			else if(current_ch == 2)
			{
				xil_printf("-------config max96772------\r\n");
				serdes_i2c_write_array_16(I2C_NO_2, max96772_dual_link);
//				xgpio_i2c_reg16_write(I2C_NO_2, 0x90 >> 1, 0xe776, 0x02, STRETCH_ON);
//				xgpio_i2c_reg16_write(I2C_NO_2, 0x90 >> 1, 0xe777, 0x80, STRETCH_ON);
			}
		}
		else if(UserInput == '7')
		{
			for (u8 ch = 0; ch < XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES; ch++)
			{
				pixel_cp_start[ch] = 1U;
				pixel_cp_start_cnt[ch] = 0U;
			}
		}
		else if(UserInput == '8')
		{
			xil_printf("XPAR_AXIS_PIXEL_COMPARE OFF\r\n");
			for (u8 ch = 0; ch < XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES; ch++)
			{
				Xil_Out32(XAxisPixelCompare_ConfigTable[ch].S00_axi_BaseAddr + STATUS, 0x0);
			}
		}
		else if((UserInput == 'h') || (UserInput == 'H'))
		{
			u8 ch;
			dp_app_help();
			ch = XUartLite_RecvByte(UARTLITE_BASEADDR) - '0';
			xil_printf("------------ch %d-----------\r\n",ch);
			UserInput = XUartLite_RecvByte(UARTLITE_BASEADDR);
			dp_debug_info(UserInput, ch);

		}
		else if((UserInput == 'f') || (UserInput == 'F'))
		{

			xil_printf("------------detect info------------\r\n");
			xil_printf("------------current_ch %d------------\r\n",current_ch);
			ret8 = 0;
			if(current_ch == 1)
			{
				xil_printf("***********max96856 current_ch***********\r\n");
				xil_printf("***GMSL link lock***\r\n");
				xgpio_i2c_reg16_read(current_ch, 0x90>>1, 0x0000, &ret8, STRETCH_ON);
				xil_printf("0x0000 == 0x%02x\r\n", ret8);
				xgpio_i2c_reg16_read(current_ch, 0x90>>1, 0x0030, &ret8, STRETCH_ON);
				xil_printf("0x0030 == 0x%02x\r\n", ret8);
				xil_printf("***Video lock***\r\n");
				xgpio_i2c_reg16_read(current_ch, 0x90>>1, 0x0094, &ret8, STRETCH_ON);
				xil_printf("0x0094 == 0x%02x\r\n", ret8);

			}
			else if(current_ch == 2)
			{
				xil_printf("***********max96722 current_ch***********\r\n");
				xil_printf("***GMSL link lock***\r\n");
				xgpio_i2c_reg16_read(current_ch, 0x90>>1, 0x0000, &ret8, STRETCH_ON);
				xil_printf("0x0000 == 0x%02x\r\n", ret8);
				xgpio_i2c_reg16_read(current_ch, 0x90>>1, 0x0013, &ret8, STRETCH_ON);
				xil_printf("0x0013 == 0x%02x\r\n", ret8);
				xil_printf("***Video lock***\r\n");
				xgpio_i2c_reg16_read(current_ch, 0x90>>1, 0x0108, &ret8, STRETCH_ON);
				xil_printf("0x0108 == 0x%02x\r\n", ret8);
				xgpio_i2c_reg16_read(current_ch, 0x90>>1, 0x01dc, &ret8, STRETCH_ON);
				xil_printf("0x01dc == 0x%02x\r\n", ret8);
				xgpio_i2c_reg16_read(current_ch, 0x90>>1, 0x07f0, &ret8, STRETCH_ON);
				xil_printf("0x07f0 == 0x%02x\r\n", ret8);
				xgpio_i2c_reg16_read(current_ch, 0x90>>1, 0x07f1, &ret8, STRETCH_ON);
				xil_printf("0x07f1 == 0x%02x\r\n", ret8);
			}
		}
/****************************************************************************/
/****************************************************************************/

	}
}

#if defined (XPAR_XAXIS_SWITCH_NUM_INSTANCES)
/*
 * 将显示/采集路径切到视频通道 1..CHANNEL_NUM（与 UART s+数字 策略一致）。
 * 若定义了 UDP/TCP 视频，会复位 iterationCounts 调试计数；供 vdma_lwip_try_pending_channel_switch 在帧间隙调用。
 */

void switch_run(void)
{
	if(switch_ch != current_ch)
	{
		if (VC_inst.send_pic_start != 0U)
		{
			return;
		}
		/* 视频当前帧分包发送中 */
		if (VC_inst.video_sending != 0U)
		{
			return;
		}
		board_apply_video_channel_switch(switch_ch);
		vdma_lwip_apply_channel_geometry(switch_ch);
	}
}

void board_apply_video_channel_switch(u8 switch_ch)
{
	if (switch_ch < 1U || switch_ch > CHANNEL_NUM)
	{
		return;
	}
	if (switch_ch == 1U)
	{
		AxisSwitch(XPAR_AXIS_SWITCH_0_DEVICE_ID, &AxisSwitch0, 0, 0);
		AxisSwitch(XPAR_AXIS_SWITCH_1_DEVICE_ID, &AxisSwitch1, 0, 0);
		AxisSwitch(XPAR_AXIS_SWITCH_2_DEVICE_ID, &AxisSwitch2, 0, 1);

	}
	else if (switch_ch == 2U)
	{
		AxisSwitch(XPAR_AXIS_SWITCH_0_DEVICE_ID, &AxisSwitch0, 1, 0);
		AxisSwitch(XPAR_AXIS_SWITCH_1_DEVICE_ID, &AxisSwitch1, 0, 1);
		AxisSwitch(XPAR_AXIS_SWITCH_2_DEVICE_ID, &AxisSwitch2, 0, 0);
	}
	else
	{
		return;
	}
	current_ch = switch_ch;
	xil_printf("--- current_ch (I2C): %d ---\r\n", current_ch);
#if defined (UDP_VIDEO) || defined (TCP_VIDEO)
	iterationCounts = 0U;
#endif
}
#endif

void Disp_State_Detect(u8 channel)
{
	u8 idx = channel - 1;
    u8 gmsl_locked = 0;
    u8 pipe_lock = 0;
    u8 video_locked = 0;
    static u8 unlock_cnt[CHANNEL_NUM] = {0};

    if ((channel == 0) || (channel > CHANNEL_NUM))
    {
        return;
    }

	ret8 = 0;
	ret32 = 0;

    if (channel == 1)//856
    {
		ret32 = xgpio_i2c_reg16_read(channel, 0x90 >> 1, 0x0030, &ret8, STRETCH_ON);//GMSL2 link locked @ bit3
		gmsl_locked = CHB(ret8, BIT(6)) && CHB(ret8, BIT(7));


		ret32 = xgpio_i2c_reg16_read(channel, 0x90 >> 1, 0x0094, &ret8, STRETCH_ON);//VID_LOCK @ bit6
		video_locked = CHB(ret8, BIT(6)) && CHB(ret8, BIT(7));

		if ((gmsl_locked == 0) || (video_locked == 0))
		{
			if (++unlock_cnt[idx] >= 2)
			{
				unlock_cnt[idx] = 0;
				lock_status[idx] = 0;
	//            xil_printf("channel %d no lock\r\n", channel);
			}
		}
		else
		{
			unlock_cnt[idx] = 0;
			lock_status[idx] = 1;
		}
    }
    if (channel == 2)//772
    {
		ret32 = xgpio_i2c_reg16_read(channel, 0x90 >> 1, 0x0013, &ret8, STRETCH_ON);//GMSL2 link locked @ bit3
		gmsl_locked = CHB(ret8, BIT(3));


		ret32 = xgpio_i2c_reg16_read(channel, 0x90 >> 1, 0x0108, &ret8, STRETCH_ON);//VID_LOCK @ bit6
		pipe_lock = CHB(ret8, BIT(6));

		ret32 = xgpio_i2c_reg16_read(channel, 0x90 >> 1, 0x01dc, &ret8, STRETCH_ON);//VID_LOCK @ bit0
		video_locked = CHB(ret8, BIT(0));

		if ((gmsl_locked == 0) || (pipe_lock == 0) || (video_locked == 0))
		{
			if (++unlock_cnt[idx] >= 2)
			{
				unlock_cnt[idx] = 0;
				lock_status[idx] = 0;
//	            xil_printf("channel %d no lock\r\n", channel);
			}
		}
		else
		{
			unlock_cnt[idx] = 0;
			lock_status[idx] = 1;
		}
    }

}


void display_fresh(void)
{
	if(timer_cnt >= 1)//200ms
	{
		timer_cnt = 0;

#if defined (XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES)
		for (u8 ch = 0; ch < XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES; ch++)
		{

			if(pixel_err_cnt[ch] >= 10)
			{
				pixel_err_cnt[ch] = 0;
				pixel_err[ch] = 0;
			}
			else if(pixel_err[ch] == 2)
			{
				pixel_err_cnt[ch]++;
			}

			if(pixel_cp_start[ch] == 1U)
			{
				u32 base_addr;
				if(pixel_cp_start_cnt[ch] == 1U)
				{
					PixelCompare_reset(ch + 1U);
//					xil_printf("----------pixel_cp_start_%d_cnt %d-----------\r\n",ch+1,pixel_cp_start_cnt[ch]);
					pixel_cp_start_cnt[ch] = pixel_cp_start_cnt[ch] + 1;
				}
				else if(pixel_cp_start_cnt[ch] == 6U)
				{
					pixel_cp_start[ch] = 0U;
					pixel_cp_start_cnt[ch] = 0U;
					base_addr = pixel_compare_axi_base_eth(ch + 1);
					Xil_Out32(base_addr + STATUS, 0x1);
					xil_printf("----------pixel_cp_%d_start-----------\r\n",ch+1);
				}
				else
				{
					pixel_cp_start_cnt[ch] = pixel_cp_start_cnt[ch] + 1;
				}

			}


		    Disp_State_Detect(ch + 1);

		    if (lock_status[ch] == 0)
		    {
		        if (clear_flag[ch] < 2)
		        {
		            clear_flag[ch]++;
		        }

		        if (clear_flag[ch] == 1)
		        {
		        	xil_printf("----------cable %d down-----------\r\n",ch+1);
		            switch (ch)
		            {
		                case 0:
		                {

		                	XGpio_DiscreteWrite(&XGpioOutput_oldi, 1, 0x1);
		                	clear_vdma_1();
		                	clear_vdma_2();
		                	XGpio_DiscreteWrite(&XGpioOutput_oldi, 1, 0x0);
#if defined (UDP_COMMAND_SRV) && defined (UDP_VIDEO)
		                	if (err_auto_send)
		                	{
		                		vcmp_m_refresh_channel(ch);
		                		err_auto_send_func(ch);
		                	}
#endif
		                	break;
		                }
		                case 1:
		                {
		                	XGpio_DiscreteWrite(&XGpioOutput_oldi, 1, 0x2);
		                	clear_vdma_3();
		                	clear_vdma_4();
		                	XGpio_DiscreteWrite(&XGpioOutput_oldi, 1, 0x0);
#if defined (UDP_COMMAND_SRV) && defined (UDP_VIDEO)
		                	if (err_auto_send)
		                	{
		                		vcmp_m_refresh_channel(ch);
		                		err_auto_send_func(ch);
		                	}
#endif
		                	break;
		                }
		                default: break;
		            }
		        }

		        reconfig_flag[ch] = 0;
		    }
		    else  //locked
		    {
		        if (reconfig_flag[ch] == 1)
		        {
		        	xil_printf("----------cable %d up-----------\r\n",ch+1);
		        	iterationCounts = 0U;
		            switch (ch)
		            {
		                case 0:
		                {
		                	PixelCompare_reset(ch + 1U);
		                	break;
		                }
		                case 1:
		                {
		                	PixelCompare_reset(ch + 1U);
		                	break;
		                }
		                default: break;
		            }
		        }

		        if (reconfig_flag[ch] == 6)
		        {
		            switch (ch)
		            {
		                case 0:
		                {
							XDp_RxDtgDis(DpRxSsInst_0.DpPtr);
							XDp_RxDtgEn(DpRxSsInst_0.DpPtr);
		                	break;
		                }
		                case 1:
		                {
							XDp_RxDtgDis(DpRxSsInst_1.DpPtr);
							XDp_RxDtgEn(DpRxSsInst_1.DpPtr);
		                	break;
		                }
		                default: break;
		            }
		        }

		        if (reconfig_flag[ch] == 18)
		        {
		    		u32 fps = 0U;
		    		u32 base_addr = 0U;
		            switch (ch)
		            {
		                case 0:
		                {
			    			base_addr = XPAR_AXIS_PASSTHROUGH_MON_0_S_AXI_LITE_BASEADDR;
				    		fps = Xil_In32(base_addr + FPS);
				    		if(fps < 10)
							{
								xil_printf("----------reconfig max96856 %d-----------\r\n",fps);
	//							serdes_i2c_write_array_16(I2C_NO_1, max96856_dual_link);
								xgpio_i2c_reg16_write(I2C_NO_1, 0x90 >> 1, 0x01E7, 0x05, STRETCH_ON);
								reconfig_flag[ch] = 3;
							}
		                	break;
		                }
		                case 1:
		                {
			    			base_addr = XPAR_AXIS_PASSTHROUGH_MON_1_S_AXI_LITE_BASEADDR;
				    		fps = Xil_In32(base_addr + FPS);
				    		if(fps < 10)
							{
								xil_printf("----------reconfig max96772 %d-----------\r\n",fps);
								serdes_i2c_write_array_16(I2C_NO_2, max96772_dual_link);
//								xgpio_i2c_reg16_write(I2C_NO_2, 0x90 >> 1, 0xe776, 0x02, STRETCH_ON);
//								xgpio_i2c_reg16_write(I2C_NO_2, 0x90 >> 1, 0xe777, 0x80, STRETCH_ON);
								reconfig_flag[ch] = 3;
							}
		                	break;
		                }
		                default: break;
		            }
		        }

		        if (reconfig_flag[ch] < 20)
		        {
		        	reconfig_flag[ch]++;
		        }

		        clear_flag[ch] = 0;
		    }
		}
#endif
	}
}
