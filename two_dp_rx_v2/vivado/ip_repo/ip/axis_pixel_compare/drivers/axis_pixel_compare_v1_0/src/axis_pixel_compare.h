
#ifndef AXI_PIXEL_COMPARE_H
#define AXI_PIXEL_COMPARE_H

/******************************************************************************
 * @file axis_pixel_compare.h
 * @brief axis_pixel_compare AXI-Lite register offsets and BSP config types.
 *
 * Version: 2.19 (must match spirit:version in component.xml and OPTION VERSION in
 *          drivers/axis_pixel_compare_v1_0/data/axis_pixel_compare.mdd)
 *
 * Stream format: tdata[47:24]=reference, tdata[23:0]=live video (RBG 24-bit each).
 *
 * Frame compare:
 *   STATUS bit0 enable; RGB_NOT_PIXEL ignore centre - skip diff when |video-mask|<=TH per ch.
 *   Otherwise |ref-video| > TH -> error + IRQ.
 *
 * RGB region statistics (independent of mask):
 *   RGB_CNT_PIXEL = target colour; count pixels in ROI with |video-target|<=TH per ch.
 *   RGB_PIXEL_TOTAL = latched at next-frame SOF beat (previous frame count).
 *
 * Point sample: write POINT_X/Y (0-based); read POINT_PIXEL = video RBG at that coordinate.
 *
 * ROI_* : 0-based inclusive [xs,xe] x [ys,ye]; power-on default from IP ROI_RST_* parameters.
 * Illegal ROI (xe<xs or ye<ys) -> count full frame.
 *
 * ERR_COL/ERR_LINE: 0-based; same col_cnt/line_cnt as ROI (ERR_LINE no SW +1).
 * LINE readback: line_cnt latched at SOF (0-based); SW may +1 for WxH display (e.g. 719->720).
 *
 * 说明 (GB2312, 标点均为英文):
 * - 帧差: 使能后比较 ref 与 video, 超阈锁存首错并中断; 忽略色 |video-mask|<=TH 不参与比较.
 * - RGB统计: ROI 内 |video-RGB_CNT_PIXEL|<=TH 的像素计数, 与忽略色无关.
 * - RGB_PIXEL_TOTAL 在下一帧 SOF 锁存上一帧计数, 首帧读回可能为 0, 请在第 2 帧后再读.
 * - ROI 寄存器 0 起算闭区间; 上位机 1 起算需先减 1; 非法 ROI 退化为全幅统计.
 *****************************************************************************/

/****************** Include Files ********************/
#include "xil_types.h"
#include "xstatus.h"
#include "xil_io.h"
#include "xparameters.h"

/**
 * BSP-generated configuration table (axis_pixel_compare_g.c, see data/axis_pixel_compare.tcl).
 * IntrId: resolved from xparameters.h - ZynqMP/Versal fabric *_INTR_INTR, Zynq-7000
 * *_IP2INTC_IRPT_INTR, canonical XPAR_FABRIC_AXIS_PIXEL_COMPARE_n_VEC_ID, MicroBlaze INTC
 * *_VEC_ID via name scan, else 0U (e.g. PMU).
 */
typedef struct {
	u16 DeviceId;
	UINTPTR S00_axi_BaseAddr;
	u32 IntrId;
} XAxisPixelCompare_Config;

extern XAxisPixelCompare_Config XAxisPixelCompare_ConfigTable[];

const XAxisPixelCompare_Config *XAxisPixelCompare_LookupConfig(u16 DeviceId);

/* --- 0x00..0x14: stream / control --- */
#define COL 0                  /* R  pixels per completed line (col_cnt+1 at EOL) */
#define LINE 4                 /* R  line_cnt latched at SOF (0-based; SW +1 for line count) */
#define FPS 8                  /* R  frames per second */
#define STATUS 12              /* R  [0]enable [1]frame_err [2]stream_valid */
#define INTR_CLEAR 16          /* W  write bit0 clears interrupt / error hold */
#define FPS_TOTAL_CNT 20       /* R  total frames since reset */

/* --- 0x18..0x24: live snapshot & first-error latch --- */
#define PIXEL_POINT 24         /* R  current ref half-word on bus */
#define STREAM_IN_DATA 28      /* R  current video half-word on bus */
#define ERROE_DATA_HOLD 32     /* R  ref @ first error this frame */
#define STREAM_IN_DATA_HOLD 36 /* R  video @ first error this frame */

