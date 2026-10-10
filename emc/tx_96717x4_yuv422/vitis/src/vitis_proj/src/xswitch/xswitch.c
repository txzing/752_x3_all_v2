#include "../bsp.h"

#if defined (XPAR_XAXIS_SWITCH_NUM_INSTANCES)

static XAxis_Switch AxisSwitchCsi3;
static u8 s_csi3_inited;
static u8 s_csi3_si = 0xFFU;

static int axis_switch_program(u8 SiIndex)
{
	XAxis_Switch_Config *Config;
	int Status;

	Config = XAxisScr_LookupConfig(AXIS_SWITCH_CSI3_DEVICE_ID);
	if (Config == NULL)
	{
		return XST_FAILURE;
	}

	if (s_csi3_inited == 0U)
	{
		Status = XAxisScr_CfgInitialize(&AxisSwitchCsi3, Config,
						Config->BaseAddress);
		if (Status != XST_SUCCESS)
		{
			bsp_printf(TXT_RED "AXI4-Stream Switch init failed\r\n"
				   TXT_RST);
			return XST_FAILURE;
		}
	}

	XAxisScr_RegUpdateDisable(&AxisSwitchCsi3);
	XAxisScr_MiPortDisableAll(&AxisSwitchCsi3);
	XAxisScr_MiPortEnable(&AxisSwitchCsi3, AXIS_SWITCH_CSI3_MI, SiIndex);
	XAxisScr_RegUpdateEnable(&AxisSwitchCsi3);

	if (XAxisScr_IsMiPortEnabled(&AxisSwitchCsi3, AXIS_SWITCH_CSI3_MI,
				     SiIndex) == 0)
	{
		return XST_FAILURE;
	}
	s_csi3_inited = 1U;
	s_csi3_si = SiIndex;
	return XST_SUCCESS;
}

int axis_switch_cfg(void)
{
#if defined (AXIS_SWITCH_CSI3_DEVICE_ID)
	/* Boot PathA: EEPROM often dual same-res merge. */
	if (axis_switch_program(AXIS_SWITCH_CSI3_SI_PATHA) != XST_SUCCESS)
	{
		bsp_printf(TXT_RED "In %s: CSI3 AxisSwitch failed\r\n" TXT_RST,
			   __func__);
		return XST_FAILURE;
	}
	xil_printf("axis_switch: CSI3 <- PathA (boot)\r\n");
	return XST_SUCCESS;
#else
	return XST_SUCCESS;
#endif
}

int axis_switch_csi3_select(u8 same_wh_fps)
{
#if defined (AXIS_SWITCH_CSI3_DEVICE_ID)
	u8 si = (same_wh_fps != 0U) ? AXIS_SWITCH_CSI3_SI_PATHA :
				      AXIS_SWITCH_CSI3_SI_PATHB;

	if ((s_csi3_inited != 0U) && (si == s_csi3_si))
	{
		return XST_SUCCESS;
	}

	if (axis_switch_program(si) != XST_SUCCESS)
	{
		xil_printf("axis_switch: CSI3 select SI%u failed\r\n",
			   (unsigned)si);
		return XST_FAILURE;
	}

	xil_printf("axis_switch: CSI3 <- %s (SI%u)\r\n",
		   (si == AXIS_SWITCH_CSI3_SI_PATHA) ? "PathA" : "PathB",
		   (unsigned)si);
	return XST_SUCCESS;
#else
	(void)same_wh_fps;
	return XST_SUCCESS;
#endif
}

#endif /* XPAR_XAXIS_SWITCH_NUM_INSTANCES */
