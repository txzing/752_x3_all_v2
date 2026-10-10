#include "../bsp.h"
#if defined (XPAR_XVPROCSS_NUM_INSTANCES)


#if 1
/*
 * note:If the processed resolution is not in the defined list, it needs to be added to the list
 *
 */
typedef enum {
	XVIDC_VM_1280x3840_30_P = (XVIDC_VM_CUSTOM + 1),
	XVIDC_VM_3840x1280_30_P,
	XVIDC_VM_3840x1280_60_P,
	XVIDC_VM_320x960_30_P,
	XVIDC_VM_960x1080_30_P,
	XVIDC_VM_1920x2560_30_P,
	XVIDC_VM_1920x2560_60_P,
	XVIDC_VM_1920x1280_30_P,
	XVIDC_VM_1920x1280_60_P,
	XVIDC_VM_1600x2560_30_P,
	XVIDC_VM_670x2160_30_P,
	XVIDC_VM_4320x720_60_ZXD_A,
	XVIDC_VM_3840x720_60,
	XVIDC_VM_2560x1440_30_P,
	XVIDC_VM_1920x3240_30_P,
	XVIDC_VM_1920x2160_30_P,
	XVIDC_VM_1280x2160_30_P,
	XVIDC_VM_1280x960_30_P,
    XVIDC_CM_NUM_SUPPORTED
} XVIDC_CUSTOM_MODES;

/* Assign Mode ID Enumeration. First entry Must be > XVIDC_VM_CUSTOM */
/* Create entry for each mode in the custom table */
const XVidC_VideoTimingMode XVidC_MyVideoTimingMode_vpss[(XVIDC_CM_NUM_SUPPORTED - (XVIDC_VM_CUSTOM + 1))] =
{
    { XVIDC_VM_1280x3840_30_P, "1280x3840@30Hz", XVIDC_FR_30HZ,
        {1280, 110, 40, 220, 1650, 1,
        		3840, 5, 5, 20, 3870, 0, 0, 0, 0, 1} },
	{ XVIDC_VM_3840x1280_30_P, "3840x1280@30Hz", XVIDC_FR_30HZ,
		{3840, 5, 5, 20, 3870, 1,
				1280, 110, 40, 220, 1650, 0, 0, 0, 0, 1} },
	{ XVIDC_VM_3840x1280_60_P, "3840x1280@60Hz", XVIDC_FR_60HZ,
		{3840, 5, 5, 20, 3870, 1,
				1280, 110, 40, 220, 1650, 0, 0, 0, 0, 1} },
    { XVIDC_VM_320x960_30_P, "320x960@30Hz", XVIDC_FR_30HZ,
        {320, 10, 10, 10, 350, 1,
        		960, 5, 5, 10, 980, 0, 0, 0, 0, 1} },
	{ XVIDC_VM_960x1080_30_P, "960x1080@30Hz", XVIDC_FR_30HZ,
		{960, 10, 10, 10, 990, 1,
				1080, 5, 5, 10, 1100, 0, 0, 0, 0, 1} },
	{ XVIDC_VM_1920x2560_30_P, "1920x2560@30Hz", XVIDC_FR_30HZ,
		{1920, 10, 10, 10, 1950, 1,
				2560, 20,20, 40, 2640, 0, 0, 0, 0, 1} },
	{ XVIDC_VM_1920x2560_60_P, "1920x2560@60Hz", XVIDC_FR_60HZ,
		{1920, 10, 10, 10, 1950, 1,
				2560, 20,20, 40, 2640, 0, 0, 0, 0, 1} },
	{ XVIDC_VM_1920x1280_30_P, "1920x1280@30Hz", XVIDC_FR_30HZ,
		{1920, 10, 10, 10, 1950, 1,
				1280, 5, 5, 80, 1370, 0, 0, 0, 0, 1} },
	{ XVIDC_VM_1920x1280_60_P, "1920x1280@60Hz", XVIDC_FR_60HZ,
		{1920, 10, 10, 10, 1950, 1,
				1280, 5, 5, 80, 1370, 0, 0, 0, 0, 1} },
	{ XVIDC_VM_1600x2560_30_P, "1600x2560@30Hz", XVIDC_FR_30HZ,
		{1600, 10, 10, 10, 1630, 1,
				2560, 5, 5, 10, 2580, 0, 0, 0, 0, 1} },
	{ XVIDC_VM_670x2160_30_P, "670x2160@30Hz", XVIDC_FR_30HZ,
		{670, 10, 10, 10, 700, 1,
				2160, 5, 5, 10, 2180, 0, 0, 0, 0, 1} },
	{ XVIDC_VM_4320x720_60_ZXD_A, "4320x720@60Hz", XVIDC_FR_60HZ,
		{4320, 144, 72, 1, 4537, 1,
		720, 20, 2, 20, 762, 0, 0, 0, 0, 1}},
	{ XVIDC_VM_3840x720_60, "3840x720@60Hz", XVIDC_FR_60HZ,
		{3840, 5, 5, 20, 3870, 1,
		720, 20, 2, 20, 762, 0, 0, 0, 0, 1}},
	{ XVIDC_VM_2560x1440_30_P, "2560x1440@30Hz", XVIDC_FR_30HZ,
		{2560, 10, 10, 20, 2600, 1,
		1440, 5, 5, 10, 1460, 0, 0, 0, 0, 1}},
	{ XVIDC_VM_1920x3240_30_P, "1920x3240@30Hz", XVIDC_FR_30HZ,
		{1920, 10, 10, 20, 1960, 1,
		3240, 5, 5, 10, 3260, 0, 0, 0, 0, 1}},
	{ XVIDC_VM_1920x2160_30_P, "1920x2160@30Hz", XVIDC_FR_30HZ,
		{1920, 10, 10, 20, 1960, 1,
		2160, 5, 5, 10, 2180, 0, 0, 0, 0, 1}},
	{ XVIDC_VM_1280x2160_30_P, "1280x2160@30Hz", XVIDC_FR_30HZ,
		{1280, 10, 10, 20, 1320, 1,
		2160, 5, 5, 10, 2180, 0, 0, 0, 0, 1}},
	{ XVIDC_VM_1280x960_30_P, "1280x960@30Hz", XVIDC_FR_30HZ,
		{1280, 10, 10, 20, 1320, 1,
		960, 5, 5, 10, 980, 0, 0, 0, 0, 1}},
};

