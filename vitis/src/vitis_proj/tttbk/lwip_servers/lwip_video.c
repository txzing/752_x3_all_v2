#include "../bsp.h"

#if defined (UDP_VIDEO) || defined (TCP_VIDEO)

packet_header packet_p;
#define UDP_VIDEO_BURST_PACKETS 4U
#define UDP_ERR_BURST_PACKETS   2U

/* 视频流每发 N 个分包短暂让出 CPU，便于 LwIP/驱动消化 TX 队列，减轻 PBUF_POOL 突发耗尽 */
#ifndef UDP_VIDEO_PACE_EVERY_N
#define UDP_VIDEO_PACE_EVERY_N  32U
#endif
#ifndef UDP_VIDEO_PACE_US
#define UDP_VIDEO_PACE_US       20U
#endif

static void fill_packet_common(VdmaChannel *channel, int channel_id, int send_flag, int picseq)
{
	packet_p.HEAD_ID    = HEADER_ID;
	packet_p.Channel_ID = channel_id;
	packet_p.SEND_FLAG  = send_flag;
	packet_p.Width      = channel->Width;
	packet_p.Height     = channel->Height;
	packet_p.total      = channel->FrameLength;
	packet_p.offset     = (channel->pkg_cnt - 1) * channel->UDP_IMG_PACKEG_SIZE;
	packet_p.picseq     = picseq;
	packet_p.frameseq   = channel->pkg_cnt;
	packet_p.framesize  = channel->UDP_IMG_PACKEG_SIZE;
}

/**
 * @brief 主传输逻辑
 */
void lwip_video_transfer(void)
{
	MonitorAndExitAfterIterations();
	/* 主循环入口：若此前 UDP 请求了切通道且当前无传输，则在此或帧发完后应用 */
	vdma_lwip_try_pending_channel_switch();
    for (int i = 0; i < ETH_VIDEO_NUM; i++) // 遍历所有通道
    {

        // 检查当前通道是否需要传输
        if (VdmaChannels[i].WriteOneFrameEnd >= 0)
        {
//        	if (((VdmaChannels[i].VIDEO_FLAG == SEND_PIC && VdmaChannels[i].send_pic_start == 1) ||
        	if (((VdmaChannels[i].send_pic_start == 1) ||
        	     (VdmaChannels[i].send_video_start == 1) ||
				 (VdmaChannels[i].video_sending == 1)))
            {
                // 调用通用传输逻辑处理
                if (handle_channel_transfer(i))
                {
                    continue; // 当前通道传输失败，跳过后续处理
                }
            }

        }
    }

#if defined (XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES)

	if (VdmaChannels[0].send_err_start[0] == 1)
	{
		if (handle_err_pic_transfer(0))
		{
			return;
		}
	}
#if (XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES >= 2U)
	else if (VdmaChannels[0].send_err_start[1] == 1)
	{
		if (handle_err_pic_transfer(1))
		{
			return;
		}
	}
#endif
#if (XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES >= 3U)
	else if (VdmaChannels[0].send_err_start[2] == 1)
	{
		if (handle_err_pic_transfer(2))
		{
			return;
		}
	}
#endif
#endif
}


/**
 *
 *
 * @brief 通用数据发送逻辑，处理图片或视频发送
 * @param channel 当前通道索引
 * @return 0 继续或本帧发完；-1 本帧发送失败（调用方可据此提前返回）
 */
#if defined (XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES)
int handle_err_pic_transfer(int channel)
{
	VdmaChannel *currentChannel = &VdmaChannels[0];

	int sta;
	u8 burst = 0U;

	// 单次调度发送多包，降低调度/函数调用开销
	while ((currentChannel->pkg_cnt <= currentChannel->udp_send_times) &&
	       (burst < UDP_ERR_BURST_PACKETS))
	{
		// 构造数据包头部
		fill_packet_common(currentChannel, channel + 1, 2, 1);

		// 刷新缓存
		Xil_DCacheInvalidateRange(
			(u32)(currentChannel->Frame_Err_Buffers[channel] + packet_p.offset),
			currentChannel->UDP_IMG_PACKEG_SIZE);


#if (defined UDP_VIDEO)
	// 发送数据包
		sta = sendpic_udp((u8 *)(currentChannel->Frame_Err_Buffers[channel] + packet_p.offset),
				  currentChannel->UDP_IMG_PACKEG_SIZE);
#elif (defined TCP_VIDEO)
	// 发送数据包
		sta = sendpic_tcp((u8 *)(currentChannel->Frame_Err_Buffers[channel] + packet_p.offset),
				  currentChannel->UDP_IMG_PACKEG_SIZE);
#endif

		// 检查发送状态
		if (sta != 0)
		{
#if defined (UDP_VIDEO)
			if (sta == UDP_SEND_TRY_LATER)
			{
				/* pbuf pool busy: retry on next scheduler tick */
				return 0;
			}
#endif
			DEBUG_PRINTF("Channel %d: sendpic error pkg_cnt:%d\r\n", packet_p.Channel_ID, currentChannel->pkg_cnt);
			currentChannel->pkg_cnt = 1;
			currentChannel->send_err_start[channel] = 0;
			return -1;
		}
        /* ========= 新增：每 100个 packet 延时  ========= */
        if ((currentChannel->pkg_cnt % 100) == 0)
        {
        	usleep(200);
        }
        /* ==================================================== */
		// 成功发送，增加计数
		currentChannel->pkg_cnt++;
		burst++;
	}
    if (currentChannel->pkg_cnt > currentChannel->udp_send_times) // 当前帧发送完成
    {
		xil_printf("Channel %d: Picture transmission complete. pkg_cnt:%d\r\n", packet_p.Channel_ID, currentChannel->pkg_cnt - 1);
		currentChannel->send_err_start[channel] = 0; // 停止图片发送
		currentChannel->pkg_cnt = 1;

		pixel_err[channel] = 0;
		pixel_err_cnt[channel] = 0U;
		return 0; // 传输完成
    }

    return 0;
}
#endif

