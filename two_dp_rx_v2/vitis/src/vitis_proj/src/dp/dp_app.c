#include "../bsp.h"
#if defined (XPAR_XDPRXSS_NUM_INSTANCES) || defined (XPAR_XDPTXSS_NUM_INSTANCES)

typedef struct
{
	 XVidC_VideoMode VideoMode_local;
	unsigned char user_bpc;
	unsigned char *user_pattern;
	unsigned char user_format;
	unsigned int user_numStreams;
	unsigned int user_stream_number;
	unsigned int mst_check_flag;
}user_config_struct;
extern user_config_struct user_config;


typedef struct
{
	unsigned char lane_count;
	unsigned char link_rate;
} lane_link_rate_struct;


typedef struct
{
	unsigned char LaneCount;
	unsigned char LineRate;
	unsigned char pixel;
	unsigned char bpc;
} dp_conf_struct;


typedef struct
{
	u8 sec_id;//DP Specific
	u8 type;
	u8 version;
	u8 length;
	u8 audio_coding_type;
	u8 audio_channel_count;
	u8 sampling_frequency;
	u8 sample_size;
	u8 level_shift;
	u8 downmix_inhibit;
	u8 channel_allocation;
	u16 info_length;
	u8 frame_count;
} XilAudioInfoFrame;




/*The structure defines Generic Frame Packet fields*/
typedef struct
{
	u32 frame_count;
	u32 frame_count_q;
	u8 Header[4];
	u8 Payload[32];
} XilAudioExtFrame;

lane_link_rate_struct lane_link_table[]=
{
	{XDP_RX_OVER_LANE_COUNT_SET_1, XDP_RX_OVER_LINK_BW_SET_162GBPS},
	{XDP_RX_OVER_LANE_COUNT_SET_2, XDP_RX_OVER_LINK_BW_SET_162GBPS},
	{XDP_RX_OVER_LANE_COUNT_SET_4, XDP_RX_OVER_LINK_BW_SET_162GBPS},
	{XDP_RX_OVER_LANE_COUNT_SET_1, XDP_RX_OVER_LINK_BW_SET_270GBPS},
	{XDP_RX_OVER_LANE_COUNT_SET_2, XDP_RX_OVER_LINK_BW_SET_270GBPS},
	{XDP_RX_OVER_LANE_COUNT_SET_4, XDP_RX_OVER_LINK_BW_SET_270GBPS},
	{XDP_RX_OVER_LANE_COUNT_SET_1, XDP_RX_OVER_LINK_BW_SET_540GBPS},
	{XDP_RX_OVER_LANE_COUNT_SET_2, XDP_RX_OVER_LINK_BW_SET_540GBPS},
	{XDP_RX_OVER_LANE_COUNT_SET_4, XDP_RX_OVER_LINK_BW_SET_540GBPS},
	{XDP_RX_OVER_LANE_COUNT_SET_1, XDP_RX_OVER_LINK_BW_SET_810GBPS},
	{XDP_RX_OVER_LANE_COUNT_SET_2, XDP_RX_OVER_LINK_BW_SET_810GBPS},
	{XDP_RX_OVER_LANE_COUNT_SET_4, XDP_RX_OVER_LINK_BW_SET_810GBPS},

};


static XVphy_User_Config PHY_User_Config_Table[] =
{
/*	Index,         TxPLL,            RxPLL,
 *	TxChId,        RxChId,
 *	LineRate,      LineRateHz,       QPLLRefClkSrc,
 *	CPLLRefClkSrc, QPLLRefClkFreqHz, CPLLRefClkFreqHz
 * */
	{   0,  XVPHY_PLL_TYPE_CPLL,   XVPHY_PLL_TYPE_CPLL,
	XVPHY_CHANNEL_ID_CHA,     XVPHY_CHANNEL_ID_CHA,
	0x06,    XVPHY_DP_LINK_RATE_HZ_162GBPS,      ONBOARD_REF_CLK_1,
	ONBOARD_REF_CLK_1,         270000000,           270000000     },
	{   1,  XVPHY_PLL_TYPE_CPLL,   XVPHY_PLL_TYPE_CPLL,
	XVPHY_CHANNEL_ID_CHA,     XVPHY_CHANNEL_ID_CHA,
	0x0A,    XVPHY_DP_LINK_RATE_HZ_270GBPS,      ONBOARD_REF_CLK_1,
	ONBOARD_REF_CLK_1,            270000000,           270000000  },
	{   2,  XVPHY_PLL_TYPE_CPLL,   XVPHY_PLL_TYPE_CPLL,
	XVPHY_CHANNEL_ID_CHA,     XVPHY_CHANNEL_ID_CHA,
	0x14,    XVPHY_DP_LINK_RATE_HZ_540GBPS,      ONBOARD_REF_CLK_1,
	ONBOARD_REF_CLK_1,            270000000,           270000000  },
	{   3,  XVPHY_PLL_TYPE_QPLL1,  XVPHY_PLL_TYPE_CPLL,
	XVPHY_CHANNEL_ID_CMN1,    XVPHY_CHANNEL_ID_CHA,
	0x06,    XVPHY_DP_LINK_RATE_HZ_162GBPS,      ONBOARD_REF_CLK_0,
	ONBOARD_REF_CLK_1,         270000000,           270000000     },
	{   4,  XVPHY_PLL_TYPE_QPLL1,  XVPHY_PLL_TYPE_CPLL,
	XVPHY_CHANNEL_ID_CMN1,    XVPHY_CHANNEL_ID_CHA,
	0x0A,    XVPHY_DP_LINK_RATE_HZ_270GBPS,      ONBOARD_REF_CLK_1,
	ONBOARD_REF_CLK_0,        270000000,           270000000      },
	{   5,  XVPHY_PLL_TYPE_QPLL1,  XVPHY_PLL_TYPE_CPLL,
	XVPHY_CHANNEL_ID_CMN1,    XVPHY_CHANNEL_ID_CHA,
	0x1E,    XVPHY_DP_LINK_RATE_HZ_540GBPS,      ONBOARD_REF_CLK_1,
	ONBOARD_REF_CLK_0,        270000000,           270000000      },

//	{   5,  XVPHY_PLL_TYPE_QPLL1,  XVPHY_PLL_TYPE_CPLL,
//	XVPHY_CHANNEL_ID_CMN1,    XVPHY_CHANNEL_ID_CHA,
//	0x14,    XVPHY_DP_LINK_RATE_HZ_540GBPS,      ONBOARD_REF_CLK_1,
//	ONBOARD_REF_CLK_0,        270000000,           270000000      },


	{   6,  XVPHY_PLL_TYPE_CPLL,   XVPHY_PLL_TYPE_CPLL,
	XVPHY_CHANNEL_ID_CHA,     XVPHY_CHANNEL_ID_CHA,
	0x06,    XVPHY_DP_LINK_RATE_HZ_162GBPS,      ONBOARD_REF_CLK_0,
	ONBOARD_REF_CLK_0,         270000000,           270000000         },
	{   7,  XVPHY_PLL_TYPE_CPLL,   XVPHY_PLL_TYPE_CPLL,
	XVPHY_CHANNEL_ID_CHA,     XVPHY_CHANNEL_ID_CHA,
	0x0A,    XVPHY_DP_LINK_RATE_HZ_270GBPS,      ONBOARD_REF_CLK_0,
	ONBOARD_REF_CLK_0,                270000000,           270000000  },
	{   8,  XVPHY_PLL_TYPE_CPLL,   XVPHY_PLL_TYPE_CPLL,
	XVPHY_CHANNEL_ID_CHA,     XVPHY_CHANNEL_ID_CHA,
	0x14,    XVPHY_DP_LINK_RATE_HZ_540GBPS,      ONBOARD_REF_CLK_0,
	ONBOARD_REF_CLK_0,                270000000,           270000000  },
	{   9,  XVPHY_PLL_TYPE_CPLL,   XVPHY_PLL_TYPE_CPLL,
	XVPHY_CHANNEL_ID_CHA,     XVPHY_CHANNEL_ID_CHA,
	0x1E,    XVPHY_DP_LINK_RATE_HZ_810GBPS,      ONBOARD_REF_CLK_0,
	ONBOARD_REF_CLK_0,                270000000,           270000000  },
	{   10,     XVPHY_PLL_TYPE_QPLL1,  XVPHY_PLL_TYPE_CPLL,
	XVPHY_CHANNEL_ID_CMN1,    XVPHY_CHANNEL_ID_CHA,
	0x1E,    XVPHY_DP_LINK_RATE_HZ_810GBPS,      ONBOARD_REF_CLK_1,
	ONBOARD_REF_CLK_1,        270000000,           270000000      }
};


typedef unsigned int    UINT32;
typedef unsigned int    UINT8;
typedef unsigned int    UINT16;


#define TIMER_RESET_VALUE 		1000
#define TIMER_HDCP_STABLIZATION_VALUE	100000000



/* The application by default uses the EDID
 * of the downstream monitor
 * Setting this to 0 will enable an internal EDID
 * */
#define USE_MONITOR_EDID 				0

#define I2C_MCDP6000_ADDR  0x14 /**< I2C MCDP6000 Address*/


#define SET_TX_TO_2BYTE	1
#define SET_RX_TO_2BYTE	1


#define XDP_RX_CRC_CONFIG       0x074
#define XDP_RX_DPC_LINK_QUAL_CONFIG 0x454
#define XDP_RX_DPC_L01_PRBS_CNTR    0x45C
#define XDP_RX_DPC_L23_PRBS_CNTR    0x460
#define XDP_RX_DPCD_LINK_QUAL_PRBS  0x3
#define XVPHY_DRP_GTHE4_PRBS_ERR_CNTR_LOWER 0x15E
#define XVPHY_DRP_GTHE4_PRBS_ERR_CNTR_UPPER 0x15F

/* Enable this only for PHY compliance test */
#define AccessErrorCounterHandler_FLAG 0


/* VPHY Specific Defines
 */
#define XVPHY_RX_SYM_ERR_CNTR_CH1_2_REG    0x084
#define XVPHY_RX_SYM_ERR_CNTR_CH3_4_REG    0x088

#define XVPHY_DRP_CPLL_FBDIV 		0x28
#define XVPHY_DRP_CPLL_REFCLK_DIV 	0x2A
#define XVPHY_DRP_RXOUT_DIV 		0x63
#define XVPHY_DRP_RXCLK25 		0x6D
#define XVPHY_DRP_TXCLK25 		0x7A
#define XVPHY_DRP_TXOUT_DIV 		0x7C

/************************** Function Prototypes ******************************/
static void Dprx_InterruptHandlerVmChange(void *InstancePtr);
static void Dprx_InterruptHandlerNoVideo(void *InstancePtr);
static void Dprx_InterruptHandlerVBlank(void *InstancePtr);
static void Dprx_InterruptHandlerTrainingLost(void *InstancePtr);
static void Dprx_InterruptHandlerVideo(void *InstancePtr);
static void Dprx_DetectResolution(void *InstancePtr);
static void Dprx_ResetVideoOutput(void *InstancePtr);

extern u8 Edid_org[128];
extern u8 Edid1_org[128];
extern u8 Edid2_org[128];
extern u8 edid_monitor[384];
extern u8 use_monitor_edid;

