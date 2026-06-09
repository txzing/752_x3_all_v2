#include "../bsp.h"

#ifdef XPAR_XAXIVDMA_NUM_INSTANCES

#if defined (UDP_VIDEO) || defined (TCP_VIDEO)

volatile u32 iterationCounts = 0; // 每个通道的计数器

static void vdma_lwip_program_s2mm(VdmaChannel *vc, u32 want_w, u32 h);
static void WriteCallBack(void *CallbackRef, u32 Mask);
static void WriteErrorCallBack(void *CallbackRef, u32 Mask);

VdmaChannel VC_inst =
{
	.channel_ID = 1,
	.DeviceID = XPAR_AXI_VDMA_LWIP_DEVICE_ID,
	.S2MM_INTR_ID = XPAR_INTC_0_AXIVDMA_0_VEC_ID,
	.FrameBuffers = {FRAME_BUFFER_1, FRAME_BUFFER_2, FRAME_BUFFER_3},
	.UDP_IMG_PACKEG_SIZE = 1200,
	.VIDEO_FLAG = SEND_VIDEO,
	.send_pic_start = 0,
	.video_sending = 0,
	.send_video_start = 0,
	.send_err_start = {0},
	/* 与 main 默认 current_ch=1（LVDS0 / 5120x1600）一致，直至 cable up 或切源后重配 */
	.Stride = 5120,
	.Width = 5120,
	.Height = 1600
};

int vdma_udp_init(void)
{
	// 初始化VDMA通道
    int Status = Vdma_Lwip_Video_Init(&VC_inst);
    if (Status != XST_SUCCESS)
    {
        xil_printf("VDMA Initialization failed\r\n");
        return XST_FAILURE;
    }

    xil_printf("VDMA Channels initialized and ready.\r\n");
    return Status;
}

int Vdma_Lwip_Video_Init(VdmaChannel *VC_inst)
{
    int Status;
	INTC *IntcInstPtr = &InterruptController;
	XAxiVdma_Config *Config;
	//
	Config = XAxiVdma_LookupConfig(VC_inst->DeviceID);
	if (Config == NULL)
	{
		xil_printf("LWIP VDMA: LookupConfig failed\r\n");
		return XST_FAILURE;
	}

	Status = XAxiVdma_CfgInitialize(&VC_inst->Vdma, Config, Config->BaseAddress);
	if (Status != XST_SUCCESS)
	{
		xil_printf("LWIP VDMA %d: CfgInitialize failed\r\n");
		return XST_FAILURE;
	}

	XAxiVdma_SetCallBack(&VC_inst->Vdma, XAXIVDMA_HANDLER_GENERAL,WriteCallBack, VC_inst, XAXIVDMA_WRITE);
	XAxiVdma_SetCallBack(&VC_inst->Vdma, XAXIVDMA_HANDLER_ERROR,WriteErrorCallBack, VC_inst, XAXIVDMA_WRITE);


	Status = INTC_CONNECT(IntcInstPtr, VC_inst->S2MM_INTR_ID,
			 (XInterruptHandler)XAxiVdma_WriteIntrHandler, &VC_inst->Vdma);
	if (Status != XST_SUCCESS)
	{
		xil_printf("Failed write channel connect intc %d\r\n", Status);
		return XST_FAILURE;
	}

	INTC_CONNECT_ENABLE(IntcInstPtr, VC_inst->S2MM_INTR_ID);

	XAxiVdma_IntrEnable(&VC_inst->Vdma, XAXIVDMA_IXR_ALL_MASK, XAXIVDMA_WRITE);

	//init channel
	VC_inst->WrIndex = 0;
	VC_inst->RdIndex = 0;
#if defined (XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES)
	for(u8 ch = 0; ch < XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES; ch++)
	{
		VC_inst->Frame_Err_Buffers[ch] = FRAME_ERR_BUFFER - (FRAME_BUFFER_SIZE0*ch);
	    memset(VC_inst->Frame_Err_Buffers[ch],0xff,FRAME_BUFFER_SIZE0);
		Xil_DCacheFlushRange(VC_inst->Frame_Err_Buffers[ch], FRAME_BUFFER_SIZE0);
		VC_inst->send_err_start[ch] = 0;
	}
#endif
	VC_inst->WriteError = 0;
	VC_inst->WriteOneFrameEnd = -1;

	vdma_lwip_program_s2mm(VC_inst, VC_inst->Width, VC_inst->Height);

	xil_printf("LWIP VDMA: Initialized successfully\r\n");

    return XST_SUCCESS;
}

static void vdma_lwip_program_s2mm(VdmaChannel *vc, u32 w, u32 h)
{
	if (vc->Width == w && vc->Height == h && vc->FrameLength == (w * h * 3U))
	{
		return;
	}

	vc->Width = w;
	vc->Height = h;
	vc->Stride = w;
	vc->FrameLength = w * h * 3U;
	vc->pkg_cnt = 1U;
	vc->udp_send_times =  (u16)(((vc->FrameLength + (vc->UDP_IMG_PACKEG_SIZE - 1U)) / vc->UDP_IMG_PACKEG_SIZE));
	xil_printf("now vdma_lwip_program_s2mm :stride = %d, w = %d, h = %d\r\n",vc->Stride,vc->Width,vc->Height);
	vdma_write_stop(&vc->Vdma);
	XAxiVdma_IntrDisable(&vc->Vdma, XAXIVDMA_IXR_ALL_MASK, XAXIVDMA_WRITE);
	vdma_write_init(vc->DeviceID, &vc->Vdma, w * 3U, h, w * 3U,
			vc->FrameBuffers[0], vc->FrameBuffers[1], vc->FrameBuffers[2]);
	XAxiVdma_IntrEnable(&vc->Vdma, XAXIVDMA_IXR_ALL_MASK, XAXIVDMA_WRITE);
	(void)vdma_write_start(&vc->Vdma);
}


void MonitorAndExitAfterIterations(void)
{

	if (iterationCounts <= MAX_ITERATIONS)
	{
		if(VC_inst.WriteOneFrameEnd >= 0)
		{
#if (DEBUG_PRINT == 1U)
			// 打印当前通道状态
			DEBUG_PRINTF("Channel %d: WriteEnd=%d, WriteError=%d, WrIndex=%d, RdIndex=%d\r\n",
					VC_inst.DeviceID, VC_inst.WriteOneFrameEnd, VC_inst.WriteError,
					VC_inst.WrIndex, VC_inst.RdIndex);

			// 检查是否达到最大运行次数
			if (iterationCounts >= MAX_ITERATIONS)
			{
				DEBUG_PRINTF("Channel %d: Reached max iterations (%d), exiting.\r\n",
						VC_inst.DeviceID, MAX_ITERATIONS);
				return; // 全部通道完成后退出
			}
#endif
			// 清除完成标志
			VC_inst.WriteOneFrameEnd = -1;
			// 增加计数器
			iterationCounts++;
		}
	}
}

/*****************************************************************************/
/*
 * Call back function for write channel
 *
 * This callback handles frame count interrupts and manages the write/read
 * buffer indices for each channel.
 *
 * @param	CallbackRef is the reference pointer to the channel (VdmaChannel)
 * @param	Mask is the interrupt mask passed from the driver
 *
 * @return	None
 *
 ******************************************************************************/
static void WriteCallBack(void *CallbackRef, u32 Mask)
{
    if (CallbackRef == NULL)
    {
#if (DEBUG_PRINT == 1U)
        DEBUG_PRINTF("WriteCallBack: CallbackRef is NULL\r\n");
#endif
        return;
    }

    VdmaChannel *Channel = (VdmaChannel *)CallbackRef;

    if (Mask & XAXIVDMA_IXR_FRMCNT_MASK)
    {
        if (Channel->WriteOneFrameEnd >= 0)
        {
            return;
        }

        int hold_rd = Channel->RdIndex;

        if (Channel->WrIndex == 2)
        {
            Channel->WrIndex = 0;
            Channel->RdIndex = 2;
        }
        else
        {
            Channel->RdIndex = Channel->WrIndex;
            Channel->WrIndex++;
        }

        int Status = XAxiVdma_StartParking(&Channel->Vdma, Channel->WrIndex, XAXIVDMA_WRITE);
        Channel->WriteOneFrameEnd = hold_rd;
#if (DEBUG_PRINT == 1U)
        if (Status != XST_SUCCESS)
        {
            DEBUG_PRINTF("Channel %d: Failed to start parking. Status=%d\r\n",
                         Channel->DeviceID, Status);
            return;
        }
        DEBUG_PRINTF("Channel %d: Frame write complete. New WrIndex=%d, RdIndex=%d\r\n",
                     Channel->DeviceID, Channel->WrIndex, Channel->RdIndex);
#endif
    }
}

/*****************************************************************************/
/*
 * Call back function for write error
 *
 * This callback handles error interrupts for the write channel.
 *
 * @param	CallbackRef is the reference pointer to the channel (VdmaChannel)
 * @param	Mask is the interrupt mask passed from the driver
 *
 * @return	None
 *
 ******************************************************************************/
static void WriteErrorCallBack(void *CallbackRef, u32 Mask)
{
    if (CallbackRef == NULL)
    {
#if (DEBUG_PRINT == 1U)
        DEBUG_PRINTF("WriteErrorCallBack: CallbackRef is NULL\r\n");
#endif
        return;
    }

    VdmaChannel *Channel = (VdmaChannel *)CallbackRef;

    if (Mask & XAXIVDMA_IXR_ERROR_MASK)
    {
        Channel->WriteError++;
        int Status = XAxiVdma_StartParking(&Channel->Vdma, Channel->WrIndex, XAXIVDMA_WRITE);
#if (DEBUG_PRINT == 1U)
        DEBUG_PRINTF("Channel %d: Write error occurred! Error count=%d\r\n",
                     Channel->DeviceID, Channel->WriteError);
        if (Status != XST_SUCCESS)
        {
            DEBUG_PRINTF("WriteErrorCallBack Channel %d: Failed to start parking. Status=%d\r\n",
                         Channel->DeviceID, Status);
            return;
        }
#endif
    }
}