/**
 * @brief 通用数据发送逻辑，处理图片或视频发送
 * @param channel 当前通道索引
 * @return 0 本帧未发完或已正常发完；-1 不可恢复发送错误（非 UDP_SEND_TRY_LATER）
 */
int handle_channel_transfer(int channel)
{
    VdmaChannel *currentChannel = &VdmaChannels[channel];

    int sta;
    u8 burst = 0U;

    // 单次调度发送多包，提升 UDP 视频吞吐
    while ((currentChannel->pkg_cnt <= currentChannel->udp_send_times) &&
    	   (burst < UDP_VIDEO_BURST_PACKETS))
    {
        // 构造数据包头部
        fill_packet_common(currentChannel,
        		           current_ch,
        		           (currentChannel->send_pic_start == 1) ? 0 : 1,
        		           channel);
        // 刷新缓存
        Xil_DCacheInvalidateRange(
            (u32)(currentChannel->FrameBuffers[currentChannel->WriteOneFrameEnd] + packet_p.offset),
            currentChannel->UDP_IMG_PACKEG_SIZE);



#if (defined UDP_VIDEO)
	// 发送数据包
        sta = sendpic_udp((u8 *)(currentChannel->FrameBuffers[currentChannel->WriteOneFrameEnd] + packet_p.offset),
				  currentChannel->UDP_IMG_PACKEG_SIZE);
#elif (defined TCP_VIDEO)
	// 发送数据包
        sta = sendpic_tcp((u8 *)(currentChannel->FrameBuffers[currentChannel->WriteOneFrameEnd] + packet_p.offset),
				  currentChannel->UDP_IMG_PACKEG_SIZE);
#endif

        // 检查发送状态
        if (sta != 0)
        {
#if defined (UDP_VIDEO)
        	if (sta == UDP_SEND_TRY_LATER)
        	{
        		/* pbuf pool busy: keep frame state and retry later */
        		return 0;
        	}
#endif
        	DEBUG_PRINTF("Channel %d: sendpic error pkg_cnt:%d\r\n", packet_p.Channel_ID, currentChannel->pkg_cnt);
            currentChannel->WriteOneFrameEnd = -1;
            currentChannel->pkg_cnt = 1;
        	/* 必须清除 video_sending，否则 vdma_lwip_try_pending_channel_switch 会一直阻塞 */
        	currentChannel->video_sending = 0;
        	if (currentChannel->VIDEO_FLAG == SEND_PIC)
        	{
        		currentChannel->send_pic_start = 0;
        	}
        	else if (currentChannel->VIDEO_FLAG == SEND_VIDEO)
        	{
        		currentChannel->send_video_start = 0;
        	}
        	return -1;
        }
        /* ========= 每 100 个 packet 延时（单张图） ========= */
        if (currentChannel->send_pic_start == 1)
        {
			if ((currentChannel->pkg_cnt % 100) == 0)
			{
				usleep(100);
			}
        }
        else
        {
        	currentChannel->video_sending = 1;
        	if (UDP_VIDEO_PACE_EVERY_N > 0U &&
        	    ((u32)currentChannel->pkg_cnt % UDP_VIDEO_PACE_EVERY_N) == 0U)
        	{
        		usleep(UDP_VIDEO_PACE_US);
        	}
        }
        /* ==================================================== */
        // 成功发送，增加计数
        currentChannel->pkg_cnt++;
        burst++;

    }
    if (currentChannel->pkg_cnt > currentChannel->udp_send_times) // 当前帧发送完成
    {
        if (currentChannel->VIDEO_FLAG == SEND_PIC) // 图片传输完成
        {
            xil_printf("Channel %d: Picture transmission complete. pkg_cnt:%d\r\n", packet_p.Channel_ID, currentChannel->pkg_cnt - 1);
            currentChannel->send_pic_start = 0; // 停止图片发送
            currentChannel->video_sending = 0;
            currentChannel->WriteOneFrameEnd = -1;
            currentChannel->pkg_cnt = 1;
            /* 本帧已全部发出，可安全应用待切换通道与 LWIP VDMA 新分辨率 */
            vdma_lwip_try_pending_channel_switch();
            return 0; // 传输完成
        }
        else if (currentChannel->VIDEO_FLAG == SEND_VIDEO) // 视频帧传输完成
        {
//            xil_printf("Channel %d: Video frame complete. pkg_cnt:%d\r\n", packet_p.Channel_ID, currentChannel->pkg_cnt - 1);
        	currentChannel->send_pic_start = 0;
        	currentChannel->video_sending = 0;
            currentChannel->WriteOneFrameEnd = -1;
            currentChannel->pkg_cnt = 1; // 重置计数
            /* 视频帧分包发完，同样在此间隙尝试应用待切换 */
            vdma_lwip_try_pending_channel_switch();
            return 0; // 传输完成
        }
    }

    return 0; // 继续传输
}

#endif //#if defined (UDP_VIDEO)