extern XVidC_VideoMode resolution_table[];
extern XVidC_VideoTimingMode XVidC_MyVideoTimingMode[(XVIDC_CM_NUM_SUPPORTED - (XVIDC_VM_CUSTOM + 1))];


user_config_struct user_config;
dp_conf_struct dp_conf;


/**************************** Type Definitions *******************************/
XDp_TxMainStreamAttributes Msa;



XDp_TxAudioInfoFrame *xilInfoFrame;
XilAudioInfoFrame AudioinfoFrame;
XilAudioExtFrame  SdpExtFrame;

XTmrCtr TmrCtr;			/* Instance of the Timer/Counter */


#if XPAR_XVPHY_NUM_INSTANCES >= 1
XVphy rx_VPhy_Instance_0;
XDpRxSs DpRxSsInst_0;		/* The DPRX Subsystem instance.*/
#endif
#if XPAR_XVPHY_NUM_INSTANCES >= 2
XVphy rx_VPhy_Instance_1;
XDpRxSs DpRxSsInst_1;		/* The DPRX Subsystem instance.*/
#endif

/************************** Variable Definitions *****************************/



volatile u32 training_done;
volatile u32 vblank_count;
volatile u8 start_tracking;
volatile u8 change_detected;


u8 bpc_table[] = {6,8,10,12,16};
double max_freq[] = {216.0, 172.8, 360.0, 288.0, 720.0, 576.0};



static u32 Status;
int track_count = 0;
int track_count1 = 0;
u32 count_track = 0;
u32 bpc_track =0;
u32 recv_clk_freq_track=0;
float recv_frame_clk_track=0.0;
u32 rxMsamisc0_track;
u32 recv_frame_clk_int_track =0;

u8 LineRate_init = XDP_DPCD_LINK_BW_SET_810GBPS;
u8 LaneCount_init = XDP_DPCD_LANE_COUNT_SET_4;
#if XPAR_XVPHY_NUM_INSTANCES >= 1
u8 LineRate_init_rx_0 = XDP_DPCD_LINK_BW_SET_540GBPS;//XDP_DPCD_LINK_BW_SET_540GBPS
#endif
#if XPAR_XVPHY_NUM_INSTANCES >= 1
u8 LineRate_init_rx_1 = XDP_DPCD_LINK_BW_SET_270GBPS;//XDP_DPCD_LINK_BW_SET_270GBPS
#endif

volatile u8 prev_line_rate; 		/* This previous line rate to keep
				 * previous info to compare
				 * with new line rate request*/


void DpPt_CustomWaitUs(void *InstancePtr, u32 MicroSeconds)
{

	u32 TimerVal;
	XDp *DpInstance = (XDp *)InstancePtr;
	u32 NumTicks = (MicroSeconds * (DpInstance->Config.SAxiClkHz /
			1000000));

	XTmrCtr_Reset(DpInstance->UserTimerPtr, 0);
	XTmrCtr_Start(DpInstance->UserTimerPtr, 0);

	/* Wait specified number of useconds. */
	do {
		TimerVal = XTmrCtr_GetValue(DpInstance->UserTimerPtr, 0);
	} while (TimerVal < NumTicks);
}



/*
 * This process takes in all the MSA values and find out resolution, BPC,
 * refresh rate. Further this sets the pixel_width based on the pixel_clock and
 * lane set. This is to ensure that it matches the values in TX driver. Else
 * video cannot be passthrough. Approximation is implemented for refresh rates.
 * Sometimes a refresh rate of 60 is detected as 59
 * and vice-versa. Approximation is done for single digit.
 * */
static void Dprx_DetectResolution(void *InstancePtr)
{
    XDpRxSs *DpRxSsInst = (XDpRxSs *)InstancePtr;
    u32 baseAddr = DpRxSsInst->Config.DpSubCore.DpConfig.BaseAddr;
    u32 dpBaseAddr = DpRxSsInst->DpPtr->Config.BaseAddr;

    // 读取分辨率相关寄存器
    u32 DpHres = XDp_ReadReg(baseAddr, XDP_RX_MSA_HRES);
    u32 DpVres = XDp_ReadReg(baseAddr, XDP_RX_MSA_VHEIGHT);
    u32 DpHres_total = XDp_ReadReg(baseAddr, XDP_RX_MSA_HTOTAL);
    u32 DpVres_total = XDp_ReadReg(baseAddr, XDP_RX_MSA_VTOTAL);

    Msa.Vtm.Timing.HActive = DpHres;
    Msa.Vtm.Timing.VActive = DpVres;
    Msa.Vtm.Timing.HTotal  = DpHres_total;
    Msa.Vtm.Timing.F0PVTotal = DpVres_total;

    // 读取 M/N VID 并保存
    u32 rxMsaMVid = XDp_ReadReg(dpBaseAddr, XDP_RX_MSA_MVID) & 0x00FFFFFF;
    u32 rxMsaNVid = XDp_ReadReg(dpBaseAddr, XDP_RX_MSA_NVID) & 0x00FFFFFF;

    Msa.MVid = rxMsaMVid;
    Msa.NVid = rxMsaNVid;

    // 若源支持 HBR3（8.1Gbps）但接收端不支持，重新估算像素时钟
    if (dp_conf.LineRate == 0x1E) // 8.1Gbps
    {
        double freq_d = (810000.0 * rxMsaMVid) / rxMsaNVid;
        u32 pixel_freq = (u32)freq_d;

        // 上取整到最近的 1000Hz
        u32 mod = pixel_freq % 1000;
        if (mod != 0)
            pixel_freq += (1000 - mod);

        Msa.MVid = pixel_freq;
        Msa.NVid = 27 * 1000 * 0x14;  // 固定值
        Msa.PixelClockHz = pixel_freq;
    }

    // 同步起始位置
    Msa.HStart = XDp_ReadReg(baseAddr, XDP_RX_MSA_HSTART);
    Msa.VStart = XDp_ReadReg(baseAddr, XDP_RX_MSA_VSTART);

    // 同步脉宽与极性
    Msa.Vtm.Timing.HSyncWidth    = XDp_ReadReg(baseAddr, XDP_RX_MSA_HSWIDTH);
    Msa.Vtm.Timing.F0PVSyncWidth = XDp_ReadReg(baseAddr, XDP_RX_MSA_VSWIDTH);
    Msa.Vtm.Timing.HSyncPolarity = XDp_ReadReg(baseAddr, XDP_RX_MSA_HSPOL);
    Msa.Vtm.Timing.VSyncPolarity = XDp_ReadReg(baseAddr, XDP_RX_MSA_VSPOL);

    // 计算链路时钟频率 (MHz)
    double recv_clk_freq = ((dp_conf.LineRate * 27.0) * rxMsaMVid) / rxMsaNVid;

    // 计算接收帧率，单位 Hz，向上取整
    float recv_frame_clk = ceil((recv_clk_freq * 1e6) / (DpHres_total * DpVres_total));
    u32 recv_frame_clk_int = (u32)recv_frame_clk;

    // 帧率标准化（处理边缘值）
    if (recv_frame_clk_int == 59 || recv_frame_clk_int == 61)
        recv_frame_clk_int = 60;
    else if (recv_frame_clk_int == 29 || recv_frame_clk_int == 31)
        recv_frame_clk_int = 30;
    else if (recv_frame_clk_int == 74 || recv_frame_clk_int == 76)
        recv_frame_clk_int = 75;
    else if (recv_frame_clk_int == 119 || recv_frame_clk_int == 121)
        recv_frame_clk_int = 120;

    Msa.Vtm.FrameRate = recv_frame_clk_int;

    // 提取 MISC0 信息（bit[7:5] 用于判断 BPC）
    u32 rxMsamisc0 = (XDp_ReadReg(dpBaseAddr, XDP_RX_MSA_MISC0) >> 5) & 0x07;

    // 根据频率和 lane 数配置 pixel width
    u32 clk_Hz = (u32)(recv_clk_freq * 1e6);
    if (clk_Hz > 540000000 && dp_conf.LaneCount == 4)
    {
        XDp_RxSetUserPixelWidth(DpRxSsInst->DpPtr, 4);
        dp_conf.pixel = Msa.UserPixelWidth = 4;
    }
    else if (clk_Hz > 270000000 && dp_conf.LaneCount != 1)
    {
        XDp_RxSetUserPixelWidth(DpRxSsInst->DpPtr, 2);
        dp_conf.pixel = Msa.UserPixelWidth = 2;
    }
    else
    {
        XDp_RxSetUserPixelWidth(DpRxSsInst->DpPtr, 1);
        dp_conf.pixel = Msa.UserPixelWidth = 1;
    }

    // 设置同步时钟模式
    Msa.SynchronousClockMode = rxMsamisc0 & 0x01;

    // 色深配置
    dp_conf.bpc = bpc_table[rxMsamisc0];
    Msa.BitsPerColor = dp_conf.bpc;

    // MISC寄存器
    Msa.Misc0 = XDp_ReadReg(dpBaseAddr, XDP_RX_MSA_MISC0);
    Msa.Misc1 = XDp_ReadReg(dpBaseAddr, XDP_RX_MSA_MISC1);

    // 强制触发 line reset
    XDp_RxSetLineReset(DpRxSsInst->DpPtr, 1);

    // 打印最终检测结果
    if (training_done)
    {
        xil_printf("\r\n *** Detected resolution: %lu x %lu @ %luHz, BPC = %lu ***\n\r",
                   DpHres, DpVres, recv_frame_clk_int, dp_conf.bpc);
    }
}


static void Dprx_ResetVideoOutput(void *InstancePtr)
{
	XDpRxSs *DpRxSsInst = (XDpRxSs *)InstancePtr;
	XDp_RxDtgDis(DpRxSsInst->DpPtr);
	DpPt_CustomWaitUs(DpRxSsInst->DpPtr, 800000);
	XDp_RxDtgEn(DpRxSsInst->DpPtr);
}

static void Dprx_InterruptHandlerVmChange(void *InstancePtr)
{

	XDpRxSs *DpRxSsInst = (XDpRxSs *)InstancePtr;
	xil_printf("modify_dp_%d ",DpRxSsInst->DpPtr->Config.DeviceId);
	xil_printf("Dprx_InterruptHandlerVmChange\n\r");
	xil_printf("vblank_count%d ",vblank_count);
	xil_printf("training_done%d ",training_done);
//	if (vblank_count >= 50 && training_done == 1)
	if (vblank_count >= 4 && training_done == 1)
	{
		xil_printf("Dprx_InterruptHandlerVmChange !\r\n");
		xil_printf("*** Interrupt > Video "
						"Mode change ***\n\r");
		XDp_RxDtgDis(DpRxSsInst->DpPtr);

		start_tracking = 0;
		change_detected = 0;
	}
}

static void Dprx_InterruptHandlerNoVideo(void *InstancePtr)
{
	XDpRxSs *DpRxSsInst = (XDpRxSs *)InstancePtr;
	xil_printf("modify_dp_%d ",DpRxSsInst->DpPtr->Config.DeviceId);
	xil_printf("*** No Video detected ***\n\r");

	XDp_WriteReg(DpRxSsInst->DpPtr->Config.BaseAddr,
		     XDP_RX_VIDEO_UNSUPPORTED, 1);

	AudioinfoFrame.frame_count=0;
	XDp_RxInterruptEnable(DpRxSsInst->DpPtr,
			XDP_RX_INTERRUPT_MASK_INFO_PKT_MASK);
}