#endif

/*
 * scaler only
 */
int vpss_config_scaler_0(u32 DeviceId)
{
	u32 Status;
	XVprocSs VprocInst;
	XVprocSs_Config *VprocCfgPtr;
	XVidC_VideoTiming *TimingPtr;
	XVidC_VideoMode resId;
	XVidC_VideoStream StreamIn, StreamOut;

	XVidC_FrameRate fpsIn = XVIDC_FR_30HZ;
	XVidC_FrameRate fpsOut = XVIDC_FR_30HZ;
	const XVidC_ColorFormat colorFmtIn = XVIDC_CSF_RGB;
	const XVidC_ColorFormat colorFmtOut = XVIDC_CSF_RGB;

    /* User registers custom timing table */
	//xil_printf("INFO> Registering Custom Timing Table with %d entries \r\n", (XVIDC_CM_NUM_SUPPORTED - (XVIDC_VM_CUSTOM + 1)));
//	Status = XVidC_RegisterCustomTimingModes(XVidC_MyVideoTimingMode, (XVIDC_CM_NUM_SUPPORTED - (XVIDC_VM_CUSTOM + 1)));
	Status = XVidC_RegisterCustomTimingModes(XVidC_MyVideoTimingMode_vpss, (XVIDC_CM_NUM_SUPPORTED - (XVIDC_VM_CUSTOM + 1)));
	if (Status != XST_SUCCESS) {
	  xil_printf("vpss_config_scaler ERR: Unable to register custom timing table\r\n\r\n");
	}

    /* VPSS Configuration*/
	memset(&VprocInst, 0, sizeof(XVprocSs));

    VprocCfgPtr = XVprocSs_LookupConfig(DeviceId);


    //XVprocSs_LogReset(&VprocInst);

    Status = XVprocSs_CfgInitialize(&VprocInst, VprocCfgPtr, VprocCfgPtr->BaseAddress);
    if(Status != XST_SUCCESS)
	{
    	 xil_printf("XVprocSs_CfgInitialize faild!\r\n\r\n");
		while(1)
			NOP();
	}
    //Get the resolution details
	resId = XVidC_GetVideoModeId(1280, 960, fpsIn, 0);
    xil_printf("vpss fpsIn XVidC_GetVideoModeId is %d\r\n",resId);
    TimingPtr = XVidC_GetTimingInfo(resId);

    //Set the input stream
    StreamIn.VmId           = resId;
    StreamIn.Timing         = *TimingPtr;
    StreamIn.ColorFormatId  = colorFmtIn;
    StreamIn.ColorDepth     = VprocCfgPtr->ColorDepth;
    StreamIn.PixPerClk      = VprocCfgPtr->PixPerClock;
    StreamIn.FrameRate      = fpsIn;
    StreamIn.IsInterlaced   = 0;
    XVprocSs_SetVidStreamIn(&VprocInst, &StreamIn);


	resId = XVidC_GetVideoModeId(1280, 960, fpsOut, 0);
    xil_printf("vpss fpsOut XVidC_GetVideoModeId is %d\r\n",resId);
    TimingPtr = XVidC_GetTimingInfo(resId);
    //Set the output stream
    StreamOut.VmId           = resId;
    StreamOut.Timing         = *TimingPtr;
    StreamOut.ColorFormatId  = colorFmtOut;
    StreamOut.ColorDepth     = VprocCfgPtr->ColorDepth;
    StreamOut.PixPerClk      = VprocCfgPtr->PixPerClock;
    StreamOut.FrameRate      = fpsOut;
    StreamOut.IsInterlaced   = 0;
    XVprocSs_SetVidStreamOut(&VprocInst, &StreamOut);

    Status = XVprocSs_SetSubsystemConfig(&VprocInst);
    if(Status!= XST_SUCCESS)
    {
            xil_printf("VPSS0 failed,Status is %d\r\n",Status);

    }
    xil_printf("vpss_config_scaler Started\r\n");
    return Status;
}