/* 按 eth_video_ch 重配 LWIP S2MM 的宽高与 stride，并刷新 FrameLength / udp_send_times；会停 DMA 再启 */
void vdma_lwip_apply_channel_geometry(u8 eth_video_ch)
{
	VdmaChannel *vc = &VC_inst;
	u32 w;
	u32 h;

	if (eth_video_ch < 1U || eth_video_ch > CHANNEL_NUM)
	{
		return;
	}
	if (eth_video_ch == 1U)
	{
		w = 5120U;
		h = 1600U;
	}
	else if (eth_video_ch == 2U)
	{
		w = 3040U;
		h = 1708U;
	}

	vdma_lwip_program_s2mm(vc, w, h);
	vc->WrIndex = 0;
	vc->RdIndex = 0;
	vc->WriteOneFrameEnd = 0;
}

void vdma_lwip_arm_pic_capture(void)
{
	VdmaChannel *vc;

	vc = &VC_inst;
	vc->VIDEO_FLAG = SEND_PIC;
	vc->send_pic_start = 1U;
	vc->send_video_start = 0U;
	vc->video_sending = 0U;
	vc->pkg_cnt = 1U;
	/* 勿用缓冲区里在发图指令之前的旧帧；等下一次 S2MM 写满再传 */
	vc->WriteOneFrameEnd = -1;
	iterationCounts = 0U;
}

void vdma_lwip_arm_video_stream(void)
{
	VdmaChannel *vc;

	vc = &VC_inst;
	vc->VIDEO_FLAG = SEND_VIDEO;
	vc->send_pic_start = 0U;
	vc->send_video_start = 1U;
	vc->video_sending = 0U;
	vc->pkg_cnt = 1U;
	vc->WriteOneFrameEnd = -1;
}

void vdma_lwip_stop_media(void)
{
	VdmaChannel *vc;

	vc = &VC_inst;
	vc->send_pic_start = 0U;
	vc->send_video_start = 0U;
	vc->video_sending = 0U;
	vc->pkg_cnt = 1U;
	vc->WriteOneFrameEnd = -1;
}

#endif //#if defined (UDP_VIDEO) || defined (TCP_VIDEO)

#if defined (XPAR_AXI_PASSTHROUGH_MONITOR_NUM_INSTANCES)
u32 vdma_passthrough_mon_base_lvds(u8 lvds_idx_based)
{
	if (lvds_idx_based >= (u8)XPAR_AXI_PASSTHROUGH_MONITOR_NUM_INSTANCES)
	{
		return 0U;
	}
	return (u32)XAxisPassthroughMonitor_ConfigTable[lvds_idx_based].S_axi_lite_BaseAddr;
}

int vdma_passthrough_read_mon(u32 mon_base, u32 *out_w, u32 *out_h, u32 *out_fps)
{
	u32 w1;
	u32 h1;

	if (mon_base == 0U || out_w == NULL || out_h == NULL)
	{
		return 0;
	}
	w1 = Xil_In32(mon_base);
	h1 = Xil_In32(mon_base + 4U);
	if ((w1 < 10U) || (h1 < 10U))
	{
		*out_w = w1;
		*out_h = h1;
		return 0;
	}
	*out_w = w1;
	*out_h = h1;
	if (out_fps != NULL)
	{
		*out_fps = Xil_In32(mon_base + 8U);
	}
	return 1;
}
#endif


u32 vdma_version(XAxiVdma *Vdma) {
	return XAxiVdma_GetVersion(Vdma);
}

int vdma_read_start(XAxiVdma *Vdma) {
	int Status;

	// MM2S Startup
	Status = XAxiVdma_DmaStart(Vdma, XAXIVDMA_READ);
	if (Status != XST_SUCCESS)
	{
	   xil_printf("Start read transfer failed %d\n\r", Status);
	   return XST_FAILURE;
	}

	return XST_SUCCESS;
}


int vdma_read_stop(XAxiVdma *Vdma) {
	XAxiVdma_DmaStop(Vdma, XAXIVDMA_READ);
	return XST_SUCCESS;
}


int vdma_read_init(short DeviceID,short HoriSizeInput,short VertSizeInput,short Stride,unsigned int FrameStoreStartAddr)
{
	XAxiVdma Vdma;
	XAxiVdma_Config *Config;
	XAxiVdma_DmaSetup ReadCfg;
	int Status;


	Config = XAxiVdma_LookupConfig(DeviceID);
	if (NULL == Config) {
		xil_printf("XAxiVdma_LookupConfig failure\r\n");
		return XST_FAILURE;
	}

	Status = XAxiVdma_CfgInitialize(&Vdma, Config, Config->BaseAddress);
	if (Status != XST_SUCCESS) {
		xil_printf("XAxiVdma_CfgInitialize failure\r\n");
		return XST_FAILURE;
	}

	

	ReadCfg.EnableCircularBuf = 1;
	ReadCfg.EnableFrameCounter = 0;
	ReadCfg.FixedFrameStoreAddr = 0;

	ReadCfg.EnableSync = 1;
	ReadCfg.PointNum = 1;

	ReadCfg.FrameDelay = 0;

	ReadCfg.VertSizeInput = VertSizeInput;
	ReadCfg.HoriSizeInput = HoriSizeInput;
	ReadCfg.Stride = Stride;

	Status = XAxiVdma_DmaConfig(&Vdma, XAXIVDMA_READ, &ReadCfg);
	if (Status != XST_SUCCESS) {
			xdbg_printf(XDBG_DEBUG_ERROR,
				"Read channel config failed %d\r\n", Status);

			return XST_FAILURE;
	}


	ReadCfg.FrameStoreStartAddr[0] = FrameStoreStartAddr;

	Status = XAxiVdma_DmaSetBufferAddr(&Vdma, XAXIVDMA_READ, ReadCfg.FrameStoreStartAddr);
	if (Status != XST_SUCCESS) {
			xdbg_printf(XDBG_DEBUG_ERROR,"Read channel set buffer address failed %d\r\n", Status);
			return XST_FAILURE;
	}


	Status = vdma_read_start(&Vdma);
	if (Status != XST_SUCCESS) {
		   xil_printf("error starting VDMA..!");
		   return Status;
	}
	return XST_SUCCESS;

}


int vdma_write_start(XAxiVdma *Vdma) {
	int Status;

	// MM2S Startup
	Status = XAxiVdma_DmaStart(Vdma, XAXIVDMA_WRITE);
	if (Status != XST_SUCCESS)
	{
	   xil_printf("Start write transfer failed %d\n\r", Status);
	   return XST_FAILURE;
	}

	return XST_SUCCESS;
}


int vdma_write_stop(XAxiVdma *Vdma) {
	XAxiVdma_DmaStop(Vdma, XAXIVDMA_WRITE);
	return XST_SUCCESS;
}


int vdma_write_init(short DeviceID,XAxiVdma *Vdma,short HoriSizeInput,short VertSizeInput,short Stride,
		unsigned int FrameStoreStartAddr0,unsigned int FrameStoreStartAddr1,unsigned int FrameStoreStartAddr2)
{
	XAxiVdma_FrameCounter FrameCfg;
	XAxiVdma_DmaSetup WriteCfg;
	int Status;

//	XAxiVdma_Config *Config;
//	Config = XAxiVdma_LookupConfig(DeviceID);
//	if (NULL == Config) {
//		xil_printf("XAxiVdma_LookupConfig failure\r\n");
//		return XST_FAILURE;
//	}
//
//	Status = XAxiVdma_CfgInitialize(Vdma, Config, Config->BaseAddress);
//	if (Status != XST_SUCCESS) {
//		xil_printf("XAxiVdma_CfgInitialize failure\r\n");
//		return XST_FAILURE;
//	}

	XAxiVdma_GetFrameCounter(Vdma, &FrameCfg) ;

	if (FrameCfg.ReadFrameCount == 0)
		FrameCfg.ReadFrameCount = 1 ;

	FrameCfg.WriteFrameCount = 1;
	FrameCfg.WriteDelayTimerCount = 10;
	Status = XAxiVdma_SetFrameCounter(Vdma, &FrameCfg);
	if (Status != XST_SUCCESS) {
		xil_printf(
			"Set frame counter failed %d\r\n", Status);

		if(Status == XST_VDMA_MISMATCH_ERROR)
			xil_printf("DMA Mismatch Error\r\n");

		return XST_FAILURE;
	}

	WriteCfg.EnableCircularBuf = 0;
	WriteCfg.EnableFrameCounter = 0;
	WriteCfg.FixedFrameStoreAddr = 0;

	WriteCfg.EnableSync = 1;
	WriteCfg.PointNum = 1;

	WriteCfg.FrameDelay = 0;

	WriteCfg.VertSizeInput = VertSizeInput;
	WriteCfg.HoriSizeInput = HoriSizeInput;
	WriteCfg.Stride = Stride;

	Status = XAxiVdma_DmaConfig(Vdma, XAXIVDMA_WRITE, &WriteCfg);
	if (Status != XST_SUCCESS) {
			xdbg_printf(XDBG_DEBUG_ERROR,
				"Read channel config failed %d\r\n", Status);

			return XST_FAILURE;
	}


	WriteCfg.FrameStoreStartAddr[0] = FrameStoreStartAddr0;
	WriteCfg.FrameStoreStartAddr[1] = FrameStoreStartAddr1;
	WriteCfg.FrameStoreStartAddr[2] = FrameStoreStartAddr2;

	Status = XAxiVdma_DmaSetBufferAddr(Vdma, XAXIVDMA_WRITE, WriteCfg.FrameStoreStartAddr);
	if (Status != XST_SUCCESS) {
			xdbg_printf(XDBG_DEBUG_ERROR,"Write channel set buffer address failed %d\r\n", Status);
			return XST_FAILURE;
	}


//	Status = vdma_write_start(Vdma);
//	if (Status != XST_SUCCESS) {
//		   xil_printf("error starting VDMA..!");
//		   return Status;
//	}

	return XST_SUCCESS;

}

