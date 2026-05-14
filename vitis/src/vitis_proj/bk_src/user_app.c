#include "bsp.h"

#if defined (PLATFORM_ZYNQMP) || defined (PLATFORM_ZYNQ)
	#define UART_BASEADDR XPAR_XUARTPS_0_BASEADDR
#else
	#define UART_BASEADDR XPAR_UARTLITE_0_BASEADDR
#endif // PLATFORM_ZYNQMP || PLATFORM_ZYNQ


u32 ret32;
u8 ret8;
u8 UserInput;
u8 current_ch;
u8 lock_status[CHANNEL_NUM] = {0};
static u8 s2mm_retune_ticks[CHANNEL_NUM];
u8 reconfig_flag[CHANNEL_NUM] = {0};
u8 clear_flag[CHANNEL_NUM] = {0};


void app_info(void)
{
	xil_printf("----------------------\r\n");
	xil_printf("\r\n%s, UTC %s\r\n",__DATE__,__TIME__);
#if defined (UDP_UPDATE) || defined (TCP_UPDATE) || defined (TCP_COMMAND_SRV) || defined (UDP_COMMAND_SRV)
	xil_printf("Now Board ip:\r\n");
	print_config(&config);
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
	xil_printf("  r   - reset LVDS (GPIO)\r\n");
	xil_printf("  a   - (reserved)\r\n");
#if defined (XPAR_XAXIS_SWITCH_NUM_INSTANCES) && (XPAR_XAXIS_SWITCH_NUM_INSTANCES >= 1U)
	xil_printf("  s   - switch video source, then");
#if (XPAR_XAXIS_SWITCH_NUM_INSTANCES >= 3U)
	xil_printf(" 1 / 2 / 3\r\n");
#elif (XPAR_XAXIS_SWITCH_NUM_INSTANCES >= 2U)
	xil_printf(" 1 / 2\r\n");
#else
	xil_printf(" 1\r\n");
#endif
#endif
	xil_printf("  0/1 - SerDes stream_id 0/1 (current_ch, reg 0x0050)\r\n");
	xil_printf("  7/8 - axis pixel compare ON / OFF\r\n");
	xil_printf("  c   - clear display + VDMA re-init\r\n");
#if defined (BSP_HAS_VDMA) && (XPAR_XAXIVDMA_NUM_INSTANCES >= 3U)
	xil_printf("  z   - VDMA 1 & 2: read geom -> 1280x640 -> apply (log)\r\n");
#endif
	xil_printf("  f/F - SerDes / pipeline status (current_ch)\r\n");
	xil_printf("--- current_ch (I2C): %u ---\r\n", (unsigned)current_ch);
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

#if defined (XPAR_AXI_PASSTHROUGH_MONITOR_NUM_INSTANCES)
u32 vdma_passthrough_mon_base_lvds(u8 lvds_idx_0based)
{
	switch (lvds_idx_0based)
	{
#if (XPAR_AXI_PASSTHROUGH_MONITOR_NUM_INSTANCES >= 1U)
	case 0U:
		return XPAR_AXIS_PASSTHROUGH_MON_0_S_AXI_LITE_BASEADDR;
#endif
#if (XPAR_AXI_PASSTHROUGH_MONITOR_NUM_INSTANCES >= 2U)
	case 1U:
		return XPAR_AXIS_PASSTHROUGH_MON_1_S_AXI_LITE_BASEADDR;
#endif
#if (XPAR_AXI_PASSTHROUGH_MONITOR_NUM_INSTANCES >= 3U)
	case 2U:
		return XPAR_AXIS_PASSTHROUGH_MON_2_S_AXI_LITE_BASEADDR;
#endif
#if (XPAR_AXI_PASSTHROUGH_MONITOR_NUM_INSTANCES >= 4U)
	case 3U:
		return XPAR_AXIS_PASSTHROUGH_MON_3_S_AXI_LITE_BASEADDR;
#endif
	default:
		return 0U;
	}
}

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
extern Pc_Config Pixel_Compare_ConfigTable[];
/* 串口 d/D：按 pixel compare 表项数循环打印分辨率与 STATUS */
static void uart_debug_dump_pixel_compare(void)
{
	u8 i;
	u8 n = (u8)XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES;
	char label[32];

	for (i = 0U; i < n; i++)
	{
		UINTPTR ba = Pixel_Compare_ConfigTable[i].BaseAddress;

		(void)snprintf(label, sizeof label, "pc_out_%u", (unsigned)i);
		video_resolution_print(label, (u32)ba);
		xil_printf("-STATUS[%u]: %u -\r\n", (unsigned)i,
				(unsigned int)Xil_In32(ba + STATUS));
	}
}
#endif

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
#if defined (XPAR_XV_TPG_NUM_INSTANCES)
		else if(UserInput == 't')
		{
		    XV_tpg_Set_motionSpeed(&tpg_inst0, 0);
		}
		else if(UserInput == 'y')
		{
			tpg_box(&tpg_inst0, 50, 1);
		}
#endif
		else if((UserInput == 'k') || (UserInput == 'K'))
		{

			xil_printf("------------test RELAY_CUTOFF------------\r\n");
			usleep(100*1000);
			XGpioPs_WritePin(&Gpio, RELAY_CUTOFF, 1) ; // RELAY_CUTOFF: 0-keep; 1-cutoff
		}
		else if(UserInput == 'r')
		{
			xil_printf("------------reset lvds------------\r\n");
			XGpio_DiscreteWrite(&XGpioOutput_oldi, 1, 0xf);
			usleep(10*1000);
			XGpio_DiscreteWrite(&XGpioOutput_oldi, 1, 0x0);
		}
		else if (UserInput == 'a')
		{

		    xil_printf("--------------------------------------------\r\n");
		}
		else if(UserInput == 's')
		{
			/* 串口切换：立即改轴开关（与 UDP 命令 0x04 的「等当前帧发完再切」策略不同） */
			xil_printf("------------swictch video source------------\r\n");
			UserInput = uart_RecvByte(UART_BASEADDR);
			if(UserInput == '1')
			{
				AxisSwitch(XPAR_AXIS_SWITCH_0_DEVICE_ID, &AxisSwitch0, 0, 0); //
				AxisSwitch(XPAR_AXIS_SWITCH_1_DEVICE_ID, &AxisSwitch1, 0, 0); //
				AxisSwitch(XPAR_AXIS_SWITCH_2_DEVICE_ID, &AxisSwitch2, 0, 1);
				AxisSwitch(XPAR_AXIS_SWITCH_3_DEVICE_ID, &AxisSwitch3, 0, 1);
				current_ch = 1;
				xil_printf("---------swictch 1---------\r\n");
			}
			#if (XPAR_XAXIS_SWITCH_NUM_INSTANCES >= 2U)
			else if(UserInput == '2')
			{
				AxisSwitch(XPAR_AXIS_SWITCH_0_DEVICE_ID, &AxisSwitch0, 1, 0); //
				AxisSwitch(XPAR_AXIS_SWITCH_1_DEVICE_ID, &AxisSwitch1, 0, 1);
				AxisSwitch(XPAR_AXIS_SWITCH_2_DEVICE_ID, &AxisSwitch2, 0, 0);
				AxisSwitch(XPAR_AXIS_SWITCH_3_DEVICE_ID, &AxisSwitch3, 0, 1);
				current_ch = 2;
				xil_printf("---------swictch 2---------\r\n");
			}
			#endif
			#if (XPAR_XAXIS_SWITCH_NUM_INSTANCES >= 3U)
			else if(UserInput == '3')
			{
				AxisSwitch(XPAR_AXIS_SWITCH_0_DEVICE_ID, &AxisSwitch0, 2, 0); //
				AxisSwitch(XPAR_AXIS_SWITCH_1_DEVICE_ID, &AxisSwitch1, 0, 1);
				AxisSwitch(XPAR_AXIS_SWITCH_2_DEVICE_ID, &AxisSwitch2, 0, 1);
				AxisSwitch(XPAR_AXIS_SWITCH_3_DEVICE_ID, &AxisSwitch3, 0, 0);
				current_ch = 3;
				xil_printf("---------swictch 3---------\r\n");
			}
			#endif
			xil_printf("--- current_ch (I2C): %u ---\r\n", (unsigned)current_ch);
			iterationCounts[0] = 0;
		}
		else if(UserInput == '0')
		{

			xil_printf("---------stream_id 0---------\r\n");
			xgpio_i2c_reg16_write(current_ch, 0x90>>1, 0x0050, 0x00, STRETCH_ON);
		}
		else if(UserInput == '1')
		{
			xil_printf("---------stream_id 1---------\r\n");
			xgpio_i2c_reg16_write(current_ch, 0x90>>1, 0x0050, 0x01, STRETCH_ON);
		}

		else if(UserInput == '7')
		{
			xil_printf("XPAR_AXIS_PIXEL_COMPARE ON\r\n");
#if (XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES >= 1U)
			Xil_Out32(XPAR_LVDS_S0_AXIS_PIXEL_COMPARE_0_S00_AXI_BASEADDR + STATUS, 0x1);
#endif
#if (XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES >= 2U)
			Xil_Out32(XPAR_LVDS_S1_AXIS_PIXEL_COMPARE_0_S00_AXI_BASEADDR + STATUS, 0x1);
#endif
#if (XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES >= 3U)
			Xil_Out32(XPAR_LVDS_S2_AXIS_PIXEL_COMPARE_0_S00_AXI_BASEADDR + STATUS, 0x1);
#endif
		}
		else if(UserInput == '8')
		{
			xil_printf("XPAR_AXIS_PIXEL_COMPARE OFF\r\n");
#if (XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES >= 1U)
			Xil_Out32(XPAR_LVDS_S0_AXIS_PIXEL_COMPARE_0_S00_AXI_BASEADDR + STATUS, 0x0);
#endif
#if (XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES >= 2U)
			Xil_Out32(XPAR_LVDS_S1_AXIS_PIXEL_COMPARE_0_S00_AXI_BASEADDR + STATUS, 0x0);
#endif
#if (XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES >= 3U)
			Xil_Out32(XPAR_LVDS_S2_AXIS_PIXEL_COMPARE_0_S00_AXI_BASEADDR + STATUS, 0x0);
#endif
		}

		else if(UserInput == 'c')
		{
			xil_printf("------------clear display------------\r\n");
			XGpio_DiscreteWrite(&XGpioOutput_oldi, 1, 0xf);
		    clear_display();
		    vdma_config();
			XGpio_DiscreteWrite(&XGpioOutput_oldi, 1, 0x0);
		}
		else if((UserInput == 'f') || (UserInput == 'F'))
		{

			xil_printf("------------detect info------------\r\n");
			xil_printf("------------current_ch %d------------\r\n",current_ch);

			ret8 = 0;
			xil_printf("***********96752 current_ch***********\r\n");
			xil_printf("***link lock [3]***\r\n");
			xgpio_i2c_reg16_read(current_ch, 0x90>>1, 0x0013, &ret8, STRETCH_ON);
			xil_printf("0x0013 == 0x%02x\r\n", ret8);
			xil_printf("***Video pipeline lock [6]***\r\n");
			xgpio_i2c_reg16_read(current_ch, 0x90>>1, 0x0108, &ret8, STRETCH_ON);
			xil_printf("0x0108 == 0x%02x\r\n", ret8);
			xgpio_i2c_reg16_read(current_ch, 0x90>>1, 0x011A, &ret8, STRETCH_ON);
			xil_printf("0x011A == 0x%02x\r\n", ret8);
			xgpio_i2c_reg16_read(current_ch, 0x90>>1, 0x012C, &ret8, STRETCH_ON);
			xil_printf("0x012C == 0x%02x\r\n", ret8);
			xgpio_i2c_reg16_read(current_ch, 0x90>>1, 0x013E, &ret8, STRETCH_ON);
			xil_printf("0x013E == 0x%02x\r\n", ret8);
			xil_printf("***video lock [0]***\r\n");
			xgpio_i2c_reg16_read(current_ch, 0x90>>1, 0x01DC, &ret8, STRETCH_ON);
			xil_printf("0x01DC == 0x%02x\r\n", ret8);
			xgpio_i2c_reg16_read(current_ch, 0x90>>1, 0x01FC, &ret8, STRETCH_ON);
			xil_printf("0x01FC == 0x%02x\r\n", ret8);
			xgpio_i2c_reg16_read(current_ch, 0x90>>1, 0x021C, &ret8, STRETCH_ON);
			xil_printf("0x021C == 0x%02x\r\n", ret8);
			xgpio_i2c_reg16_read(current_ch, 0x90>>1, 0x023C, &ret8, STRETCH_ON);
			xil_printf("0x023C == 0x%02x\r\n", ret8);
			xil_printf("------------------------\r\n");
			xgpio_i2c_reg16_read(current_ch, 0x90>>1, 0x01d2, &ret8, STRETCH_ON);
			xil_printf("0x01d2 == 0x%02x\r\n", ret8);
			xgpio_i2c_reg16_read(current_ch, 0x90>>1, 0x01d4, &ret8, STRETCH_ON);
			xil_printf("0x01d4 == 0x%02x\r\n", ret8);
			xgpio_i2c_reg16_read(current_ch, 0x90>>1, 0x0003, &ret8, STRETCH_ON);
			xil_printf("0x0003 == 0x%02x\r\n", ret8);
			xgpio_i2c_reg16_read(current_ch, 0x90>>1, 0x0050, &ret8, STRETCH_ON);
			xil_printf("0x0050 == 0x%02x\r\n", ret8);

			xil_printf("------------------------\r\n");
		}



/****************************************************************************/
/****************************************************************************/

	}
}

