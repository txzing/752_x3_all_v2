#ifndef __PIXEL_COMPARE_H__

#if defined (XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES)
#define __PIXEL_COMPARE_H__
#include "axis_pixel_compare.h"

#define PC_STATUS_ON    	0x1
#define PC_STATUS_OFF  		0x0
#define PC_STATUS_ERROR  	0x7

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
}vcmp_message;


extern volatile u8 pixel_err[XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES];
extern volatile u8 pixel_err_cnt[XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES];
extern volatile u8 err_auto_send;
extern vcmp_message  vcmp_m[XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES];


Pc_Config *Pc_Config_LookupConfig(u16 DeviceId);
int Pc_Config_Initialize(Pc_Config *InstancePtr, u16 DeviceId);

void PixelCompareIntrHandler(void *CallbackRef);
int PixelCompare_init(void);
uint32_t rbg_swap_rgb(uint32_t pixel);
void vcmp_m_refresh_channel(u8 ch);


#endif
#endif