#if 1

#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 1U)
void vdma_config_0(void)
{
	 /* Start of VDMA Configuration */
	u32 bytePerPixels = 3;
	u32 VDMA_BASE_ADDR = XPAR_AXIVDMA_0_BASEADDR;
	int offset0 = 0; // (y*w+x)*Bpp
	int offset1 = 0; // (y*w+x)*Bpp
	u32 stride0 = 0;
	u32 width0 = 0;
	u32 height0 = 0;
	u32 stride1 = 0;  // crop keeps write Stride
	u32 width1 = 0;
	u32 height1 = 0;

#if (defined R1080P60)
	offset0 = 0; // (y*w+x)*Bpp
	offset1 = 0; // (y*w+x)*Bpp
	stride0 = 1920;
	width0 = 1920;
	height0 = 1080;
	stride1 = 1920;  // crop keeps write Stride
	width1 = 1920;
	height1 = 1080;
#elif (defined R4K30W)
	offset0 = 0; // (y*w+x)*Bpp
	offset1 = 0; // (y*w+x)*Bpp
	stride0 = 3840;
	width0 = 3840;
	height0 = 2160;
	stride1 = 3840;  // crop keeps write Stride
	width1 = 3840;
	height1 = 2160;
#else
	offset0 = 0; // (y*w+x)*Bpp
	offset1 = 0; // (y*w+x)*Bpp
	stride0 = 3840;
	width0 = 3840;
	height0 = 2160;
	stride1 = 3840;  // crop keeps write Stride
	width1 = 3840;
	height1 = 2160;
#endif


#if 1
    /* Configure the Write interface (S2MM)*/
    // S2MM Control Register
    Xil_Out32(VDMA_BASE_ADDR + 0x30, 0x8B);
    //S2MM Start Address 1
    Xil_Out32(VDMA_BASE_ADDR + 0xAC, FRAME_BUFFER_1 + offset0);
    //S2MM Start Address 2
    Xil_Out32(VDMA_BASE_ADDR + 0xB0, FRAME_BUFFER_2 + offset0);
    //S2MM Start Address 3
    Xil_Out32(VDMA_BASE_ADDR + 0xB4, FRAME_BUFFER_3 + offset0);
    //S2MM Frame delay / Stride register
    Xil_Out32(VDMA_BASE_ADDR + 0xA8, stride0*bytePerPixels);
    // S2MM HSIZE register
    Xil_Out32(VDMA_BASE_ADDR + 0xA4, width0*bytePerPixels);
    // S2MM VSIZE register
    Xil_Out32(VDMA_BASE_ADDR + 0xA0, height0);

    /* Configure the Read interface (MM2S)*/
    // MM2S Control Register
    Xil_Out32(VDMA_BASE_ADDR + 0x00, 0x8B);
    // MM2S Start Address 1
    Xil_Out32(VDMA_BASE_ADDR + 0x5C, FRAME_BUFFER_1 + offset1);
    // MM2S Start Address 2
    Xil_Out32(VDMA_BASE_ADDR + 0x60, FRAME_BUFFER_2 + offset1);
    // MM2S Start Address 3
    Xil_Out32(VDMA_BASE_ADDR + 0x64, FRAME_BUFFER_3 + offset1);
    // MM2S Frame delay / Stride register
    Xil_Out32(VDMA_BASE_ADDR + 0x58, stride1*bytePerPixels);
    // MM2S HSIZE register
    Xil_Out32(VDMA_BASE_ADDR + 0x54, width1*bytePerPixels);
    // MM2S VSIZE register
    Xil_Out32(VDMA_BASE_ADDR + 0x50, height1);


    /* End of VDMA Configuration */

#else
    /* Configure the Write interface (S2MM)*/
	// S2MM Control Register
	Xil_Out32(VDMA_BASE_ADDR + 0x30, 0x8B);
	//S2MM Start Address 1
	Xil_Out32(VDMA_BASE_ADDR + 0xAC, FRAME_BUFFER_1 + offset0);
	Xil_Out32(VDMA_BASE_ADDR + 0xB0, 0);
	//S2MM Start Address 2
	Xil_Out32(VDMA_BASE_ADDR + 0xB4, FRAME_BUFFER_2+ offset0);
	Xil_Out32(VDMA_BASE_ADDR + 0xB8, 0);
	//S2MM Start Address 3
	Xil_Out32(VDMA_BASE_ADDR + 0xBC, FRAME_BUFFER_3 + offset0);
	Xil_Out32(VDMA_BASE_ADDR + 0xC0, 0);
	//S2MM Frame delay / Stride register
	Xil_Out32(VDMA_BASE_ADDR + 0xA8, stride0*bytePerPixels);
	// S2MM HSIZE register
	Xil_Out32(VDMA_BASE_ADDR + 0xA4, width0*bytePerPixels);
	// S2MM VSIZE register
	Xil_Out32(VDMA_BASE_ADDR + 0xA0, height0);

	/* Configure the Read interface (MM2S)*/
	// MM2S Control Register
	Xil_Out32(VDMA_BASE_ADDR + 0x00, 0x8B);
	// MM2S Start Address 1
	Xil_Out32(VDMA_BASE_ADDR + 0x5C, FRAME_BUFFER_1 + offset1);
	Xil_Out32(VDMA_BASE_ADDR + 0x60, 0);
	// MM2S Start Address 2
	Xil_Out32(VDMA_BASE_ADDR + 0x64, FRAME_BUFFER_2 + offset1);
	Xil_Out32(VDMA_BASE_ADDR + 0x68, 0);
	// MM2S Start Address 3
	Xil_Out32(VDMA_BASE_ADDR + 0x6C, FRAME_BUFFER_3 + offset1);
	Xil_Out32(VDMA_BASE_ADDR + 0x70, 0);
	// MM2S Frame delay / Stride register
	Xil_Out32(VDMA_BASE_ADDR + 0x58, stride1*bytePerPixels);
	// MM2S HSIZE register
	Xil_Out32(VDMA_BASE_ADDR + 0x54, width1*bytePerPixels);
	// MM2S VSIZE register
	Xil_Out32(VDMA_BASE_ADDR + 0x50, height1);


	/* End of VDMA Configuration */

#endif
	xil_printf("VDMA_0 started!\r\n");
}
#endif

#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 2U)
void vdma_config_1(void)
{
	 /* Start of VDMA Configuration */
	u32 bytePerPixels = 3;
	u32 VDMA_BASE_ADDR = XPAR_AXIVDMA_1_BASEADDR;
	int offset0 = 0; // (y*w+x)*Bpp
	int offset1 = 0; // (y*w+x)*Bpp
	u32 stride0 = 0;
	u32 width0 = 0;
	u32 height0 = 0;
	u32 stride1 = 0;  // crop keeps write Stride
	u32 width1 = 0;
	u32 height1 = 0;

#if (defined R1080P60)
	offset0 = 0; // (y*w+x)*Bpp
	offset1 = 0; // (y*w+x)*Bpp
	stride0 = 1920;
	width0 = 1920;
	height0 = 1080;
	stride1 = 1920;  // crop keeps write Stride
	width1 = 1920;
	height1 = 1080;
#elif (defined R4K30W)
	offset0 = 0; // (y*w+x)*Bpp
	offset1 = 0; // (y*w+x)*Bpp
	stride0 = 3840;
	width0 = 3840;
	height0 = 2160;
	stride1 = 3840;  // crop keeps write Stride
	width1 = 3840;
	height1 = 2160;
#else
	offset0 = 0; // (y*w+x)*Bpp
	offset1 = 0; // (y*w+x)*Bpp
	stride0 = 5120;
	width0 = 5120;
	height0 = 1600;
	stride1 = 5120;  // crop keeps write Stride
	width1 = 5120;
	height1 = 1600;
#endif


#if 1
    /* Configure the Write interface (S2MM)*/
    // S2MM Control Register
    Xil_Out32(VDMA_BASE_ADDR + 0x30, 0x8B);
    //S2MM Start Address 1
    Xil_Out32(VDMA_BASE_ADDR + 0xAC, FRAME_BUFFER_4 + offset0);
    //S2MM Start Address 2
    Xil_Out32(VDMA_BASE_ADDR + 0xB0, FRAME_BUFFER_5 + offset0);
    //S2MM Start Address 3
    Xil_Out32(VDMA_BASE_ADDR + 0xB4, FRAME_BUFFER_6 + offset0);
    //S2MM Frame delay / Stride register
    Xil_Out32(VDMA_BASE_ADDR + 0xA8, stride0*bytePerPixels);
    // S2MM HSIZE register
    Xil_Out32(VDMA_BASE_ADDR + 0xA4, width0*bytePerPixels);
    // S2MM VSIZE register
    Xil_Out32(VDMA_BASE_ADDR + 0xA0, height0);

    /* Configure the Read interface (MM2S)*/
    // MM2S Control Register
    Xil_Out32(VDMA_BASE_ADDR + 0x00, 0x8B);
    // MM2S Start Address 1
    Xil_Out32(VDMA_BASE_ADDR + 0x5C, FRAME_BUFFER_4 + offset1);
    // MM2S Start Address 2
    Xil_Out32(VDMA_BASE_ADDR + 0x60, FRAME_BUFFER_5 + offset1);
    // MM2S Start Address 3
    Xil_Out32(VDMA_BASE_ADDR + 0x64, FRAME_BUFFER_6 + offset1);
    // MM2S Frame delay / Stride register
    Xil_Out32(VDMA_BASE_ADDR + 0x58, stride1*bytePerPixels);
    // MM2S HSIZE register
    Xil_Out32(VDMA_BASE_ADDR + 0x54, width1*bytePerPixels);
    // MM2S VSIZE register
    Xil_Out32(VDMA_BASE_ADDR + 0x50, height1);


    /* End of VDMA Configuration */

#else
    /* Configure the Write interface (S2MM)*/
	// S2MM Control Register
	Xil_Out32(VDMA_BASE_ADDR + 0x30, 0x8B);
	//S2MM Start Address 1
	Xil_Out32(VDMA_BASE_ADDR + 0xAC, FRAME_BUFFER_4 + offset0);
	Xil_Out32(VDMA_BASE_ADDR + 0xB0, 0);
	//S2MM Start Address 2
	Xil_Out32(VDMA_BASE_ADDR + 0xB4, FRAME_BUFFER_5 + offset0);
	Xil_Out32(VDMA_BASE_ADDR + 0xB8, 0);
	//S2MM Start Address 3
	Xil_Out32(VDMA_BASE_ADDR + 0xBC, FRAME_BUFFER_6 + offset0);
	Xil_Out32(VDMA_BASE_ADDR + 0xC0, 0);
	//S2MM Frame delay / Stride register
	Xil_Out32(VDMA_BASE_ADDR + 0xA8, stride0*bytePerPixels);
	// S2MM HSIZE register
	Xil_Out32(VDMA_BASE_ADDR + 0xA4, width0*bytePerPixels);
	// S2MM VSIZE register
	Xil_Out32(VDMA_BASE_ADDR + 0xA0, height0);

	/* Configure the Read interface (MM2S)*/
	// MM2S Control Register
	Xil_Out32(VDMA_BASE_ADDR + 0x00, 0x8B);
	// MM2S Start Address 1
	Xil_Out32(VDMA_BASE_ADDR + 0x5C, FRAME_BUFFER_4 + offset1);
	Xil_Out32(VDMA_BASE_ADDR + 0x60, 0);
	// MM2S Start Address 2
	Xil_Out32(VDMA_BASE_ADDR + 0x64, FRAME_BUFFER_5 + offset1);
	Xil_Out32(VDMA_BASE_ADDR + 0x68, 0);
	// MM2S Start Address 3
	Xil_Out32(VDMA_BASE_ADDR + 0x6C, FRAME_BUFFER_6 + offset1);
	Xil_Out32(VDMA_BASE_ADDR + 0x70, 0);
	// MM2S Frame delay / Stride register
	Xil_Out32(VDMA_BASE_ADDR + 0x58, stride1*bytePerPixels);
	// MM2S HSIZE register
	Xil_Out32(VDMA_BASE_ADDR + 0x54, width1*bytePerPixels);
	// MM2S VSIZE register
	Xil_Out32(VDMA_BASE_ADDR + 0x50, height1);


	/* End of VDMA Configuration */

#endif
	xil_printf("VDMA_1 started!\r\n");
}
#endif

