#ifndef __PIXEL_COMPARE_H__

#if defined (XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES)
#define __PIXEL_COMPARE_H__
#include "axis_pixel_compare.h"

/*
 * STATUS 寄存器 (offset STATUS / slv_reg3 读回) 与 AXI_LITE_REG_v1_0_S00_AXI 一致：
 *   bit0 axis_compare_enable
 *   bit1 intr_level（本帧已锁存比较错误，接 axis_pixel_compare 的 frame_error_flag）
 *   bit2 stream_valid（!stream_invalid）
 * 故「有错误」至少要求 bit0+bit1；bit2 为 0 时读数为 0x3，旧代码用 ==0x7 会漏判。
 */
#define PC_STATUS_AXIS_CMP_EN	0x1u
#define PC_STATUS_FRAME_ERR	0x2u
#define PC_STATUS_STREAM_OK	0x4u
#define PC_STATUS_ON		PC_STATUS_AXIS_CMP_EN
#define PC_STATUS_OFF		0x0u
#define PC_STATUS_ERROR		(PC_STATUS_AXIS_CMP_EN | PC_STATUS_FRAME_ERR | PC_STATUS_STREAM_OK)
#define PC_STATUS_ERR_CMP_MASK	(PC_STATUS_AXIS_CMP_EN | PC_STATUS_FRAME_ERR)

typedef struct {
	u16 DeviceId;		/* Unique ID  of device */
	UINTPTR BaseAddress;	/* Device base address */
	u32  Interrupt_ID;
}Pc_Config;


typedef struct __attribute__((packed))
{
 u32 channel;//采集通道
 u32 status;//状态码
 u32 Width;//宽度
 u32 Height;//高度
 u32 fps;//帧率
 u32 fps_total_cnt;//总帧数
 u32 error_pixel_hold;//发生错误时上一帧的像素值
 u32 pixel_hold; //错误时输入的像素点
 u32 pixel_threshold;//像素阈值
 u32 error_col;   /* 错误列 */
 u32 error_line;  //发生错误时的坐标行
 u32 rgb_cnt_pixel; //需要进行统计的像素值  rgb24位
 u32 rgb_pixel_total; //统计后的像素值个数
 u32 rgb_not_pixel; //不进行比较像素值
 /* RGB 统计区域限定（与 IP ROI 寄存器低 16 位一致：0 起算、闭区间 [xs,xe]×[ys,ye]） */
 u32 roi_x_start;
 u32 roi_x_end;
 u32 roi_y_start;
 u32 roi_y_end;
 u32 point_x;
 u32 point_y;
 u32 point_pixel;
}vcmp_message;


extern volatile u8 pixel_err[XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES];
extern volatile u8 pixel_err_cnt[XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES];
extern volatile u8 pixel_cp_start[XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES];
extern volatile u8 pixel_cp_start_cnt[XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES];
extern volatile u8 err_auto_send;
extern vcmp_message  vcmp_m[XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES];


int Pc_Config_Initialize(Pc_Config *InstancePtr, u16 DeviceId);

void PixelCompareIntrHandler(void *CallbackRef);
int PixelCompare_init(void);
uint32_t rbg_swap_rgb(uint32_t pixel);
/* RBG 寄存器值 -> 上位机协议（LE 线上字节顺序 R,G,B） */
uint32_t rgb_host_from_reg_rbg(uint32_t reg_rbg);
/* RBG 寄存器值 -> 调试显示 0x00RRGGBB */
uint32_t rgb_from_reg_rbg(uint32_t reg_rbg);
void vcmp_m_refresh_channel(u8 ch);

/* 以太网命令 ch 为 1..N；无效通道返回 0 */
u32 pixel_compare_axi_base_eth(u8 ch_based);

#endif
#endif
