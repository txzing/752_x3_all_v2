#include "../bsp.h"

#if defined (UDP_VIDEO) || defined (TCP_VIDEO)

/* 单张图：给缓冲时间，减少丢包率 */
#define UDP_PIC_PACE_EVERY_N  10U //包数
#define UDP_PIC_PACE_US       1000U  //delay_us

packet_header packet_p;
u8 err_ch;

/**
 * @brief 主传输逻辑
 */
void lwip_video_transfer(void)
{
	MonitorAndExitAfterIterations();//会丢弃前 MAX_ITERATIONS 帧

	// 检查当前通道是否需要传输
	if (VC_inst.WriteOneFrameEnd >= 0)
	{
		if (((VC_inst.send_pic_start == 1) ||
			 (VC_inst.send_video_start == 1) ||
			 (VC_inst.video_sending == 1)) &&
			 (iterationCounts > MAX_ITERATIONS))
		{
			// 调用通用传输逻辑处理
			handle_channel_transfer();
		}
	}

#if defined (XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES)
	if (VC_inst.send_err_start[0] == 1 && (VC_inst.send_err_sending[1] == 0) && (VC_inst.send_err_sending[2] == 0))
//	if (VC_inst.send_err_start[0])
	{
		if (handle_err_pic_transfer(1))
		{
		    bsp_printf(TXT_RED "\r\n__FILE__:%s, __LINE__:%d\r\n" TXT_RST,__FILE__, __LINE__);
			return;
		}
	}
#if (XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES >= 2U)
	else if (VC_inst.send_err_start[1] == 1 && (VC_inst.send_err_sending[0] == 0) && (VC_inst.send_err_sending[2] == 0))
//	else if (VC_inst.send_err_start[1])
	{
		if (handle_err_pic_transfer(2))
		{
		    bsp_printf(TXT_RED "\r\n__FILE__:%s, __LINE__:%d\r\n" TXT_RST,__FILE__, __LINE__);
			return;
		}
	}
#endif
#if (XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES >= 3U)
	else if (VC_inst.send_err_start[2] == 1 && (VC_inst.send_err_sending[0] == 0) && (VC_inst.send_err_sending[1] == 0))
//	else if (VC_inst.send_err_start[2])
	{
		if (handle_err_pic_transfer(3))
		{
		    bsp_printf(TXT_RED "\r\n__FILE__:%s, __LINE__:%d\r\n" TXT_RST,__FILE__, __LINE__);
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
	VdmaChannel *currentChannel = &VC_inst;
	static VdmaChannel err_inst =
	{
		.UDP_IMG_PACKEG_SIZE = 1200,
		.VIDEO_FLAG = SEND_PIC,
		.pkg_cnt = 1,
	};


	int sta;
	u32 mon_base;
	u32 w;
	u32 h;

	if (channel < 1U || channel > CHANNEL_NUM)
	{
	    bsp_printf(TXT_RED "\r\n__FILE__:%s, __LINE__:%d\r\n" TXT_RST,__FILE__, __LINE__);
		return;
	}
	if(channel != err_ch)
	{
		mon_base = vdma_passthrough_mon_base_lvds(channel - 1);
		if (mon_base == 0U)
		{
			currentChannel->send_err_start[channel - 1] = 0; // 停止图片发送
			currentChannel->send_err_sending[channel - 1] = 0;
			err_inst.pkg_cnt = 1;
		    bsp_printf(TXT_RED "\r\n__FILE__:%s, __LINE__:%d\r\n" TXT_RST,__FILE__, __LINE__);
			return;
		}

		if(vdma_passthrough_read_mon(mon_base, &w, &h,NULL) == 0)
		{
			currentChannel->send_err_start[channel - 1] = 0; // 停止图片发送
			currentChannel->send_err_sending[channel - 1] = 0;
			err_inst.pkg_cnt = 1;
		    bsp_printf(TXT_RED "\r\n__FILE__:%s, __LINE__:%d\r\n" TXT_RST,__FILE__, __LINE__);
			return;
		}

		err_inst.Width = w;
		err_inst.Height = h;
		err_inst.FrameLength = w * h * 3U;
		err_inst.pkg_cnt = 1U;
		err_inst.UDP_IMG_PACKEG_SIZE = currentChannel->UDP_IMG_PACKEG_SIZE;
		err_inst.VIDEO_FLAG == SEND_PIC;
		err_inst.udp_send_times =  (u16)(((err_inst.FrameLength + (err_inst.UDP_IMG_PACKEG_SIZE - 1U))
											/ err_inst.UDP_IMG_PACKEG_SIZE));
//        xil_printf("err_channel:%d, err_ch:%d ,current_ch:%d --", channel, err_ch, current_ch);
//        xil_printf("start send err_%d_pic , Width:%d ,Height:%d\r\n",channel, err_inst.Width, err_inst.Height);
		err_ch = channel;
	}
	// 检查当前帧是否已发送完所有数据包
	if (err_inst.pkg_cnt <= err_inst.udp_send_times)
	{
		// 构造数据包头部
		packet_p.HEAD_ID    = HEADER_ID;
		packet_p.Channel_ID = channel;
		packet_p.SEND_FLAG  = 2;
		packet_p.Width      = err_inst.Width;
		packet_p.Height     = err_inst.Height;
		packet_p.total      = err_inst.FrameLength;
		packet_p.picseq     = 1; // 通道的独立序列号
		packet_p.frameseq   = err_inst.pkg_cnt;
        packet_p.offset  = (err_inst.pkg_cnt - 1U) * err_inst.UDP_IMG_PACKEG_SIZE;

        if(err_inst.pkg_cnt == err_inst.udp_send_times)
        {
            packet_p.framesize = packet_p.total - packet_p.offset;
//            xil_printf("packet_p.framesize:%d\r\n", packet_p.framesize);
        }
        else
        {
            packet_p.framesize = err_inst.UDP_IMG_PACKEG_SIZE;
        }
//stride都是1920
		// 刷新缓存
		Xil_DCacheInvalidateRange(
			(u32)(currentChannel->Frame_Err_Buffers[channel - 1] + packet_p.offset),
			packet_p.framesize);


#if (defined UDP_VIDEO)
	// 发送数据包
		sta = sendpic_udp((u8 *)(currentChannel->Frame_Err_Buffers[channel - 1] + packet_p.offset),
				packet_p.framesize);
#elif (defined TCP_VIDEO)
	// 发送数据包
		sta = sendpic_tcp((u8 *)(currentChannel->Frame_Err_Buffers[channel - 1] + packet_p.offset),
				packet_p.framesize);
#endif

		// 检查发送状态
		if (sta != 0)
		{
#if (DEBUG_PRINT == 1U)
			DEBUG_PRINTF("Channel %d: sendpic error pkg_cnt:%d\r\n", packet_p.Channel_ID, err_inst.pkg_cnt);
#endif
			err_inst.pkg_cnt = 1;
			return -1; // 传输失败，停止
		}
        /* =========每 UDP_PIC_PACE_EVERY_N 个 packet 延时 ========= */
        if (err_inst.VIDEO_FLAG == SEND_PIC)//只在发送图片时
        {
			if ((err_inst.pkg_cnt % UDP_PIC_PACE_EVERY_N) == 0)
			{
				usleep(UDP_PIC_PACE_US);
			}
        }
        /* ==================================================== */
		// 成功发送，增加计数
		err_inst.pkg_cnt++;
		currentChannel->send_err_sending[channel - 1] = 1;
	}
	else // 当前帧发送完成[channel]
	{
//        xil_printf("err Channel %d: Video frame complete. pkg_cnt:%d\r\n", packet_p.Channel_ID, err_inst.pkg_cnt - 1);
		currentChannel->send_err_start[channel - 1] = 0; // 停止图片发送
		currentChannel->send_err_sending[channel - 1] = 0;
		err_inst.pkg_cnt = 1;

		pixel_err[channel - 1] = 0;
		pixel_err_cnt[channel - 1] = 0U;
	}
    return 0;
}
#endif


/**
 * @brief 通用数据发送逻辑，处理图片或视频发送
 * @param channel 当前通道索引
 * @return 是否需要继续传输 (1 表示继续, 0 表示停止)
 */
int handle_channel_transfer(void)
{
    VdmaChannel *currentChannel = &VC_inst;

    int sta;

    // 检查当前帧是否已发送完所有数据包
    if (currentChannel->pkg_cnt <= currentChannel->udp_send_times)
    {
        // 构造数据包头部
        packet_p.HEAD_ID    = HEADER_ID;
        packet_p.Channel_ID = current_ch;
        packet_p.Width      = currentChannel->Width;
        packet_p.Height     = currentChannel->Height;
        packet_p.total      = currentChannel->FrameLength;
        packet_p.picseq     = 1; // 通道的独立序列号
        packet_p.frameseq   = currentChannel->pkg_cnt;
        packet_p.offset  = (currentChannel->pkg_cnt - 1U) * currentChannel->UDP_IMG_PACKEG_SIZE;
        if(currentChannel->pkg_cnt == currentChannel->udp_send_times)
        {
            packet_p.framesize = packet_p.total - packet_p.offset;
//            xil_printf("packet_p.framesize:%d\r\n", packet_p.framesize);
        }
        else
        {
            packet_p.framesize = currentChannel->UDP_IMG_PACKEG_SIZE;
        }

        if (currentChannel->send_pic_start == 1)
        	packet_p.SEND_FLAG  = 0;
        else
        	packet_p.SEND_FLAG  = 1;

        // 刷新缓存
        Xil_DCacheInvalidateRange(
            (u32)(currentChannel->FrameBuffers[currentChannel->WriteOneFrameEnd] + packet_p.offset),
			packet_p.framesize);

#if (defined UDP_VIDEO)
	// 发送数据包
        sta = sendpic_udp((u8 *)(currentChannel->FrameBuffers[currentChannel->WriteOneFrameEnd] + packet_p.offset),
        		packet_p.framesize);
#elif (defined TCP_VIDEO)
	// 发送数据包
        sta = sendpic_tcp((u8 *)(currentChannel->FrameBuffers[currentChannel->WriteOneFrameEnd] + packet_p.offset),
        		packet_p.framesize);
#endif

        // 检查发送状态
        if (sta != 0)
        {
#if (DEBUG_PRINT == 1U)
        	DEBUG_PRINTF("Channel %d: sendpic error pkg_cnt:%d\r\n", packet_p.Channel_ID, currentChannel->pkg_cnt);
#endif
            currentChannel->WriteOneFrameEnd = -1;
            currentChannel->pkg_cnt = 1;
            currentChannel->video_sending = 0;
        	if (currentChannel->VIDEO_FLAG == SEND_PIC)
        	{
        		currentChannel->send_pic_start = 0;
        	}
        	else if (currentChannel->VIDEO_FLAG == SEND_VIDEO)
        	{
        		currentChannel->send_video_start = 0;
        	}

            return -1; // 传输失败，停止
        }
        /* =========每 UDP_PIC_PACE_EVERY_N 个 packet 延时 ========= */
        if (currentChannel->VIDEO_FLAG == SEND_PIC)//只在发送图片时
        {
			if ((currentChannel->pkg_cnt % UDP_PIC_PACE_EVERY_N) == 0)
			{
				usleep(UDP_PIC_PACE_US);
			}
        }
        else
        {
        	currentChannel->video_sending = 1;
        }
        /* ==================================================== */
        // 成功发送，增加计数
        currentChannel->pkg_cnt++;

    }
    else // 当前帧发送完成
    {
//		xil_printf("Channel %d:transmission complete. pkg_cnt:%d\r\n", packet_p.Channel_ID, currentChannel->pkg_cnt - 1);
		currentChannel->send_pic_start = 0; // 停止图片发送
		currentChannel->video_sending = 0;
		currentChannel->WriteOneFrameEnd = -1;
		currentChannel->pkg_cnt = 1;
    }

    return 0; // 继续传输
}

#endif //#if defined (UDP_VIDEO)