/*
 * 根据 Passthrough Monitor（+0x0 水平 beat，+0x4 垂直行）只调 MM2S：
 * m_width/m_height 由 vdma_apply_detected_rgb_geom 内按 RGB888 固定 ×2 换算为像素宽高，
 * m_stride = 当前 s_stride（DDR 行距）。S2MM s_* 不改。
 * 返回 0 表示读数仍为 0（调用方安排重试）。
 */
#if defined (BSP_HAS_VDMA) && defined (XPAR_AXI_PASSTHROUGH_MONITOR_NUM_INSTANCES)

#if defined (XPAR_XAXIS_SWITCH_NUM_INSTANCES)
/*
 * 将显示/采集路径切到视频通道 1..CHANNEL_NUM（与 UART s+数字 策略一致）。
 * 若定义了 UDP/TCP 视频，会复位 iterationCounts 调试计数；供 vdma_lwip_try_pending_channel_switch 在帧间隙调用。
 */
void board_apply_video_channel_switch(u8 eth_video_ch)
{
	if (eth_video_ch < 1U || eth_video_ch > CHANNEL_NUM)
	{
		return;
	}
	if (eth_video_ch == 1U)
	{
		AxisSwitch(XPAR_AXIS_SWITCH_0_DEVICE_ID, &AxisSwitch0, 0, 0);
		AxisSwitch(XPAR_AXIS_SWITCH_1_DEVICE_ID, &AxisSwitch1, 0, 0);
#if (XPAR_XAXIS_SWITCH_NUM_INSTANCES >= 3U)
		AxisSwitch(XPAR_AXIS_SWITCH_2_DEVICE_ID, &AxisSwitch2, 0, 1);
#endif
#if (XPAR_XAXIS_SWITCH_NUM_INSTANCES >= 4U)
		AxisSwitch(XPAR_AXIS_SWITCH_3_DEVICE_ID, &AxisSwitch3, 0, 1);
#endif
		current_ch = 1U;
		xil_printf("---------swictch 1---------\r\n");
	}
#if (XPAR_XAXIS_SWITCH_NUM_INSTANCES >= 2U)
	else if (eth_video_ch == 2U)
	{
		AxisSwitch(XPAR_AXIS_SWITCH_0_DEVICE_ID, &AxisSwitch0, 1, 0);
		AxisSwitch(XPAR_AXIS_SWITCH_1_DEVICE_ID, &AxisSwitch1, 0, 1);
#if (XPAR_XAXIS_SWITCH_NUM_INSTANCES >= 3U)
		AxisSwitch(XPAR_AXIS_SWITCH_2_DEVICE_ID, &AxisSwitch2, 0, 0);
#endif
#if (XPAR_XAXIS_SWITCH_NUM_INSTANCES >= 4U)
		AxisSwitch(XPAR_AXIS_SWITCH_3_DEVICE_ID, &AxisSwitch3, 0, 1);
#endif
		current_ch = 2U;
		xil_printf("---------swictch 2---------\r\n");
	}
#endif
#if (XPAR_XAXIS_SWITCH_NUM_INSTANCES >= 3U)
	else if (eth_video_ch == 3U)
	{
		AxisSwitch(XPAR_AXIS_SWITCH_0_DEVICE_ID, &AxisSwitch0, 2, 0);
		AxisSwitch(XPAR_AXIS_SWITCH_1_DEVICE_ID, &AxisSwitch1, 0, 1);
		AxisSwitch(XPAR_AXIS_SWITCH_2_DEVICE_ID, &AxisSwitch2, 0, 1);
#if (XPAR_XAXIS_SWITCH_NUM_INSTANCES >= 4U)
		AxisSwitch(XPAR_AXIS_SWITCH_3_DEVICE_ID, &AxisSwitch3, 0, 0);
#endif
		current_ch = 3U;
		xil_printf("---------swictch 3---------\r\n");
	}
#endif
	else
	{
		return;
	}
	xil_printf("--- current_ch (I2C): %u ---\r\n", (unsigned)current_ch);
#if defined (UDP_VIDEO) || defined (TCP_VIDEO)
	extern volatile u32 iterationCounts[]; /* ETH_VIDEO_NUM==1，与 vdma.c 中定义对应 */
	iterationCounts[0] = 0U;
#endif
}
#else /* no axis switch IP */
/* 无 AxisSwitch IP 时仅更新 current_ch，供无 PL 开关的构建 */
void board_apply_video_channel_switch(u8 eth_video_ch)
{
	if (eth_video_ch >= 1U && eth_video_ch <= CHANNEL_NUM)
	{
		current_ch = eth_video_ch;
	}
}
#endif