/* --- 0x28..0x3C: threshold / RGB compare config --- */
#define PIXEL_THRESHOLD 40     /* W  slv_reg10: per-channel tolerance (0..255) */
#define ERR_COL 44             /* R  0-based error pixel column */
#define ERR_LINE 48            /* R  0-based error row (same line_cnt as ROI) */
#define RGB_CNT_PIXEL 52       /* W  slv_reg13: target RBG; stats use |video-target|<=TH */
#define RGB_PIXEL_TOTAL 56     /* R  RGB hits for previous frame (latched at SOF) */
#define RGB_NOT_PIXEL 60       /* W  slv_reg15: ignore centre RBG; skip diff if |video-this|<=TH */

/* --- 0x40..0x4C: ROI (slv_reg16..19), 0-based inclusive --- */
#define ROI_X_START 64
#define ROI_X_END   68
#define ROI_Y_START 72
#define ROI_Y_END   76

/* --- 0x50..0x58: point sample (slv_reg20..22) --- */
#define POINT_X      80        /* W  sample column, 0-based */
#define POINT_Y      84        /* W  sample row, 0-based */
#define POINT_PIXEL  88        /* R  latched video RBG at (POINT_X, POINT_Y) */

#define AXI_LITE_REG_S00_AXI_SLV_REG23_OFFSET 92
#define AXI_LITE_REG_S00_AXI_SLV_REG24_OFFSET 96
#define AXI_LITE_REG_S00_AXI_SLV_REG25_OFFSET 100
#define AXI_LITE_REG_S00_AXI_SLV_REG26_OFFSET 104
#define AXI_LITE_REG_S00_AXI_SLV_REG27_OFFSET 108
#define AXI_LITE_REG_S00_AXI_SLV_REG28_OFFSET 112
#define AXI_LITE_REG_S00_AXI_SLV_REG29_OFFSET 116
#define AXI_LITE_REG_S00_AXI_SLV_REG30_OFFSET 120
#define AXI_LITE_REG_S00_AXI_SLV_REG31_OFFSET 124


/**************************** Type Definitions *****************************/
/**
 *
 * Write a value to a AXI_LITE_REG register. A 32 bit write is performed.
 * If the component is implemented in a smaller width, only the least
 * significant data is written.
 *
 * @param   BaseAddress is the base address of the AXI_LITE_REGdevice.
 * @param   RegOffset is the register offset from the base to write to.
 * @param   Data is the data written to the register.
 *
 * @return  None.
 *
 * @note
 * C-style signature:
 * 	void AXI_LITE_REG_mWriteReg(u32 BaseAddress, unsigned RegOffset, u32 Data)
 *
 */
#define AXI_LITE_REG_mWriteReg(BaseAddress, RegOffset, Data) \
  	Xil_Out32((BaseAddress) + (RegOffset), (u32)(Data))

/**
 *
 * Read a value from a AXI_LITE_REG register. A 32 bit read is performed.
 * If the component is implemented in a smaller width, only the least
 * significant data is read from the register. The most significant data
 * will be read as 0.
 *
 * @param   BaseAddress is the base address of the AXI_LITE_REG device.
 * @param   RegOffset is the register offset from the base to write to.
 *
 * @return  Data is the data from the register.
 *
 * @note
 * C-style signature:
 * 	u32 AXI_LITE_REG_mReadReg(u32 BaseAddress, unsigned RegOffset)
 *
 */
#define AXI_LITE_REG_mReadReg(BaseAddress, RegOffset) \
    Xil_In32((BaseAddress) + (RegOffset))

/************************** Function Prototypes ****************************/
/**
 *
 * Run a self-test on the driver/device. Note this may be a destructive test if
 * resets of the device are performed.
 *
 * If the hardware system is not built correctly, this function may never
 * return to the caller.
 *
 * @param   baseaddr_p is the base address of the AXI_LITE_REG instance to be worked on.
 *
 * @return
 *
 *    - XST_SUCCESS   if all self-test code passed
 *    - XST_FAILURE   if any self-test code failed
 *
 * @note    Caching must be turned off for this function to work.
 * @note    Self test may fail if data memory and device are not on the same bus.
 *
 */
XStatus AXI_LITE_REG_Reg_SelfTest(void * baseaddr_p);

#endif /* AXI_PIXEL_COMPARE_H */
