
// *****************************************************
// Dependencies
// *****************************************************
#include "../bsp.h"

#if defined (XPAR_XVTC_NUM_INSTANCES)

#if (XPAR_XVTC_NUM_INSTANCES >= 1U)
XVtc        VtcInst0;
#endif // XPAR_XVTC_NUM_INSTANCES == 1U
#if (XPAR_XVTC_NUM_INSTANCES >= 2U)
XVtc        VtcInst1;
#endif // XPAR_XVTC_NUM_INSTANCES == 2U
#if (XPAR_XVTC_NUM_INSTANCES >= 3U)
XVtc        VtcInst2;
#endif // XPAR_XVTC_NUM_INSTANCES == 3U
#if (XPAR_XVTC_NUM_INSTANCES >= 4U)
XVtc        VtcInst3;
#endif // XPAR_XVTC_NUM_INSTANCES == 4U


vres_timing_t vres_resolutions[VIDEO_RESOLUTION_NO_BUTT] = {
   // name     	vav,  vfp,  vsw,  vbp,  vsp,  hav,  hfp,  hsw,  hbp,  hsp
   { "VGA",    	480,   10,    2,   33,    0,  640,   16,   96,   48,    0 }, // VIDEO_RESOLUTION_VGA
   { "480P",   	480,    9,    6,   30,    0,  720,   16,   62,   60,    0 }, // VIDEO_RESOLUTION_480P
   { "576P",   	576,    5,    5,   39,    0,  720,   12,   64,   68,    0 }, // VIDEO_RESOLUTION_576P
   { "SVGA",   	600,    1,    4,   23,    1,  800,   40,  128,   88,    1 }, // VIDEO_RESOLUTION_SVGA
   { "XGA",    	768,    3,    6,   29,    0, 1024,   24,  136,  160,    0 }, // VIDEO_RESOLUTION_XGA
   { "720P",   	720,    5,    5,   20,    1, 1280,  110,   40,  220,    1 }, // VIDEO_RESOLUTION_720P
   { "SXGA",   1024,    1,    3,   26,    0, 1280,   48,  184,  200,    0 }, // VIDEO_RESOLUTION_SXGA
   { "1080P",  1080,    4,    5,   36,    1, 1920,   88,   44,  148,    1 }, // VIDEO_RESOLUTION_1080P
   { "FHDPLUS",1280,    5,   10,   80,    1, 1920,   48,   32,   80,    1 }, // VIDEO_RESOLUTION_FHDPLUS
   { "UXGA",   1200,    1,    3,   46,    0, 1600,   64,  192,  304,    0 }, // VIDEO_RESOLUTION_UXGA
  {"1920X960",  960,   10,   20,   10,    0, 1920,   20,   40,   20,    0 }, // VIDEO_RESOLUTION_1920X960
   { "4K",     2160,    8,   10,   72,    1, 3840,  176,   88,  296,    1 }, // VIDEO_RESOLUTION_4K
   { "4KFULL", 2160,    8,   10,   72,    1, 4096,   88,   88,  128,    1 }, // VIDEO_RESOLUTION_4KW
   { "CUSTOM1",1080,   24,    2,   10,    1, 1920,   40,   40,   80,    1 }, // VIDEO_RESOLUTION_CUSTOM1
   { "4KCUST", 2160,    3,    5,   18,    1, 3840,   48,   32,   80,    1 }, // VIDEO_RESOLUTION_4KCUST
   { "BAIDUSF",1536,    8,    8,    8,    1, 3840,    5,    5,    5,    1 }, //VIDEO_RESOLUTION_BAIDUSF
   { "1920X720", 720,    5,    5,   20,    1, 1920,   88,   44,  148,    1 } // VIDEO_RESOLUTION_720P
};

char *vres_get_name(u32 resolutionId)
{
   if ( resolutionId < VIDEO_RESOLUTION_NO_BUTT )
   {
      return vres_resolutions[resolutionId].pName;
   }
   else
   {
      return "{UNKNOWN}";
   }
}

u32 vres_get_width(u32 resolutionId)
{
   return vres_resolutions[resolutionId].HActiveVideo; // horizontal active
}

u32 vres_get_height(u32 resolutionId)
{
   return vres_resolutions[resolutionId].VActiveVideo; // vertical active
}

