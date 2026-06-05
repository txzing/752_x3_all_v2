#include "../bsp.h"

#ifdef XPAR_XAXIVDMA_NUM_INSTANCES

#if defined (UDP_VIDEO) || defined (TCP_VIDEO)

#define MAX_ITERATIONS 3 // 每个通道运行的最大次数
volatile u32 iterationCounts = 0; // 每个通道的计数器

static void vdma_lwip_program_s2mm(VdmaChannel *vc, u32 want_w, u32 h);
static void WriteCallBack(void *CallbackRef, u32 Mask);
static void WriteErrorCallBack(void *CallbackRef, u32 Mask);

VdmaChannel VC_inst =
{
	.channel_ID = 1,
	.DeviceID = XPAR_AXI_VDMA_LWIP_DEVICE_ID,
	.S2MM_INTR_ID = XPAR_FABRIC_AXI_VDMA_LWIP_S2MM_INTROUT_INTR,
	.FrameBuffers = {FRAME_BUFFER_1, FRAME_BUFFER_2, FRAME_BUFFER_3},
	.UDP_IMG_PACKEG_SIZE = 1200,
	.VIDEO_FLAG = SEND_VIDEO,
	.send_pic_start = 0,
	.video_sending = 0,
	.send_video_start = 0,
	.send_err_start = {0},
	.Stride = 1920,
	.Width = 1920,
	.Height = 1080
//	.Stride = 3840,
//	.Width = 3840,
//	.Height = 2160
//	.Stride = 1920,
//	.Width = 1920,
//	.Height = 720
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

	if (iterationCounts < MAX_ITERATIONS)
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


/*---------------------------------------------------------------------------
 * LWIP 上传用 VDMA（S2MM）：按「当前视频通道」的实际分辨率配置，避免固定 1080p
 * 把画布外其它通道残留像素一并上传。分辨率来源：passthrough monitor 实时读数，
 * 读失败则用下面缓存（由 vdma_lwip_note_channel_resolution 在链路稳定后更新）。
 * 切换视频源：先 vdma_lwip_request_channel_switch 登记，再由
 * vdma_lwip_try_pending_channel_switch 在「当前帧以太网发完且 DMA 无待发帧」时
 * 调用 board_apply_video_channel_switch + vdma_lwip_apply_channel_geometry。
 * 提示：vdma_lwip_note_channel_resolution 宜在 cable up / retune 成功检测分辨率后调用，
 *       以便 monitor 暂不可读时仍能用各 LVDS 最近已知宽高作回退。
 *---------------------------------------------------------------------------*/
/* LVDS 0..CHANNEL_NUM-1 最近已知宽高；未写入前为 0，几何回退用默认 1920x1080 */
static u32 s_lwip_ch_w[CHANNEL_NUM];
static u32 s_lwip_ch_h[CHANNEL_NUM];
static u8 s_pending_lwip_ch; /* 待切换的视频通道 1..CHANNEL_NUM，0 表示无 */

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

/* 在 display_fresh / retune 等路径检测到分辨率后写入，供 LWIP 读 monitor 失败时回退 */
void vdma_lwip_note_channel_resolution(u8 lvds_idx_based, u32 w, u32 h)
{
	if (lvds_idx_based >= CHANNEL_NUM || w < 10U || h < 10U)
	{
		return;
	}
	s_lwip_ch_w[lvds_idx_based] = w;
	s_lwip_ch_h[lvds_idx_based] = h;
}

/* 读指定 LVDS 通道宽高：monitor 实时值 > s_lwip_ch_* 缓存 > 默认 1920x1080 */
int vdma_lwip_get_channel_dims(u8 lvds_idx_based, u32 *out_w, u32 *out_h)
{
	u32 w = 1920U;
	u32 h = 1080U;
	u32 mon;
	u32 rw;
	u32 rh;

	if (lvds_idx_based >= CHANNEL_NUM || out_w == NULL || out_h == NULL)
	{
		return 0;
	}
#if defined (XPAR_AXI_PASSTHROUGH_MONITOR_NUM_INSTANCES)
	mon = vdma_passthrough_mon_base_lvds(lvds_idx_based);
#else
	mon = 0U;
#endif
	if (mon != 0U && vdma_passthrough_read_mon(mon, &rw, &rh, NULL) != 0)
	{
		w = rw;
		h = rh;
	}
	else if (s_lwip_ch_w[lvds_idx_based] > 0U && s_lwip_ch_h[lvds_idx_based] > 0U)
	{
		w = s_lwip_ch_w[lvds_idx_based];
		h = s_lwip_ch_h[lvds_idx_based];
	}
	*out_w = w;
	*out_h = h;
	return 1;
}

/* 按 eth_video_ch 重配 LWIP S2MM 的宽高与 stride，并刷新 FrameLength / udp_send_times；会停 DMA 再启 */
void vdma_lwip_apply_channel_geometry(u8 eth_video_ch)
{
	VdmaChannel *vc = &VC_inst;
	u8 idx;
	u32 w;
	u32 h;

	if (eth_video_ch < 1U || eth_video_ch > CHANNEL_NUM)
	{
		return;
	}
	idx = (u8)(eth_video_ch - 1U);
	(void)vdma_lwip_get_channel_dims(idx, &w, &h);

	if (vc->Width == w && vc->Height == h && vc->FrameLength == (w * h * 3U))
	{
		return;
	}

	vc->Width = w;
	vc->Height = h;
	vc->Stride = w;

	vdma_lwip_program_s2mm(vc, w, h);
	vc->WrIndex = 0;
	vc->RdIndex = 0;
	vc->WriteOneFrameEnd = 0;
}

/* 请求切到指定视频通道；仅写入 pending，真正切换在 try 里做 */
void vdma_lwip_request_channel_switch(u8 eth_video_ch)
{
	if (eth_video_ch >= 1U && eth_video_ch <= CHANNEL_NUM)
	{
		s_pending_lwip_ch = eth_video_ch;
	}
}

/* 若当前没有在传图/传视频包且无已写完待取的帧，则执行 pending：轴切 + current_ch + S2MM 几何 */
void vdma_lwip_try_pending_channel_switch(void)
{
	u8 p;
	VdmaChannel *v;

	p = s_pending_lwip_ch;
	if (p == 0U)
	{
		return;
	}
	v = &VC_inst;
	/* 整图发送中：等 handle_channel_transfer 发完再切 */
	if (v->send_pic_start != 0U)
	{
		return;
	}
	/* 视频当前帧分包发送中 */
	if (v->video_sending != 0U)
	{
		return;
	}
	/* 已有一帧在缓冲区待上传，避免与读 DDR 竞态 */
	if (v->WriteOneFrameEnd >= 0)
	{
		return;
	}

	s_pending_lwip_ch = 0U;
	board_apply_video_channel_switch(p);
	usleep(10*1000U); /* 给轴开关与视频路径几微秒稳定时间 */
	vdma_lwip_apply_channel_geometry(p);
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


/*
 * vdma_write_start —— 启动 S2MM（写）DMA 传输。
 */
int vdma_write_start(XAxiVdma *Vdma)
{
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

/*
 * vdma_write_stop —— 停止 S2MM 写通道。
 */
int vdma_write_stop(XAxiVdma *Vdma)
{
	XAxiVdma_DmaStop(Vdma, XAXIVDMA_WRITE);
	return XST_SUCCESS;
}

/*
 * vdma_write_init —— 配置已 CfgInitialize 的 Vdma 写通道：帧计数、三帧地址、DmaConfig（不自动 Start）。
 * 供 MultiVdmaInit 等调用；DeviceID 目前未再 Lookup，保留形参与历史接口一致。
 */
int vdma_write_init(short DeviceID,XAxiVdma *Vdma,short HoriSizeInput,short VertSizeInput,short Stride,
		unsigned int FrameStoreStartAddr0,unsigned int FrameStoreStartAddr1,unsigned int FrameStoreStartAddr2)
{
	XAxiVdma_FrameCounter FrameCfg;
	XAxiVdma_DmaSetup WriteCfg;
	int Status;

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
				"Write channel config failed %d\r\n", Status);

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

	return XST_SUCCESS;
}


/*===========================================================================
 * 本文件「直接写寄存器」配置 VDMA 的入口与数据流
 *---------------------------------------------------------------------------
 *  1) 对外入口：vdma_config() / vdma_config_instance(id) / vdma_lvds_path_op()。
 *  2) 写寄存器集中在 vdma_program_triple_buffer_geom()。
 *  3) S2MM=写 DDR，MM2S=读 DDR。运行期几何：vdma_get/set_triple_geom_apply。
 *  4) 清屏见 vdma_clear_fb_triple / clear_vdma_instance。
 *===========================================================================*/
/*
 * vivado中配置32位宽地址vdma
 */
#define VDMA_POKE_FB_ADDR_32BIT 1

/*
 * vdma_timing_slot —— 编译期选择 kVdmaRegGeometry[][列] 的列下标：
 *   R1080 → VDMA_TIMING_SLOT_R1080；R4K → VDMA_TIMING_SLOT_R4K；否则 FALLBACK。
 * @return 0、1 或 2，与 VdmaTimingSlot 枚举一致。
 */
static u8 vdma_timing_slot(void)
{
#if defined (R1080)
	return VDMA_TIMING_SLOT_R1080;
#elif defined (R4K)
	return VDMA_TIMING_SLOT_R4K;
#else
	return VDMA_TIMING_SLOT_FALLBACK;
#endif
}


/*
 * 用户自定义：行数必须与 XPAR_XAXIVDMA_NUM_INSTANCES 一致（与 vdma_init_table 展开方式相同）。
 * 每行对应实例 0、1、…；可按需在各行替换不同的 VDMA_ROW* 组合。
 */
static const VdmaTripleGeom kVdmaRegGeometry[XPAR_XAXIVDMA_NUM_INSTANCES][3] =
{
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 1U)
	{ VDMA_ROW_1080, VDMA_ROW_4K, VDMA_ROW_8K },
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 2U)
	{ VDMA_ROW_1080, VDMA_ROW_4K, VDMA_ROW_8K },
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 3U)
	{ VDMA_ROW_1080, VDMA_ROW_4K, VDMA_ROW_8K },
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 4U)
	{ VDMA_ROW_1080, VDMA_ROW_4K, VDMA_ROW_8K },
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 5U)
	{ VDMA_ROW_1080, VDMA_ROW_4K, VDMA_ROW_8K },
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 6U)
	{ VDMA_ROW_1080, VDMA_ROW_4K, VDMA_ROW_8K },
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 7U)
	{ VDMA_ROW_1080, VDMA_ROW_4K, VDMA_ROW_8K },
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 8U)
	{ VDMA_ROW_1080, VDMA_ROW_4K, VDMA_ROW_8K },
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 9U)
	{ VDMA_ROW_1080, VDMA_ROW_4K, VDMA_ROW_8K },
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 10U)
	{ VDMA_ROW_1080, VDMA_ROW_4K, VDMA_ROW_8K },
