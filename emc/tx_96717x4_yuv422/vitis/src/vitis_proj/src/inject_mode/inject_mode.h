#ifndef __INJECT_MODE_H__
#define __INJECT_MODE_H__

#include "xil_types.h"
#include "xparameters.h"

/* Internal TPG index: 0 .. VIDEO_SRC_COUNT-1.
 * Ethernet 0x50/0x51 host channel is 1-based: 1 .. VIDEO_SRC_COUNT.
 */
#if defined (XPAR_XV_TPG_NUM_INSTANCES)
#define VIDEO_SRC_COUNT    XPAR_XV_TPG_NUM_INSTANCES
#else
#define VIDEO_SRC_COUNT    0
#endif

#define VIDEO_SRC_0        0U
#define VIDEO_SRC_1        1U

/* Host-facing channel source type (0x51 sub=0). */
#define VIDEO_CH_SRC_NONE  0U
#define VIDEO_CH_SRC_TPG   1U

#define VIDEO_DEFAULT_W    1920U
#define VIDEO_DEFAULT_H    1080U
#define VIDEO_DEFAULT_FPS  30U

/* Host-set limits: even W, W/H > MIN, pclk <= MAX. */
#define VIDEO_MIN_ACTIVE   100U
#define VIDEO_PCLK_MAX_KHZ 300000U

typedef struct {
	u16 width;
	u16 height;
	u8 fps;
	u8 is_custom; /* 1 = host/custom blanking (not table-derived) */
	u32 clk_khz;
	u32 h_fp;
	u32 h_sw;
	u32 h_bp;
	u32 h_pol;
	u32 v_fp;
	u32 v_sw;
	u32 v_bp;
	u32 v_pol;
} video_src_cfg_t;

/* TPG instance count (host channel count). */
u8 video_src_host_count(void);
/* Source type for 0-based TPG index; VIDEO_CH_SRC_NONE if invalid. */
u8 video_src_source_type(u8 idx);

const video_src_cfg_t *video_src_cfg_get(u8 ch);

/*
 * Host timing: W/H/fps + blanking. pclk = f(Htot,Vtot,fps), snapped to
 * clk_wiz; HBP may be stretched slightly so measured fps meets request.
 * Runtime only - does not write EEPROM (use video_inject_save_nv).
 */
int video_src_set_timing(u8 ch, u16 width, u16 height, u8 fps,
			 u16 h_fp, u16 h_sw, u16 h_bp, u8 h_pol,
			 u16 v_fp, u16 v_sw, u16 v_bp, u8 v_pol);

/* Read TPG passthrough monitor (measured). Returns XST_FAILURE if invalid. */
int video_src_get_measured(u8 ch, u16 *width, u16 *height, u8 *fps);

int video_inject_apply(void);
/* Boot: restore last *saved* EEPROM geometry, else factory 1920x1080@30. */
int video_inject_apply_default(void);
/*
 * Persist dirty regions to EEPROM (explicit 0x51 sub=4 only).
 * Writes VEM2 and/or VTP1 only when that region is dirty.
 * Geom OK then TPG fail: keep geom dirty so host retry rewrites both.
 */
int video_inject_save_nv(void);
/* UART/factory: apply defaults and persist them to EEPROM. */
int video_inject_factory_default(void);

#endif
