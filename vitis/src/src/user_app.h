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
 * 与 axi_timer.c 中 TIMER_TLR（500ms）+ display_fresh(timer_cnt>=1) 对齐：
 * 每进入一次本逻辑表示约 500ms；超时 = TICKS * 500ms。
 */
	#define PIXEL_ERR_HOST_IDLE_TICKS  10
/*
 * 开比较: cnt==0 复位 LVDS, 之后每拍读 monitor。
 * 10 拍 * 500ms 约 5s 仍无合法宽高则放弃, 不写比较使能。
 */
	#define PIXEL_CP_START_GIVEUP_TICKS  10U

/*
 * S2MM 侧热轮询防抖（s2mm_hot_ch_t / s2mm_hot[]，见下方类型与 extern）：
 * - 检测：passthrough monitor 表征链路上当前帧尺寸；display_fresh 周期轻读，
 *   连续 N 次一致后认为输入维稳定，再与上次已写入 VDMA 的 applied_* 比较。
 * - 应用：有变化则 vdma_apply_detected_rgb_geom，S2MM 与 MM2S 都改成该宽高。
 * - cable up 的 seed 不写 applied_*，否则锁定时已经稳定的分辨率不会再配寄存器。
 * - N：约 500ms/次，N=5 约 2.5s 稳态后再重配。
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
/* Touch INT. 0 = ok, *reg23 is reg 0x23. Fail: bits 15:8 step, bits 7:0 I2C st.
 * step: 1 param, 2 clear, 3 open_remote, 4 close_remote, 5 read. */
int tp_touch_intr_read(u8 ch, u8 *reg23);
/* Touch INT: ch 1..3, I2C addr 0x50. Same return as above. */
int tp_touch_intr_read(u8 ch, u8 *reg23);

#endif /* SRC_USER_APP_H_ */
