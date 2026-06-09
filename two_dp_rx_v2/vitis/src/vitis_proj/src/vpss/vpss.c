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
	XVIDC_VM_480x320_60_P,

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
	{ XVIDC_VM_480x320_60_P, "480x320@60Hz", XVIDC_FR_60HZ,
		{480, 5, 5, 10, 500, 1,
		320, 5, 5, 10, 340, 0, 0, 0, 0, 1}}
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

	XVidC_FrameRate fpsIn = XVIDC_FR_60HZ;
	XVidC_FrameRate fpsOut = XVIDC_FR_60HZ;
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
	resId = XVidC_GetVideoModeId(1920, 1080, fpsIn, 0);
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


	resId = XVidC_GetVideoModeId(480, 320, fpsOut, 0);
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

	XVidC_FrameRate fpsIn = XVIDC_FR_60HZ;
	XVidC_FrameRate fpsOut = XVIDC_FR_60HZ;
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
    resId = XVidC_GetVideoModeId(1920, 1080, fpsIn, 0);
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


    resId = XVidC_GetVideoModeId(480, 320, fpsOut, 0);
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
            xil_printf("VPSS1 failed,Status is %d\r\n",Status);

    }
    xil_printf("vpss_config_scaler Started\r\n");
    return Status;
}


/*
 * Color Space Conversion
 */
int vpss_config_color_Space_Conversion(u32 DeviceId)
{
	u32 Status;
	XVprocSs VprocInst;
	XVprocSs_Config *VprocCfgPtr;
	XVidC_VideoTiming *TimingPtr;
	XVidC_VideoMode resId;
	XVidC_VideoStream StreamIn, StreamOut;

	XVidC_FrameRate fpsIn = XVIDC_FR_60HZ;
	XVidC_FrameRate fpsOut = XVIDC_FR_60HZ;
	const XVidC_ColorFormat colorFmtIn = XVIDC_CSF_RGB;
	const XVidC_ColorFormat colorFmtOut = XVIDC_CSF_YCRCB_422;

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
		while(1)
			NOP();
	}
	//Get the resolution details
//	resId = XVidC_GetVideoModeId(3840, 2160, fpsIn, 0);
	resId = XVidC_GetVideoModeId(1920, 1080, fpsIn, 0);
	xil_printf("XVidC_GetVideoModeId is %d\r\n",resId);
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


//	resId = XVidC_GetVideoModeId(3840, 2160, fpsOut, 0);
	resId = XVidC_GetVideoModeId(1920, 1080, fpsOut, 0);
	xil_printf("XVidC_GetVideoModeId is %d\r\n",resId);
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
	xil_printf("vpss_config_color_Space_Conversion Started\r\n");
	return Status;
}


#endif