#endif
};

/*
 * 寄存器路径下每路 VDMA 的「当前几何」镜像：
 * - 首次 vdma_config_instance(id) 从 kVdmaRegGeometry[id][vdma_timing_slot()] 拷贝进来并置 inited；
 * - vdma_set_triple_geom_apply 会更新本镜像并写硬件；
 * - vdma_get_triple_geom：已 inited 则读镜像，否则读编译表（与即将首次装入的值一致）。
 */
static VdmaTripleGeom s_vdma_runtime_geom[XPAR_XAXIVDMA_NUM_INSTANCES];
static u8 s_vdma_runtime_geom_inited[XPAR_XAXIVDMA_NUM_INSTANCES];

/*
 * vdma_program_triple_buffer_geom —— 对指定 AXI VDMA 基址做「三缓冲」寄存器直写（不经 Xilinx 驱动）：
 *   使用调用方提供的 VdmaTripleGeom → 配 S2MM（从 0x30）→ 配 MM2S（从 0x00）；RGB 按 bytePerPixels。
 * @param VdmaBase     该 IP 的 AXI 基址（如 XPAR_AXIVDMA_n_BASEADDR）。
 * @param fb0,fb1,fb2  三帧 DDR 起始地址（u32；与 off_s2mm/off_mm2s 相加后写入寄存器）。
 * @param resolution   几何（不得为 NULL）。
 * @param inst_index   第几路 VDMA（仅用于打印）。
 * 帧地址写法由 VDMA_POKE_FB_ADDR_32BIT 控制（见文件头「帧缓冲起始地址寄存器映射」）。
 */
