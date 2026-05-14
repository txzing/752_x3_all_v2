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
 * 每 tick 与 display_fresh 周期一致（约 200ms）。
 */
	#define S2MM_RETUNE_MAX_TICKS  40U

#else
	#define CHANNEL_NUM  4
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