static void Dprx_InterruptHandlerVBlank(void *InstancePtr)
{
	XDpRxSs *DpRxSsInst = (XDpRxSs *)InstancePtr;
	if (DpRxSsInst->VBlankEnable == 1)
	{
		vblank_count++;
		if ((vblank_count % 20) == 0 && (vblank_count > 10))
		{
			xil_printf("*");
		}
		if (vblank_count >= 50) {
			DpRxSsInst->VBlankEnable = 0;
			XDp_RxInterruptDisable(DpRxSsInst->DpPtr,
					XDP_RX_INTERRUPT_MASK_VBLANK_MASK);

		/* when Vblank is received, HDCP is put in enabled
		 * state and the timer is started TX is not setup until
		 * the timer is done. This ensures that certain sources
		 * like MacBook gets time to Authenticate.
		 * */
		} else if (vblank_count == 20) {
			XDp_RxInterruptEnable(DpRxSsInst->DpPtr,0x80000000);
		}
	}
}

static void Dprx_InterruptHandlerVideo(void *InstancePtr)
{
	XDpRxSs *DpRxSsInst = (XDpRxSs *)InstancePtr;
	xil_printf("modify_dp_%d ",DpRxSsInst->DpPtr->Config.DeviceId);
	xil_printf("*** Interrupt > Video detected ***\n\r");
	XDp_WriteReg(DpRxSsInst->DpPtr->Config.BaseAddr, XDP_RX_VIDEO_UNSUPPORTED , 0);

}


//this is a handler in TP1
void Dprx_InterruptHandlerLinkBW(void *InstancePtr)
{

	u32 Status;

	XVphy rx_VPhy_Instance;
	XDpRxSs *DpRxSsInst = (XDpRxSs *)InstancePtr;
	xil_printf("modify_dp_%d ",DpRxSsInst->DpPtr->Config.DeviceId);
	xil_printf("Dprx_InterruptHandlerLinkBW\r\n");
	xil_printf("DpRxSsInst->UsrOpt.LinkRate = 0x%x\r\n",DpRxSsInst->UsrOpt.LinkRate);
	switch(DpRxSsInst->DpPtr->Config.DeviceId)
	{
		case 0x0: rx_VPhy_Instance = rx_VPhy_Instance_0; break;
#if XPAR_XVPHY_NUM_INSTANCES >= 2
		case 0x1: rx_VPhy_Instance = rx_VPhy_Instance_1; break;
#endif
		default: rx_VPhy_Instance = rx_VPhy_Instance_0; break;
	}
	PLLRefClkSel(&rx_VPhy_Instance, DpRxSsInst->UsrOpt.LinkRate);

	switch (DpRxSsInst->UsrOpt.LinkRate)
	{
	case XDP_DPCD_MAX_LINK_RATE_162GBPS:
			XVphy_PllInitialize(
			&rx_VPhy_Instance, 0, XVPHY_CHANNEL_ID_CHA,
			ONBOARD_REF_CLK_1, ONBOARD_REF_CLK_0,
			XVPHY_PLL_TYPE_QPLL1, XVPHY_PLL_TYPE_CPLL);
		break;

	default:
//			XVphy_PllInitialize(
//			&rx_VPhy_Instance, 0, XVPHY_CHANNEL_ID_CHA,
//			ONBOARD_REF_CLK_0, ONBOARD_REF_CLK_1,
//			XVPHY_PLL_TYPE_QPLL1, XVPHY_PLL_TYPE_CPLL);


		XVphy_PllInitialize(
		&rx_VPhy_Instance, 0, XVPHY_CHANNEL_ID_CHA,
		ONBOARD_REF_CLK_1, ONBOARD_REF_CLK_0,
		XVPHY_PLL_TYPE_QPLL1, XVPHY_PLL_TYPE_CPLL);

		break;
	}

	Status = XVphy_ClkInitialize(&rx_VPhy_Instance, 0,
			 XVPHY_CHANNEL_ID_CHA, XVPHY_DIR_RX);

	if (Status != XST_SUCCESS) {
		xil_printf("+++++++ RX GT configuration "
						"encountered an error "
						"(TP1) +++++++\r\n");
	}
}




void Dprx_InterruptHandlerPllReset(void *InstancePtr)
{
	u32 Status1;
	u32 Status2;
	XVphy rx_VPhy_Instance;

	XDpRxSs *DpRxSsInst = (XDpRxSs *)InstancePtr;
	switch(DpRxSsInst->DpPtr->Config.DeviceId)
	{
		case 0x0: rx_VPhy_Instance = rx_VPhy_Instance_0; break;
#if XPAR_XVPHY_NUM_INSTANCES >= 2
		case 0x1: rx_VPhy_Instance = rx_VPhy_Instance_1; break;
#endif
		default: rx_VPhy_Instance = rx_VPhy_Instance_0; break;
	}
	xil_printf("modify_dp_%d ",DpRxSsInst->DpPtr->Config.DeviceId);
	xil_printf("Dprx_InterruptHandlerPllReset start\r\n");
	XVphy_BufgGtReset(&rx_VPhy_Instance, XVPHY_DIR_RX,(TRUE));
	XVphy_ResetGtPll(&rx_VPhy_Instance, 0, XVPHY_CHANNEL_ID_CHA,
			 XVPHY_DIR_RX, (TRUE));
	XVphy_ResetGtPll(&rx_VPhy_Instance, 0, XVPHY_CHANNEL_ID_CHA,
			 XVPHY_DIR_RX, (FALSE));
	XVphy_BufgGtReset(&rx_VPhy_Instance, XVPHY_DIR_RX,(FALSE));

	XDp_RxInterruptEnable(DpRxSsInst->DpPtr,0x0007FFFF);

	Status2 = XVphy_WaitForResetDone(&rx_VPhy_Instance, 0,
					XVPHY_CHANNEL_ID_CHA, XVPHY_DIR_RX);
	Status1 = XVphy_WaitForPllLock(&rx_VPhy_Instance, 0,
					XVPHY_CHANNEL_ID_CHA);

	if (Status1 != XST_SUCCESS || Status2 != XST_SUCCESS)
	{

		xil_printf("Status1 = %lu ,Status2 =  %lu , ", Status1, Status2);
		xil_printf("Dprx_InterruptHandlerPllReset faild\r\n");

	}
	else
	{
		xil_printf("Dprx_InterruptHandlerPllReset SUCCESS\r\n");
	}
}



void Dprx_InterruptHandlerTrainingDone(void *InstancePtr)
{
	XDpRxSs *DpRxSsInst = (XDpRxSs *)InstancePtr;
	dp_conf.LaneCount =XDp_ReadReg(DpRxSsInst->Config.DpSubCore.DpConfig.BaseAddr,XDP_RX_DPCD_LANE_COUNT_SET);
	dp_conf.LineRate = DpRxSsInst->UsrOpt.LinkRate;

	xil_printf("modify_dp_%d ",DpRxSsInst->DpPtr->Config.DeviceId);
	xil_printf("Dprx_InterruptHandlerTrainingDone\r\n");
	xil_printf("> Interrupt: Training done !!! " "(BW: 0x%x, Lanes: 0x%x, Status: "
		    "0x%x;0x%x).\n\r", dp_conf.LineRate, dp_conf.LaneCount,
		    XDp_ReadReg(DpRxSsInst->Config.DpSubCore.DpConfig.BaseAddr,
				XDP_RX_DPCD_LANE01_STATUS),
		    XDp_ReadReg(DpRxSsInst->Config.DpSubCore.DpConfig.BaseAddr,
				XDP_RX_DPCD_LANE23_STATUS));


	xil_printf("Reset Video DTG in DisplayPort Controller...\r\n");
	XDp_RxDtgDis(DpRxSsInst->DpPtr);
	XDp_RxDtgEn(DpRxSsInst->DpPtr);

	training_done = 1;

//	XDp_RxInterruptDisable(DpRxSsInst->DpPtr, 0xFFFFFFFF);

}

static void Dprx_InterruptHandlerTrainingLost(void *InstancePtr)
{
	training_done = 0;
	vblank_count =0;
	dp_conf.pixel = 8;
	start_tracking = 0;
	change_detected = 0;

	XDpRxSs *DpRxSsInst = (XDpRxSs *)InstancePtr;

	XDp_RxInterruptEnable(DpRxSsInst->DpPtr,0x80000000);
	XDp_RxGenerateHpdInterrupt(DpRxSsInst->DpPtr, 750);
	xil_printf("modify_dp_%d ",DpRxSsInst->DpPtr->Config.DeviceId);
	xil_printf("Dprx_InterruptHandlerTrainingLost\r\n");

}

void Dprx_InterruptHandlerBwChange(void *InstancePtr)
{
	XDpRxSs *DpRxSsInst = (XDpRxSs *)InstancePtr;
	xil_printf("modify_dp_%d ",DpRxSsInst->DpPtr->Config.DeviceId);
	xil_printf("> Interrupt: Bandwidth Change !\n\r");
}

void Dprx_InterruptHandlerUplug(void *InstancePtr)
{
	XDpRxSs *DpRxSsInst = (XDpRxSs *)InstancePtr;
	xil_printf("modify_dp_%d ",DpRxSsInst->DpPtr->Config.DeviceId);
	xil_printf("Dprx_InterruptHandlerUplug\r\n");
	XDp_RxInterruptDisable(DpRxSsInst->DpPtr, 0xFFF87FFF);


	training_done = 0;
	vblank_count =0;

	XDp_RxDtgDis(DpRxSsInst->DpPtr);
	XDp_RxDtgEn(DpRxSsInst->DpPtr);

	start_tracking = 0;
	change_detected = 0;
}

void Dprx_InterruptHandlerPwr(void *InstancePtr)
{
	XDpRxSs *DpRxSsInst = (XDpRxSs *)InstancePtr;

	XDp_ReadReg(DpRxSsInst->DpPtr->Config.BaseAddr,XDP_RX_DPCD_SET_POWER_STATE);
	xil_printf("modify_dp_%d ",DpRxSsInst->DpPtr->Config.DeviceId);
	xil_printf("Dprx_InterruptHandlerPwr\n\r");

}

void Dprx_InterruptHandlerInfoPkt(void *InstancePtr)
{
	XDpRxSs *DpRxSsInst = (XDpRxSs *)InstancePtr;
	xil_printf("modify_dp_%d ",DpRxSsInst->DpPtr->Config.DeviceId);
	xil_printf("Dprx_InterruptHandlerInfoPkt\n\r");

}
void Dprx_InterruptHandlerExtPkt(void *InstancePtr)
{
	XDpRxSs *DpRxSsInst = (XDpRxSs *)InstancePtr;
	xil_printf("modify_dp_%d ",DpRxSsInst->DpPtr->Config.DeviceId);
	xil_printf("Dprx_InterruptHandlerExtPkt\n\r");
}