#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 3U)
void vdma_config_2(void)
{
	 /* Start of VDMA Configuration */
	u32 bytePerPixels = 3;
	u32 VDMA_BASE_ADDR = XPAR_AXIVDMA_2_BASEADDR;
	int offset0 = 0; // (y*w+x)*Bpp
	int offset1 = 0; // (y*w+x)*Bpp
	u32 stride0 = 0;
	u32 width0 = 0;
	u32 height0 = 0;
	u32 stride1 = 0;  // crop keeps write Stride
	u32 width1 = 0;
	u32 height1 = 0;

#if (defined R1080P60)
	offset0 = 0; // (y*w+x)*Bpp
	offset1 = 0; // (y*w+x)*Bpp
	stride0 = 1920;
	width0 = 1920;
	height0 = 1080;
	stride1 = 1920;  // crop keeps write Stride
	width1 = 1920;
	height1 = 1080;
#elif (defined R4K30W)
	offset0 = 0; // (y*w+x)*Bpp
	offset1 = 0; // (y*w+x)*Bpp
	stride0 = 3840;
	width0 = 3840;
	height0 = 2160;
	stride1 = 3840;  // crop keeps write Stride
	width1 = 3840;
	height1 = 2160;
#else
	offset0 = 0; // (y*w+x)*Bpp
	offset1 = 0; // (y*w+x)*Bpp
	stride0 = 5120;
	width0 = 5120;
	height0 = 1600;
	stride1 = 5120;  // crop keeps write Stride
	width1 = 5120;
	height1 = 1600;
#endif




#if 1
    /* Configure the Write interface (S2MM)*/
    // S2MM Control Register
    Xil_Out32(VDMA_BASE_ADDR + 0x30, 0x8B);
    //S2MM Start Address 1
    Xil_Out32(VDMA_BASE_ADDR + 0xAC, FRAME_BUFFER_7 + offset0);
    //S2MM Start Address 2
    Xil_Out32(VDMA_BASE_ADDR + 0xB0, FRAME_BUFFER_8 + offset0);
    //S2MM Start Address 3
    Xil_Out32(VDMA_BASE_ADDR + 0xB4, FRAME_BUFFER_9 + offset0);
    //S2MM Frame delay / Stride register
    Xil_Out32(VDMA_BASE_ADDR + 0xA8, stride0*bytePerPixels);
    // S2MM HSIZE register
    Xil_Out32(VDMA_BASE_ADDR + 0xA4, width0*bytePerPixels);
    // S2MM VSIZE register
    Xil_Out32(VDMA_BASE_ADDR + 0xA0, height0);

    /* Configure the Read interface (MM2S)*/
    // MM2S Control Register
    Xil_Out32(VDMA_BASE_ADDR + 0x00, 0x8B);
    // MM2S Start Address 1
    Xil_Out32(VDMA_BASE_ADDR + 0x5C, FRAME_BUFFER_7 + offset1);
    // MM2S Start Address 2
    Xil_Out32(VDMA_BASE_ADDR + 0x60, FRAME_BUFFER_8 + offset1);
    // MM2S Start Address 3
    Xil_Out32(VDMA_BASE_ADDR + 0x64, FRAME_BUFFER_9 + offset1);
    // MM2S Frame delay / Stride register
    Xil_Out32(VDMA_BASE_ADDR + 0x58, stride1*bytePerPixels);
    // MM2S HSIZE register
    Xil_Out32(VDMA_BASE_ADDR + 0x54, width1*bytePerPixels);
    // MM2S VSIZE register
    Xil_Out32(VDMA_BASE_ADDR + 0x50, height1);


    /* End of VDMA Configuration */

#else
    /* Configure the Write interface (S2MM)*/
	// S2MM Control Register
	Xil_Out32(VDMA_BASE_ADDR + 0x30, 0x8B);
	//S2MM Start Address 1
	Xil_Out32(VDMA_BASE_ADDR + 0xAC, FRAME_BUFFER_7 + offset0);
	Xil_Out32(VDMA_BASE_ADDR + 0xB0, 0);
	//S2MM Start Address 2
	Xil_Out32(VDMA_BASE_ADDR + 0xB4, FRAME_BUFFER_8 + offset0);
	Xil_Out32(VDMA_BASE_ADDR + 0xB8, 0);
	//S2MM Start Address 3
	Xil_Out32(VDMA_BASE_ADDR + 0xBC, FRAME_BUFFER_9 + offset0);
	Xil_Out32(VDMA_BASE_ADDR + 0xC0, 0);
	//S2MM Frame delay / Stride register
	Xil_Out32(VDMA_BASE_ADDR + 0xA8, stride0*bytePerPixels);
	// S2MM HSIZE register
	Xil_Out32(VDMA_BASE_ADDR + 0xA4, width0*bytePerPixels);
	// S2MM VSIZE register
	Xil_Out32(VDMA_BASE_ADDR + 0xA0, height0);

	/* Configure the Read interface (MM2S)*/
	// MM2S Control Register
	Xil_Out32(VDMA_BASE_ADDR + 0x00, 0x8B);
	// MM2S Start Address 1
	Xil_Out32(VDMA_BASE_ADDR + 0x5C, FRAME_BUFFER_7 + offset1);
	Xil_Out32(VDMA_BASE_ADDR + 0x60, 0);
	// MM2S Start Address 2
	Xil_Out32(VDMA_BASE_ADDR + 0x64, FRAME_BUFFER_8 + offset1);
	Xil_Out32(VDMA_BASE_ADDR + 0x68, 0);
	// MM2S Start Address 3
	Xil_Out32(VDMA_BASE_ADDR + 0x6C, FRAME_BUFFER_9 + offset1);
	Xil_Out32(VDMA_BASE_ADDR + 0x70, 0);
	// MM2S Frame delay / Stride register
	Xil_Out32(VDMA_BASE_ADDR + 0x58, stride1*bytePerPixels);
	// MM2S HSIZE register
	Xil_Out32(VDMA_BASE_ADDR + 0x54, width1*bytePerPixels);
	// MM2S VSIZE register
	Xil_Out32(VDMA_BASE_ADDR + 0x50, height1);


	/* End of VDMA Configuration */

#endif

    xil_printf("VDMA_2 started!\r\n");
}
#endif

