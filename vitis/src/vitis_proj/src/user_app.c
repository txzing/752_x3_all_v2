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
	xil_printf("  r   - reset LVDS (GPIO)\r\n");
	xil_printf("  a   - (reserved)\r\n");
#if defined (XPAR_XAXIS_SWITCH_NUM_INSTANCES) && (XPAR_XAXIS_SWITCH_NUM_INSTANCES >= 1U)
	xil_printf("  s   - switch video source, then");
	xil_printf(" 1 / 2 / 3\r\n");
#endif
	xil_printf("  0/1 - SerDes stream_id 0/1 (current_ch, reg 0x0050)\r\n");
	xil_printf("  7/8 - axis pixel compare ON / OFF\r\n");
	xil_printf("  c   - clear display + VDMA re-init\r\n");
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
		else if(UserInput == 'r')
		{
			xil_printf("------------reset lvds------------\r\n");
			XGpio_DiscreteWrite(&XGpioOutput_oldi, 1, 0xf);
			usleep(20*1000);
			XGpio_DiscreteWrite(&XGpioOutput_oldi, 1, 0x0);
		}
		else if(UserInput == 'c')
		{
			xil_printf("------------clear display------------\r\n");
			XGpio_DiscreteWrite(&XGpioOutput_oldi, 1, 0xf);
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
			/* 串口切换：立即改轴开关（与 UDP 命令 0x04 的「等当前帧发完再切」策略不同） */
			xil_printf("------------swictch video source------------\r\n");
			UserInput = uart_RecvByte(UART_BASEADDR);
			if(UserInput == '1')
			{
				switch_ch = 1;
			}
			else if(UserInput == '2')
			{
				switch_ch = 2;
			}
			else if(UserInput == '3')
			{
				switch_ch = 3;
			}
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
			for (u8 ch = 0; ch < XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES; ch++)
			{
				pixel_cp_start[ch] = 1U;
				pixel_cp_start_cnt[ch] = 0U;
			}
		}
		else if(UserInput == '8')
		{
			xil_printf("XPAR_AXIS_PIXEL_COMPARE OFF\r\n");
			xil_printf("XPAR_AXIS_PIXEL_COMPARE OFF\r\n");
			for (u8 ch = 0; ch < XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES; ch++)
			{
				Xil_Out32(XAxisPixelCompare_ConfigTable[ch].S00_axi_BaseAddr + STATUS, 0x0);
			}
		}
		else if((UserInput == 'f') || (UserInput == 'F'))
		{

			xil_printf("------------detect info------------\r\n");
			xil_printf("------------current_ch %d------------\r\n",current_ch);

			ret8 = 0;
			xil_printf("***********96752 current_ch***********\r\n");
			xgpio_i2c_reg16_read(current_ch, 0x90>>1, 0x0000, &ret8, STRETCH_ON);
			xil_printf("0x0000 == 0x%02x\r\n", ret8);
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

#if defined (BSP_HAS_VDMA) && defined (XPAR_AXI_PASSTHROUGH_MONITOR_NUM_INSTANCES)

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
		AxisSwitch(XPAR_AXIS_SWITCH_3_DEVICE_ID, &AxisSwitch3, 0, 1);
	}
	else if (switch_ch == 2U)
	{
		AxisSwitch(XPAR_AXIS_SWITCH_0_DEVICE_ID, &AxisSwitch0, 1, 0);
		AxisSwitch(XPAR_AXIS_SWITCH_1_DEVICE_ID, &AxisSwitch1, 0, 1);
		AxisSwitch(XPAR_AXIS_SWITCH_2_DEVICE_ID, &AxisSwitch2, 0, 0);
		AxisSwitch(XPAR_AXIS_SWITCH_3_DEVICE_ID, &AxisSwitch3, 0, 1);
	}
	else if (switch_ch == 3U)
	{
		AxisSwitch(XPAR_AXIS_SWITCH_0_DEVICE_ID, &AxisSwitch0, 2, 0);
		AxisSwitch(XPAR_AXIS_SWITCH_1_DEVICE_ID, &AxisSwitch1, 0, 1);
		AxisSwitch(XPAR_AXIS_SWITCH_2_DEVICE_ID, &AxisSwitch2, 0, 1);
		AxisSwitch(XPAR_AXIS_SWITCH_3_DEVICE_ID, &AxisSwitch3, 0, 0);
	}
	else
	{
		return;
	}
	current_ch = switch_ch;
	xil_printf("--- current_ch (I2C): %u ---\r\n", current_ch);