/*****************************************************************************/
/**
 * This function will detect video resolution/frequency change
 * Many GPUs won't re-train when only video res/freq change.
 * Application needs to know if it changed and need to detect new parameter.
 * Based on the parameter, re-set and re-start
 *
 * @return None
 *
 *****************************************************************************/
void video_change_detect(u32 *count_track, u32 *rxMsamisc0_track,
                         u32 *bpc_track, u32 *recv_clk_freq_track,
                         float *recv_frame_clk_track,
                         u32 *recv_frame_clk_int_track,
                         int *track_count, void *InstancePtr)
{
    XDpRxSs *DpRxSsInst = (XDpRxSs *)InstancePtr;

    // 开始跟踪变化时的逻辑
    if (start_tracking == 1)
    {
        // 等待积累 5000 次，防止误判
        if (*count_track < 5000)
        {
            (*count_track)++;
        }
        else
        {
            // 读取当前的 MVID 和 NVID，用于计算接收端时钟频率
            u32 baseAddr = DpRxSsInst->DpPtr->Config.BaseAddr;

            u32 rxMsaMVid = XDp_ReadReg(baseAddr, XDP_RX_MSA_MVID) & 0x00FFFFFF;
            u32 rxMsaNVid = XDp_ReadReg(baseAddr, XDP_RX_MSA_NVID) & 0x00FFFFFF;

            // 读取 MISC0 的 bit[7:5] 获取色深信息索引
            *rxMsamisc0_track = (XDp_ReadReg(baseAddr, XDP_RX_MSA_MISC0) >> 5) & 0x7;

            // 根据 MISC0 对应表获得色深（bpc）
            *bpc_track = bpc_table[(int)(*rxMsamisc0_track)];

            // 根据 LineRate、MVid 和 NVid 计算接收端链路时钟频率（单位：MHz）
            *recv_clk_freq_track = ((dp_conf.LineRate * 27.0) * rxMsaMVid) / rxMsaNVid;

            // 读取水平和垂直总像素数（包括 blanking）
            u32 DpHres_total = XDp_ReadReg(DpRxSsInst->Config.DpSubCore.DpConfig.BaseAddr, XDP_RX_MSA_HTOTAL);
            u32 DpVres_total = XDp_ReadReg(DpRxSsInst->Config.DpSubCore.DpConfig.BaseAddr, XDP_RX_MSA_VTOTAL);

            // 计算帧率：链路频率 / 像素总数，再上取整
            *recv_frame_clk_track = ceil(((*recv_clk_freq_track) * 1e6) / (DpHres_total * DpVres_total));
            *recv_frame_clk_int_track = (u32)(*recv_frame_clk_track);

            // 若帧率为 59/61，则归一化为 60；同理 29/31 -> 30，74/76 -> 75
            switch (*recv_frame_clk_int_track)
            {
                case 59:
                case 61:
                    *recv_frame_clk_int_track = 60;
                    break;
                case 29:
                case 31:
                    *recv_frame_clk_int_track = 30;
                    break;
                case 74:
                case 76:
                    *recv_frame_clk_int_track = 75;
                    break;
            }

            // 检查帧率是否变化
            if (*recv_frame_clk_int_track != Msa.Vtm.FrameRate)
            {
                xil_printf("Refresh rate changed from %d to %d\r\n",
                           Msa.Vtm.FrameRate, *recv_frame_clk_int_track);

                // 如果是 4K120 输入对接 4K60 显示器，不触发重新训练
                if (*recv_frame_clk_int_track < 79)
                {
                    change_detected = 1;
                    start_tracking = 0;
                    *count_track = 0;
                }
            }
            // 检查 BPC（Bit Per Component）是否变化
            else if (dp_conf.bpc != *bpc_track)
            {
                xil_printf("BPC changed from %d to %d\r\n", dp_conf.bpc, *bpc_track);
                change_detected = 1;
                start_tracking = 0;
                *count_track = 0;
            }
        }
    }

    // 若已检测到变化且训练完成，进行稳定性验证
    if (change_detected == 1 && training_done == 1)
    {
        (*track_count)++;

        // 达到稳定观测次数后确认变化
        if (*track_count >= 5000)
        {
            xil_printf(TXT_RED "\r\n__FILE__:%s, __LINE__:%d\r\n" TXT_RST, __FILE__, __LINE__);

            // 调用分辨率检测函数
            Dprx_DetectResolution(DpRxSsInst);

            // 复位状态以重新开始追踪
            start_tracking = 1;
            change_detected = 0;
            *track_count = 0;
        }
    }
    else
    {
        // 若检测不成立则重置状态
        change_detected = 0;
        *track_count = 0;
    }
}


/*****************************************************************************/
/**
 * @brief Detects incoming video after stable link training and sets up video output.
 *
 * This function:
 * 1. Waits until link training is completed.
 * 2. Tracks stability using a counter.
 * 3. On threshold, reads MSA and configures output.
 *
 * @param track_count1 Pointer to static frame counter.
 * @param InstancePtr  Pointer to XDpRxSs instance.
 *
 *****************************************************************************/
void detect_rx_video(int *track_count1, void *InstancePtr)
{
    // If link training hasn't completed, reset tracking and exit
    if (training_done != 1)
    {
        *track_count1 = 0;
        return;
    }

    // Training is done; increment frame tracking counter
    (*track_count1)++;

    // Wait for counter to reach threshold (e.g., stable for a period)
    if (*track_count1 < 20000)
        return;

    // Ready to detect resolution and configure video output
    XDpRxSs *DpRxSsInst = (XDpRxSs *)InstancePtr;

    Dprx_DetectResolution(DpRxSsInst);

    if (training_done == 1)
    {
        Dprx_ResetVideoOutput(DpRxSsInst);
        start_tracking = 1;
        xil_printf("detect_rx_video >>>>>> training_done == 1\r\n");
    }
    else
    {
        start_tracking = 0;
        xil_printf("detect_rx_video >>>>>> training_done != 1\r\n");
    }

    // Reset change tracking and counter
    change_detected = 0;
    *track_count1 = 0;
}


/*****************************************************************************/
/**
*
* This function is the callback function for Info Packet Handling.
*
* @param    InstancePtr is a pointer to the XDpRxSs instance.
*
* @return    None.
*
* @note        None.
*
******************************************************************************/
void DpRxSs_InfoPacketHandler(void *InstancePtr)
{
	u32 InfoFrame[9];
	int i=1;
	XDpRxSs *DpRxSsInst = (XDpRxSs *)InstancePtr;
	for(i = 1 ; i < 9 ; i++) {
		InfoFrame[i] = XDp_ReadReg(DpRxSsInst->DpPtr->Config.BaseAddr,
				XDP_RX_AUDIO_INFO_DATA(i));
	}

	AudioinfoFrame.frame_count++;

	AudioinfoFrame.version = InfoFrame[1]>>26;
	AudioinfoFrame.type = (InfoFrame[1]>>8)&0xFF;
	AudioinfoFrame.sec_id = InfoFrame[1]&0xFF;
	AudioinfoFrame.info_length = (InfoFrame[1]>>16)&0x3FF;

	AudioinfoFrame.audio_channel_count = InfoFrame[2]&0x7;
	AudioinfoFrame.audio_coding_type = (InfoFrame[2]>>4)&0xF;
	AudioinfoFrame.sample_size = (InfoFrame[2]>>8)&0x3;
	AudioinfoFrame.sampling_frequency = (InfoFrame[2]>>10)&0x7;
	AudioinfoFrame.channel_allocation = (InfoFrame[2]>>24)&0xFF;

	AudioinfoFrame.level_shift = (InfoFrame[3]>>3)&0xF;
	AudioinfoFrame.downmix_inhibit = (InfoFrame[3]>>7)&0x1;

	Print_InfoPkt();
}

/*****************************************************************************/
/**
*
* This function is the callback function for Generic Packet Handling of
* 32-Bytes payload.
*
* @param    InstancePtr is a pointer to the XDpRxSs instance.
*
* @return    None.
*
* @note        None.
*
******************************************************************************/
void DpRxSs_ExtPacketHandler(void *InstancePtr)
{
	u32 ExtFrame[9];
	int i=1;

	SdpExtFrame.frame_count++;
	XDpRxSs *DpRxSsInst = (XDpRxSs *)InstancePtr;
	/*Header Information*/
	ExtFrame[0] = XDp_ReadReg(DpRxSsInst->DpPtr->Config.BaseAddr,
			XDP_RX_AUDIO_INFO_DATA(1));
	SdpExtFrame.Header[0] =  ExtFrame[0]&0xFF;
	SdpExtFrame.Header[1] = (ExtFrame[0]&0xFF00)>>8;
	SdpExtFrame.Header[2] = (ExtFrame[0]&0xFF0000)>>16;
	SdpExtFrame.Header[3] = (ExtFrame[0]&0xFF000000)>>24;

	/*Payload Information*/
	for (i = 0 ; i < 8 ; i++)
	{
		ExtFrame[i+1] = XDp_ReadReg(DpRxSsInst->DpPtr->Config.BaseAddr,
				XDP_RX_AUDIO_INFO_DATA(i+2));
		SdpExtFrame.Payload[(i*4)]   =  ExtFrame[i+1]&0xFF;
		SdpExtFrame.Payload[(i*4)+1] = (ExtFrame[i+1]&0xFF00)>>8;
		SdpExtFrame.Payload[(i*4)+2] = (ExtFrame[i+1]&0xFF0000)>>16;
		SdpExtFrame.Payload[(i*4)+3] = (ExtFrame[i+1]&0xFF000000)>>24;
	}

}

/*****************************************************************************/
/**
*
* This function is the callback function for Access link qual request.
*
* @param    InstancePtr is a pointer to the XDpRxSs instance.
*
* @return    None.
*
* @note        None.
*
******************************************************************************/
void DpRxSs_AccessLinkQualHandler(void *InstancePtr)
{
	u32 ReadVal;
	u32 DrpVal;
	XVphy rx_VPhy_Instance;

	XDpRxSs *DpRxSsInst = (XDpRxSs *)InstancePtr;
	switch(DpRxSsInst->DpPtr->Config.DeviceId)
	{
		case 0x0: rx_VPhy_Instance = rx_VPhy_Instance_0; break;
#if XPAR_XVPHY_NUM_INSTANCES >= 2
		case 0x1: rx_VPhy_Instance = rx_VPhy_Instance_1; break;
#endif
		default: rx_VPhy_Instance = rx_VPhy_Instance_0; break;
	}

	ReadVal = XDp_ReadReg(DpRxSsInst->DpPtr->Config.BaseAddr,
			XDP_RX_DPC_LINK_QUAL_CONFIG);

	xil_printf("DpRxSs_AccessLinkQualHandler : 0x%x\r\n", ReadVal);

	/*Check for PRBS Mode*/
	if( (ReadVal&0x00000007) == XDP_RX_DPCD_LINK_QUAL_PRBS)
	{
		/*Enable PRBS Mode in Video PHY*/
		DrpVal = XVphy_ReadReg(rx_VPhy_Instance.Config.BaseAddr,
				XVPHY_RX_CONTROL_REG);
		DrpVal = DrpVal | 0x10101010;
		XVphy_WriteReg(rx_VPhy_Instance.Config.BaseAddr,
				XVPHY_RX_CONTROL_REG, DrpVal);

		/*Reset PRBS7 Counters*/
		DrpVal = XVphy_ReadReg(rx_VPhy_Instance.Config.BaseAddr,
				XVPHY_RX_CONTROL_REG);
		DrpVal = DrpVal | 0x08080808;
		XDp_WriteReg(rx_VPhy_Instance.Config.BaseAddr,
				XVPHY_RX_CONTROL_REG, DrpVal);
		DrpVal = XVphy_ReadReg(rx_VPhy_Instance.Config.BaseAddr,
				XVPHY_RX_CONTROL_REG);
		DrpVal = DrpVal & 0xF7F7F7F7;
		XVphy_WriteReg(rx_VPhy_Instance.Config.BaseAddr, XVPHY_RX_CONTROL_REG,
				DrpVal);

	} else {
		/*Disable PRBS Mode in Video PHY*/
		DrpVal = XVphy_ReadReg(rx_VPhy_Instance.Config.BaseAddr,
				XVPHY_RX_CONTROL_REG);
		DrpVal = DrpVal & 0xEFEFEFEF;
		XVphy_WriteReg(rx_VPhy_Instance.Config.BaseAddr, XVPHY_RX_CONTROL_REG,
				DrpVal);
	}
}

