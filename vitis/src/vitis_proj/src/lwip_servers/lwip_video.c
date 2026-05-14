#include "../bsp.h"

#if defined (UDP_VIDEO) || defined (TCP_VIDEO)

packet_header packet_p;
/* 默认在可靠性与带宽间折中：BURST=2、适度节流；极限可靠可设 BURST=1，冲带宽可设 3～4 并略增大 PACE_EVERY_N */
#ifndef UDP_VIDEO_BURST_PACKETS
#define UDP_VIDEO_BURST_PACKETS 2U
#endif
#ifndef UDP_ERR_BURST_PACKETS
#define UDP_ERR_BURST_PACKETS   2U
#endif

#ifndef UDP_VIDEO_SEND_RETRY_MAX
#define UDP_VIDEO_SEND_RETRY_MAX      64U
#endif
#ifndef UDP_VIDEO_SEND_RETRY_DELAY_US
#define UDP_VIDEO_SEND_RETRY_DELAY_US 25U
#endif
#ifndef TCP_VIDEO_SEND_RETRY_MAX
#define TCP_VIDEO_SEND_RETRY_MAX      48U
#endif
#ifndef TCP_VIDEO_SEND_RETRY_DELAY_US
#define TCP_VIDEO_SEND_RETRY_DELAY_US 40U
#endif

/* 单张图：节流略放宽于「每 16 包」，减少 usleep 总时间，仍抑制 PBUF/TX 突发 */
#ifndef UDP_PIC_PACE_EVERY_N
#define UDP_PIC_PACE_EVERY_N  32U
#endif
#ifndef UDP_PIC_PACE_US
#define UDP_PIC_PACE_US       40U
#endif
#ifndef UDP_VIDEO_PACE_EVERY_N
#define UDP_VIDEO_PACE_EVERY_N  24U
#endif
#ifndef UDP_VIDEO_PACE_US
#define UDP_VIDEO_PACE_US       28U
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
 * 单包发送：对 pbuf 不足(UDP_SEND_TRY_LATER)与 udp_send 失败做短退避重试，尽量不丢序号、不把失败当成功。
 * 仍失败则返回 TRY_LATER / TCP 侧 -4，由上层下周期再发同一 pkg_cnt。
 */
static int video_link_send(uint8_t *payload, uint16_t len)
{
#if defined(UDP_VIDEO)
	unsigned n;
	int sta;

	for (n = 0U; n < UDP_VIDEO_SEND_RETRY_MAX; n++)
	{
		sta = sendpic_udp(payload, len);
		if (sta == 0)
		{
			return 0;
		}
		if (sta == -1)
		{
			return -1;
		}
		if (sta == UDP_SEND_TRY_LATER || sta == -2)
		{
			usleep(UDP_VIDEO_SEND_RETRY_DELAY_US);
			continue;
		}
		return sta;
	}
	return UDP_SEND_TRY_LATER;
#elif defined(TCP_VIDEO)
	unsigned n;
	int sta;

	for (n = 0U; n < TCP_VIDEO_SEND_RETRY_MAX; n++)
	{
		sta = sendpic_tcp(payload, len);
		if (sta == 0)
		{
			return 0;
		}
		if (sta == -1)
		{
			return -1;
		}
		/* sendpic_tcp 已对 tcp_write/tcp_output 的 ERR_MEM 做内部重试；此处仅对「窗口满未写」退避 */
		if (sta == TCP_SEND_TRY_LATER)
		{
			usleep(TCP_VIDEO_SEND_RETRY_DELAY_US);
			continue;
		}
		return sta;
	}
	return TCP_SEND_TRY_LATER;
#else
	(void)payload;
	(void)len;
	return -1;
#endif
}

static int video_send_needs_reschedule(int sta)
{
#if defined(UDP_VIDEO)
	if (sta == UDP_SEND_TRY_LATER)
	{
		return 1;
	}
#endif
#if defined(TCP_VIDEO)
	if (sta == TCP_SEND_TRY_LATER)
	{
		return 1;
	}
#endif
	(void)sta;
	return 0;
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


		sta = video_link_send(
			(u8 *)(currentChannel->Frame_Err_Buffers[channel] + packet_p.offset),
			currentChannel->UDP_IMG_PACKEG_SIZE);

		// 检查发送状态
		if (sta != 0)
		{
			if (video_send_needs_reschedule(sta))
			{
				return 0;
			}
			/* 与 x1 一致：保留 send_err_start，下周期从 pkg_cnt==1 重试（仅 TRY_LATER 提前返回） */
			DEBUG_PRINTF("Channel %d: sendpic error pkg_cnt:%d\r\n", packet_p.Channel_ID, currentChannel->pkg_cnt);
			currentChannel->pkg_cnt = 1;
			return -1;
		}
        if (UDP_PIC_PACE_EVERY_N > 0U &&
            ((u32)currentChannel->pkg_cnt % UDP_PIC_PACE_EVERY_N) == 0U)
        {
        	usleep(UDP_PIC_PACE_US);
        }
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

    // 每轮最多发 BURST 包；BURST=1 时与「每调度周期一包」最接近，利于栈排空
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



        sta = video_link_send(
            (u8 *)(currentChannel->FrameBuffers[currentChannel->WriteOneFrameEnd] + packet_p.offset),
            currentChannel->UDP_IMG_PACKEG_SIZE);

        // 检查发送状态
        if (sta != 0)
        {
        	if (video_send_needs_reschedule(sta))
        	{
        		return 0;
        	}
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
        if (currentChannel->send_pic_start == 1)
        {
			if (UDP_PIC_PACE_EVERY_N > 0U &&
			    ((u32)currentChannel->pkg_cnt % UDP_PIC_PACE_EVERY_N) == 0U)
			{
				usleep(UDP_PIC_PACE_US);
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
            xil_printf("Channel %d: Video frame complete. pkg_cnt:%d\r\n", packet_p.Channel_ID, currentChannel->pkg_cnt - 1);
        	currentChannel->send_pic_start = 0;
        	currentChannel->video_sending = 0;
            currentChannel->WriteOneFrameEnd = -1;
            currentChannel->pkg_cnt = 1; // 重置计数
            /* 视频帧分包发完，同样在此间隙尝试应用待切换 */
            vdma_lwip_try_pending_channel_switch();
            return 0; // 传输完成
        }
        else
        {
        	/* 与 x1 相比增加兜底：异常 VIDEO_FLAG 时仍释放缓冲与 pending，避免 pkg_cnt 卡死 */
        	currentChannel->send_pic_start = 0;
        	currentChannel->video_sending = 0;
        	currentChannel->WriteOneFrameEnd = -1;
        	currentChannel->pkg_cnt = 1;
        	vdma_lwip_try_pending_channel_switch();
        	return 0;
        }
    }

    return 0; // 继续传输
}

#endif //#if defined (UDP_VIDEO)
