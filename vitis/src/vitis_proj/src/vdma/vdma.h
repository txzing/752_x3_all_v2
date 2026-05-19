#ifndef __VDMA_H__

//#include "xparameters.h"
#if defined (XPAR_XAXIVDMA_NUM_INSTANCES)
#define __VDMA_H__

#include "xaxivdma.h"
#include "xil_cache.h"
#include "xstatus.h"


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


/*---------------------------------------------------------------------------
 *  对外 API：实例 id 与三帧 FRAME_BUFFER 一一对应（vdma_init_table）。
 *  寄存器顺序见 vdma_program_triple_buffer()；vdma_config_n / clear_vdma_n 为薄封装。
 *---------------------------------------------------------------------------*/

typedef struct {
	u8 id;               /* 0..7，与 kVdmaRegGeometry / s_clear_dims 下标一致 */
	u32 baseAddr;              /* XPAR_AXIVDMA_n_BASEADDR */
	u32 frameBuffers[3];   /* 第 n 路：FRAME_BUFFER_(3n+1)..(3n+3) */
} Vdma_Init_Table;

/* 一路 VDMA 里 S2MM（写 DDR）与 MM2S（读 DDR）可各自不同的 stride/有效宽高/缓冲内偏移 */
typedef struct {
	u8  bytePerPixels;
	u32 off_s2mm;
	u32 off_mm2s;
	u32 s_stride;	/* 行 stride（像素），写寄存器时再乘 bytes-per-pixel */
	u32 s_width;	/* S2MM 有效行宽（像素） */
	u32 s_height;
	u32 m_stride;
	u32 m_width;
	u32 m_height;
} VdmaTripleGeom;

/*
 * kVdmaRegGeometry[第几个VDMA][时序列]：
 *   列 0 = config.h 定义了 R1080；列 1 = 定义了 R4K；列 2 = 二者都未定义（else 默认）。
 *  修改某一格即可只改该 IP 在该模式下的几何，不必动其它实例。
 */
typedef enum {
	VDMA_TIMING_SLOT_R1080 = 0,
	VDMA_TIMING_SLOT_R4K = 1,
	VDMA_TIMING_SLOT_FALLBACK = 2,
} VdmaTimingSlot;

#define VDMA_ROW1080	{3, 0, 0, 1920U, 1920U, 1080U, 1920U, 1920U, 1080U }
#define VDMA_ROW4K		{3, 0, 0, 3840U, 3840U, 2160U, 3840U, 3840U, 2160U }
#define VDMA_ROW_MAIN720	{3, 0, 0, 1920U, 1920U, 720U, 1920U, 1920U, 720U }

static const Vdma_Init_Table vdma_init_table[] = {
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 1U)
	{ 0U, XPAR_AXIVDMA_0_BASEADDR,
	  { FRAME_BUFFER_1, FRAME_BUFFER_2, FRAME_BUFFER_3 } },
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 2U)
	{ 1U, XPAR_AXIVDMA_1_BASEADDR,
	  { FRAME_BUFFER_4, FRAME_BUFFER_5, FRAME_BUFFER_6 } },
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 3U)
	{ 2U, XPAR_AXIVDMA_2_BASEADDR,
	  { FRAME_BUFFER_7, FRAME_BUFFER_8, FRAME_BUFFER_9 } },
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 4U)
	{ 3U, XPAR_AXIVDMA_3_BASEADDR,
	  { FRAME_BUFFER_10, FRAME_BUFFER_11, FRAME_BUFFER_12 } },
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 5U)
	{ 4U, XPAR_AXIVDMA_4_BASEADDR,
	  { FRAME_BUFFER_13, FRAME_BUFFER_14, FRAME_BUFFER_15 } },
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 6U)
	{ 5U, XPAR_AXIVDMA_5_BASEADDR,
	  { FRAME_BUFFER_16, FRAME_BUFFER_17, FRAME_BUFFER_18 } },
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 7U)
	{ 6U, XPAR_AXIVDMA_6_BASEADDR,
	  { FRAME_BUFFER_19, FRAME_BUFFER_20, FRAME_BUFFER_21 } },
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 8U)
	{ 7U, XPAR_AXIVDMA_7_BASEADDR,
	  { FRAME_BUFFER_22, FRAME_BUFFER_23, FRAME_BUFFER_24 } },
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 9U)
	{ 8U, XPAR_AXIVDMA_8_BASEADDR,
	  { FRAME_BUFFER_25, FRAME_BUFFER_26, FRAME_BUFFER_27 } },
