#include "../bsp.h"

#if defined (XPAR_XVTC_NUM_INSTANCES)

#if (XPAR_XVTC_NUM_INSTANCES >= 1U)
XVtc VtcInst0;
#endif
#if (XPAR_XVTC_NUM_INSTANCES >= 2U)
XVtc VtcInst1;
#endif

vres_timing_t vres_resolutions[VIDEO_RESOLUTION_NO_BUTT] = {
   /* name       vav,  vfp, vsw, vbp, vsp,  hav,  hfp, hsw,  hbp, hsp */
   { "VGA",      480,   10,   2,  33,   0,  640,   16,  96,   48,   0 },
   { "480P",     480,    9,   6,  30,   0,  720,   16,  62,   60,   0 },
   { "576P",     576,    5,   5,  39,   0,  720,   12,  64,   68,   0 },
   { "SVGA",     600,    1,   4,  23,   1,  800,   40, 128,   88,   1 },
   { "XGA",      768,    3,   6,  29,   0, 1024,   24, 136,  160,   0 },
   { "720P",     720,    5,   5,  20,   1, 1280,  110,  40,  220,   1 },
   { "SXGA",    1024,    1,   3,  26,   0, 1280,   48, 184,  200,   0 },
   { "1080P",   1080,    4,   5,  36,   1, 1920,   88,  44,  148,   1 },
   { "FHDPLUS", 1280,    5,  10,  80,   1, 1920,   48,  32,   80,   1 },
   { "UXGA",    1200,    1,   3,  46,   0, 1600,   64, 192,  304,   0 },
   {"1920X960",  960,   10,  20,  10,   0, 1920,   20,  40,   20,   0 },
   { "4K",      2160,    8,  10,  72,   1, 3840,  176,  88,  296,   1 },
   { "4KFULL",  2160,    8,  10,  72,   1, 4096,   88,  88,  128,   1 },
   { "CUSTOM1", 1080,   24,   2,  10,   1, 1920,   40,  40,   80,   1 },
   { "4KCUST",  2160,    3,   5,  18,   1, 3840,   48,  32,   80,   1 },
   { "BAIDUSF", 1536,    8,   8,   8,   1, 3840,    5,   5,    5,   1 },
   {"1920X3240",3240,   10,  10,  80,   1, 1920,   48,  32,   80,   1 },
   {"1920X2160",2160,   10,  10,  70,   1, 1920,   88,  44,  148,   1 },
   {"1280X960",  960,   10,  20,  10,   1, 1280,  110,  40,  200,   1 },
};

void vtiming_gen_run_timing(XVtc *p_vtg_inst, const vres_timing_t *pTiming,
			    int verbose)
{
	XVtc_Signal Signal;
	XVtc_Polarity Polarity;
	XVtc_SourceSelect SourceSelect;
	int LineWidth;
	int FrameHeight;

	if ((p_vtg_inst == NULL) || (pTiming == NULL))
	{
		return;
	}

	LineWidth = (int)pTiming->HActiveVideo;
	FrameHeight = (int)pTiming->VActiveVideo;

	if (verbose)
	{
		bsp_printf("\tVideo Resolution = %s\n\r",
			   (pTiming->pName != NULL) ? pTiming->pName : "CUSTOM");
	}

	XVtc_Disable(p_vtg_inst);
	XVtc_Reset(p_vtg_inst);

	memset((void *)&Polarity, 0, sizeof(Polarity));
	Polarity.ActiveChromaPol = 1;
	Polarity.ActiveVideoPol = 1;
	Polarity.VBlankPol = 1;
	Polarity.HBlankPol = 1;
	Polarity.VSyncPol = (int)pTiming->VSyncPolarity;
	Polarity.HSyncPol = (int)pTiming->HSyncPolarity;

	memset((void *)&Signal, 0, sizeof(XVtc_Signal));
	Signal.OriginMode = 1;
	Signal.HTotal = LineWidth + (int)pTiming->HFrontPorch +
			(int)pTiming->HSyncWidth + (int)pTiming->HBackPorch;
	Signal.HActiveStart = 0;
	Signal.HFrontPorchStart = LineWidth;
	Signal.HSyncStart = LineWidth + (int)pTiming->HFrontPorch;
	Signal.HBackPorchStart = Signal.HSyncStart + (int)pTiming->HSyncWidth;
	Signal.V0Total = FrameHeight + (int)pTiming->VFrontPorch +
			 (int)pTiming->VSyncWidth + (int)pTiming->VBackPorch;
	Signal.V0ActiveStart = 0;
	Signal.V0FrontPorchStart = FrameHeight;
	Signal.V0SyncStart = FrameHeight + (int)pTiming->VFrontPorch;
	Signal.V0BackPorchStart = Signal.V0SyncStart + (int)pTiming->VSyncWidth;

	memset((void *)&SourceSelect, 0, sizeof(SourceSelect));
	SourceSelect.VBlankPolSrc = 1;
	SourceSelect.VSyncPolSrc = 1;
	SourceSelect.HBlankPolSrc = 1;
	SourceSelect.HSyncPolSrc = 1;
	SourceSelect.ActiveVideoPolSrc = 1;
	SourceSelect.ActiveChromaPolSrc = 1;
	SourceSelect.VChromaSrc = 1;
	SourceSelect.VActiveSrc = 1;
	SourceSelect.VBackPorchSrc = 1;
	SourceSelect.VSyncSrc = 1;
	SourceSelect.VFrontPorchSrc = 1;
	SourceSelect.VTotalSrc = 1;
	SourceSelect.HActiveSrc = 1;
	SourceSelect.HBackPorchSrc = 1;
	SourceSelect.HSyncSrc = 1;
	SourceSelect.HFrontPorchSrc = 1;
	SourceSelect.HTotalSrc = 1;

	XVtc_SetPolarity(p_vtg_inst, &Polarity);
	XVtc_SetGenerator(p_vtg_inst, &Signal);
	XVtc_SetSource(p_vtg_inst, &SourceSelect);
	XVtc_DisableSync(p_vtg_inst);
	XVtc_EnableGenerator(p_vtg_inst);
	XVtc_RegUpdate(p_vtg_inst);
	XVtc_Enable(p_vtg_inst);
	XVtc_RegUpdate(p_vtg_inst);
}

#endif /* XPAR_XVTC_NUM_INSTANCES */
