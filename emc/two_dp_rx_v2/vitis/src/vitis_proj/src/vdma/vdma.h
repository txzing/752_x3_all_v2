#ifndef __VDMA_H__

//#include "xparameters.h"
#if defined (XPAR_XAXIVDMA_NUM_INSTANCES)
#define __VDMA_H__

#include "xaxivdma.h"
#include "xil_cache.h"


#if defined (__MICROBLAZE__)
	#define DDR_BASEADDR XPAR_MICROBLAZE_DCACHE_BASEADDR
//	#define DDR_BASEADDR 0x80000000
#else
	#define DDR_BASEADDR XPAR_DDR_MEM_BASEADDR
#endif

#define FRAME_BUFFER_BASE_ADDR  	(DDR_BASEADDR + (0x20000000))

#define FRAME_BUFFER_SIZE0      0x2000000    //0x2000000 for max 4KW RGB888 8bpc
//#define FRAME_BUFFER_SIZE0      0x600000    //0x600000 for max 1080p RGB888 8bpc

#define FRAME_BUFFER_1          FRAME_BUFFER_BASE_ADDR + (FRAME_BUFFER_SIZE0*1)
#define FRAME_BUFFER_2          FRAME_BUFFER_BASE_ADDR + (FRAME_BUFFER_SIZE0*2)
#define FRAME_BUFFER_3          FRAME_BUFFER_BASE_ADDR + (FRAME_BUFFER_SIZE0*3)
#define FRAME_BUFFER_4          FRAME_BUFFER_BASE_ADDR + (FRAME_BUFFER_SIZE0*4)
#define FRAME_BUFFER_5          FRAME_BUFFER_BASE_ADDR + (FRAME_BUFFER_SIZE0*5)
#define FRAME_BUFFER_6          FRAME_BUFFER_BASE_ADDR + (FRAME_BUFFER_SIZE0*6)
#define FRAME_BUFFER_7          FRAME_BUFFER_BASE_ADDR + (FRAME_BUFFER_SIZE0*7)
#define FRAME_BUFFER_8          FRAME_BUFFER_BASE_ADDR + (FRAME_BUFFER_SIZE0*8)
#define FRAME_BUFFER_9          FRAME_BUFFER_BASE_ADDR + (FRAME_BUFFER_SIZE0*9)
#define FRAME_BUFFER_10         FRAME_BUFFER_BASE_ADDR + (FRAME_BUFFER_SIZE0*10)
#define FRAME_BUFFER_11         FRAME_BUFFER_BASE_ADDR + (FRAME_BUFFER_SIZE0*11)
#define FRAME_BUFFER_12         FRAME_BUFFER_BASE_ADDR + (FRAME_BUFFER_SIZE0*12)
#define FRAME_BUFFER_13         FRAME_BUFFER_BASE_ADDR + (FRAME_BUFFER_SIZE0*13)
#define FRAME_BUFFER_14         FRAME_BUFFER_BASE_ADDR + (FRAME_BUFFER_SIZE0*14)
#define FRAME_BUFFER_15         FRAME_BUFFER_BASE_ADDR + (FRAME_BUFFER_SIZE0*15)
#define FRAME_BUFFER_16         FRAME_BUFFER_BASE_ADDR + (FRAME_BUFFER_SIZE0*16)
#define FRAME_BUFFER_17         FRAME_BUFFER_BASE_ADDR + (FRAME_BUFFER_SIZE0*17)
#define FRAME_BUFFER_18         FRAME_BUFFER_BASE_ADDR + (FRAME_BUFFER_SIZE0*18)
#define FRAME_BUFFER_19         FRAME_BUFFER_BASE_ADDR + (FRAME_BUFFER_SIZE0*19)
#define FRAME_BUFFER_20         FRAME_BUFFER_BASE_ADDR + (FRAME_BUFFER_SIZE0*20)
#define FRAME_BUFFER_21         FRAME_BUFFER_BASE_ADDR + (FRAME_BUFFER_SIZE0*21)
#define FRAME_BUFFER_22         FRAME_BUFFER_BASE_ADDR + (FRAME_BUFFER_SIZE0*22)
#define FRAME_BUFFER_23         FRAME_BUFFER_BASE_ADDR + (FRAME_BUFFER_SIZE0*23)
#define FRAME_BUFFER_24         FRAME_BUFFER_BASE_ADDR + (FRAME_BUFFER_SIZE0*24)
#define FRAME_BUFFER_25         FRAME_BUFFER_BASE_ADDR + (FRAME_BUFFER_SIZE0*25)
#define FRAME_BUFFER_26         FRAME_BUFFER_BASE_ADDR + (FRAME_BUFFER_SIZE0*26)
#define FRAME_BUFFER_27         FRAME_BUFFER_BASE_ADDR + (FRAME_BUFFER_SIZE0*27)
#define FRAME_BUFFER_28         FRAME_BUFFER_BASE_ADDR + (FRAME_BUFFER_SIZE0*28)
#define FRAME_BUFFER_29         FRAME_BUFFER_BASE_ADDR + (FRAME_BUFFER_SIZE0*29)
#define FRAME_BUFFER_30         FRAME_BUFFER_BASE_ADDR + (FRAME_BUFFER_SIZE0*30)
#define FRAME_BUFFER_31         FRAME_BUFFER_BASE_ADDR + (FRAME_BUFFER_SIZE0*31)
#define FRAME_BUFFER_32         FRAME_BUFFER_BASE_ADDR + (FRAME_BUFFER_SIZE0*32)
#define FRAME_BUFFER_33         FRAME_BUFFER_BASE_ADDR + (FRAME_BUFFER_SIZE0*33)
#define FRAME_BUFFER_34         FRAME_BUFFER_BASE_ADDR + (FRAME_BUFFER_SIZE0*34)
#define FRAME_BUFFER_35         FRAME_BUFFER_BASE_ADDR + (FRAME_BUFFER_SIZE0*35)
#define FRAME_BUFFER_36         FRAME_BUFFER_BASE_ADDR + (FRAME_BUFFER_SIZE0*36)