#if defined (UDP_VIDEO) || defined (TCP_VIDEO)
	iterationCounts = 0U;
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

/*---------------------------------------------------------------------------
 * S2MM 侧稳定分辨率检测 + MM2S 读向几何适配
 *
 * 设计要点：S2MM 写 DDR 按固定画布 1920×1080（vdma triple 中 s_width/s_height/s_stride），
 * 任意不超过该范围的源均可落同一缓冲；passthrough monitor 表征该链路上「当前稳定帧尺寸」，
 * 热轮询跟踪其防抖变化后，调用 vdma_apply_detected_rgb_geom 仅更新 MM2S 的 m_*，不改 S2MM。
 *
 * 与 display_fresh 中 lock_status / reconfig_flag 配合：
 * - cable up：reconfig_flag==1 时 vdma_lvds_path_op(ch,1) + s2mm_hot_seed_from_mon。
 * - reconfig_flag>=2：s2mm_hot_runtime_geom_poll_ch 防抖后 apply MM2S。
 *---------------------------------------------------------------------------*/

s2mm_hot_ch_t s2mm_hot[CHANNEL_NUM];

static u8 s2mm_resolve_ch(u8 ch, u32 *out_mon, u8 *out_first, u8 *out_num)
{
	if (ch >= CHANNEL_NUM || out_mon == NULL || out_first == NULL || out_num == NULL)
	{
		return 0U;
	}
	*out_mon = vdma_passthrough_mon_base_lvds(ch);
	if (*out_mon == 0U || (u8)XPAR_XAXIVDMA_NUM_INSTANCES < (u8)((2U * ch) + 3U))
	{
		return 0U;
	}
	*out_first = (u8)((2U * ch) + 1U);
	*out_num = 2U;
	return 1U;
}

static void s2mm_hot_reset_ch(u8 ch)
{
	if (ch >= CHANNEL_NUM)
	{
		return;
	}
	s2mm_hot[ch].applied_w = 0U;
	s2mm_hot[ch].applied_h = 0U;
	s2mm_hot[ch].stable_w = 0U;
	s2mm_hot[ch].stable_h = 0U;
	s2mm_hot[ch].stable_n = 0U;
}

static void s2mm_hot_seed_from_mon(u8 ch, u32 mon_base)
{
	u32 sw;
	u32 sh;
	s2mm_hot_ch_t *st;

	if (ch >= CHANNEL_NUM || mon_base == 0U)
	{
		return;
	}
	if (vdma_passthrough_read_mon(mon_base, &sw, &sh, NULL) == 0)
	{
		return;
	}
	st = &s2mm_hot[ch];
	st->applied_w = sw;
	st->applied_h = sh;
	st->stable_w = sw;
	st->stable_h = sh;
	st->stable_n = S2MM_HOT_GEOM_STABLE_TICKS;
}

static void s2mm_hot_runtime_geom_poll_ch(u8 ch)
{
	u32 mon_base;
	u8 first_id;
	u8 num_vdma;
	u32 rw;
	u32 rh;
	s2mm_hot_ch_t *st;

	if (ch >= CHANNEL_NUM || lock_status[ch] == 0U || reconfig_flag[ch] < 2U)
	{
		return;
	}
	if (s2mm_resolve_ch(ch, &mon_base, &first_id, &num_vdma) == 0U)
	{
		return;
	}

	if (vdma_passthrough_read_mon(mon_base, &rw, &rh, NULL) == 0)
	{
//		xil_printf("------------reset ch_%d lvds------------\r\n",ch + 1U);
//    	XGpio_DiscreteWrite(&XGpioOutput_oldi, 1, 0x1);
//    	usleep(20*1000);
//    	XGpio_DiscreteWrite(&XGpioOutput_oldi, 1, 0x0);
//    	iterationCounts = 0U;
//    	vdma_lvds_path_op(ch, 1U);
		return;
	}

	st = &s2mm_hot[ch];
	if (rw == st->stable_w && rh == st->stable_h)
	{
		if (st->stable_n < 255U)
		{
			st->stable_n++;
		}
	}
	else
	{
		st->stable_w = rw;
		st->stable_h = rh;
		st->stable_n = 1U;
	}
	if (st->stable_n < S2MM_HOT_GEOM_STABLE_TICKS || (rw == st->applied_w && rh == st->applied_h))
	{
		return;
	}
	if (vdma_apply_detected_rgb_geom(first_id, num_vdma, mon_base) != 0)
	{
		st->applied_w = rw;
		st->applied_h = rh;
	}
}