static void vdma_program_triple_buffer_geom(u32 VdmaBase, u32 fb0, u32 fb1, u32 fb2,
					    const VdmaTripleGeom *resolution,
					    u8 inst_index)
{
	/* 步骤二：S2MM — 控制寄存器 + 三帧起始地址 + stride / HSIZE / VSIZE */
	Xil_Out32(VdmaBase + 0x30U, 0x8BU);
#if VDMA_POKE_FB_ADDR_32BIT
	Xil_Out32(VdmaBase + 0xACU, (u32)(fb0 + resolution->off_s2mm));
	Xil_Out32(VdmaBase + 0xB0U, (u32)(fb1 + resolution->off_s2mm));
	Xil_Out32(VdmaBase + 0xB4U, (u32)(fb2 + resolution->off_s2mm));
#else
	Xil_Out32(VdmaBase + 0xACU, (u32)(fb0 + resolution->off_s2mm));
	Xil_Out32(VdmaBase + 0xB0U, 0);
	Xil_Out32(VdmaBase + 0xB4U, (u32)(fb1 + resolution->off_s2mm));
	Xil_Out32(VdmaBase + 0xB8U, 0);
	Xil_Out32(VdmaBase + 0xBCU, (u32)(fb2 + resolution->off_s2mm));
	Xil_Out32(VdmaBase + 0xC0U, 0);
#endif
	Xil_Out32(VdmaBase + 0xA8U, resolution->s_stride * resolution->bytePerPixels);
	Xil_Out32(VdmaBase + 0xA4U, resolution->s_width * resolution->bytePerPixels);
	Xil_Out32(VdmaBase + 0xA0U, resolution->s_height);

	/* 步骤三：MM2S — 控制 + 三帧起始地址 + stride / HSIZE / VSIZE */
	Xil_Out32(VdmaBase + 0x00U, 0x8BU);
#if VDMA_POKE_FB_ADDR_32BIT
	Xil_Out32(VdmaBase + 0x5CU, (u32)(fb0 + resolution->off_mm2s));
	Xil_Out32(VdmaBase + 0x60U, (u32)(fb1 + resolution->off_mm2s));
	Xil_Out32(VdmaBase + 0x64U, (u32)(fb2 + resolution->off_mm2s));
#else
	Xil_Out32(VdmaBase + 0x5CU, (u32)(fb0 + resolution->off_mm2s));
	Xil_Out32(VdmaBase + 0x60U, 0);
	Xil_Out32(VdmaBase + 0x64U, (u32)(fb1 + resolution->off_mm2s));
	Xil_Out32(VdmaBase + 0x68U, 0);
	Xil_Out32(VdmaBase + 0x6CU, (u32)(fb2 + resolution->off_mm2s));
	Xil_Out32(VdmaBase + 0x70U, 0);
#endif
	Xil_Out32(VdmaBase + 0x58U, resolution->m_stride * resolution->bytePerPixels);
	Xil_Out32(VdmaBase + 0x54U, resolution->m_width * resolution->bytePerPixels);
	Xil_Out32(VdmaBase + 0x50U, resolution->m_height);

	xil_printf("VDMA_%u started (register path).\r\n", (u8)inst_index);
}

