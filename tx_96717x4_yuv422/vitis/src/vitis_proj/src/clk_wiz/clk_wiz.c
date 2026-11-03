#include "../bsp.h"

#if defined (XPAR_XCLK_WIZ_NUM_INSTANCES)

#if (XPAR_XCLK_WIZ_NUM_INSTANCES >= 1U)
XClk_Wiz ClkWiz_Dynamic0;
#endif

#define CLK_LOCK	(1)

static int Wait_For_Lock(XClk_Wiz_Config *CfgPtr_Dynamic)
{
	u32 Count = 0;

	while (!(*(volatile u32 *)(CfgPtr_Dynamic->BaseAddr + 0x04) & CLK_LOCK)) {
		if (Count >= 2000U) {
			return 1;
		}
		usleep(100);
		Count++;
	}
	return 0;
}

static u32 s_clkwiz_applied0_khz;
static u32 s_clkwiz_applied1_khz;
static u8 s_clkwiz_have_applied;

/* MMCM: 200MHz * 37.125 / 5 = 1485 MHz VCO. Same as bitstream. */
#define CLKWIZ_MULT_INT   37U
#define CLKWIZ_MULT_FRAC  125U
#define CLKWIZ_DIVCLK     5U
#define CLKWIZ_VCO_KHZ    1485000U

static u32 clkwiz_rd(XClk_Wiz_Config *cfg, u32 off)
{
	return *(volatile u32 *)(cfg->BaseAddr + off);
}

static void clkwiz_wr(XClk_Wiz_Config *cfg, u32 off, u32 val)
{
	*(volatile u32 *)(cfg->BaseAddr + off) = val;
}

static int clkwiz_wait_unlock(XClk_Wiz_Config *cfg)
{
	u32 i;

	for (i = 0U; i < 200U; i++)
	{
		if ((clkwiz_rd(cfg, 0x04U) & CLK_LOCK) == 0U)
		{
			return XST_SUCCESS;
		}
		usleep(50);
	}
	return XST_FAILURE;
}

static void clkwiz_program_divs(XClk_Wiz_Config *cfg, u32 div0, u32 div1)
{
	clkwiz_wr(cfg, 0x200U,
		  (CLKWIZ_MULT_FRAC << 16) | (CLKWIZ_MULT_INT << 8) | CLKWIZ_DIVCLK);
	clkwiz_wr(cfg, 0x204U, 0U);
	clkwiz_wr(cfg, 0x208U, div0);
	clkwiz_wr(cfg, 0x20CU, 0U);
	clkwiz_wr(cfg, 0x214U, div1);
	clkwiz_wr(cfg, 0x218U, 0U);
	clkwiz_wr(cfg, 0x220U, 0U);
	clkwiz_wr(cfg, 0x224U, 0U);
	clkwiz_wr(cfg, 0x22CU, 0U);
	clkwiz_wr(cfg, 0x230U, 0U);
}

static int clkwiz_load_mmcm(XClk_Wiz_Config *cfg)
{
	clkwiz_wr(cfg, 0x25CU, 0x07U);
	(void)clkwiz_wait_unlock(cfg);
	usleep(20);
	clkwiz_wr(cfg, 0x25CU, 0x02U);
	if (Wait_For_Lock(cfg) != 0)
	{
		return XST_FAILURE;
	}
	return XST_SUCCESS;
}

static int clkwiz_divs_hw_match(XClk_Wiz_Config *cfg, u32 div0, u32 div1,
				u32 *hw0, u32 *hw1)
{
	u32 r0 = clkwiz_rd(cfg, 0x208U) & 0xFFU;
	u32 r1 = clkwiz_rd(cfg, 0x214U) & 0xFFU;

	if (hw0 != NULL)
	{
		*hw0 = r0;
	}
	if (hw1 != NULL)
	{
		*hw1 = r1;
	}
	return ((r0 == (div0 & 0xFFU)) && (r1 == (div1 & 0xFFU))) ? 1 : 0;
}

int clkwiz_set_dual_out_khz(u32 clk0_khz, u32 clk1_khz)
{
	XClk_Wiz_Config *CfgPtr_Dynamic;
	u32 div0;
	u32 div1;
	u32 hw0 = 0U;
	u32 hw1 = 0U;
	int Status;
	int matched;

	if ((clk0_khz == 0U) || (clk1_khz == 0U))
	{
		return XST_FAILURE;
	}

	CfgPtr_Dynamic = XClk_Wiz_LookupConfig(XPAR_CLK_WIZ_0_DEVICE_ID);
	if (CfgPtr_Dynamic == NULL)
	{
		return XST_FAILURE;
	}

	Status = XClk_Wiz_CfgInitialize(&ClkWiz_Dynamic0, CfgPtr_Dynamic,
					CfgPtr_Dynamic->BaseAddr);
	if (Status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}

	div0 = CLKWIZ_VCO_KHZ / clk0_khz;
	div1 = CLKWIZ_VCO_KHZ / clk1_khz;
	if ((div0 == 0U) || (div1 == 0U) || (div0 > 255U) || (div1 > 255U) ||
	    ((CLKWIZ_VCO_KHZ / div0) != clk0_khz) ||
	    ((CLKWIZ_VCO_KHZ / div1) != clk1_khz))
	{
		xil_printf("clkwiz_set_dual_out_khz: cannot hit %u/%u kHz\r\n",
			   clk0_khz, clk1_khz);
		return XST_FAILURE;
	}

	clkwiz_program_divs(CfgPtr_Dynamic, div0, div1);
	matched = 0;
	if (clkwiz_load_mmcm(CfgPtr_Dynamic) == XST_SUCCESS)
	{
		matched = clkwiz_divs_hw_match(CfgPtr_Dynamic, div0, div1,
					       &hw0, &hw1);
	}

	if (matched == 0)
	{
		xil_printf("clkwiz: load miss shadow div=%u/%u, retry reset\r\n",
			   hw0, hw1);
		clkwiz_wr(CfgPtr_Dynamic, 0x00U, 0xAU);
		usleep(100);
		(void)Wait_For_Lock(CfgPtr_Dynamic);
		clkwiz_program_divs(CfgPtr_Dynamic, div0, div1);
		if (clkwiz_load_mmcm(CfgPtr_Dynamic) != XST_SUCCESS)
		{
			xil_printf("clkwiz_set_dual_out_khz: not locked\r\n");
			return XST_FAILURE;
		}
		(void)clkwiz_divs_hw_match(CfgPtr_Dynamic, div0, div1,
					   &hw0, &hw1);
	}

	if (Wait_For_Lock(CfgPtr_Dynamic) != 0)
	{
		xil_printf("clkwiz_set_dual_out_khz: lock lost after apply\r\n");
		return XST_FAILURE;
	}
	s_clkwiz_applied0_khz = clk0_khz;
	s_clkwiz_applied1_khz = clk1_khz;
	s_clkwiz_have_applied = 1U;
	xil_printf("clkwiz: out1=%u kHz div=%u  out2=%u kHz div=%u (hw %u/%u)\r\n",
		   clk0_khz, div0, clk1_khz, div1, hw0, hw1);
	return XST_SUCCESS;
}