#endif /* BSP_HAS_VDMA && XPAR_AXI_PASSTHROUGH_MONITOR_NUM_INSTANCES */

void Disp_State_Detect(u8 channel)
{

	static u8 cnt[CHANNEL_NUM] = {0};
	u32 ret32;
	u8 ret8;
	u8  idx;
    u8 gmsl_locked;
    u8 video_locked;

    if ((channel == 0) || (channel > CHANNEL_NUM))
    {
        return;
    }

    if(cnt[channel] <= 2)
    {
    	cnt[channel]++;
    	return;
    }
    cnt[channel] = 0;
    idx = channel - 1;
    static u8 unlock_cnt[CHANNEL_NUM] = {0};

	ret32 = xgpio_i2c_reg16_read(channel, 0x90 >> 1, 0x0013, &ret8, STRETCH_ON);//GMSL2 link locked @ bit3
	gmsl_locked = CHB(ret8, BIT(3));

	ret32 = xgpio_i2c_reg16_read(channel, 0x90 >> 1, 0x0108, &ret8, STRETCH_ON);//VID_LOCK @ bit6
	video_locked = CHB(ret8, BIT(6));

	if (gmsl_locked && !video_locked)
	{
		if(stream_id[channel - 1] == 0)
		{
			stream_id[channel - 1] = 1;
		}
		else if(stream_id[channel - 1] == 1)
		{
			stream_id[channel - 1] = 0;
		}

		xil_printf("channel %d now stream_id: %d\r\n",channel, stream_id[channel - 1]);
		xgpio_i2c_reg16_write(channel, 0x90>>1, 0x0050, stream_id[channel - 1]&0x1, STRETCH_ON);
		xgpio_i2c_reg16_write(channel, 0x90>>1, 0x0010, 0x31, STRETCH_ON);
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
	if(timer_cnt >= 1)//500ms
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
					VC_inst.send_err_start[ch] = 0U;
					VC_inst.pkg_cnt = 1U;
#endif
				}
			}
			else
			{
				pixel_err_cnt[ch] = 0U;
			}

			if(pixel_cp_start[ch] == 1U)
			{
				u32 base_addr;
				if(pixel_cp_start_cnt[ch] == 1U)
				{
					vdma_lvds_path_op(ch, 1U);
//					xil_printf("----------pixel_cp_start_%d_cnt %d-----------\r\n",ch+1,pixel_cp_start_cnt[ch]);
					pixel_cp_start_cnt[ch] = pixel_cp_start_cnt[ch] + 1;
				}
				else if(pixel_cp_start_cnt[ch] == 3U)
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
#if defined (BSP_HAS_VDMA) && defined (XPAR_AXI_PASSTHROUGH_MONITOR_NUM_INSTANCES)
		        	s2mm_hot_reset_ch(ch);
#endif
		        	vdma_lvds_path_op(ch, 0U);
#if defined (UDP_COMMAND_SRV) && defined (UDP_VIDEO)
		        	if (err_auto_send)
		        	{
		        		vcmp_m_refresh_channel(ch);
		        		err_auto_send_func(ch);
		        	}
#endif
		        }

		        reconfig_flag[ch] = 0;
		    }
		    else
		    {
		        /* 已 lock：reconfig_flag==1 仅首拍 cable-up；>=2 后靠 s2mm_hot_runtime_geom_poll_ch 跟分辨率 */
		        if (reconfig_flag[ch] < 2)
		        {
		        	reconfig_flag[ch]++;
		        }

		        if (reconfig_flag[ch] == 1)
		        {


		        	xil_printf("----------cable %d up-----------\r\n",ch+1);
	            	XGpio_DiscreteWrite(&XGpioOutput_oldi, 1, 0x1);
	            	usleep(20*1000);
	            	XGpio_DiscreteWrite(&XGpioOutput_oldi, 1, 0x0);
		        	iterationCounts = 0U;
		        	vdma_lvds_path_op(ch, 1U);
#if defined (BSP_HAS_VDMA) && defined (XPAR_AXI_PASSTHROUGH_MONITOR_NUM_INSTANCES)
		        	s2mm_hot_seed_from_mon(ch, vdma_passthrough_mon_base_lvds(ch));
#endif
		        }

		        clear_flag[ch] = 0;
		    }
#if defined (BSP_HAS_VDMA) && defined (XPAR_AXI_PASSTHROUGH_MONITOR_NUM_INSTANCES)
		    s2mm_hot_runtime_geom_poll_ch(ch);
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
