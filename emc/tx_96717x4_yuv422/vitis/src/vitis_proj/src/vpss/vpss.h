#ifndef __VPSS_H__

//#include "xparameters.h"
#if defined (XPAR_XVPROCSS_NUM_INSTANCES)
#define __VPSS_H__

#include "xil_types.h"
#include "xvidc.h"
#include "xbasic_types.h"
#include "xvprocss.h"

//#include "../dp/video_timing_table.h"
//extern XVidC_VideoTimingMode XVidC_MyVideoTimingMode[(XVIDC_CM_NUM_SUPPORTED - (XVIDC_VM_CUSTOM + 1))];


int vpss_config_scaler_0(u32 DeviceId);
int vpss_config_scaler_1(u32 DeviceId);
int vpss_config_color_Space_Conversion(u32 DeviceId);
int vpss_config_csc(u32 DeviceId, u32 width, u32 height, u8 fps);
int vpss_config_csc_timing(u32 DeviceId, u32 width, u32 height, u8 fps,
			   u32 h_fp, u32 h_sw, u32 h_bp, u32 h_pol,
			   u32 v_fp, u32 v_sw, u32 v_bp, u32 v_pol);
void vpss_invalidate(void);


#endif

#endif