/*
 * vdma_clear_fb_triple —— 停 DMA 后对三帧缓冲做 memset 清屏；宽高由编译宏 R1080 / R4K 等决定，
 * 与 vdma_get_triple_geom / vdma_set_triple_geom_apply 的运行时几何无关。
 */
static void vdma_clear_fb_triple(u32 VdmaBase, u32 fb0, u32 fb1, u32 fb2,
				  u8 inst_index)
{
	u32 line = 0;
	u32 column = 0;
	u32 bytePerPixels = 3;
	u32 nbytes;

	if (inst_index >= XPAR_XAXIVDMA_NUM_INSTANCES)
	{
		return;
	}

#if (defined R1080)
	line = 1920;
	column = 1080;
#elif (defined R4K)
	line = 3840;
	column = 2160;
#else
	line = 3840;
	column = 2160;
#endif

	nbytes = line * column * bytePerPixels;

	/* 先停 DMA，再关 DCache 后 memset，避免与 cache 不一致 */
	Xil_Out32(VdmaBase + 0x00U, 0x8AU);
	Xil_Out32(VdmaBase + 0x30U, 0x8AU);

	Xil_DCacheDisable();
	memset(fb0, 0xff, nbytes);
	memset(fb1, 0xff, nbytes);
	memset(fb2, 0xff, nbytes);
	Xil_DCacheEnable();

	xil_printf("clear vdma_%u Done\n\r", inst_index);
}