u32 vres_get_timing(u32 ResolutionId, vres_timing_t *pTiming )
{
	pTiming->pName         = vres_resolutions[ResolutionId].pName;
	pTiming->HActiveVideo  = vres_resolutions[ResolutionId].HActiveVideo;
	pTiming->HFrontPorch   = vres_resolutions[ResolutionId].HFrontPorch;
	pTiming->HSyncWidth    = vres_resolutions[ResolutionId].HSyncWidth;
	pTiming->HBackPorch    = vres_resolutions[ResolutionId].HBackPorch;
	pTiming->HSyncPolarity = vres_resolutions[ResolutionId].HSyncPolarity;
	pTiming->VActiveVideo  = vres_resolutions[ResolutionId].VActiveVideo;
	pTiming->VFrontPorch   = vres_resolutions[ResolutionId].VFrontPorch;
	pTiming->VSyncWidth    = vres_resolutions[ResolutionId].VSyncWidth;
	pTiming->VBackPorch    = vres_resolutions[ResolutionId].VBackPorch;
	pTiming->VSyncPolarity = vres_resolutions[ResolutionId].VSyncPolarity;

#if defined (VTC_DBG)
	bsp_printf("vres_get_timing(%d,...)\n\r", ResolutionId );
	bsp_printf("\tpName = %s\n\r", pTiming->pName );
	bsp_printf("\tHActiveVideo = %d\n\r", pTiming->HActiveVideo );
	bsp_printf("\tHFrontPorch = %d\n\r", pTiming->HFrontPorch );
	bsp_printf("\tHSyncWidth = %d\n\r", pTiming->HSyncWidth );
	bsp_printf("\tHBackPorch = %d\n\r", pTiming->HBackPorch );
	bsp_printf("\tHSyncPolarity = %d\n\r", pTiming->HSyncPolarity );
	bsp_printf("\tVActiveVideo = %d\n\r", pTiming->VActiveVideo );
	bsp_printf("\tVFrontPorch = %d\n\r", pTiming->VFrontPorch );
	bsp_printf("\tVSyncWidth = %d\n\r", pTiming->VSyncWidth );
	bsp_printf("\tVBackPorch = %d\n\r", pTiming->VBackPorch );
	bsp_printf("\tVSyncPolarity = %d\n\r", pTiming->VSyncPolarity );
#endif

	return 0;
}
/////////////////////////////////////////////////////////////////////////
s32 vres_detect( u32 width, u32 height )
{
  s32 i;
  s32 resolution = -1;

  for ( i = 0; i < VIDEO_RESOLUTION_NO_BUTT; i++ )
  {
     if ( width == vres_get_width(i) && height == vres_get_height(i) )
     {
#if defined (VTC_DBG)
        bsp_printf( "Detected Video Resolution = %s\r\n", vres_get_name(i) );
#endif
        resolution = i;
        break;
     }
  }
  return resolution;
}

// *****************************************************
// Public functions
// *****************************************************