#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 4U)
void vdma_config_3(void)
{
	 /* Start of VDMA Configuration */
	u32 bytePerPixels = 3;
	u32 VDMA_BASE_ADDR = XPAR_AXIVDMA_3_BASEADDR;
	int offset0 = 0; // (y*w+x)*Bpp
	int offset1 = 0; // (y*w+x)*Bpp
	u32 stride0 = 0;
	u32 width0 = 0;
	u32 height0 = 0;
	u32 stride1 = 0;  // crop keeps write Stride
	u32 width1 = 0;
	u32 height1 = 0;

#if (defined R1080P60)
	offset0 = 0; // (y*w+x)*Bpp
	offset1 = 0; // (y*w+x)*Bpp
	stride0 = 1920;
	width0 = 1920;
	height0 = 1080;
	stride1 = 1920;  // crop keeps write Stride
	width1 = 1920;
	height1 = 1080;
#elif (defined R4K30W)
	offset0 = 0; // (y*w+x)*Bpp
	offset1 = 0; // (y*w+x)*Bpp
	stride0 = 3840;
	width0 = 3840;
	height0 = 2160;
	stride1 = 3840;  // crop keeps write Stride
	width1 = 3840;
	height1 = 2160;
#else
	offset0 = 0; // (y*w+x)*Bpp
	offset1 = 0; // (y*w+x)*Bpp
	stride0 = 3040;
	width0 = 3040;
	height0 = 1708;
	stride1 = 3040;  // crop keeps write Stride
	width1 = 3040;
	height1 = 1708;
#endif


#if 1
    /* Configure the Write interface (S2MM)*/
    // S2MM Control Register
    Xil_Out32(VDMA_BASE_ADDR + 0x30, 0x8B);
    //S2MM Start Address 1
    Xil_Out32(VDMA_BASE_ADDR + 0xAC, FRAME_BUFFER_10 + offset0);
    //S2MM Start Address 2
    Xil_Out32(VDMA_BASE_ADDR + 0xB0, FRAME_BUFFER_11 + offset0);
    //S2MM Start Address 3
    Xil_Out32(VDMA_BASE_ADDR + 0xB4, FRAME_BUFFER_12 + offset0);
    //S2MM Frame delay / Stride register
    Xil_Out32(VDMA_BASE_ADDR + 0xA8, stride0*bytePerPixels);
    // S2MM HSIZE register
    Xil_Out32(VDMA_BASE_ADDR + 0xA4, width0*bytePerPixels);
    // S2MM VSIZE register
    Xil_Out32(VDMA_BASE_ADDR + 0xA0, height0);

    /* Configure the Read interface (MM2S)*/
    // MM2S Control Register
    Xil_Out32(VDMA_BASE_ADDR + 0x00, 0x8B);
    // MM2S Start Address 1
    Xil_Out32(VDMA_BASE_ADDR + 0x5C, FRAME_BUFFER_10 + offset1);
    // MM2S Start Address 2
    Xil_Out32(VDMA_BASE_ADDR + 0x60, FRAME_BUFFER_11 + offset1);
    // MM2S Start Address 3
    Xil_Out32(VDMA_BASE_ADDR + 0x64, FRAME_BUFFER_12 + offset1);

    // MM2S Frame delay / Stride register
    Xil_Out32(VDMA_BASE_ADDR + 0x58, stride1*bytePerPixels);
    // MM2S HSIZE register
    Xil_Out32(VDMA_BASE_ADDR + 0x54, width1*bytePerPixels);
    // MM2S VSIZE register
    Xil_Out32(VDMA_BASE_ADDR + 0x50, height1);


    /* End of VDMA Configuration */

#else
    /* Configure the Write interface (S2MM)*/
	// S2MM Control Register
	Xil_Out32(VDMA_BASE_ADDR + 0x30, 0x8B);
	//S2MM Start Address 1
	Xil_Out32(VDMA_BASE_ADDR + 0xAC, FRAME_BUFFER_10 + offset0);
	Xil_Out32(VDMA_BASE_ADDR + 0xB0, 0);
	//S2MM Start Address 2
	Xil_Out32(VDMA_BASE_ADDR + 0xB4, FRAME_BUFFER_11 + offset0);
	Xil_Out32(VDMA_BASE_ADDR + 0xB8, 0);
	//S2MM Start Address 3
	Xil_Out32(VDMA_BASE_ADDR + 0xBC, FRAME_BUFFER_12 + offset0);
	Xil_Out32(VDMA_BASE_ADDR + 0xC0, 0);
	//S2MM Frame delay / Stride register
	Xil_Out32(VDMA_BASE_ADDR + 0xA8, stride0*bytePerPixels);
	// S2MM HSIZE register
	Xil_Out32(VDMA_BASE_ADDR + 0xA4, width0*bytePerPixels);
	// S2MM VSIZE register
	Xil_Out32(VDMA_BASE_ADDR + 0xA0, height0);

	/* Configure the Read interface (MM2S)*/
	// MM2S Control Register
	Xil_Out32(VDMA_BASE_ADDR + 0x00, 0x8B);
	// MM2S Start Address 1
	Xil_Out32(VDMA_BASE_ADDR + 0x5C, FRAME_BUFFER_10 + offset1);
	Xil_Out32(VDMA_BASE_ADDR + 0x60, 0);
	// MM2S Start Address 2
	Xil_Out32(VDMA_BASE_ADDR + 0x64, FRAME_BUFFER_11 + offset1);
	Xil_Out32(VDMA_BASE_ADDR + 0x68, 0);
	// MM2S Start Address 3
	Xil_Out32(VDMA_BASE_ADDR + 0x6C, FRAME_BUFFER_12 + offset1);
	Xil_Out32(VDMA_BASE_ADDR + 0x70, 0);
	// MM2S Frame delay / Stride register
	Xil_Out32(VDMA_BASE_ADDR + 0x58, stride1*bytePerPixels);
	// MM2S HSIZE register
	Xil_Out32(VDMA_BASE_ADDR + 0x54, width1*bytePerPixels);
	// MM2S VSIZE register
	Xil_Out32(VDMA_BASE_ADDR + 0x50, height1);


	/* End of VDMA Configuration */

#endif
	xil_printf("VDMA_3 started!\r\n");
}
#endif

#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 5U)
void vdma_config_4(void)
{
	 /* Start of VDMA Configuration */
	u32 bytePerPixels = 3;
	u32 VDMA_BASE_ADDR = XPAR_AXIVDMA_4_BASEADDR;
	int offset0 = 0; // (y*w+x)*Bpp
	int offset1 = 0; // (y*w+x)*Bpp
	u32 stride0 = 0;
	u32 width0 = 0;
	u32 height0 = 0;
	u32 stride1 = 0;  // crop keeps write Stride
	u32 width1 = 0;
	u32 height1 = 0;

#if (defined R1080P60)
	offset0 = 0; // (y*w+x)*Bpp
	offset1 = 0; // (y*w+x)*Bpp
	stride0 = 1920;
	width0 = 1920;
	height0 = 1080;
	stride1 = 1920;  // crop keeps write Stride
	width1 = 1920;
	height1 = 1080;
#elif (defined R4K30W)
	offset0 = 0; // (y*w+x)*Bpp
	offset1 = 0; // (y*w+x)*Bpp
	stride0 = 3840;
	width0 = 3840;
	height0 = 2160;
	stride1 = 3840;  // crop keeps write Stride
	width1 = 3840;
	height1 = 2160;
#else
	offset0 = 0; // (y*w+x)*Bpp
	offset1 = 0; // (y*w+x)*Bpp
	stride0 = 3040;
	width0 = 3040;
	height0 = 1708;
	stride1 = 3040;  // crop keeps write Stride
	width1 = 3040;
	height1 = 1708;
#endif


#if 1
    /* Configure the Write interface (S2MM)*/
    // S2MM Control Register
    Xil_Out32(VDMA_BASE_ADDR + 0x30, 0x8B);
    //S2MM Start Address 1
    Xil_Out32(VDMA_BASE_ADDR + 0xAC, FRAME_BUFFER_13 + offset0);
    //S2MM Start Address 2
    Xil_Out32(VDMA_BASE_ADDR + 0xB0, FRAME_BUFFER_14 + offset0);
    //S2MM Start Address 3
    Xil_Out32(VDMA_BASE_ADDR + 0xB4, FRAME_BUFFER_15 + offset0);
    //S2MM Frame delay / Stride register
    Xil_Out32(VDMA_BASE_ADDR + 0xA8, stride0*bytePerPixels);
    // S2MM HSIZE register
    Xil_Out32(VDMA_BASE_ADDR + 0xA4, width0*bytePerPixels);
    // S2MM VSIZE register
    Xil_Out32(VDMA_BASE_ADDR + 0xA0, height0);

    /* Configure the Read interface (MM2S)*/
    // MM2S Control Register
    Xil_Out32(VDMA_BASE_ADDR + 0x00, 0x8B);
    // MM2S Start Address 1
    Xil_Out32(VDMA_BASE_ADDR + 0x5C, FRAME_BUFFER_13 + offset1);
    // MM2S Start Address 2
    Xil_Out32(VDMA_BASE_ADDR + 0x60, FRAME_BUFFER_14 + offset1);
    // MM2S Start Address 3
    Xil_Out32(VDMA_BASE_ADDR + 0x64, FRAME_BUFFER_15 + offset1);
    // MM2S Frame delay / Stride register
    Xil_Out32(VDMA_BASE_ADDR + 0x58, stride1*bytePerPixels);
    // MM2S HSIZE register
    Xil_Out32(VDMA_BASE_ADDR + 0x54, width1*bytePerPixels);
    // MM2S VSIZE register
    Xil_Out32(VDMA_BASE_ADDR + 0x50, height1);


    /* End of VDMA Configuration */

#else
    /* Configure the Write interface (S2MM)*/
	// S2MM Control Register
	Xil_Out32(VDMA_BASE_ADDR + 0x30, 0x8B);
	//S2MM Start Address 1
	Xil_Out32(VDMA_BASE_ADDR + 0xAC, FRAME_BUFFER_13 + offset0);
	Xil_Out32(VDMA_BASE_ADDR + 0xB0, 0);
	//S2MM Start Address 2
	Xil_Out32(VDMA_BASE_ADDR + 0xB4, FRAME_BUFFER_14 + offset0);
	Xil_Out32(VDMA_BASE_ADDR + 0xB8, 0);
	//S2MM Start Address 3
	Xil_Out32(VDMA_BASE_ADDR + 0xBC, FRAME_BUFFER_15 + offset0);
	Xil_Out32(VDMA_BASE_ADDR + 0xC0, 0);
	//S2MM Frame delay / Stride register
	Xil_Out32(VDMA_BASE_ADDR + 0xA8, stride0*bytePerPixels);
	// S2MM HSIZE register
	Xil_Out32(VDMA_BASE_ADDR + 0xA4, width0*bytePerPixels);
	// S2MM VSIZE register
	Xil_Out32(VDMA_BASE_ADDR + 0xA0, height0);

	/* Configure the Read interface (MM2S)*/
	// MM2S Control Register
	Xil_Out32(VDMA_BASE_ADDR + 0x00, 0x8B);
	// MM2S Start Address 1
	Xil_Out32(VDMA_BASE_ADDR + 0x5C, FRAME_BUFFER_13 + offset1);
	Xil_Out32(VDMA_BASE_ADDR + 0x60, 0);
	// MM2S Start Address 2
	Xil_Out32(VDMA_BASE_ADDR + 0x64, FRAME_BUFFER_14 + offset1);
	Xil_Out32(VDMA_BASE_ADDR + 0x68, 0);
	// MM2S Start Address 3
	Xil_Out32(VDMA_BASE_ADDR + 0x6C, FRAME_BUFFER_15 + offset1);
	Xil_Out32(VDMA_BASE_ADDR + 0x70, 0);
	// MM2S Frame delay / Stride register
	Xil_Out32(VDMA_BASE_ADDR + 0x58, stride1*bytePerPixels);
	// MM2S HSIZE register
	Xil_Out32(VDMA_BASE_ADDR + 0x54, width1*bytePerPixels);
	// MM2S VSIZE register
	Xil_Out32(VDMA_BASE_ADDR + 0x50, height1);


	/* End of VDMA Configuration */

#endif
	xil_printf("VDMA_4 started!\r\n");
}
#endif

