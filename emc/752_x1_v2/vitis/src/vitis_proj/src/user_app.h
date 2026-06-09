/*
 * user_app.h
 *
 *  Created on: 2024年6月26日
 *      Author: 13648
 */

#ifndef SRC_USER_APP_H_
#define SRC_USER_APP_H_

#if defined (XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES)
	#define CHANNEL_NUM  XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES
/*
 * 与 axi_timer.c 中 TIMER_TLR（200ms）+ display_fresh(timer_cnt>=1) 对齐：
 * 每进入一次本逻辑表示约 200ms；超时 = TICKS * 200ms。
 */
	#define PIXEL_ERR_HOST_IDLE_TICKS  30

/*
 * S2MM 侧热轮询防抖（s2mm_hot_ch_t / s2mm_hot[]，见下方类型与 extern）：
 * - 画布：S2MM 写 DDR 维固定（vdma triple 中 s_*），源不超过该范围即可共用布局。
 * - 检测：passthrough monitor 表征链路上当前稳定帧尺寸；display_fresh 周期轻读，
 *   连续 N 次一致后认为输入维稳定，再与上次已用于配置 MM2S 的值比较。
 * - 应用：有变化则 vdma_apply_detected_rgb_geom，仅重配 MM2S 读向 m_*（见 vdma.c）。
 * - N：约 200ms/次，N=5 约 1s 稳态后再重配，可按噪声调大。
 */
	#define S2MM_HOT_GEOM_STABLE_TICKS  5U

#else
	#define CHANNEL_NUM  4
#endif

#if defined (XPAR_AXI_PASSTHROUGH_MONITOR_NUM_INSTANCES) && defined (XPAR_XAXIVDMA_NUM_INSTANCES)
typedef struct
{
	u32 applied_w;
	u32 applied_h;
	u32 stable_w;
	u32 stable_h;
	u8 stable_n;
} s2mm_hot_ch_t;

extern s2mm_hot_ch_t s2mm_hot[CHANNEL_NUM];
#endif

extern u32 ret32;
extern u8 ret8;
extern u8 current_ch;
extern u8 switch_ch;
extern  u8 stream_id[CHANNEL_NUM];

/* 切换视频源：配置 AxisSwitch 与 current_ch；UDP 侧在传完当前帧后由 vdma_lwip_try_pending_channel_switch 调用本函数 */
void board_apply_video_channel_switch(u8 switch_ch);
void app_info(void);
void video_resolution_print(char *info,u32 baseaddr);
void uart_receive_process(void);

#endif /* SRC_USER_APP_H_ */
