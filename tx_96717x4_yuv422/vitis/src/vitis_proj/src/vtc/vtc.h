#ifndef __VTC_H__

#ifdef XPAR_XVTC_NUM_INSTANCES
#define __VTC_H__

#include "xil_types.h"
#include "xvtc.h"

typedef enum video_resolution_no
{
	VIDEO_RESOLUTION_VGA = 0,
	VIDEO_RESOLUTION_480P,
	VIDEO_RESOLUTION_576P,
	VIDEO_RESOLUTION_SVGA,
	VIDEO_RESOLUTION_XGA,
	VIDEO_RESOLUTION_720P,
	VIDEO_RESOLUTION_SXGA,
	VIDEO_RESOLUTION_1080P,
	VIDEO_RESOLUTION_FHDPLUS,
	VIDEO_RESOLUTION_UXGA,
	VIDEO_RESOLUTION_1920X960,
	VIDEO_RESOLUTION_4K,
	VIDEO_RESOLUTION_4KFULL,
	VIDEO_RESOLUTION_CUSTOM1,
	VIDEO_RESOLUTION_4KCUST,
	VIDEO_RESOLUTION_BAIDUSF,
	VIDEO_RESOLUTION_1920X3240,
	VIDEO_RESOLUTION_1920X2160,
	VIDEO_RESOLUTION_1280X960,
	VIDEO_RESOLUTION_NO_BUTT,
} resolution_no;

struct struct_vres_timing_t
{
	char *pName;
	u32 VActiveVideo;
	u32 VFrontPorch;
	u32 VSyncWidth;
	u32 VBackPorch;
	u32 VSyncPolarity;
	u32 HActiveVideo;
	u32 HFrontPorch;
	u32 HSyncWidth;
	u32 HBackPorch;
	u32 HSyncPolarity;
};
typedef struct struct_vres_timing_t vres_timing_t;

#if (XPAR_XVTC_NUM_INSTANCES >= 1U)
extern XVtc VtcInst0;
#endif
#if (XPAR_XVTC_NUM_INSTANCES >= 2U)
extern XVtc VtcInst1;
#endif

extern vres_timing_t vres_resolutions[VIDEO_RESOLUTION_NO_BUTT];

/* Program and enable VTC generator from explicit blanking (free-run). */
void vtiming_gen_run_timing(XVtc *p_vtg_inst, const vres_timing_t *pTiming,
			    int verbose);

#endif /* XPAR_XVTC_NUM_INSTANCES */

#endif /* __VTC_H__ */