/*****************************************************************************/
/**
*
* This function is the callback function for Access prbs error count.
*
* @param    InstancePtr is a pointer to the XDpRxSs instance.
*
* @return    None.
*
* @note        None.
*
******************************************************************************/
void DpRxSs_AccessErrorCounterHandler(void *InstancePtr)
{
	xil_printf("DpRxSs_AccessErrorCounterHandler\r\n");
#if ( AccessErrorCounterHandler_FLAG == 1)
	u16 DrpVal;
	u16 DrpVal_lower_lane0;
	u16 DrpVal_lower_lane1;
	u16 DrpVal_lower_lane2;
	u16 DrpVal_lower_lane3;

	/*Read PRBS Error Counter Value from Video PHY*/

	/*Lane 0 - Store only lower 16 bits*/
	XVphy_DrpRd(&rx_VPhy_Instance, 0, XVPHY_CHANNEL_ID_CH1,
			XVPHY_DRP_GTHE4_PRBS_ERR_CNTR_LOWER, &DrpVal_lower_lane0);
	XVphy_DrpRd(&rx_VPhy_Instance, 0, XVPHY_CHANNEL_ID_CH1,
			XVPHY_DRP_GTHE4_PRBS_ERR_CNTR_UPPER, &DrpVal);

	/*Lane 1 - Store only lower 16 bits*/
	XVphy_DrpRd(&rx_VPhy_Instance, 0, XVPHY_CHANNEL_ID_CH2,
			XVPHY_DRP_GTHE4_PRBS_ERR_CNTR_LOWER, &DrpVal_lower_lane1);
	XVphy_DrpRd(&rx_VPhy_Instance, 0, XVPHY_CHANNEL_ID_CH2,
			XVPHY_DRP_GTHE4_PRBS_ERR_CNTR_UPPER, &DrpVal);

	/*Lane 2 - Store only lower 16 bits*/
	XVphy_DrpRd(&rx_VPhy_Instance, 0, XVPHY_CHANNEL_ID_CH3,
			XVPHY_DRP_GTHE4_PRBS_ERR_CNTR_LOWER, &DrpVal_lower_lane2);
	XVphy_DrpRd(&rx_VPhy_Instance, 0, XVPHY_CHANNEL_ID_CH3,
			XVPHY_DRP_GTHE4_PRBS_ERR_CNTR_UPPER, &DrpVal);

	/*Lane 3 - Store only lower 16 bits*/
	XVphy_DrpRd(&rx_VPhy_Instance, 0, XVPHY_CHANNEL_ID_CH4,
			XVPHY_DRP_GTHE4_PRBS_ERR_CNTR_LOWER, &DrpVal_lower_lane3);
	XVphy_DrpRd(&rx_VPhy_Instance, 0, XVPHY_CHANNEL_ID_CH4,
			XVPHY_DRP_GTHE4_PRBS_ERR_CNTR_UPPER, &DrpVal);

	/*Write into DP Core - Validity bit and lower 15 bit counter value*/
	XDp_WriteReg(DpRxSsInst->DpPtr->Config.BaseAddr,
			XDP_RX_DPC_L01_PRBS_CNTR,
			(0x8000|DrpVal_lower_lane0) |
			((0x8000|DrpVal_lower_lane1)<<16));
	XDp_WriteReg(DpRxSsInst->DpPtr->Config.BaseAddr,
			XDP_RX_DPC_L23_PRBS_CNTR,
			(0x8000|DrpVal_lower_lane2) |
			((0x8000|DrpVal_lower_lane3)<<16));

	/*Reset PRBS7 Counters*/
	DrpVal = XVphy_ReadReg(rx_VPhy_Instance.Config.BaseAddr, XVPHY_RX_CONTROL_REG);
	DrpVal = DrpVal | 0x08080808;
	XDp_WriteReg(rx_VPhy_Instance.Config.BaseAddr, XVPHY_RX_CONTROL_REG, DrpVal);
	DrpVal = XVphy_ReadReg(rx_VPhy_Instance.Config.BaseAddr, XVPHY_RX_CONTROL_REG);
	DrpVal = DrpVal & 0xF7F7F7F7;
	XVphy_WriteReg(rx_VPhy_Instance.Config.BaseAddr, XVPHY_RX_CONTROL_REG, DrpVal);
#endif
}

/*****************************************************************************/
/**
*
* This function is the callback function for Test CRC Event request.
*
* @param    InstancePtr is a pointer to the XDpRxSs instance.
*
* @return    None.
*
* @note        None.
*
******************************************************************************/
void DpRxSs_CRCTestEventHandler(void *InstancePtr)
{
	u16 ReadVal;
	u32 TrainingAlgoValue;
	XDpRxSs *DpRxSsInst = (XDpRxSs *)InstancePtr;
	ReadVal = XDp_ReadReg(DpRxSsInst->DpPtr->Config.BaseAddr, XDP_RX_CRC_CONFIG);

	/*Record Training Algo Value - to be restored in non-phy test mode*/
	TrainingAlgoValue = XDp_ReadReg(DpRxSsInst->Config.BaseAddress,
			XDP_RX_MIN_VOLTAGE_SWING);

	/*Refer to DPCD 0x270 Register*/
	if ((ReadVal&0x8000) == 0x8000) {
		/*Enable PHY test mode - Set Min voltage swing to 0*/
		XDp_WriteReg(DpRxSsInst->DpPtr->Config.BaseAddr,
				XDP_RX_MIN_VOLTAGE_SWING,
				(TrainingAlgoValue & 0xFFFFFFFC) | 0x80000000);

			/*Disable Training timeout*/
			ReadVal = XDp_ReadReg(DpRxSsInst->Config.BaseAddress,
				XDP_RX_CDR_CONTROL_CONFIG);
			XDp_WriteReg(DpRxSsInst->DpPtr->Config.BaseAddr,
				XDP_RX_CDR_CONTROL_CONFIG, ReadVal | 0x40000000);

	} else {
		/*Disable PHY test mode & Set min voltage swing back to level 1*/
		XDp_WriteReg(DpRxSsInst->DpPtr->Config.BaseAddr,
				XDP_RX_MIN_VOLTAGE_SWING,
				(TrainingAlgoValue & 0x7FFFFFFF) | 0x1);

		/*Enable Training timeout*/
		ReadVal = XDp_ReadReg(DpRxSsInst->Config.BaseAddress,
					XDP_RX_CDR_CONTROL_CONFIG);
		XDp_WriteReg(DpRxSsInst->DpPtr->Config.BaseAddr,
				XDP_RX_CDR_CONTROL_CONFIG, ReadVal & 0xBFFFFFFF);
	}
}

int dp_init_peripherals(void)
{
	u32 Status;

	/* Initialize timer. */
	Status = XTmrCtr_Initialize(&TmrCtr, XPAR_TMRCTR_1_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		xil_printf("ERR:Timer failed to initialize. \r\n");
		return XST_FAILURE;
	}

	/* Set up timer options. */
	XTmrCtr_SetResetValue(&TmrCtr, 0, TIMER_RESET_VALUE);
	XTmrCtr_Start(&TmrCtr, 0);
#if XPAR_XVPHY_NUM_INSTANCES >= 1
	/* Initialize Video PHY Controller */
	XVphy_Config *CfgPtr_0 =
		XVphy_LookupConfig(XPAR_VID_PHY_CONTROLLER_0_DEVICE_ID);

	XVphy_DpInitialize(&rx_VPhy_Instance_0, CfgPtr_0, 0,
			   PHY_User_Config_Table[5].CPLLRefClkSrc,
			   PHY_User_Config_Table[5].QPLLRefClkSrc,
			   PHY_User_Config_Table[5].TxPLL,
			   PHY_User_Config_Table[5].RxPLL,
			   PHY_User_Config_Table[5].LineRate);

	Two_byte_set(&rx_VPhy_Instance_0, 0, SET_RX_TO_2BYTE);


	XVphy_ResetGtPll(&rx_VPhy_Instance_0, 0,
			 PHY_User_Config_Table[5].RxChId,
			 XVPHY_DIR_RX, (TRUE));
	XVphy_BufgGtReset(&rx_VPhy_Instance_0, XVPHY_DIR_RX,(TRUE));

	XVphy_ResetGtPll(&rx_VPhy_Instance_0, 0,
			 PHY_User_Config_Table[5].RxChId,
			 XVPHY_DIR_RX, (FALSE));
	XVphy_BufgGtReset(&rx_VPhy_Instance_0, XVPHY_DIR_RX,(FALSE));

#endif

#if XPAR_XVPHY_NUM_INSTANCES >= 2
	XVphy_Config *CfgPtr_1 =
		XVphy_LookupConfig(XPAR_VID_PHY_CONTROLLER_1_DEVICE_ID);

	XVphy_DpInitialize(&rx_VPhy_Instance_1, CfgPtr_1, 0,
			   PHY_User_Config_Table[4].CPLLRefClkSrc,
			   PHY_User_Config_Table[4].QPLLRefClkSrc,
			   PHY_User_Config_Table[4].TxPLL,
			   PHY_User_Config_Table[4].RxPLL,
			   PHY_User_Config_Table[4].LineRate);

	Two_byte_set(&rx_VPhy_Instance_1, 0, SET_RX_TO_2BYTE);


	XVphy_ResetGtPll(&rx_VPhy_Instance_1, 0,
			 PHY_User_Config_Table[4].RxChId,
			 XVPHY_DIR_RX, (TRUE));
	XVphy_BufgGtReset(&rx_VPhy_Instance_1, XVPHY_DIR_RX,(TRUE));

	XVphy_ResetGtPll(&rx_VPhy_Instance_1, 0,
			 PHY_User_Config_Table[4].RxChId,
			 XVPHY_DIR_RX, (FALSE));
	XVphy_BufgGtReset(&rx_VPhy_Instance_1, XVPHY_DIR_RX,(FALSE));

#endif
	return XST_SUCCESS;
}

