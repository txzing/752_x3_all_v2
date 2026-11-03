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

#else
	/* 无 PixelCompare IP 时 CHANNEL_NUM 取默认 */
	#define CHANNEL_NUM  4
#endif

extern u32 ret32;
extern u8 ret8;
extern u8 current_ch;
extern u8 switch_ch;


void app_info(void);
void video_resolution_print(char *info,u32 baseaddr);
void uart_receive_process(void);

//void MODS_Switch(void);

#endif /* SRC_USER_APP_H_ */