/*
 * vdma_config_instance —— 按 vdma_init_table[id] 写该路 VDMA 寄存器。
 * 若该路尚未建立运行时几何（首次调用）：从 kVdmaRegGeometry[id][vdma_timing_slot()] 装入 shadow；
 * 若已建立（例如已调用过 vdma_set_triple_geom_apply）：保留 shadow 内容，仅重新写寄存器。
 */
void vdma_config_instance(u8 id)
{
	const Vdma_Init_Table *table;

	if (id >= XPAR_XAXIVDMA_NUM_INSTANCES)
	{
		return;
	}
	table = &vdma_init_table[id];
	if (s_vdma_runtime_geom_inited[id] == 0U)
	{
		u8 tm = vdma_timing_slot();

		s_vdma_runtime_geom[id] = kVdmaRegGeometry[id][tm];
		s_vdma_runtime_geom_inited[id] = 1U;
	}
	vdma_program_triple_buffer_geom(table->baseAddr,
					table->frameBuffers[0], table->frameBuffers[1],
					table->frameBuffers[2], &s_vdma_runtime_geom[id],
					table->id);
}

/*
 * vdma_get_triple_geom —— 读出指定实例当前使用的 VdmaTripleGeom，便于在本地修改后再写回。
 * @param id   与 vdma_init_table / vdma_config_instance 一致，0 .. NUM_INSTANCES-1。
 * @param out  输出缓冲区，非 NULL。
 * @return     XST_SUCCESS / XST_FAILURE（id 越界或 out 为 NULL）。
 * 说明：若该路尚未执行过 vdma_config_instance（inited==0），返回的是编译表 kVdmaRegGeometry
 *       在 vdma_timing_slot() 列上的默认值，与首次 vdma_config_instance 将装入 shadow 的内容相同。
 */
int vdma_get_triple_geom(u8 id, VdmaTripleGeom *out)
{
	if (id >= XPAR_XAXIVDMA_NUM_INSTANCES || out == NULL)
	{
		return XST_FAILURE;
	}
	if (s_vdma_runtime_geom_inited[id] != 0U)
	{
		*out = s_vdma_runtime_geom[id];
	}
	else
	{
		u8 tm = vdma_timing_slot();

		*out = kVdmaRegGeometry[id][tm];
	}
	return XST_SUCCESS;
}

/*
 * vdma_set_triple_geom_apply —— 将 geom 写入该路的运行时 shadow，并立即按 vdma_program_triple_buffer_geom
 * 重配 S2MM/MM2S 寄存器（三帧地址仍来自 vdma_init_table）。
 * @param id    实例下标，与 vdma_config_instance 相同。
 * @param geom  完整几何；字段含义见 VdmaTripleGeom（bytePerPixels、off_*、stride、width、height 等）。
 * @return      XST_SUCCESS / XST_FAILURE（id 越界或 geom 为 NULL）。
 * 典型用法：vdma_get_triple_geom → 修改局部副本 → 本函数写回并生效。改分辨率时需保证与 PL 时序、
 *           DDR 帧缓冲布局及 FRAME_BUFFER_SIZE0 一致，否则可能花屏或越界。
 */