int vpss_config_scaler_1(u32 DeviceId)
{
	u32 Status;
	XVprocSs VprocInst;
	XVprocSs_Config *VprocCfgPtr;
	XVidC_VideoTiming *TimingPtr;
	XVidC_VideoMode resId;
	XVidC_VideoStream StreamIn, StreamOut;

	XVidC_FrameRate fpsIn = XVIDC_FR_30HZ;
	XVidC_FrameRate fpsOut = XVIDC_FR_30HZ;
	const XVidC_ColorFormat colorFmtIn = XVIDC_CSF_RGB;
	const XVidC_ColorFormat colorFmtOut = XVIDC_CSF_RGB;

    /* User registers custom timing table */
	//xil_printf("INFO> Registering Custom Timing Table with %d entries \r\n", (XVIDC_CM_NUM_SUPPORTED - (XVIDC_VM_CUSTOM + 1)));
//	Status = XVidC_RegisterCustomTimingModes(XVidC_MyVideoTimingMode, (XVIDC_CM_NUM_SUPPORTED - (XVIDC_VM_CUSTOM + 1)));
	Status = XVidC_RegisterCustomTimingModes(XVidC_MyVideoTimingMode_vpss, (XVIDC_CM_NUM_SUPPORTED - (XVIDC_VM_CUSTOM + 1)));
	if (Status != XST_SUCCESS) {
	  xil_printf("vpss_config_scaler ERR: Unable to register custom timing table\r\n\r\n");
	}

    /* VPSS Configuration*/
	memset(&VprocInst, 0, sizeof(XVprocSs));

    VprocCfgPtr = XVprocSs_LookupConfig(DeviceId);


    //XVprocSs_LogReset(&VprocInst);

    Status = XVprocSs_CfgInitialize(&VprocInst, VprocCfgPtr, VprocCfgPtr->BaseAddress);
    if(Status != XST_SUCCESS)
	{
    	 xil_printf("XVprocSs_CfgInitialize faild!\r\n\r\n");
		while(1)
			NOP();
	}
    //Get the resolution details
    resId = XVidC_GetVideoModeId(1280, 3840, fpsIn, 0);
    xil_printf("vpss fpsIn XVidC_GetVideoModeId is %d\r\n",resId);
    TimingPtr = XVidC_GetTimingInfo(resId);

    //Set the input stream
    StreamIn.VmId           = resId;
    StreamIn.Timing         = *TimingPtr;
    StreamIn.ColorFormatId  = colorFmtIn;
    StreamIn.ColorDepth     = VprocCfgPtr->ColorDepth;
    StreamIn.PixPerClk      = VprocCfgPtr->PixPerClock;
    StreamIn.FrameRate      = fpsIn;
    StreamIn.IsInterlaced   = 0;
    XVprocSs_SetVidStreamIn(&VprocInst, &StreamIn);


    resId = XVidC_GetVideoModeId(1280, 2160, fpsOut, 0);
    xil_printf("vpss fpsOut XVidC_GetVideoModeId is %d\r\n",resId);
    TimingPtr = XVidC_GetTimingInfo(resId);
    //Set the output stream
    StreamOut.VmId           = resId;
    StreamOut.Timing         = *TimingPtr;
    StreamOut.ColorFormatId  = colorFmtOut;
    StreamOut.ColorDepth     = VprocCfgPtr->ColorDepth;
    StreamOut.PixPerClk      = VprocCfgPtr->PixPerClock;
    StreamOut.FrameRate      = fpsOut;
    StreamOut.IsInterlaced   = 0;
    XVprocSs_SetVidStreamOut(&VprocInst, &StreamOut);

    Status = XVprocSs_SetSubsystemConfig(&VprocInst);
    if(Status!= XST_SUCCESS)
    {
            xil_printf("VPSS0 failed,Status is %d\r\n",Status);

    }
    xil_printf("vpss_config_scaler Started\r\n");
    return Status;
}