#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 6U)
void vdma_config_5(void)
{
	 /* Start of VDMA Configuration */
	u32 bytePerPixels = 3;
	u32 VDMA_BASE_ADDR = XPAR_AXIVDMA_5_BASEADDR;
	int offset0 = 0; // (y*w+x)*Bpp
	int offset1 = 0; // (y*w+x)*Bpp
	u32 stride0 = 0;
	u32 width0 = 0;
	u32 height0 = 0;
	u32 stride1 = 0;  // crop keeps write Stride
	u32 width1 = 0;
	u32 height1 = 0;

#if (defined R1080P60)
	offset0 = 0; // (y*w+x)*Bpp
	offset1 = 0; // (y*w+x)*Bpp
	stride0 = 1920;
	width0 = 1920;
	height0 = 1080;
	stride1 = 1920;  // crop keeps write Stride
	width1 = 1920;
	height1 = 1080;
#elif (defined R4K30W)
	offset0 = 0; // (y*w+x)*Bpp
	offset1 = 0; // (y*w+x)*Bpp
	stride0 = 3840;
	width0 = 3840;
	height0 = 2160;
	stride1 = 3840;  // crop keeps write Stride
	width1 = 3840;
	height1 = 2160;
#else
	offset0 = 0; // (y*w+x)*Bpp
	offset1 = 0; // (y*w+x)*Bpp
	stride0 = 480;
	width0 = 480;
	height0 = 320;
	stride1 = 480;  // crop keeps write Stride
	width1 = 480;
	height1 = 320;
#endif


#if 1
    /* Configure the Write interface (S2MM)*/
    // S2MM Control Register
    Xil_Out32(VDMA_BASE_ADDR + 0x30, 0x8B);
    //S2MM Start Address 1
    Xil_Out32(VDMA_BASE_ADDR + 0xAC, FRAME_BUFFER_16 + offset0);
    //S2MM Start Address 2
    Xil_Out32(VDMA_BASE_ADDR + 0xB0, FRAME_BUFFER_17 + offset0);
    //S2MM Start Address 3
    Xil_Out32(VDMA_BASE_ADDR + 0xB4, FRAME_BUFFER_18 + offset0);
    //S2MM Frame delay / Stride register
    Xil_Out32(VDMA_BASE_ADDR + 0xA8, stride0*bytePerPixels);
    // S2MM HSIZE register
    Xil_Out32(VDMA_BASE_ADDR + 0xA4, width0*bytePerPixels);
    // S2MM VSIZE register
    Xil_Out32(VDMA_BASE_ADDR + 0xA0, height0);

    /* Configure the Read interface (MM2S)*/
    // MM2S Control Register
    Xil_Out32(VDMA_BASE_ADDR + 0x00, 0x8B);
    // MM2S Start Address 1
    Xil_Out32(VDMA_BASE_ADDR + 0x5C, FRAME_BUFFER_16 + offset1);
    // MM2S Start Address 2
    Xil_Out32(VDMA_BASE_ADDR + 0x60, FRAME_BUFFER_17 + offset1);
    // MM2S Start Address 3
    Xil_Out32(VDMA_BASE_ADDR + 0x64, FRAME_BUFFER_18 + offset1);
    // MM2S Frame delay / Stride register
    Xil_Out32(VDMA_BASE_ADDR + 0x58, stride1*bytePerPixels);
    // MM2S HSIZE register
    Xil_Out32(VDMA_BASE_ADDR + 0x54, width1*bytePerPixels);
    // MM2S VSIZE register
    Xil_Out32(VDMA_BASE_ADDR + 0x50, height1);


    /* End of VDMA Configuration */

#else
    /* Configure the Write interface (S2MM)*/
	// S2MM Control Register
	Xil_Out32(VDMA_BASE_ADDR + 0x30, 0x8B);
	//S2MM Start Address 1
	Xil_Out32(VDMA_BASE_ADDR + 0xAC, FRAME_BUFFER_16 + offset0);
	Xil_Out32(VDMA_BASE_ADDR + 0xB0, 0);
	//S2MM Start Address 2
	Xil_Out32(VDMA_BASE_ADDR + 0xB4, FRAME_BUFFER_17 + offset0);
	Xil_Out32(VDMA_BASE_ADDR + 0xB8, 0);
	//S2MM Start Address 3
	Xil_Out32(VDMA_BASE_ADDR + 0xBC, FRAME_BUFFER_18 + offset0);
	Xil_Out32(VDMA_BASE_ADDR + 0xC0, 0);
	//S2MM Frame delay / Stride register
	Xil_Out32(VDMA_BASE_ADDR + 0xA8, stride0*bytePerPixels);
	// S2MM HSIZE register
	Xil_Out32(VDMA_BASE_ADDR + 0xA4, width0*bytePerPixels);
	// S2MM VSIZE register
	Xil_Out32(VDMA_BASE_ADDR + 0xA0, height0);

	/* Configure the Read interface (MM2S)*/
	// MM2S Control Register
	Xil_Out32(VDMA_BASE_ADDR + 0x00, 0x8B);
	// MM2S Start Address 1
	Xil_Out32(VDMA_BASE_ADDR + 0x5C, FRAME_BUFFER_16 + offset1);
	Xil_Out32(VDMA_BASE_ADDR + 0x60, 0);
	// MM2S Start Address 2
	Xil_Out32(VDMA_BASE_ADDR + 0x64, FRAME_BUFFER_17 + offset1);
	Xil_Out32(VDMA_BASE_ADDR + 0x68, 0);
	// MM2S Start Address 3
	Xil_Out32(VDMA_BASE_ADDR + 0x6C, FRAME_BUFFER_18 + offset1);
	Xil_Out32(VDMA_BASE_ADDR + 0x70, 0);
	// MM2S Frame delay / Stride register
	Xil_Out32(VDMA_BASE_ADDR + 0x58, stride1*bytePerPixels);
	// MM2S HSIZE register
	Xil_Out32(VDMA_BASE_ADDR + 0x54, width1*bytePerPixels);
	// MM2S VSIZE register
	Xil_Out32(VDMA_BASE_ADDR + 0x50, height1);


	/* End of VDMA Configuration */

#endif
	xil_printf("VDMA_5 started!\r\n");
}
#endif

