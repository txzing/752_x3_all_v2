#ifndef __VTC_H__

//#include "xparameters.h"
#ifdef XPAR_XVTC_NUM_INSTANCES
#define __VTC_H__
// *****************************************************
// Notes
// *****************************************************
//   - The purpose of this driver is to provide a layer
//     of abstraction over the Video Timing Controller
//     drivers when it is used in generate-only mode. It
//     assumes the caller has allocated and initialized
//     the XVtc instance to be used.

// *****************************************************
// Dependencies
// *****************************************************
#include "xil_types.h"
#include "xvtc.h"


// *****************************************************
// Function status return values
// *****************************************************
#define VTIMING_GEN_SUCCESS        0
#define VTIMING_GEN_ERROR_UNKNOWN -1

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
	VIDEO_RESOLUTION_4K,//4K30
	VIDEO_RESOLUTION_4KFULL,//4K30W
	VIDEO_RESOLUTION_CUSTOM1,
	VIDEO_RESOLUTION_4KCUST,
	VIDEO_RESOLUTION_BAIDUSF,

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
extern XVtc        VtcInst0;
#endif // XPAR_XVTC_NUM_INSTANCES == 1U
#if (XPAR_XVTC_NUM_INSTANCES >= 2U)
extern XVtc        VtcInst1;
#endif // XPAR_XVTC_NUM_INSTANCES == 2U
#if (XPAR_XVTC_NUM_INSTANCES >= 3U)
extern XVtc        VtcInst2;
#endif // XPAR_XVTC_NUM_INSTANCES == 3U
#if (XPAR_XVTC_NUM_INSTANCES >= 4U)
extern XVtc        VtcInst3;
#endif // XPAR_XVTC_NUM_INSTANCES == 4U

// *****************************************************
// Public functions
// *****************************************************

char *  vres_get_name(u32 resolutionId);
u32 vres_get_width(u32 resolutionId);
u32 vres_get_height(u32 resolutionId);
u32 vres_get_timing(u32 resolutionId, vres_timing_t *pTiming );

s32 vres_detect( u32 width, u32 height );

// vtiming_gen_run() - Set up and start VTC generator registers and enable.
//   - p_vtg_inst    - Pointer to object to work on
//   - resolution_id - Which video resolution id to use from video_resolution.h
//   - verbose       - Whether or not to print debug info
//   - return        - None
void vtiming_gen_run
(
	XVtc* p_vtg_inst,
	int   resolution_id,
	int   verbose
);

int vtc_init(XVtc *InstancePtr, u16 DeviceId);
int vtc_config(void);


#endif // XPAR_XVTC_NUM_INSTANCES

#endif // __VTC_H__