static void s2mm_retune_try_ch(u8 ch)
{
	u32 mon_base;
	u8 first_id;
	u8 num_vdma;

	if (ch >= CHANNEL_NUM || lock_status[ch] == 0U || s2mm_retune_ticks[ch] == 0U)
	{
		return;
	}
	mon_base = vdma_passthrough_mon_base_lvds(ch);
	if (mon_base == 0U)
	{
		s2mm_retune_ticks[ch] = 0U;
		return;
	}
	if (ch == 0U)
	{
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 3U)
		first_id = 1U;
		num_vdma = 2U;
#else
		s2mm_retune_ticks[ch] = 0U;
		return;
#endif
	}
#if (XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES >= 2U)
	else if (ch == 1U)
	{
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 5U)
		first_id = 3U;
		num_vdma = 2U;
#else
		s2mm_retune_ticks[ch] = 0U;
		return;
#endif
	}
#endif
#if (XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES >= 3U)
	else if (ch == 2U)
	{
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 7U)
		first_id = 5U;
		num_vdma = 2U;
#else
		s2mm_retune_ticks[ch] = 0U;
		return;
#endif
	}
#endif
	else
	{
		s2mm_retune_ticks[ch] = 0U;
		return;
	}

	if (vdma_apply_detected_rgb_geom(first_id, num_vdma, mon_base) != 0)
	{
		s2mm_retune_ticks[ch] = 0U;
	}
	else
	{
		s2mm_retune_ticks[ch]--;
	}
}
#endif /* BSP_HAS_VDMA && XPAR_AXI_PASSTHROUGH_MONITOR_NUM_INSTANCES */