#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 1U)
extern	XAxiVdma Vdma0;
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 2U)
extern	XAxiVdma Vdma1;
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 3U)
extern	XAxiVdma Vdma2;
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 4U)
extern	XAxiVdma Vdma3;
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 5U)
extern	XAxiVdma Vdma4;
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 6U)
extern	XAxiVdma Vdma5;
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 7U)
extern	XAxiVdma Vdma6;
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 8U)
extern	XAxiVdma Vdma7;
#endif
#endif

int vdma_read_init(short DeviceID,short HoriSizeInput,short VertSizeInput,short Stride,unsigned int FrameStoreStartAddr);
int vdma_write_init(short DeviceID,XAxiVdma *Vdma,short HoriSizeInput,short VertSizeInput,short Stride,unsigned int FrameStoreStartAddr0,unsigned int FrameStoreStartAddr1,unsigned int FrameStoreStartAddr2);
u32 vdma_version();
void vdma_config(void);
void clear_display(void);


#if defined (UDP_VIDEO) || defined (TCP_VIDEO)
int vdma_udp_init(void);
void MonitorAndExitAfterIterations(void);
void vdma_lwip_apply_channel_geometry(u8 eth_video_ch);
void vdma_lwip_arm_pic_capture(void);
void vdma_lwip_arm_video_stream(void);
void vdma_lwip_stop_media(void);

#if defined (XPAR_AXI_PASSTHROUGH_MONITOR_NUM_INSTANCES)
u32 vdma_passthrough_mon_base_lvds(u8 lvds_idx_based);
int vdma_passthrough_read_mon(u32 mon_base, u32 *out_w, u32 *out_h, u32 *out_fps);
#endif

/*
易于切换：
只需修改一个宏即可启用或禁用调试输出。
*/
#define DEBUG_PRINT 0

#if DEBUG_PRINT
    #define DEBUG_PRINTF(format, ...) xil_printf(format, ##__VA_ARGS__)
#else
    #define DEBUG_PRINTF(format, ...)
#endif

#define SEND_PIC 	0
#define SEND_VIDEO 	1
#define MAX_ITERATIONS 3 // 每个通道运行的最大次数

#if defined (XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES)
	#define FRAME_ERR_BUFFER         	FRAME_BUFFER_BASE_ADDR - (FRAME_BUFFER_SIZE0*1)
	#define Err_Buffers_CNT  			XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES
#else
	#define Err_Buffers_CNT  1
#endif

typedef struct {
	u8 channel_ID;
    XAxiVdma Vdma;         // 指向对应的Vdma实例
    u16 DeviceID;           // VDMA设备ID
    u32 FrameBuffers[3];    // 帧缓冲区地址（最多3个）
    u32 Frame_Err_Buffers[Err_Buffers_CNT];
    u32 Width;              // 图像宽度（像素）
    u32 Height;             // 图像高度（像素）
    u32 Stride;             // 行跨度（字节）
    u32 FrameLength;        // 帧大小（字节）
    u32 S2MM_INTR_ID;
    u8 VIDEO_FLAG;          //0,传图片；1，传视频
    u8 send_pic_start;
    u8 video_sending;
    u8 send_video_start;
    u8 send_err_start[Err_Buffers_CNT];
    int WrIndex;            // 当前写缓冲区索引
    int RdIndex;            // 当前读缓冲区索引
    int WriteOneFrameEnd;   // 写完成标志
    int WriteError;         // 写错误计数
    u16 UDP_IMG_PACKEG_SIZE;//以太网传送一包的长度
    u32 pkg_cnt;            //以太网发送包次数
    u16 udp_send_times;     //一帧发送总包数
} VdmaChannel;

extern VdmaChannel VC_inst;
extern volatile u32 iterationCounts; // 每个通道的计数器
#endif //#if defined (UDP_VIDEO)

#endif /* VDMA_H_ */
