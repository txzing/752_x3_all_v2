#include "../bsp.h"
#if defined (XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES)

int Pc_Config_Initialize(Pc_Config *InstancePtr, u16 DeviceId)
{
	const XAxisPixelCompare_Config *x;

	Xil_AssertNonvoid(InstancePtr != NULL);

	x = XAxisPixelCompare_LookupConfig(DeviceId);
	if (x == NULL) {
		return XST_DEVICE_NOT_FOUND;
	}

	InstancePtr->DeviceId = x->DeviceId;
	InstancePtr->BaseAddress = x->S00_axi_BaseAddr;
	InstancePtr->Interrupt_ID = x->IntrId;

	Xil_Out32(InstancePtr->BaseAddress + STATUS, 0x0);
	Xil_Out32(InstancePtr->BaseAddress + PIXEL_THRESHOLD, 0x5);

	return XST_SUCCESS;
}


volatile u8 pixel_err[XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES] = {0};
volatile u8 pixel_err_cnt[XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES] = {0};
volatile u8 pixel_cp_start[XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES] = {0};
volatile u8 pixel_cp_start_cnt[XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES] = {0};
volatile u8 err_auto_send = 0;
Pc_Config PC_inst[XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES] =  {0};
vcmp_message  vcmp_m[XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES] = {0};

/* VDMA 错误帧快照：ch0->buf7, ch1->buf13, ch2->buf19（与 FRAME_BUFFER_7/13/19 一致） */
static u32 pixel_err_snapshot_src(u8 ch)
{
	return FRAME_BUFFER_BASE_ADDR + (FRAME_BUFFER_SIZE0 * (7U + 6U * (u32)ch));
}

static void vcmp_m_fill_from_base(u32 ba, u8 ch)
{
	u32 mon_ba;

	vcmp_m[ch].status = Xil_In32(ba + STATUS);
	vcmp_m[ch].channel = ch + 1U;
#if defined (XPAR_AXI_PASSTHROUGH_MONITOR_NUM_INSTANCES)
	mon_ba = vdma_passthrough_mon_base_lvds(ch);
	if (mon_ba == 0U
	    || vdma_passthrough_read_mon(mon_ba, &vcmp_m[ch].Width, &vcmp_m[ch].Height,
					 &vcmp_m[ch].fps) == 0)
#endif
	{
		vcmp_m[ch].Width = Xil_In32(ba + COL);
		vcmp_m[ch].Height = Xil_In32(ba + LINE);
		vcmp_m[ch].fps = Xil_In32(ba + FPS);
	}
	vcmp_m[ch].fps_total_cnt = Xil_In32(ba + FPS_TOTAL_CNT);
	vcmp_m[ch].error_pixel_hold = rgb_host_from_reg_rbg(Xil_In32(ba + ERROE_DATA_HOLD));
	vcmp_m[ch].pixel_hold = rgb_host_from_reg_rbg(Xil_In32(ba + STREAM_IN_DATA_HOLD));
	vcmp_m[ch].pixel_threshold = Xil_In32(ba + PIXEL_THRESHOLD);
	vcmp_m[ch].error_col = Xil_In32(ba + ERR_COL);
	vcmp_m[ch].error_line = Xil_In32(ba + ERR_LINE);
	vcmp_m[ch].rgb_cnt_pixel = rgb_host_from_reg_rbg(Xil_In32(ba + RGB_CNT_PIXEL));
	vcmp_m[ch].rgb_pixel_total = Xil_In32(ba + RGB_PIXEL_TOTAL);
	vcmp_m[ch].rgb_not_pixel = rgb_host_from_reg_rbg(Xil_In32(ba + RGB_NOT_PIXEL));
	/* ROI：寄存器低 16 位有效，高 16 位读回保留为 0（与 axis_pixel_compare 一致） */
	vcmp_m[ch].roi_x_start = (Xil_In32(ba + ROI_X_START));
	vcmp_m[ch].roi_x_end = (Xil_In32(ba + ROI_X_END));
	vcmp_m[ch].roi_y_start = (Xil_In32(ba + ROI_Y_START));
	vcmp_m[ch].roi_y_end = (Xil_In32(ba + ROI_Y_END));
	vcmp_m[ch].point_x = (Xil_In32(ba + POINT_X));
	vcmp_m[ch].point_y = (Xil_In32(ba + POINT_Y));
	vcmp_m[ch].point_pixel = rgb_host_from_reg_rbg(Xil_In32(ba + POINT_PIXEL));
}

void vcmp_m_refresh_channel(u8 ch)
{
	u32 ba;

	if (ch >= XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES)
	{
		return;
	}
	ba = pixel_compare_axi_base_eth((u8)(ch + 1U));
	if (ba == 0U)
	{
		return;
	}
	vcmp_m_fill_from_base(ba, ch);
}

u32 pixel_compare_axi_base_eth(u8 ch_based)
{
	u8 idx;

	if (ch_based < 1U)
	{
		return 0U;
	}
	idx = ch_based - 1U;
	if (idx >= (u8)XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES)
	{
		return 0U;
	}
	if (PC_inst[idx].BaseAddress != 0U)
	{
		return (u32)PC_inst[idx].BaseAddress;
	}
	return (u32)XAxisPixelCompare_ConfigTable[idx].S00_axi_BaseAddr;
}

void pixel_err_handel(void)
{
	VdmaChannel *const currentChannel = &VC_inst;

	for (u8 ch = 0U; ch < XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES; ch++)
	{
		if (pixel_err[ch] != 1U)
		{
			continue;
		}

		xil_printf("cp err_buf ch[%d] err!\r\n", (int)ch + 1);
		memcpy(currentChannel->Frame_Err_Buffers[ch],
		       (void *)pixel_err_snapshot_src(ch), FRAME_BUFFER_SIZE0);
		Xil_DCacheFlushRange(currentChannel->Frame_Err_Buffers[ch], FRAME_BUFFER_SIZE0);
		pixel_err[ch] = 2U;
		pixel_err_cnt[ch] = 0U;
#if defined (UDP_COMMAND_SRV) && defined (UDP_VIDEO)
		if (err_auto_send)
		{
			err_auto_send_func(ch);
		}
#endif
	}
}