void Disp_State_Detect(u8 channel)
{
	u32 ret32;
	u8 ret8;
	u8  idx;
	static u8 stream_id = 0;
    u8 gmsl_locked;
    u8 video_locked;

    if ((channel == 0) || (channel > CHANNEL_NUM))
    {
        return;
    }
    idx = channel - 1;
    static u8 unlock_cnt[CHANNEL_NUM] = {0};
    static u8 stream_switch_notified[CHANNEL_NUM] = {0};

	ret32 = xgpio_i2c_reg16_read(channel, 0x90 >> 1, 0x0013, &ret8, STRETCH_ON);//GMSL2 link locked @ bit3
	gmsl_locked = CHB(ret8, BIT(3));

	ret32 = xgpio_i2c_reg16_read(channel, 0x90 >> 1, 0x0108, &ret8, STRETCH_ON);//VID_LOCK @ bit6
	video_locked = CHB(ret8, BIT(6));

	if (gmsl_locked && !video_locked)
	{
		stream_id ^= 1;
		if (!stream_switch_notified[idx])
		{
			xil_printf("channel %d now stream_id: %d\r\n",channel, stream_id);
			stream_switch_notified[idx] = 1;
		}
		xgpio_i2c_reg16_write(channel, 0x90>>1, 0x0050, stream_id, STRETCH_ON);
	}
	else if (!gmsl_locked)
	{
		stream_switch_notified[idx] = 0;
	}

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

void display_fresh(void)
{
	if(timer_cnt >= 1)//200ms
	{
		timer_cnt = 0;

#if defined (XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES)
		for (u8 ch = 0; ch < CHANNEL_NUM; ch++)
		{

			/*
			 * pixel_err: 0 idle; 1 IRQ latched (PixelCompareIntrHandler, only if was 0);
			 * 2 snapshot copied in pixel_err_handel, waiting host err-pic path to clear to 0.
			 * 高帧率：同一事故只锁第一帧；上位机取完图后 lwip_video 清 0。
			 * 若长时间未清 0，超时后清 latch，并复位可能卡住的 err 图发送状态。
			 */
			if (pixel_err[ch] == 2U)
			{
				pixel_err_cnt[ch]++;
				if (pixel_err_cnt[ch] >= PIXEL_ERR_HOST_IDLE_TICKS)
				{
					pixel_err_cnt[ch] = 0U;
					pixel_err[ch] = 0U;
#if defined (UDP_VIDEO) || defined (TCP_VIDEO)
					VdmaChannels[0].send_err_start[ch] = 0U;
					VdmaChannels[0].pkg_cnt = 1U;
#endif
				}
			}
			else
			{
				pixel_err_cnt[ch] = 0U;
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
#if defined (BSP_HAS_VDMA) && defined (XPAR_AXI_PASSTHROUGH_MONITOR_NUM_INSTANCES)
		        	s2mm_retune_ticks[ch] = 0U;
#endif
		        	if (ch == 0)
		        	{
		        		XGpio_DiscreteWrite(&XGpioOutput_oldi, 1, 0x2);
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
		        	}
#if (XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES >= 2U)
		        	else if (ch == 1)
		        	{
		        		XGpio_DiscreteWrite(&XGpioOutput_oldi, 1, 0x4);
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
		        	}
#endif
#if (XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES >= 3U)
		        	else if (ch == 2)
		        	{
		        		XGpio_DiscreteWrite(&XGpioOutput_oldi, 1, 0x8);
		        		clear_vdma_5();
		        		clear_vdma_6();
		        		XGpio_DiscreteWrite(&XGpioOutput_oldi, 1, 0x0);
#if defined (UDP_COMMAND_SRV) && defined (UDP_VIDEO)
		        		if (err_auto_send)
		        		{
		        			vcmp_m_refresh_channel(ch);
		        			err_auto_send_func(ch);
		        		}
#endif
		        	}
#endif
		        }

		        reconfig_flag[ch] = 0;
		    }
		    else
		    {

		        if (reconfig_flag[ch] < 2)
		        {
		        	reconfig_flag[ch]++;
		        }

		        if (reconfig_flag[ch] == 1)
		        {


		        	xil_printf("----------cable %d up-----------\r\n",ch+1);
                	XGpio_DiscreteWrite(&XGpioOutput_oldi, 1, 0x1);
                	usleep(10*1000);
                	XGpio_DiscreteWrite(&XGpioOutput_oldi, 1, 0x0);

		        	if (ch == 0)
		        	{
		        		XGpio_DiscreteWrite(&XGpioOutput_oldi, 1, 0x2);
		        		vdma_config_1();
		        		vdma_config_2();
		        		if (vdma_apply_detected_rgb_geom(1U, 2U,
		        			vdma_passthrough_mon_base_lvds(0U)) != 0)
		        		{
		        			s2mm_retune_ticks[ch] = 0U;
		        		}
		        		else
		        		{
		        			s2mm_retune_ticks[ch] = S2MM_RETUNE_MAX_TICKS;
		        		}
		        		XGpio_DiscreteWrite(&XGpioOutput_oldi, 1, 0x0);
		        	}
#if (XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES >= 2U)
		        	else if (ch == 1)
		        	{
		        		XGpio_DiscreteWrite(&XGpioOutput_oldi, 1, 0x4);
		        		vdma_config_3();
		        		vdma_config_4();
		        		if (vdma_apply_detected_rgb_geom(3U, 2U,
		        			vdma_passthrough_mon_base_lvds(1U)) != 0)
		        		{
		        			s2mm_retune_ticks[ch] = 0U;
		        		}
		        		else
		        		{
		        			s2mm_retune_ticks[ch] = S2MM_RETUNE_MAX_TICKS;
		        		}
		        		XGpio_DiscreteWrite(&XGpioOutput_oldi, 1, 0x0);
		        	}
#endif
#if (XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES >= 3U)
		        	else if (ch == 2)
		        	{
		        		XGpio_DiscreteWrite(&XGpioOutput_oldi, 1, 0x8);
		        		vdma_config_5();
		        		vdma_config_6();
		        		if (vdma_apply_detected_rgb_geom(5U, 2U,
		        			vdma_passthrough_mon_base_lvds(2U)) != 0)
		        		{
		        			s2mm_retune_ticks[ch] = 0U;
		        		}
		        		else
		        		{
		        			s2mm_retune_ticks[ch] = S2MM_RETUNE_MAX_TICKS;
		        		}
		        		XGpio_DiscreteWrite(&XGpioOutput_oldi, 1, 0x0);
		        	}
#endif
		        }

		        clear_flag[ch] = 0;
		    }
#if defined (BSP_HAS_VDMA) && defined (XPAR_AXI_PASSTHROUGH_MONITOR_NUM_INSTANCES)
		    s2mm_retune_try_ch(ch);
#endif
		}
#endif
	}
}


/*
 *
 * 思路，vdma正常中断运行，检测到比较错误，vdma处于park模式，取出当前帧，继续
 *
 *
 */
