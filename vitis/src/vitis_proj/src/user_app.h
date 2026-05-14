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
 * Passthrough 在 link lock 后常晚几帧才有非 0 分辨率；cable up 若只调 apply 且读数为 0，
 * 则不会执行 vdma_set_triple_geom_apply，DMA 仍停在 clear 后状态。此处在锁存期间重试若干 tick。
 * 每 tick 与 display_fresh 周期一致（约 200ms）。命名侧重 S2MM：写 DDR 侧画布固定 1920×1080，
 * 此处计数用于「等 monitor 给出稳定输入尺寸」后再去配 MM2S。
 */
	#define S2MM_RETUNE_MAX_TICKS  40U

/*
 * S2MM 侧热轮询防抖（与 user_app.c 中 s2mm_hot_* 配套）：
 * - 画布：S2MM 写 DDR 维固定 1920×1080（vdma triple 中 s_*），源不超过该范围即可共用布局。
 * - 检测：passthrough monitor 表征链路上当前稳定帧尺寸；display_fresh 周期轻读，
 *   连续 N 次一致后认为输入维稳定，再与上次已用于配置 MM2S 的值比较。
 * - 应用：有变化则 vdma_apply_detected_rgb_geom，仅重配 MM2S 读向 m_*（见 vdma.c）。
 * - N：约 200ms/次，N=2 约 0.4s 稳态后再重配，可按噪声调大。
 */
	#define S2MM_HOT_GEOM_STABLE_TICKS  2U

#else
	/* 无 PixelCompare IP 时 CHANNEL_NUM 取默认；宏仍定义，避免条件编译分支缺符号 */
	#define CHANNEL_NUM  4
	#define S2MM_HOT_GEOM_STABLE_TICKS  2U
#endif

extern u32 ret32;
extern u8 ret8;
extern u8 current_ch;

#if defined (XPAR_AXI_PASSTHROUGH_MONITOR_NUM_INSTANCES)
/* LVDS 链路 axis passthrough monitor 基址（与 pixel compare 通道一一对应时使用） */
u32 vdma_passthrough_mon_base_lvds(u8 lvds_idx_0based);
#endif

/* 切换视频源：配置 AxisSwitch 与 current_ch；UDP 侧在传完当前帧后由 vdma_lwip_try_pending_channel_switch 调用本函数 */
void board_apply_video_channel_switch(u8 eth_video_ch);
void app_info(void);
void video_resolution_print(char *info,u32 baseaddr);
void uart_receive_process(void);

//void MODS_Switch(void);

#endif /* SRC_USER_APP_H_ */