int vdma_set_triple_geom_apply(u8 id, const VdmaTripleGeom *geom)
{
	const Vdma_Init_Table *table;

	if (id >= XPAR_XAXIVDMA_NUM_INSTANCES || geom == NULL)
	{
		return XST_FAILURE;
	}
	table = &vdma_init_table[id];
	s_vdma_runtime_geom[id] = *geom;
	s_vdma_runtime_geom_inited[id] = 1U;
	vdma_program_triple_buffer_geom(table->baseAddr,
					table->frameBuffers[0], table->frameBuffers[1],
					table->frameBuffers[2], &s_vdma_runtime_geom[id],
					table->id);
	return XST_SUCCESS;
}

int vdma_apply_detected_rgb_geom(u8 first_vdma_id, u8 num_vdma, u32 mon_base)
{
	u32 w;
	u32 h;
	u32 fps_cnt;
	u32 w1;
	u32 h1;
	VdmaTripleGeom g;

	if (mon_base == 0U || num_vdma == 0U)
	{
		return 0;
	}

	usleep(100*1000);
	if (vdma_passthrough_read_mon(mon_base, &w, &h, &fps_cnt) == 0)
	{
		return 0;
	}
	if (fps_cnt < 10U)
	{
		xil_printf(
			"WARN passthrough mon@0x%08x: fps=%u < 10 (abnormal), geom raw %ux%u\r\n",
			(unsigned)mon_base, (unsigned)fps_cnt, (unsigned)w, (unsigned)h);
		return 0;
	}
	w1 = w;
	h1 = h;

#if (defined (UDP_VIDEO) || defined (TCP_VIDEO)) && defined (XPAR_AXI_PASSTHROUGH_MONITOR_NUM_INSTANCES)
	for (u8 lvds = 0U; lvds < CHANNEL_NUM; lvds++)
	{
		u32 mb = vdma_passthrough_mon_base_lvds(lvds);
		if (mb != 0U && mb == mon_base)// 只有「当前 lvds 的 monitor 基址 == 传入的 mon_base」才进分支
		{
			vdma_lwip_note_channel_resolution(lvds, w, h);
			/*
			 * Boot 时 main 里 vdma_lwip_apply_channel_geometry 往往早于 SerDes/视频稳定，
			 * monitor 可能给出错误 beat（×2 后成 3840 宽）或读失败用默认 1080p。
			 * 此处已读到稳定几何且与以太网当前预览 LVDS 一致时，重配 LWIP S2MM，
			 * 与网络切换通道后走同一套几何逻辑，避免上位机一直错分辨至手动切源。
			 */
			if (current_ch >= 1U && current_ch <= CHANNEL_NUM
			    && (u8)(current_ch - 1U) == lvds)
			{
				vdma_lwip_apply_channel_geometry(current_ch);
			}
			break;  // 已经找到「是哪一个 LVDS」对应这次读的 mon，后面 lvds 不用再扫了
		}
		 // 不匹配：不进 if，自然进入下一轮 for，相当于 continue
	}
#endif

	for (u8 i = 0U; i < num_vdma; i++)
	{
		u8 id = (u8)(first_vdma_id + i);
		if (id >= XPAR_XAXIVDMA_NUM_INSTANCES)
		{
			break;
		}
		if (vdma_get_triple_geom(id, &g) != XST_SUCCESS)
		{
			continue;
		}

		if ((g.m_width == w) && (g.m_height == h))
		{
			continue;
		}
		g.m_stride = g.s_stride;
		g.m_width = w;
		g.m_height = h;
		if (vdma_set_triple_geom_apply(id, &g) == XST_SUCCESS)
		{
			xil_printf(
				"VDMA%u MM2S: %ux%u stride(px)=%u (S2MM %ux%u stride %u) mon raw %ux%u (RGB888 w x2)\r\n",
				(unsigned)id,
				(unsigned)g.m_width, (unsigned)g.m_height, (unsigned)g.m_stride,
				(unsigned)g.s_width, (unsigned)g.s_height, (unsigned)g.s_stride,
				(unsigned)w1, (unsigned)h1);
		}
	}
	return 1;
}