#endif
};


/* ---------------------------------------------------------------------------
 * 运行时分辨率（寄存器直写路径）：读/写 VdmaTripleGeom 镜像并刷新硬件。
 * - vdma_get_triple_geom：取当前几何（已配置则镜像，否则为编译表默认列）。
 * - vdma_set_triple_geom_apply：写入新几何并立即 vdma_program_triple_buffer_geom。
 * - vdma_reload_triple_geom_defaults：从 kVdmaRegGeometry 当前时序槽恢复并写寄存器。
 * 清屏 clear_vdma_* / vdma_clear_fb_triple 仍只认编译宏 R1080、R4K 等，不随本组 API 变化。
 *--------------------------------------------------------------------------- */
void uart_print_vdma_triple_geom(const char *stage, u8 id, const VdmaTripleGeom *g);
int vdma_get_triple_geom(u8 id, VdmaTripleGeom *out);
int vdma_set_triple_geom_apply(u8 id, const VdmaTripleGeom *geom);
int vdma_reload_triple_geom_defaults(u8 id);

int vdma_read_init(short DeviceID,short HoriSizeInput,short VertSizeInput,short Stride,unsigned int FrameStoreStartAddr);
int vdma_write_init(short DeviceID,XAxiVdma *Vdma,short HoriSizeInput,short VertSizeInput,short Stride,unsigned int FrameStoreStartAddr0,
					unsigned int FrameStoreStartAddr1,unsigned int FrameStoreStartAddr2);
int vdma_write_start(XAxiVdma *Vdma);
int vdma_write_stop(XAxiVdma *Vdma);
u32 vdma_version(XAxiVdma *Vdma);
void vdma_config(void);
void clear_display(void);
int vdma_apply_detected_rgb_geom(u8 first_vdma_id, u8 num_vdma, u32 mon_base);
void vdma_config_instance(u8 id);
void clear_vdma_instance(u8 id);

#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 1U)
extern	XAxiVdma Vdma0;
#endif
#if (XPAR_XAXIVDMA_NUM_INSTANCES >= 2U)
extern	XAxiVdma Vdma1;
#endif


#if defined (UDP_VIDEO) || defined (TCP_VIDEO)
int vdma_udp_init(void);

/* LWIP 上传：按通道分辨率与延迟切换（实现见 vdma.c 同文件内注释块） */
int vdma_passthrough_read_rgb_dims(u32 mon_base, u32 *out_w, u32 *out_h);
void vdma_lwip_note_channel_resolution(u8 lvds_idx_0based, u32 w, u32 h);
int vdma_lwip_get_channel_dims(u8 lvds_idx_0based, u32 *out_w, u32 *out_h);
void vdma_lwip_apply_channel_geometry(u8 eth_video_ch);
void vdma_lwip_request_channel_switch(u8 eth_video_ch);
void vdma_lwip_try_pending_channel_switch(void);
/* 0x80 抓图：丢弃待发旧帧，等下一帧完整写入后再分包上传 */
void vdma_lwip_arm_pic_capture(int eth_video_ch_idx);
void vdma_lwip_arm_video_stream(int eth_video_ch_idx);
void vdma_lwip_stop_media(int eth_video_ch_idx);

#define ETH_VIDEO_NUM 1

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

extern VdmaChannel VdmaChannels[ETH_VIDEO_NUM];
extern volatile u32 iterationCounts[ETH_VIDEO_NUM]; // 每个通道的计数器
#endif //#if defined (UDP_VIDEO) || defined (TCP_VIDEO)

#endif //#if defined (XPAR_XAXIVDMA_NUM_INSTANCES)

#endif //__VDMA_H__