int clkwiz_set_dual_out_khz_if_changed(u32 clk0_khz, u32 clk1_khz,
				       u8 *did_reconfig)
{
	if (did_reconfig != NULL)
	{
		*did_reconfig = 0U;
	}
	if ((s_clkwiz_have_applied != 0U) &&
	    (clk0_khz == s_clkwiz_applied0_khz) &&
	    (clk1_khz == s_clkwiz_applied1_khz))
	{
		return XST_SUCCESS;
	}
	if (did_reconfig != NULL)
	{
		*did_reconfig = 1U;
	}
	return clkwiz_set_dual_out_khz(clk0_khz, clk1_khz);
}

u32 clkwiz_pick_pclk_khz(u32 want_khz, u32 max_khz)
{
	const u32 vco_khz = 1485000U;
	u32 div;
	u32 clk;
	u32 best_ceil = 0U;
	u32 best_floor = 0U;
	u32 best_near = 0U;
	u32 best_err = 0xFFFFFFFFU;
	u32 err;

	if ((want_khz == 0U) || (max_khz == 0U))
	{
		return 0U;
	}

	for (div = 1U; div <= 255U; div++)
	{
		clk = vco_khz / div;
		if (clk == 0U)
		{
			continue;
		}
		if ((clk >= want_khz) &&
		    ((best_ceil == 0U) || (clk < best_ceil)))
		{
			best_ceil = clk;
		}
		if ((clk <= want_khz) && (clk > best_floor))
		{
			best_floor = clk;
		}
		if (clk <= max_khz)
		{
			err = (clk > want_khz) ? (clk - want_khz) :
						(want_khz - clk);
			if (err < best_err)
			{
				best_err = err;
				best_near = clk;
			}
		}
	}

	if ((best_ceil != 0U) && (best_ceil <= max_khz))
	{
		return best_ceil;
	}
	if (best_near != 0U)
	{
		return best_near;
	}
	if ((best_floor != 0U) && (best_floor <= max_khz))
	{
		return best_floor;
	}
	return 0U;
}

#if defined (XPAR_XVTC_NUM_INSTANCES)
int clkwiz_vtc_cfg(void)
{
	u32 Status;
	XClk_Wiz_Config *CfgPtr;
	XVtc_Config *VtcConfig0;
	XVtc_Config *VtcConfig1;

	CfgPtr = XClk_Wiz_LookupConfig(XPAR_CLK_WIZ_0_DEVICE_ID);
	if (CfgPtr == NULL)
	{
		xil_printf("XClk_Wiz0 lookup failed.\r\n");
		return XST_FAILURE;
	}
	Status = XClk_Wiz_CfgInitialize(&ClkWiz_Dynamic0, CfgPtr, CfgPtr->BaseAddr);
	if (Status != XST_SUCCESS)
	{
		xil_printf("XClk_Wiz0 init failed.\r\n");
		return XST_FAILURE;
	}
	if (Wait_For_Lock(CfgPtr) != 0)
	{
		xil_printf("XClk_Wiz0 not locked at bitstream default.\r\n");
		return XST_FAILURE;
	}
	s_clkwiz_have_applied = 0U;
	xil_printf("XClk_Wiz0 init ok (rate set by inject_mode)\r\n");

	VtcConfig0 = XVtc_LookupConfig(XPAR_VTC_0_DEVICE_ID);
	Status = XVtc_CfgInitialize(&VtcInst0, VtcConfig0, VtcConfig0->BaseAddress);
	if (Status != XST_SUCCESS)
	{
		xil_printf("VTC0 Initialization failed %d\r\n", Status);
		return XST_FAILURE;
	}
	xil_printf("VTC0 Initialization ok\r\n");

	VtcConfig1 = XVtc_LookupConfig(XPAR_VTC_1_DEVICE_ID);
	Status = XVtc_CfgInitialize(&VtcInst1, VtcConfig1, VtcConfig1->BaseAddress);
	if (Status != XST_SUCCESS)
	{
		xil_printf("VTC1 Initialization failed %d\r\n", Status);
		return XST_FAILURE;
	}
	xil_printf("VTC1 Initialization ok\r\n");
	xil_printf("clkwiz_vtc_cfg done (timing applied by inject_mode)\r\n");
	return XST_SUCCESS;
}
#endif

#endif /* XPAR_XCLK_WIZ_NUM_INSTANCES */