void clear_vdma_instance(u8 id)
{
	const Vdma_Init_Table *table;

	if (id >= XPAR_XAXIVDMA_NUM_INSTANCES)
	{
		return;
	}
	table = &vdma_init_table[id];
	vdma_clear_fb_triple(table->baseAddr,
			     table->frameBuffers[0], table->frameBuffers[1],
			     table->frameBuffers[2], table->id);
}

/* lvds: 0-based；with_config=1 清屏+默认几何，0 仅清屏。GPIO 掩码与 cable-up/down 一致 */
void vdma_lvds_path_op(u8 lvds, u8 with_config)
{
	u8 v0;

	if (lvds >= CHANNEL_NUM)
	{
		return;
	}
	v0 = (u8)((2U * lvds) + 1U);
	if ((u8)XPAR_XAXIVDMA_NUM_INSTANCES < (u8)(v0 + 2U))
	{
		return;
	}
	XGpio_DiscreteWrite(&XGpioOutput_oldi, 1, with_config ? (8U << lvds) : ((8U << lvds) | 0x7));
	clear_vdma_instance(v0);
	clear_vdma_instance((u8)(v0 + 1U));
	if (with_config != 0U)
	{
		vdma_config_instance(v0);
		vdma_config_instance((u8)(v0 + 1U));
	}
	XGpio_DiscreteWrite(&XGpioOutput_oldi, 1, 0U);
}

/*
 * vdma_config —— 按表依次配置。
 * UDP/TCP 视频：须同时满足才做寄存器三帧配置（vdma_program_triple_buffer_geom）：
 *   - 非表项 0（与原逻辑一致：AXIVDMA_0 留给 vdma_udp_init / 网口写通道）；
 *   - 且本行 XPAR_AXIVDMA_i_DEVICE_ID 不等于 XPAR_AXI_VDMA_LWIP_DEVICE_ID。
 * 仅按 Device ID 判断在部分 BSP 上会错：LWIP 实际挂在 AXIVDMA_0，但
 * XPAR_AXI_VDMA_LWIP_DEVICE_ID 与 XPAR_AXIVDMA_0_DEVICE_ID 数值可能不一致，
 * 若只比 Device ID 会在 i==0 时误配置，先于 vdma_udp_init 破坏寄存器导致花屏。
 * 未定义 UDP/TCP 视频：配置全部实例。
 */
void vdma_config(void)
{

	for (u8 i = 0U; i < XPAR_XAXIVDMA_NUM_INSTANCES; i++)
	{
#if defined (UDP_VIDEO) || defined (TCP_VIDEO)
		if (vdma_init_table[i].id != XPAR_AXI_VDMA_LWIP_DEVICE_ID)
		{
			usleep(10*1000);
			vdma_config_instance(i);
		}
#else
		vdma_config_instance(i);
#endif
	}
	xil_printf("vdma config Done\n\r");
}

/*
 * clear_display —— 按表对各路三帧 memset 清屏（见 vdma_clear_fb_triple）。
 * UDP/TCP 视频：与 vdma_config 相同条件再清屏（跳过表项 0 与 LWIP DeviceID 命中行）。
 * 非网络视频：清全部实例。
 */
void clear_display(void)
{

	for (u8 i = 0U; i < XPAR_XAXIVDMA_NUM_INSTANCES; i++)
	{
#if defined (UDP_VIDEO) || defined (TCP_VIDEO)
		if (vdma_init_table[i].id != XPAR_AXI_VDMA_LWIP_DEVICE_ID)
		{
			usleep(10*1000);
			clear_vdma_instance(i);
		}
#else
		clear_vdma_instance(i);
#endif
	}

	xil_printf("vdma clear Done\n\r");
}

#endif /* XPAR_XAXIVDMA_NUM_INSTANCES */