#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 7U)
void vdma_config_6(void)
{
	 /* Start of VDMA Configuration */
	u32 bytePerPixels = 3;
	u32 VDMA_BASE_ADDR = XPAR_AXIVDMA_6_BASEADDR;
	int offset0 = 0; // (y*w+x)*Bpp
	int offset1 = 0; // (y*w+x)*Bpp
	u32 stride0 = 0;
	u32 width0 = 0;
	u32 height0 = 0;
	u32 stride1 = 0;  // crop keeps write Stride
	u32 width1 = 0;
	u32 height1 = 0;

#if (defined R1080P60)
	offset0 = 0; // (y*w+x)*Bpp
	offset1 = 0; // (y*w+x)*Bpp
	stride0 = 1920;
	width0 = 1920;
	height0 = 1080;
	stride1 = 1920;  // crop keeps write Stride
	width1 = 1920;
	height1 = 1080;
#elif (defined R4K30W)
	offset0 = 0; // (y*w+x)*Bpp
	offset1 = 0; // (y*w+x)*Bpp
	stride0 = 3840;
	width0 = 3840;
	height0 = 2160;
	stride1 = 3840;  // crop keeps write Stride
	width1 = 3840;
	height1 = 2160;
#else
	offset0 = 0; // (y*w+x)*Bpp
	offset1 = 0; // (y*w+x)*Bpp
	stride0 = 480;
	width0 = 480;
	height0 = 320;
	stride1 = 480;  // crop keeps write Stride
	width1 = 480;
	height1 = 320;
#endif


#if 1
    /* Configure the Write interface (S2MM)*/
    // S2MM Control Register
    Xil_Out32(VDMA_BASE_ADDR + 0x30, 0x8B);
    //S2MM Start Address 1
    Xil_Out32(VDMA_BASE_ADDR + 0xAC, FRAME_BUFFER_19 + offset0);
    //S2MM Start Address 2
    Xil_Out32(VDMA_BASE_ADDR + 0xB0, FRAME_BUFFER_20 + offset0);
    //S2MM Start Address 3
    Xil_Out32(VDMA_BASE_ADDR + 0xB4, FRAME_BUFFER_21 + offset0);
    //S2MM Frame delay / Stride register
    Xil_Out32(VDMA_BASE_ADDR + 0xA8, stride0*bytePerPixels);
    // S2MM HSIZE register
    Xil_Out32(VDMA_BASE_ADDR + 0xA4, width0*bytePerPixels);
    // S2MM VSIZE register
    Xil_Out32(VDMA_BASE_ADDR + 0xA0, height0);

    /* Configure the Read interface (MM2S)*/
    // MM2S Control Register
    Xil_Out32(VDMA_BASE_ADDR + 0x00, 0x8B);
    // MM2S Start Address 1
    Xil_Out32(VDMA_BASE_ADDR + 0x5C, FRAME_BUFFER_19 + offset1);
    // MM2S Start Address 2
    Xil_Out32(VDMA_BASE_ADDR + 0x60, FRAME_BUFFER_20 + offset1);
    // MM2S Start Address 3
    Xil_Out32(VDMA_BASE_ADDR + 0x64, FRAME_BUFFER_21 + offset1);
    // MM2S Frame delay / Stride register
    Xil_Out32(VDMA_BASE_ADDR + 0x58, stride1*bytePerPixels);
    // MM2S HSIZE register
    Xil_Out32(VDMA_BASE_ADDR + 0x54, width1*bytePerPixels);
    // MM2S VSIZE register
    Xil_Out32(VDMA_BASE_ADDR + 0x50, height1);


    /* End of VDMA Configuration */

#else
    /* Configure the Write interface (S2MM)*/
	// S2MM Control Register
	Xil_Out32(VDMA_BASE_ADDR + 0x30, 0x8B);
	//S2MM Start Address 1
	Xil_Out32(VDMA_BASE_ADDR + 0xAC, FRAME_BUFFER_19 + offset0);
	Xil_Out32(VDMA_BASE_ADDR + 0xB0, 0);
	//S2MM Start Address 2
	Xil_Out32(VDMA_BASE_ADDR + 0xB4, FRAME_BUFFER_20 + offset0);
	Xil_Out32(VDMA_BASE_ADDR + 0xB8, 0);
	//S2MM Start Address 3
	Xil_Out32(VDMA_BASE_ADDR + 0xBC, FRAME_BUFFER_21 + offset0);
	Xil_Out32(VDMA_BASE_ADDR + 0xC0, 0);
	//S2MM Frame delay / Stride register
	Xil_Out32(VDMA_BASE_ADDR + 0xA8, stride0*bytePerPixels);
	// S2MM HSIZE register
	Xil_Out32(VDMA_BASE_ADDR + 0xA4, width0*bytePerPixels);
	// S2MM VSIZE register
	Xil_Out32(VDMA_BASE_ADDR + 0xA0, height0);

	/* Configure the Read interface (MM2S)*/
	// MM2S Control Register
	Xil_Out32(VDMA_BASE_ADDR + 0x00, 0x8B);
	// MM2S Start Address 1
	Xil_Out32(VDMA_BASE_ADDR + 0x5C, FRAME_BUFFER_19 + offset1);
	Xil_Out32(VDMA_BASE_ADDR + 0x60, 0);
	// MM2S Start Address 2
	Xil_Out32(VDMA_BASE_ADDR + 0x64, FRAME_BUFFER_20 + offset1);
	Xil_Out32(VDMA_BASE_ADDR + 0x68, 0);
	// MM2S Start Address 3
	Xil_Out32(VDMA_BASE_ADDR + 0x6C, FRAME_BUFFER_21 + offset1);
	Xil_Out32(VDMA_BASE_ADDR + 0x70, 0);
	// MM2S Frame delay / Stride register
	Xil_Out32(VDMA_BASE_ADDR + 0x58, stride1*bytePerPixels);
	// MM2S HSIZE register
	Xil_Out32(VDMA_BASE_ADDR + 0x54, width1*bytePerPixels);
	// MM2S VSIZE register
	Xil_Out32(VDMA_BASE_ADDR + 0x50, height1);


	/* End of VDMA Configuration */

#endif
	xil_printf("VDMA_6 started!\r\n");
}
#endif



#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 8U)
void vdma_config_7(void)
{
	 /* Start of VDMA Configuration */
	u32 bytePerPixels = 3;
	u32 VDMA_BASE_ADDR = XPAR_AXIVDMA_7_BASEADDR;
	int offset0 = 0; // (y*w+x)*Bpp
	int offset1 = 0; // (y*w+x)*Bpp
	u32 stride0 = 0;
	u32 width0 = 0;
	u32 height0 = 0;
	u32 stride1 = 0;  // crop keeps write Stride
	u32 width1 = 0;
	u32 height1 = 0;

#if (defined R1080P60)
	offset0 = 0; // (y*w+x)*Bpp
	offset1 = 0; // (y*w+x)*Bpp
	stride0 = 1920;
	width0 = 1920;
	height0 = 1080;
	stride1 = 1920;  // crop keeps write Stride
	width1 = 1920;
	height1 = 1080;
#elif (defined R4K30W)
	offset0 = 0; // (y*w+x)*Bpp
	offset1 = 0; // (y*w+x)*Bpp
	stride0 = 3840;
	width0 = 3840;
	height0 = 2160;
	stride1 = 3840;  // crop keeps write Stride
	width1 = 3840;
	height1 = 2160;
#else
	offset0 = 0; // (y*w+x)*Bpp
	offset1 = 0; // (y*w+x)*Bpp
	stride0 = 480;
	width0 = 480;
	height0 = 320;
	stride1 = 480;  // crop keeps write Stride
	width1 = 480;
	height1 = 320;
#endif


#if 1
    /* Configure the Write interface (S2MM)*/
    // S2MM Control Register
    Xil_Out32(VDMA_BASE_ADDR + 0x30, 0x8B);
    //S2MM Start Address 1
    Xil_Out32(VDMA_BASE_ADDR + 0xAC, FRAME_BUFFER_22 + offset0);
    //S2MM Start Address 2
    Xil_Out32(VDMA_BASE_ADDR + 0xB0, FRAME_BUFFER_23 + offset0);
    //S2MM Start Address 3
    Xil_Out32(VDMA_BASE_ADDR + 0xB4, FRAME_BUFFER_24 + offset0);
    //S2MM Frame delay / Stride register
    Xil_Out32(VDMA_BASE_ADDR + 0xA8, stride0*bytePerPixels);
    // S2MM HSIZE register
    Xil_Out32(VDMA_BASE_ADDR + 0xA4, width0*bytePerPixels);
    // S2MM VSIZE register
    Xil_Out32(VDMA_BASE_ADDR + 0xA0, height0);

    /* Configure the Read interface (MM2S)*/
    // MM2S Control Register
    Xil_Out32(VDMA_BASE_ADDR + 0x00, 0x8B);
    // MM2S Start Address 1
    Xil_Out32(VDMA_BASE_ADDR + 0x5C, FRAME_BUFFER_22 + offset1);
    // MM2S Start Address 2
    Xil_Out32(VDMA_BASE_ADDR + 0x60, FRAME_BUFFER_23 + offset1);
    // MM2S Start Address 3
    Xil_Out32(VDMA_BASE_ADDR + 0x64, FRAME_BUFFER_24 + offset1);
    // MM2S Frame delay / Stride register
    Xil_Out32(VDMA_BASE_ADDR + 0x58, stride1*bytePerPixels);
    // MM2S HSIZE register
    Xil_Out32(VDMA_BASE_ADDR + 0x54, width1*bytePerPixels);
    // MM2S VSIZE register
    Xil_Out32(VDMA_BASE_ADDR + 0x50, height1);


    /* End of VDMA Configuration */

#else
    /* Configure the Write interface (S2MM)*/
	// S2MM Control Register
	Xil_Out32(VDMA_BASE_ADDR + 0x30, 0x8B);
	//S2MM Start Address 1
	Xil_Out32(VDMA_BASE_ADDR + 0xAC, FRAME_BUFFER_22 + offset0);
	Xil_Out32(VDMA_BASE_ADDR + 0xB0, 0);
	//S2MM Start Address 2
	Xil_Out32(VDMA_BASE_ADDR + 0xB4, FRAME_BUFFER_23 + offset0);
	Xil_Out32(VDMA_BASE_ADDR + 0xB8, 0);
	//S2MM Start Address 3
	Xil_Out32(VDMA_BASE_ADDR + 0xBC, FRAME_BUFFER_24+ offset0);
	Xil_Out32(VDMA_BASE_ADDR + 0xC0, 0);
	//S2MM Frame delay / Stride register
	Xil_Out32(VDMA_BASE_ADDR + 0xA8, stride0*bytePerPixels);
	// S2MM HSIZE register
	Xil_Out32(VDMA_BASE_ADDR + 0xA4, width0*bytePerPixels);
	// S2MM VSIZE register
	Xil_Out32(VDMA_BASE_ADDR + 0xA0, height0);

	/* Configure the Read interface (MM2S)*/
	// MM2S Control Register
	Xil_Out32(VDMA_BASE_ADDR + 0x00, 0x8B);
	// MM2S Start Address 1
	Xil_Out32(VDMA_BASE_ADDR + 0x5C, FRAME_BUFFER_22 + offset1);
	Xil_Out32(VDMA_BASE_ADDR + 0x60, 0);
	// MM2S Start Address 2
	Xil_Out32(VDMA_BASE_ADDR + 0x64, FRAME_BUFFER_23 + offset1);
	Xil_Out32(VDMA_BASE_ADDR + 0x68, 0);
	// MM2S Start Address 3
	Xil_Out32(VDMA_BASE_ADDR + 0x6C, FRAME_BUFFER_24 + offset1);
	Xil_Out32(VDMA_BASE_ADDR + 0x70, 0);
	// MM2S Frame delay / Stride register
	Xil_Out32(VDMA_BASE_ADDR + 0x58, stride1*bytePerPixels);
	// MM2S HSIZE register
	Xil_Out32(VDMA_BASE_ADDR + 0x54, width1*bytePerPixels);
	// MM2S VSIZE register
	Xil_Out32(VDMA_BASE_ADDR + 0x50, height1);


	/* End of VDMA Configuration */

#endif
	xil_printf("VDMA_7 started!\r\n");
}
#endif