/*
 * Color Space Conversion
 */
static XVprocSs s_vproc[2];
static u8 s_vproc_ready[2] = {0, 0};
static u8 s_custom_timing_ready = 0;

void vpss_invalidate(void)
{
	s_vproc_ready[0] = 0U;
	s_vproc_ready[1] = 0U;
}

int vpss_config_csc_timing(u32 DeviceId, u32 width, u32 height, u8 fps,
			   u32 h_fp, u32 h_sw, u32 h_bp, u32 h_pol,
			   u32 v_fp, u32 v_sw, u32 v_bp, u32 v_pol)
{
	u32 Status;
	XVprocSs *VprocInst;
	XVprocSs_Config *VprocCfgPtr;
	XVidC_VideoTiming *TimingPtr;
	XVidC_VideoTiming CustomTiming;
	XVidC_VideoMode resId;
	XVidC_VideoStream StreamIn, StreamOut;
	XVidC_FrameRate frameRate;
	u8 idx;

	if (DeviceId > 1U)
	{
		return XST_FAILURE;
	}
	idx = (u8)DeviceId;
	VprocInst = &s_vproc[idx];
	frameRate = (fps >= 60U) ? XVIDC_FR_60HZ : XVIDC_FR_30HZ;

	if (s_custom_timing_ready == 0U)
	{
		Status = XVidC_RegisterCustomTimingModes(
			XVidC_MyVideoTimingMode_vpss,
			(XVIDC_CM_NUM_SUPPORTED - (XVIDC_VM_CUSTOM + 1)));
		if (Status == XST_SUCCESS)
		{
			s_custom_timing_ready = 1U;
		}
	}

	VprocCfgPtr = XVprocSs_LookupConfig(DeviceId);
	if (s_vproc_ready[idx] == 0U)
	{
		memset(VprocInst, 0, sizeof(XVprocSs));
		Status = XVprocSs_CfgInitialize(VprocInst, VprocCfgPtr,
						VprocCfgPtr->BaseAddress);
		if (Status != XST_SUCCESS)
		{
			xil_printf("vpss_csc: init fail\r\n");
			return Status;
		}
		s_vproc_ready[idx] = 1U;
	}

	if ((width > 65535U) || (height > 65535U) ||
	    ((width + h_fp + h_sw + h_bp) > 65535U) ||
	    ((height + v_fp + v_sw + v_bp) > 65535U))
	{
		return XST_FAILURE;
	}

	resId = XVidC_GetVideoModeId(width, height, frameRate, 0);
	/*
	 * Always program Timing from inject blanking so VPSS matches VTC
	 * (including HBP pad after clk snap). Keep VmId when library knows
	 * the mode; otherwise CUSTOM.
	 */
	memset(&CustomTiming, 0, sizeof(CustomTiming));
	CustomTiming.HActive = (u16)width;
	CustomTiming.HFrontPorch = (u16)h_fp;
	CustomTiming.HSyncWidth = (u16)h_sw;
	CustomTiming.HBackPorch = (u16)h_bp;
	CustomTiming.HTotal = (u16)(width + h_fp + h_sw + h_bp);
	CustomTiming.HSyncPolarity = (u8)h_pol;
	CustomTiming.VActive = (u16)height;
	CustomTiming.F0PVFrontPorch = (u16)v_fp;
	CustomTiming.F0PVSyncWidth = (u16)v_sw;
	CustomTiming.F0PVBackPorch = (u16)v_bp;
	CustomTiming.F0PVTotal = (u16)(height + v_fp + v_sw + v_bp);
	CustomTiming.VSyncPolarity = (u8)v_pol;
	TimingPtr = &CustomTiming;
	if (resId == XVIDC_VM_NOT_SUPPORTED)
	{
		resId = XVIDC_VM_CUSTOM;
	}

	memset(&StreamIn, 0, sizeof(StreamIn));
	StreamIn.VmId = resId;
	StreamIn.Timing = *TimingPtr;
	StreamIn.ColorFormatId = XVIDC_CSF_RGB;
	StreamIn.ColorDepth = VprocCfgPtr->ColorDepth;
	StreamIn.PixPerClk = VprocCfgPtr->PixPerClock;
	StreamIn.FrameRate = frameRate;
	XVprocSs_SetVidStreamIn(VprocInst, &StreamIn);

	StreamOut = StreamIn;
	StreamOut.ColorFormatId = XVIDC_CSF_YCRCB_422;
	XVprocSs_SetVidStreamOut(VprocInst, &StreamOut);

	Status = XVprocSs_SetSubsystemConfig(VprocInst);
	if (Status != XST_SUCCESS)
	{
		xil_printf("vpss_csc fail %d (%ux%u@%u)\r\n",
			   Status, width, height, (unsigned)fps);
	}
	return Status;
}

int vpss_config_csc(u32 DeviceId, u32 width, u32 height, u8 fps)
{
	return vpss_config_csc_timing(DeviceId, width, height, fps,
				      88U, 44U, 148U, 1U, 4U, 5U, 36U, 1U);
}

int vpss_config_color_Space_Conversion(u32 DeviceId)
{
	if (DeviceId == 0U)
	{
		return vpss_config_csc(DeviceId, 1920, 1080, 60);
	}
	return vpss_config_csc(DeviceId, 1920, 2160, 30);
}


#endif