int DPRxInitialize()
{
	u32 Status;
#if XPAR_XVPHY_NUM_INSTANCES >= 1
	/* Lookup and Initialize DP Rx Subsystem */
	XDpRxSs_Config *DPRxSSConfig_0;
	DPRxSSConfig_0 = XDpRxSs_LookupConfig(XPAR_DPRXSS_0_DEVICE_ID);
	if (DPRxSSConfig_0 == NULL) {
		xil_printf("ERR: DPRX SS core not found!\n\r");
		return (XST_FAILURE);
	}
	Status = XDpRxSs_CfgInitialize(&DpRxSsInst_0, DPRxSSConfig_0,
				DPRxSSConfig_0->BaseAddress);

	if (Status != XST_SUCCESS) {
		xil_printf("ERR: DPRX SS initialize failed!\n\r");
		return (XST_FAILURE);
	}

	/* Set custom timer wait */
	XDpRxSs_SetUserTimerHandler(&DpRxSsInst_0, &DpPt_CustomWaitUs, &TmrCtr);

	/* Setup callbacks */
	XDpRxSs_SetCallBack(&DpRxSsInst_0, XDPRXSS_HANDLER_PLL_RESET_EVENT,
				&Dprx_InterruptHandlerPllReset, &DpRxSsInst_0);
	XDpRxSs_SetCallBack(&DpRxSsInst_0, XDPRXSS_HANDLER_LINKBW_EVENT,
				&Dprx_InterruptHandlerLinkBW, &DpRxSsInst_0);
	XDpRxSs_SetCallBack(&DpRxSsInst_0, XDPRXSS_HANDLER_DP_TDONE_EVENT,
				&Dprx_InterruptHandlerTrainingDone, &DpRxSsInst_0);
	XDpRxSs_SetCallBack(&DpRxSsInst_0, XDPRXSS_HANDLER_DP_BW_CHG_EVENT,
				&Dprx_InterruptHandlerBwChange, &DpRxSsInst_0);
	XDpRxSs_SetCallBack(&DpRxSsInst_0, XDPRXSS_HANDLER_DP_INFO_PKT_EVENT,
				&Dprx_InterruptHandlerInfoPkt, &DpRxSsInst_0);
	XDpRxSs_SetCallBack(&DpRxSsInst_0, XDPRXSS_HANDLER_DP_EXT_PKT_EVENT,
				&Dprx_InterruptHandlerExtPkt, &DpRxSsInst_0);
	XDpRxSs_SetCallBack(&DpRxSsInst_0, XDPRXSS_HANDLER_UNPLUG_EVENT,
				&Dprx_InterruptHandlerUplug, &DpRxSsInst_0);
	XDpRxSs_SetCallBack(&DpRxSsInst_0, XDPRXSS_HANDLER_DP_PWR_CHG_EVENT,
				&Dprx_InterruptHandlerPwr, &DpRxSsInst_0);
	XDpRxSs_SetCallBack(&DpRxSsInst_0, XDPRXSS_HANDLER_DP_VM_CHG_EVENT,
				&Dprx_InterruptHandlerVmChange, &DpRxSsInst_0);
	XDpRxSs_SetCallBack(&DpRxSsInst_0, XDPRXSS_HANDLER_DP_NO_VID_EVENT,
				&Dprx_InterruptHandlerNoVideo, &DpRxSsInst_0);
	XDpRxSs_SetCallBack(&DpRxSsInst_0, XDPRXSS_HANDLER_DP_VBLANK_EVENT,
				&Dprx_InterruptHandlerVBlank, &DpRxSsInst_0);
	XDpRxSs_SetCallBack(&DpRxSsInst_0, XDPRXSS_HANDLER_DP_TLOST_EVENT,
				&Dprx_InterruptHandlerTrainingLost, &DpRxSsInst_0);
	XDpRxSs_SetCallBack(&DpRxSsInst_0, XDPRXSS_HANDLER_DP_VID_EVENT,
				&Dprx_InterruptHandlerVideo, &DpRxSsInst_0);

	XDpRxSs_SetCallBack(&DpRxSsInst_0, XDPRXSS_HANDLER_ACCESS_LINK_QUAL_EVENT,
			DpRxSs_AccessLinkQualHandler, &DpRxSsInst_0);
	XDpRxSs_SetCallBack(&DpRxSsInst_0, XDPRXSS_HANDLER_ACCESS_ERROR_COUNTER_EVENT,
			DpRxSs_AccessErrorCounterHandler, &DpRxSsInst_0);
	XDpRxSs_SetCallBack(&DpRxSsInst_0, XDPRXSS_HANDLER_DP_CRC_TEST_EVENT,
			DpRxSs_CRCTestEventHandler, &DpRxSsInst_0);
	XDpRxSs_SetCallBack(&DpRxSsInst_0, XDPRXSS_HANDLER_DP_INFO_PKT_EVENT,
			&DpRxSs_InfoPacketHandler, &DpRxSsInst_0);
	XDpRxSs_SetCallBack(&DpRxSsInst_0, XDPRXSS_HANDLER_DP_EXT_PKT_EVENT,
			&DpRxSs_ExtPacketHandler, &DpRxSsInst_0);
#endif
#if XPAR_XVPHY_NUM_INSTANCES >= 2

	/* Lookup and Initialize DP Rx Subsystem */
	XDpRxSs_Config *DPRxSSConfig_1;
	DPRxSSConfig_1 = XDpRxSs_LookupConfig(XPAR_DPRXSS_1_DEVICE_ID);
	if (DPRxSSConfig_1 == NULL) {
		xil_printf("ERR: DPRX SS core not found!\n\r");
		return (XST_FAILURE);
	}
	Status = XDpRxSs_CfgInitialize(&DpRxSsInst_1, DPRxSSConfig_1,
				DPRxSSConfig_1->BaseAddress);

	if (Status != XST_SUCCESS) {
		xil_printf("ERR: DPRX SS initialize failed!\n\r");
		return (XST_FAILURE);
	}

	/* Set custom timer wait */
	XDpRxSs_SetUserTimerHandler(&DpRxSsInst_1, &DpPt_CustomWaitUs, &TmrCtr);

	/* Setup callbacks */
	XDpRxSs_SetCallBack(&DpRxSsInst_1, XDPRXSS_HANDLER_PLL_RESET_EVENT,
				&Dprx_InterruptHandlerPllReset, &DpRxSsInst_1);
	XDpRxSs_SetCallBack(&DpRxSsInst_1, XDPRXSS_HANDLER_LINKBW_EVENT,
				&Dprx_InterruptHandlerLinkBW, &DpRxSsInst_1);
	XDpRxSs_SetCallBack(&DpRxSsInst_1, XDPRXSS_HANDLER_DP_TDONE_EVENT,
				&Dprx_InterruptHandlerTrainingDone, &DpRxSsInst_1);
	XDpRxSs_SetCallBack(&DpRxSsInst_1, XDPRXSS_HANDLER_DP_BW_CHG_EVENT,
				&Dprx_InterruptHandlerBwChange, &DpRxSsInst_1);
	XDpRxSs_SetCallBack(&DpRxSsInst_1, XDPRXSS_HANDLER_DP_INFO_PKT_EVENT,
				&Dprx_InterruptHandlerInfoPkt, &DpRxSsInst_1);
	XDpRxSs_SetCallBack(&DpRxSsInst_1, XDPRXSS_HANDLER_DP_EXT_PKT_EVENT,
				&Dprx_InterruptHandlerExtPkt, &DpRxSsInst_1);
	XDpRxSs_SetCallBack(&DpRxSsInst_1, XDPRXSS_HANDLER_UNPLUG_EVENT,
				&Dprx_InterruptHandlerUplug, &DpRxSsInst_1);
	XDpRxSs_SetCallBack(&DpRxSsInst_1, XDPRXSS_HANDLER_DP_PWR_CHG_EVENT,
				&Dprx_InterruptHandlerPwr, &DpRxSsInst_1);
	XDpRxSs_SetCallBack(&DpRxSsInst_1, XDPRXSS_HANDLER_DP_VM_CHG_EVENT,
				&Dprx_InterruptHandlerVmChange, &DpRxSsInst_1);
	XDpRxSs_SetCallBack(&DpRxSsInst_1, XDPRXSS_HANDLER_DP_NO_VID_EVENT,
				&Dprx_InterruptHandlerNoVideo, &DpRxSsInst_1);
	XDpRxSs_SetCallBack(&DpRxSsInst_1, XDPRXSS_HANDLER_DP_VBLANK_EVENT,
				&Dprx_InterruptHandlerVBlank, &DpRxSsInst_1);
	XDpRxSs_SetCallBack(&DpRxSsInst_1, XDPRXSS_HANDLER_DP_TLOST_EVENT,
				&Dprx_InterruptHandlerTrainingLost, &DpRxSsInst_1);
	XDpRxSs_SetCallBack(&DpRxSsInst_1, XDPRXSS_HANDLER_DP_VID_EVENT,
				&Dprx_InterruptHandlerVideo, &DpRxSsInst_1);

	XDpRxSs_SetCallBack(&DpRxSsInst_1, XDPRXSS_HANDLER_ACCESS_LINK_QUAL_EVENT,
			DpRxSs_AccessLinkQualHandler, &DpRxSsInst_1);
	XDpRxSs_SetCallBack(&DpRxSsInst_1, XDPRXSS_HANDLER_ACCESS_ERROR_COUNTER_EVENT,
			DpRxSs_AccessErrorCounterHandler, &DpRxSsInst_1);
	XDpRxSs_SetCallBack(&DpRxSsInst_1, XDPRXSS_HANDLER_DP_CRC_TEST_EVENT,
			DpRxSs_CRCTestEventHandler, &DpRxSsInst_1);
	XDpRxSs_SetCallBack(&DpRxSsInst_1, XDPRXSS_HANDLER_DP_INFO_PKT_EVENT,
			&DpRxSs_InfoPacketHandler, &DpRxSsInst_1);
	XDpRxSs_SetCallBack(&DpRxSsInst_1, XDPRXSS_HANDLER_DP_EXT_PKT_EVENT,
			&DpRxSs_ExtPacketHandler, &DpRxSsInst_1);
#endif


	return (XST_SUCCESS);
}


/*****************************************************************************/
/**
*
* This function is sets up Interrupt system and start it
*
* @return	None.
*
******************************************************************************/
int DpPt_SetupIntrSystem()
{
	int Status;
	XIntc *IntcInstPtr = &InterruptController;

#if XPAR_XVPHY_NUM_INSTANCES >= 1
	/* Hook up Rx interrupt service routine */
	Status = XIntc_Connect(IntcInstPtr, XPAR_INTC_0_DP14RXSS_0_VEC_ID,
				(XInterruptHandler)XDpRxSs_DpIntrHandler,
				&DpRxSsInst_0);
	if (Status != XST_SUCCESS) {
		xil_printf("ERR:0 DP RX SS DP interrupt connect failed!\n\r");
		return XST_FAILURE;
	}
#endif

#if XPAR_XVPHY_NUM_INSTANCES >= 2
	/* Hook up Rx interrupt service routine */
	Status = XIntc_Connect(IntcInstPtr, XPAR_INTC_0_DP14RXSS_1_VEC_ID,
				(XInterruptHandler)XDpRxSs_DpIntrHandler,
				&DpRxSsInst_1);
	if (Status != XST_SUCCESS) {
		xil_printf("ERR:1 DP RX SS DP interrupt connect failed!\n\r");
		return XST_FAILURE;
	}
#endif

	/* Hook up Rx interrupt service routine */
	Status = XIntc_Connect(IntcInstPtr, XPAR_INTC_0_TMRCTR_1_VEC_ID,
				(XInterruptHandler)XTmrCtr_InterruptHandler,
				&TmrCtr);
	if (Status != XST_SUCCESS) {
		xil_printf("ERR: Timer interrupt connect failed!\n\r");
		return XST_FAILURE;
	}



	return (XST_SUCCESS);
}