void vdma_config(void)
{
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 1U)
//	vdma_config_0();
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 2U)
	vdma_config_1();
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 3U)
	vdma_config_2();
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 4U)
	vdma_config_3();
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 5U)
	vdma_config_4();
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 6U)
	vdma_config_5();
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 7U)
	vdma_config_6();
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 8U)
	vdma_config_7();
#endif
}

#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 1U)
void clear_vdma_0(void)
{
	u32 bytePerPixels = 3;
	u32 VDMA_BASE_ADDR = XPAR_AXIVDMA_0_BASEADDR;
	u32 line = 0;
	u32 column = 0;

#if (defined R1080P60)
	line = 1920;
	column = 1080;
#elif (defined R4K30W)
	line = 3840;
	column = 2160;
#else
	line = 5200;
	column = 1800;
#endif

    Xil_Out32(VDMA_BASE_ADDR + 0x00, 0x8A);//stop mm2s
	Xil_Out32(VDMA_BASE_ADDR + 0x30, 0x8A);//stop s2mm

	Xil_DCacheDisable();
    memset(FRAME_BUFFER_1,0xff,line*column*bytePerPixels);//background
    memset(FRAME_BUFFER_2,0xff,line*column*bytePerPixels);//background
    memset(FRAME_BUFFER_3,0xff,line*column*bytePerPixels);//background

	Xil_DCacheEnable();

	xil_printf("clear vdma_0 Done\n\r");
//	vdma_config_0();
}
#endif

#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 2U)
void clear_vdma_1(void)
{
	u32 bytePerPixels = 3;
	u32 VDMA_BASE_ADDR = XPAR_AXIVDMA_1_BASEADDR;
	u32 line = 0;
	u32 column = 0;

#if (defined R1080P60)
	line = 1920;
	column = 1080;
#elif (defined R4K30W)
	line = 3840;
	column = 2160;
#else
	line = 5200;
	column = 1800;
#endif

    Xil_Out32(VDMA_BASE_ADDR + 0x00, 0x8A);//stop mm2s
	Xil_Out32(VDMA_BASE_ADDR + 0x30, 0x8A);//stop s2mm

	Xil_DCacheDisable();
    memset(FRAME_BUFFER_4,0xff,line*column*bytePerPixels);//background
    memset(FRAME_BUFFER_5,0xff,line*column*bytePerPixels);//background
    memset(FRAME_BUFFER_6,0xff,line*column*bytePerPixels);//background
	Xil_DCacheEnable();

	xil_printf("clear vdma_1 Done\n\r");
//	vdma_config_1();
}
#endif

#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 3U)
void clear_vdma_2(void)
{
	u32 bytePerPixels = 3;
	u32 VDMA_BASE_ADDR = XPAR_AXIVDMA_2_BASEADDR;
	u32 line = 0;
	u32 column = 0;

#if (defined R1080P60)
	line = 1920;
	column = 1080;
#elif (defined R4K30W)
	line = 3840;
	column = 2160;
#else
	line = 5200;
	column = 1800;
#endif

    Xil_Out32(VDMA_BASE_ADDR + 0x00, 0x8A);//stop mm2s
	Xil_Out32(VDMA_BASE_ADDR + 0x30, 0x8A);//stop s2mm

	Xil_DCacheDisable();
    memset(FRAME_BUFFER_7,0xff,line*column*bytePerPixels);//background
    memset(FRAME_BUFFER_8,0xff,line*column*bytePerPixels);//background
    memset(FRAME_BUFFER_9,0xff,line*column*bytePerPixels);//background
	Xil_DCacheEnable();

	xil_printf("clear vdma_2 Done\n\r");
//	vdma_config_2();

}
#endif

#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 4U)
void clear_vdma_3(void)
{
	u32 bytePerPixels = 3;
	u32 VDMA_BASE_ADDR = XPAR_AXIVDMA_3_BASEADDR;
	u32 line = 0;
	u32 column = 0;

#if (defined R1080P60)
	line = 1920;
	column = 1080;
#elif (defined R4K30W)
	line = 3840;
	column = 2160;
#else
	line = 3040;
	column = 1708;
#endif


    Xil_Out32(VDMA_BASE_ADDR + 0x00, 0x8A);//stop mm2s
	Xil_Out32(VDMA_BASE_ADDR + 0x30, 0x8A);//stop s2mm

	Xil_DCacheDisable();

    memset(FRAME_BUFFER_10,0xff,line*column*bytePerPixels);//background
    memset(FRAME_BUFFER_11,0xff,line*column*bytePerPixels);//background
    memset(FRAME_BUFFER_12,0xff,line*column*bytePerPixels);//background

    Xil_DCacheEnable();
    xil_printf("clear vdma_3 Done\n\r");
//	vdma_config_3();
}
#endif

#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 5U)
void clear_vdma_4(void)
{
	u32 bytePerPixels = 3;
	u32 VDMA_BASE_ADDR = XPAR_AXIVDMA_4_BASEADDR;
	u32 line = 0;
	u32 column = 0;

#if (defined R1080P60)
	line = 1920;
	column = 1080;
#elif (defined R4K30W)
	line = 3840;
	column = 2160;
#else
	line = 3040;
	column = 1708;
#endif

    Xil_Out32(VDMA_BASE_ADDR + 0x00, 0x8A);//stop mm2s
	Xil_Out32(VDMA_BASE_ADDR + 0x30, 0x8A);//stop s2mm

	Xil_DCacheDisable();

    memset(FRAME_BUFFER_13,0xff,line*column*bytePerPixels);//background
    memset(FRAME_BUFFER_14,0xff,line*column*bytePerPixels);//background
    memset(FRAME_BUFFER_15,0xff,line*column*bytePerPixels);//background

    Xil_DCacheEnable();
    xil_printf("clear vdma_4 Done\n\r");
//	vdma_config_4();
}

#endif


#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 6U)
void clear_vdma_5(void)
{
	u32 bytePerPixels = 3;
	u32 VDMA_BASE_ADDR = XPAR_AXIVDMA_5_BASEADDR;
	u32 line = 0;
	u32 column = 0;

#if (defined R1080P60)
	line = 1920;
	column = 1080;
#elif (defined R4K30W)
	line = 1920;
	column = 1286;
#else
	line = 1920;
	column = 1280;
#endif

    Xil_Out32(VDMA_BASE_ADDR + 0x00, 0x8A);//stop mm2s
	Xil_Out32(VDMA_BASE_ADDR + 0x30, 0x8A);//stop s2mm

	Xil_DCacheDisable();

    memset(FRAME_BUFFER_16,0xff,line*column*bytePerPixels);//background
    memset(FRAME_BUFFER_17,0xff,line*column*bytePerPixels);//background
    memset(FRAME_BUFFER_18,0xff,line*column*bytePerPixels);//background

    Xil_DCacheEnable();
    xil_printf("clear vdma_5 Done\n\r");
//	vdma_config_5();
}

#endif

#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 7U)
void clear_vdma_6(void)
{
	u32 bytePerPixels = 3;
	u32 VDMA_BASE_ADDR = XPAR_AXIVDMA_6_BASEADDR;
	u32 line = 0;
	u32 column = 0;

#if (defined R1080P60)
	line = 1920;
	column = 1080;
#elif (defined R4K30W)
	line = 1920;
	column = 1080;
#else
	line = 1920;
	column = 1280;
#endif

    Xil_Out32(VDMA_BASE_ADDR + 0x00, 0x8A);//stop mm2s
	Xil_Out32(VDMA_BASE_ADDR + 0x30, 0x8A);//stop s2mm

	Xil_DCacheDisable();

    memset(FRAME_BUFFER_19,0xff,line*column*bytePerPixels);//background
    memset(FRAME_BUFFER_20,0xff,line*column*bytePerPixels);//background
    memset(FRAME_BUFFER_21,0xff,line*column*bytePerPixels);//background

    Xil_DCacheEnable();
    xil_printf("clear vdma_6 Done\n\r");
//	vdma_config_6();
}

#endif

#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 8U)
void clear_vdma_7(void)
{
	u32 bytePerPixels = 3;
	u32 VDMA_BASE_ADDR = XPAR_AXIVDMA_7_BASEADDR;
	u32 line = 0;
	u32 column = 0;

#if (defined R1080P60)
	line = 1920;
	column = 1080;
#elif (defined R4K30W)
	line = 1920;
	column = 1080;
#else
	line = 1920;
	column = 1280;
#endif

    Xil_Out32(VDMA_BASE_ADDR + 0x00, 0x8A);//stop mm2s
	Xil_Out32(VDMA_BASE_ADDR + 0x30, 0x8A);//stop s2mm

	Xil_DCacheDisable();

    memset(FRAME_BUFFER_22,0xff,line*column*bytePerPixels);//background
    memset(FRAME_BUFFER_23,0xff,line*column*bytePerPixels);//background
    memset(FRAME_BUFFER_24,0xff,line*column*bytePerPixels);//background

    Xil_DCacheEnable();
    xil_printf("clear vdma_6 Done\n\r");
//	vdma_config_7();
}

#endif

void clear_display(void)
{
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 1U)
//	clear_vdma_0();
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 2U)
	clear_vdma_1();
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 3U)
	clear_vdma_2();
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 4U)
	clear_vdma_3();
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 5U)
	clear_vdma_4();
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 6U)
	clear_vdma_5();
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 7U)
	clear_vdma_6();
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 8U)
	clear_vdma_7();
#endif

	xil_printf("clear Done\n\r");

}

#endif

#endif