//中断产生给状态标志
//检测到出错,当前vdma地址正在写入,等待写完，获取该地址复制到另一块区域
void PixelCompareIntrHandler(void *CallbackRef)
{
	if (CallbackRef == NULL)
	{
		xil_printf("PixelCompareIntrHandler: CallbackRef is NULL\r\n");
		return;
	}

	Pc_Config *const PC_p = (Pc_Config *)CallbackRef;
	const u32 intr_status = Xil_In32(PC_p->BaseAddress + STATUS);
	const u8 ch = (u8)PC_p->DeviceId;

	if (ch >= XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES)
	{
		return;
	}

	/* 与 RTL 一致：只要本帧锁存了比较错误且比较仍开启即处理（勿要求 bit2==1） */
	if ((intr_status & PC_STATUS_ERR_CMP_MASK) == PC_STATUS_ERR_CMP_MASK)
	{
//		xil_printf("intr %d status %x\r\n", (int)ch + 1, intr_status);
		if (pixel_err[ch] == 0U)
		{
			pixel_err[ch] = 1U;
			vcmp_m_fill_from_base(PC_p->BaseAddress, ch);
			xil_printf("PCITR %d\r\n", (int)ch + 1);
//			xil_printf("-FPS_TOTAL_CNT: %d -\r\n", Xil_In32(PC_p->BaseAddress + FPS_TOTAL_CNT));
			xil_printf("-PIXEL_POINT: %x -\r\n", rbg_swap_rgb(Xil_In32(PC_p->BaseAddress + ERROE_DATA_HOLD)));
			xil_printf("-STREAM_IN_DATA_HOLD: %x -\r\n", rbg_swap_rgb(Xil_In32(PC_p->BaseAddress + STREAM_IN_DATA_HOLD)));
			xil_printf("-ERR_COL: %d -\r\n", vcmp_m[ch].error_col);
			xil_printf("-ERR_LINE: %d -\r\n", vcmp_m[ch].error_line);
//			xil_printf("-RGB_CNT_PIXEL: %x -\r\n", rbg_swap_rgb(Xil_In32(PC_p->BaseAddress + RGB_CNT_PIXEL)));
//			xil_printf("-RGB_PIXEL_TOTAL: %d -\r\n", Xil_In32(PC_p->BaseAddress + RGB_PIXEL_TOTAL));
//			xil_printf("-RGB_NOT_PIXEL: %x -\r\n", rbg_swap_rgb(Xil_In32(PC_p->BaseAddress + RGB_NOT_PIXEL)));
//			xil_printf("-ROI_X_START: %d -\r\n", (Xil_In32(PC_p->BaseAddress + ROI_X_START)) & 0xFFFFU);
//			xil_printf("-ROI_X_END: %d -\r\n", (Xil_In32(PC_p->BaseAddress + ROI_X_END)) & 0xFFFFU);
//			xil_printf("-ROI_Y_START: %d -\r\n", (Xil_In32(PC_p->BaseAddress + ROI_Y_START)) & 0xFFFFU);
//			xil_printf("-ROI_Y_END: %d -\r\n", (Xil_In32(PC_p->BaseAddress + ROI_Y_END)) & 0xFFFFU);
		}
		Xil_Out32(PC_p->BaseAddress + INTR_CLEAR, 0x1);
	}
}

int PixelCompare_init(void)
{
	INTC *const IntcInstPtr = &InterruptController;
	int Status;

	for (u8 Index = 0U; Index < XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES; Index++)
	{
		Pc_Config_Initialize(&PC_inst[Index],
				XAxisPixelCompare_ConfigTable[Index].DeviceId);
		xil_printf("PC_inst[Index].Interrupt_ID [%d]\r\n", PC_inst[Index].Interrupt_ID);
		Status = INTC_CONNECT(IntcInstPtr, PC_inst[Index].Interrupt_ID,
				      (Xil_ExceptionHandler)PixelCompareIntrHandler,
				      (void *)&PC_inst[Index]);
		XScuGic_SetPriorityTriggerType(IntcInstPtr, PC_inst[Index].Interrupt_ID,
					       (0x28U + Index * 8U), 0x3U);

		if (Status != XST_SUCCESS)
		{
			return Status;
		}

		INTC_CONNECT_ENABLE(IntcInstPtr, PC_inst[Index].Interrupt_ID);
		pixel_err_cnt[Index] = 0U;
		pixel_cp_start[Index] = 0U;
		pixel_cp_start_cnt[Index] = 0U;
	}

	return XST_SUCCESS;
}

uint32_t rbg_swap_rgb(uint32_t pixel)
{
	/* Host 0x00RRGGBB -> HW RBG register (R@[23:16], B@[15:8], G@[7:0]). */
	return (pixel & 0x00FF0000U) | ((pixel & 0x0000FF00U) >> 8) | ((pixel & 0x000000FFU) << 8);
}

uint32_t rgb_host_from_reg_rbg(uint32_t reg_rbg)
{
	const uint32_t r = (reg_rbg >> 16) & 0xFFU;
	const uint32_t b = (reg_rbg >> 8) & 0xFFU;
	const uint32_t g = reg_rbg & 0xFFU;

	/* LE 线上字节顺序 R,G,B，与上位机 Set/Get Statistics Pixel 一致 */
	return r | (g << 8) | (b << 16);
}

#endif
