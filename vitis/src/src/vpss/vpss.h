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


#endif

#endif