void vtiming_gen_run
(
	XVtc* p_vtg_inst,
	int   resolution_id,
	int   verbose
)
{
	// Local variables
	vres_timing_t     VideoTiming;
	int               HFrontPorch;
	int               HSyncWidth;
	int               HSyncPol;
	int               HBackPorch;
	int               VFrontPorch;
	int               VSyncWidth;
	int               VSyncPol;
	int               VBackPorch;
	int               LineWidth;
	int               FrameHeight;
	XVtc_Signal       Signal;
	XVtc_Polarity     Polarity;
	XVtc_SourceSelect SourceSelect;

	// Get Video Resolution timing
    if (verbose)
    {
		bsp_printf("\tVideo Resolution = %s\n\r", vres_get_name(resolution_id));
	}
	vres_get_timing(resolution_id, &VideoTiming);
	HFrontPorch = VideoTiming.HFrontPorch;
	HSyncWidth  = VideoTiming.HSyncWidth;
	HSyncPol    = VideoTiming.HSyncPolarity;
	HBackPorch  = VideoTiming.HBackPorch;
	VFrontPorch = VideoTiming.VFrontPorch;
	VSyncWidth  = VideoTiming.VSyncWidth;
	VSyncPol    = VideoTiming.VSyncPolarity;
	VBackPorch  = VideoTiming.VBackPorch;
	LineWidth   = VideoTiming.HActiveVideo;
	FrameHeight = VideoTiming.VActiveVideo;

	// Disable/Reset VTC
	XVtc_Disable(p_vtg_inst);
	XVtc_Reset(p_vtg_inst);

    // Set up Polarity of all outputs
	memset((void *)&Polarity, 0, sizeof(Polarity));
	Polarity.ActiveChromaPol = 1;
	Polarity.ActiveVideoPol  = 1;
	Polarity.FieldIdPol      = 0;
	Polarity.VBlankPol       = 1;
	Polarity.VSyncPol        = VSyncPol;
	Polarity.HBlankPol       = 1;
	Polarity.HSyncPol        = HSyncPol;

	// Set up Generator
	memset((void *)&Signal, 0, sizeof(XVtc_Signal));
	Signal.OriginMode        = 1; //Set Frame Origin to Start of Active Video
	Signal.HTotal            = HFrontPorch + HSyncWidth + HBackPorch + LineWidth;
	Signal.HActiveStart      = 0;
	Signal.HFrontPorchStart  = LineWidth;
	Signal.HSyncStart        = LineWidth + HFrontPorch;
	Signal.HBackPorchStart   = LineWidth + HFrontPorch + HSyncWidth;
	Signal.V0Total           = FrameHeight + VFrontPorch + VSyncWidth + VBackPorch;
	Signal.V0ChromaStart     = 0;
	Signal.V0ActiveStart     = 0;
	Signal.V0FrontPorchStart = FrameHeight;
	Signal.V0SyncStart       = FrameHeight + VFrontPorch;
	Signal.V0BackPorchStart  = FrameHeight + VFrontPorch + VSyncWidth;

	// Set up source select
	memset((void *)&SourceSelect, 0, sizeof(SourceSelect));
	SourceSelect.VBlankPolSrc       = 1;
	SourceSelect.VSyncPolSrc        = 1;
	SourceSelect.HBlankPolSrc       = 1;
	SourceSelect.HSyncPolSrc        = 1;
	SourceSelect.ActiveVideoPolSrc  = 1;
	SourceSelect.ActiveChromaPolSrc = 1;
	SourceSelect.VChromaSrc         = 1;
	SourceSelect.VActiveSrc         = 1;
	SourceSelect.VBackPorchSrc      = 1;
	SourceSelect.VSyncSrc           = 1;
	SourceSelect.VFrontPorchSrc     = 1;
	SourceSelect.VTotalSrc          = 1;
	SourceSelect.HActiveSrc         = 1;
	SourceSelect.HBackPorchSrc      = 1;
	SourceSelect.HSyncSrc           = 1;
	SourceSelect.HFrontPorchSrc     = 1;
	SourceSelect.HTotalSrc          = 1;

	if ( verbose == 1 )
	{
		bsp_printf("\tVTC Generator Configuration\n\r" );
		bsp_printf("\t\tHorizontal Timing:\n\r" );
		bsp_printf("\t\t\tHActiveStart = %d\r\n", Signal.HActiveStart);
		bsp_printf("\t\t\tHFrontPorchStart %d\r\n", Signal.HFrontPorchStart);
		bsp_printf("\t\t\tHSyncStart %d\r\n", Signal.HSyncStart);
		bsp_printf("\t\t\tHBackPorchStart %d\r\n", Signal.HBackPorchStart);
		bsp_printf("\t\t\tHTotal = %d\r\n", Signal.HTotal);
		bsp_printf("\t\tVertical Timing:\n\r" );
		bsp_printf("\t\t\tV0ActiveStart %d\r\n", Signal.V0ActiveStart);
		bsp_printf("\t\t\tV0FrontPorchStart %d\r\n", Signal.V0FrontPorchStart);
		bsp_printf("\t\t\tV0SyncStart %d\r\n", Signal.V0SyncStart);
		bsp_printf("\t\t\tV0BackPorchStart %d\r\n", Signal.V0BackPorchStart);
		bsp_printf("\t\t\tV0Total %d\r\n", Signal.V0Total);
	}

	// Write configuration to hardware
	XVtc_SetPolarity(p_vtg_inst, &Polarity);
	XVtc_SetGenerator(p_vtg_inst, &Signal);
	XVtc_SetSource(p_vtg_inst, &SourceSelect);

	// Enable the generator module
	XVtc_DisableSync(p_vtg_inst);
	XVtc_Enable(p_vtg_inst);
	XVtc_RegUpdate(p_vtg_inst);
}


int vtc_init(XVtc *InstancePtr, u16 DeviceId)
{
	int Status;
	XVtc_Config *VtcConfig;

	VtcConfig = XVtc_LookupConfig(DeviceId);
    Status = XVtc_CfgInitialize(InstancePtr, VtcConfig, VtcConfig->BaseAddress);
    if(Status != XST_SUCCESS)
    {
    	bsp_printf(TXT_RED "In %s: Vtc %d Initialization failed...\r\n" TXT_RST, __func__, DeviceId);
        return(XST_FAILURE);
    }

    return XST_SUCCESS;
}

int vtc_config(void)
{
	int Status;

	Status = vtc_init(&VtcInst0, XPAR_VTC_0_DEVICE_ID);
	if(Status != XST_SUCCESS)
    {
        //Xil_Assert(__FILE__, __LINE__);
        return(XST_FAILURE);
    }

	xil_printf("VTC0 Initialization ok\r\n");
	#if defined (R1080)
	vtiming_gen_run(&VtcInst0, VIDEO_RESOLUTION_1080P, 0);
	#elif defined (R4K)
	vtiming_gen_run(&VtcInst0, VIDEO_RESOLUTION_4K, 0);
	#else
	vtiming_gen_run(&VtcInst0, VIDEO_RESOLUTION_4K, 0);
	#endif

    bsp_printf("VTC 0 config done!\r\n");

    return XST_SUCCESS;
}


#endif // XPAR_XVTC_NUM_INSTANCES