/*****************************************************************************/
/**
*
* This function is the callback function for Info Packet Handling.
*
* @param    InstancePtr is a pointer to the XDpRxSs instance.
*
* @return    None.
*
* @note        None.
*
******************************************************************************/
void Print_InfoPkt()
{
	xil_printf("Received Audio Info Packet::\r\n");
	xil_printf(" -frame_count		 	 : 0x%x \r\n",
			AudioinfoFrame.frame_count);
	xil_printf(" -version			 	 : 0x%x \r\n",
			AudioinfoFrame.version);
	xil_printf(" -type				 	 : 0x%x \r\n",
			AudioinfoFrame.type);
	xil_printf(" -sec_id				 : 0x%x \r\n",
			AudioinfoFrame.sec_id);
	xil_printf(" -info_length			 : 0x%x \r\n",
			AudioinfoFrame.info_length);
	xil_printf(" -audio_channel_count	 : 0x%x \r\n",
			AudioinfoFrame.audio_channel_count);
	xil_printf(" -audio_coding_type		 : 0x%x \r\n",
			AudioinfoFrame.audio_coding_type);
	xil_printf(" -sample_size			 : 0x%x \r\n",
			AudioinfoFrame.sample_size);
	xil_printf(" -sampling_frequency	 : 0x%x \r\n",
			AudioinfoFrame.sampling_frequency);
	xil_printf(" -channel_allocation	 : 0x%x \r\n",
			AudioinfoFrame.channel_allocation);
	xil_printf(" -level_shift			 : 0x%x \r\n",
			AudioinfoFrame.level_shift);
	xil_printf(" -downmix_inhibit		 : 0x%x \r\n",
			AudioinfoFrame.downmix_inhibit);
}

/*****************************************************************************/
/**
*
* This function is the callback function for Ext Packet Handling.
*
* @param    InstancePtr is a pointer to the XDpRxSs instance.
*
* @return    None.
*
* @note        None.
*
******************************************************************************/
void Print_ExtPkt()
{
	xil_printf("Received SDP Packet Type::\r\n");
	switch(SdpExtFrame.Header[1])
	{
		case 0x04: xil_printf(" -> Extension\r\n"); break;
		case 0x05: xil_printf(" -> Audio_CopyManagement\r\n"); break;
		case 0x06: xil_printf(" -> ISRC\r\n"); break;
		case 0x07: xil_printf(" -> Video Stream Configuration (VSC)\r\n");break;
		case 0x20: xil_printf(" -> Video Stream Configuration Extension"
				" for VESA (VSC_EXT_VESA) - Used for HDR Metadata\r\n"); break;
		case 0x21: xil_printf(" -> VSC_EXT_CEA for future CEA INFOFRAME with "
				"payload of more than 28 bytes\r\n"); break;
		default: xil_printf(" -> Reserved/Not Defined\r\n"); break;
	}
	xil_printf(" Header Bytes : 0x%x, 0x%x, 0x%x, 0x%x \r\n",
			SdpExtFrame.Header[0],
			SdpExtFrame.Header[1],
			SdpExtFrame.Header[2],
			SdpExtFrame.Header[3]);
	xil_printf(" Frame Count : %d \r\n",SdpExtFrame.frame_count);
}

void report_info(u8 ch)
{
	xil_printf("==========************ch %d*************===========\r\n",ch);
	XDpRxSs *DpRxSsInst;
	switch(ch)
	{
		case 1:
		{
			DpRxSsInst = &DpRxSsInst_0;
			break;
		}
#if XPAR_XVPHY_NUM_INSTANCES >= 2
		case 2:
		{
			DpRxSsInst = &DpRxSsInst_1;
			break;
		}
#endif
		default:
			{
				DpRxSsInst = &DpRxSsInst_0;
				break;
			}
	}
	xil_printf("==========RX Debug Data===========\r\n");
	XDpRxSs_ReportLinkInfo(DpRxSsInst);
	XDpRxSs_ReportMsaInfo(DpRxSsInst);

	/* Sink-side inbound AUX: counts rise when DP Source (e.g. MAX96772) talks DPCD/EDID */
	{
		u32 const auxb = DpRxSsInst->DpPtr->Config.BaseAddr;
		xil_printf("==========RX AUX inbound (Sink)===========\r\n");
		xil_printf("REQ_COUNT=%lu ERR_COUNT=%lu AUX_REQ_IP=0x%lx\r\n",
			   (unsigned long)XDp_ReadReg(auxb, XDP_RX_REQ_COUNT),
			   (unsigned long)XDp_ReadReg(auxb, XDP_RX_REQ_ERROR_COUNT),
			   (unsigned long)XDp_ReadReg(auxb, XDP_RX_AUX_REQ_IN_PROGRESS));
		xil_printf("last REQ: CMD=0x%lx ADDR=0x%lx LEN=0x%lx\r\n",
			   (unsigned long)XDp_ReadReg(auxb, XDP_RX_REQ_CMD),
			   (unsigned long)XDp_ReadReg(auxb, XDP_RX_REQ_ADDRESS),
			   (unsigned long)XDp_ReadReg(auxb, XDP_RX_REQ_LENGTH));
	}


	xil_printf("==========**************************===========\r\n");

}


/*****************************************************************************/
/**
*
* This function prints Menu
*
* @param    None.
*
* @return    None.
*
* @note        None.
*
******************************************************************************/
void dp_app_help()
{
	xil_printf("\n\n\r");
	xil_printf("-----------------------------------------------------\n\r");
	xil_printf("--                       Menu                      --\n\r");
	xil_printf("-----------------------------------------------------\n\r");
	xil_printf("\n\r");
	xil_printf(" Select option\n\r");
	xil_printf(" 2 = Reset AUX Logic  \n\r");
	xil_printf(" h = Assert HPD pulse (see log: AUX REQ before/after)\n\r");
	xil_printf(" e = Report VPHY Error & Status  \n\r");
	xil_printf(" c = Core Info  \n\r");
	xil_printf(" r = Reset DTG  \n\r");
	xil_printf(" p = Report register info  \n\r");
	xil_printf(" m = Report Video MSA Attributes, Time Stamps "
	                "Values  \n\r");
	xil_printf(" . = Show Menu  \n\r");
	xil_printf("\n\r");
	xil_printf("-----------------------------------------------------\n\r");
}

void dp_debug_info(u8 UserInput, u8 ch)
{
	XDpRxSs *DpRxSsInst;
	u32 phy_addr;
	XVphy rx_VPhy_Instance;
	switch(ch)
	{
		case 1:DpRxSsInst = &DpRxSsInst_0;break;
#if XPAR_XVPHY_NUM_INSTANCES >= 2
		case 2:DpRxSsInst = &DpRxSsInst_1;break;
#endif
		default:DpRxSsInst = &DpRxSsInst_0;break;
	}
	switch(ch)
	{
		case 1:phy_addr =XPAR_VID_PHY_CONTROLLER_0_BASEADDR;break;
#if XPAR_XVPHY_NUM_INSTANCES >= 2
		case 2:phy_addr =XPAR_VID_PHY_CONTROLLER_1_BASEADDR;break;
#endif
		default:phy_addr =XPAR_VID_PHY_CONTROLLER_0_BASEADDR;break;
	}
	switch(ch)
	{
		case 1:rx_VPhy_Instance =rx_VPhy_Instance_0;break;
#if XPAR_XVPHY_NUM_INSTANCES >= 2
		case 2:rx_VPhy_Instance =rx_VPhy_Instance_1;break;
#endif
		default:rx_VPhy_Instance =rx_VPhy_Instance_0;break;
	}
	switch(UserInput)
	{
		u32 ReadVal=0;
		u16 DrpVal;
		case '2':
			/* Reset the AUX logic from DP RX */
			xil_printf("\r\nReset the AUX logic from DP RX\r\n");
			XDp_WriteReg(DpRxSsInst->DpPtr->Config.BaseAddr,
				 XDP_RX_SOFT_RESET,
				 XDP_RX_SOFT_RESET_AUX_MASK);
			break;
		case 'c':
			XDpRxSs_ReportCoreInfo(DpRxSsInst);
			break;

		case 'h':
		{
			u32 const b = DpRxSsInst->DpPtr->Config.BaseAddr;
			u32 const rc0 = XDp_ReadReg(b, XDP_RX_REQ_COUNT);
			u32 const er0 = XDp_ReadReg(b, XDP_RX_REQ_ERROR_COUNT);

			xil_printf("\r\n- HPD pulse (FPGA Sink -> Source HPD pin) -\n\r");
			xil_printf("AUX before: REQ_COUNT=%lu ERR_COUNT=%lu\r\n",
				   (unsigned long)rc0, (unsigned long)er0);
			/* Do not strip training/link IRQs; mask reg is 1=masked (see xdp_intr). */
			XDp_WriteReg(b, XDP_RX_INTERRUPT_MASK, 0xFFF87FFD);
			/* Duration field is u16 us; use 5 ms per menu text */
			XDp_RxGenerateHpdInterrupt(DpRxSsInst->DpPtr, 50000);
			usleep(50 * 1000);
			{
				u32 const rc1 = XDp_ReadReg(b, XDP_RX_REQ_COUNT);
				u32 const er1 = XDp_ReadReg(b, XDP_RX_REQ_ERROR_COUNT);
				xil_printf("AUX after ~50ms: REQ_COUNT=%lu (+%lu) ERR_COUNT=%lu (+%lu)\r\n",
					   (unsigned long)rc1,
					   (unsigned long)(rc1 - rc0),
					   (unsigned long)er1,
					   (unsigned long)(er1 - er0));
			}
			xil_printf("If REQ_COUNT rises, Source likely saw HPD and issued AUX (DPCD/EDID).\r\n");
			break;
		}
		case 'd':
			xil_printf("==========RX Debug Data===========\r\n");
			XDpRxSs_ReportLinkInfo(&DpRxSsInst);
			XDpRxSs_ReportMsaInfo(&DpRxSsInst);
			break;
		case 'm':
			xil_printf(" XDP_RX_USER_FIFO_OVERFLOW (0x110) = 0x%x\n\r",
			XDp_ReadReg(DpRxSsInst->DpPtr->Config.BaseAddr,
				    XDP_RX_USER_FIFO_OVERFLOW));
			XDpRxSs_ReportMsaInfo(&DpRxSsInst);
			xil_printf(" XDP_RX_LINE_RESET_DISABLE (0x008) = 0x%x\n\r",
			XDp_ReadReg(DpRxSsInst->DpPtr->Config.BaseAddr,
				    XDP_RX_LINE_RESET_DISABLE));
			break;

		case 'r':
			xil_printf("Reset Video DTG in DisplayPort Controller...\r\n");
			XDp_RxDtgDis(DpRxSsInst->DpPtr);
			XDp_RxDtgEn(DpRxSsInst->DpPtr);

			break;

		case 'p':
			report_info(ch);
			break;
	}
}

void dp_init(void)
{
	/* Initializing user_config parameters */
	user_config.user_numStreams = 1;
	user_config.user_bpc = 8;
	user_config.user_format = 1;
	user_config.mst_check_flag= 0;
	xil_printf("\n********************************"
			 "********************************\n\r");

	/* EDID mode start with pass-through mode. */
	use_monitor_edid = 0; //是否使用显示器的EDID, 作为 DP rx模块的EDID

	// Adding custom resolutions at here.

//if vpss already registering
#if 1
	xil_printf("INFO> Registering Custom Timing Table with %d entries \r\n",
		   (XVIDC_CM_NUM_SUPPORTED - (XVIDC_VM_CUSTOM + 1)));
	Status = XVidC_RegisterCustomTimingModes(XVidC_MyVideoTimingMode,
			(XVIDC_CM_NUM_SUPPORTED - (XVIDC_VM_CUSTOM + 1)));

	if (Status != XST_SUCCESS) {
		xil_printf("ERR: Unable to register custom "
			   "timing table\r\r\n\n");
	}

#endif
	xil_printf("\n********************************"
			 "********************************\n\r");
	xil_printf("            DisplayPort rx \n\r");
	xil_printf("%s %s\n\r\r\n", __DATE__  ,__TIME__ );
	xil_printf("\n********************************"
			 "********************************\n\r");

	dp_init_peripherals();

#if XPAR_XDPRXSS_NUM_INSTANCES
	DPRxInitialize();
	/* DPRxSs uses its own I2C driver */
//	XDpRxSs_McDp6000_init(&DpRxSsInst_0);//
//	XDpRxSs_McDp6000_init_flipping(&DpRxSsInst_0);

//	XDpRxSs_McDp6000_init(&DpRxSsInst_1);//
//	XDpRxSs_McDp6000_init_flipping(&DpRxSsInst_1);
#endif

	/* Initializing Interrupts */
	/* Setup interrupt handling */
	Status = DpPt_SetupIntrSystem();
	if (Status != XST_SUCCESS) {
		xil_printf("ERR:Interrupt set up failed!\n\r");
		return XST_FAILURE;
	}

	if (use_monitor_edid)
	{
		/* This is EDID pass-through mode. */
		xil_printf("Setting same EDID contents in DP RX..\r\n");
#if XPAR_XVPHY_NUM_INSTANCES >= 1
		update_edid(XPAR_DP_RX_HIER_0_VID_EDID_0_BASEADDR);
#endif
#if XPAR_XVPHY_NUM_INSTANCES >= 2
		update_edid(XPAR_DP_RX_HIER_1_VID_EDID_0_BASEADDR);
#endif
	}
	else
	{
		/* This is None-EDID pass-through mode */
		/* setting default Xilinx EDID which supports 8K30, */
#if XPAR_XVPHY_NUM_INSTANCES >= 1
//		edid_default(XPAR_DP_RX_HIER_0_VID_EDID_0_BASEADDR);
		edid_change(3,XPAR_DP_RX_HIER_0_VID_EDID_0_BASEADDR);
//		edid_monitor_songren(XPAR_DP_RX_HIER_0_VID_EDID_0_BASEADDR);
//		edid_monitor_linsu_screen(XPAR_DP_RX_HIER_0_VID_EDID_0_BASEADDR);
#endif
#if XPAR_XVPHY_NUM_INSTANCES >= 2
//		edid_default(XPAR_DP_RX_HIER_1_VID_EDID_0_BASEADDR);
		edid_change(3,XPAR_DP_RX_HIER_1_VID_EDID_0_BASEADDR);
//		edid_monitor_songren(XPAR_DP_RX_HIER_1_VID_EDID_0_BASEADDR);
//		edid_monitor_linsu_screen(XPAR_DP_RX_HIER_1_VID_EDID_0_BASEADDR);
#endif
	}


	xil_printf("System capabilities set to: LineRate %x, LaneCount %x\r\n", LineRate_init, LaneCount_init);
	prev_line_rate=LineRate_init;

#if XPAR_XVPHY_NUM_INSTANCES >= 1
	/* Setting RX link to disabled state. This is to ensure
	 * that Source gets enough time to authenticate and do the
	 * HDCP stuff (such as writing AKSVs)
	 * */
	XDp_WriteReg(DpRxSsInst_0.DpPtr->Config.BaseAddr, XDP_RX_LINK_ENABLE, 0x0);
	/* Programming the unplug time register of DP RX
	 * for long value else the cable unplug events
	 * come very frequently
	 * */
	XDp_WriteReg(DpRxSsInst_0.DpPtr->Config.BaseAddr, XDP_RX_BS_IDLE_TIME, 0x047868C0);
	XDp_WriteReg(DpRxSsInst_0.DpPtr->Config.BaseAddr, XDP_RX_INTERRUPT_MASK, 0xFFF87FFD);


	dp_conf.LineRate  = LineRate_init_rx_0;
	dp_conf.LaneCount = XDP_DPCD_LANE_COUNT_SET_4;
	training_done = 0;
	start_tracking = 0;

	XDpRxSs_SetLinkRate(&DpRxSsInst_0, LineRate_init_rx_0);
	XDpRxSs_SetLaneCount(&DpRxSsInst_0, XDP_DPCD_LANE_COUNT_SET_4);
	XDpRxSs_Start(&DpRxSsInst_0);


	xil_printf("RX Link & Lane Capability is set to %x, %x\r\n",
		(XDp_ReadReg(DpRxSsInst_0.DpPtr->Config.BaseAddr ,
				XDP_RX_DPCD_LINK_BW_SET)),
		(XDp_ReadReg(DpRxSsInst_0.DpPtr->Config.BaseAddr ,
				XDP_RX_DPCD_LANE_COUNT_SET)));

	switch (LineRate_init_rx_0) {
	case 0x6:
		Status = PHY_Configuration_Rx(&rx_VPhy_Instance_0,
				PHY_User_Config_Table[3]);
		break;

	case 0xA:
		Status = PHY_Configuration_Rx(&rx_VPhy_Instance_0,
				PHY_User_Config_Table[4]);
		break;

	case 0x14:
		Status = PHY_Configuration_Rx(&rx_VPhy_Instance_0,
				PHY_User_Config_Table[5]);
		break;

	case 0x1E:
		Status = PHY_Configuration_Rx(&rx_VPhy_Instance_0,
				PHY_User_Config_Table[10]);
		break;
	}

	if (Status != XST_SUCCESS) {
		xil_printf("+++++++ RX GT configuration "
			   "encountered a failure +++++++\r\n");
	}

	XIntc_Enable(&InterruptController, XPAR_INTC_0_DP14RXSS_0_VEC_ID);

	xil_printf("Please plug in RX cable "
		   "to initiate training...\r\n");
	XDp_WriteReg(DpRxSsInst_0.DpPtr->Config.BaseAddr,
			XDP_RX_LINK_ENABLE, 0x1);
	XDp_WriteReg(DpRxSsInst_0.DpPtr->Config.BaseAddr,
			XDP_RX_AUDIO_CONTROL, 0x0);

	usleep(20*1000);

	/* Toggle HPD once */
	XDp_WriteReg(DpRxSsInst_0.DpPtr->Config.BaseAddr,
			XDP_RX_HPD_INTERRUPT, 0x0BB80001);
#endif

#if XPAR_XVPHY_NUM_INSTANCES >= 2
	/* Setting RX link to disabled state. This is to ensure
	 * that Source gets enough time to authenticate and do the
	 * HDCP stuff (such as writing AKSVs)
	 * */
	XDp_WriteReg(DpRxSsInst_1.DpPtr->Config.BaseAddr, XDP_RX_LINK_ENABLE, 0x0);
	/* Programming the unplug time register of DP RX
	 * for long value else the cable unplug events
	 * come very frequently
	 * */
	XDp_WriteReg(DpRxSsInst_1.DpPtr->Config.BaseAddr, XDP_RX_BS_IDLE_TIME, 0x047868C0);
	XDp_WriteReg(DpRxSsInst_1.DpPtr->Config.BaseAddr, XDP_RX_INTERRUPT_MASK, 0xFFF87FFD);


	dp_conf.LineRate  = LineRate_init_rx_1;
	dp_conf.LaneCount = XDP_DPCD_LANE_COUNT_SET_4;
	training_done = 0;
	start_tracking = 0;

	XDpRxSs_SetLinkRate(&DpRxSsInst_1, LineRate_init_rx_1);
	XDpRxSs_SetLaneCount(&DpRxSsInst_1, XDP_DPCD_LANE_COUNT_SET_4);
	XDpRxSs_Start(&DpRxSsInst_1);


	xil_printf("RX Link & Lane Capability is set to %x, %x\r\n",
		(XDp_ReadReg(DpRxSsInst_1.DpPtr->Config.BaseAddr ,
				XDP_RX_DPCD_LINK_BW_SET)),
		(XDp_ReadReg(DpRxSsInst_1.DpPtr->Config.BaseAddr ,
				XDP_RX_DPCD_LANE_COUNT_SET)));

	switch (LineRate_init_rx_1) {
	case 0x6:
		Status = PHY_Configuration_Rx(&rx_VPhy_Instance_1,
				PHY_User_Config_Table[3]);
		break;

	case 0xA:
		Status = PHY_Configuration_Rx(&rx_VPhy_Instance_1,
				PHY_User_Config_Table[4]);
		break;

	case 0x14:
		Status = PHY_Configuration_Rx(&rx_VPhy_Instance_1,
				PHY_User_Config_Table[5]);
		break;

	case 0x1E:
		Status = PHY_Configuration_Rx(&rx_VPhy_Instance_1,
				PHY_User_Config_Table[10]);
		break;
	}

	if (Status != XST_SUCCESS) {
		xil_printf("+++++++ RX GT configuration "
			   "encountered a failure +++++++\r\n");
	}

	XIntc_Enable(&InterruptController, XPAR_INTC_0_DP14RXSS_1_VEC_ID);

	xil_printf("Please plug in RX cable "
		   "to initiate training...\r\n");
	XDp_WriteReg(DpRxSsInst_1.DpPtr->Config.BaseAddr,
			XDP_RX_LINK_ENABLE, 0x1);
	XDp_WriteReg(DpRxSsInst_1.DpPtr->Config.BaseAddr,
			XDP_RX_AUDIO_CONTROL, 0x0);

	usleep(20*1000);

	/* Toggle HPD once */
	XDp_WriteReg(DpRxSsInst_1.DpPtr->Config.BaseAddr,
			XDP_RX_HPD_INTERRUPT, 0x0BB80001);
#endif


}


#endif
