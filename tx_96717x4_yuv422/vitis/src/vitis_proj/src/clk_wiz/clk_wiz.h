#ifndef __CLK_WIZ_H__

#if defined (XPAR_XCLK_WIZ_NUM_INSTANCES)
#define __CLK_WIZ_H__
#include "xil_types.h"
#include "xclk_wiz.h"

#define CLK_LOCK	(1)

#if (XPAR_XCLK_WIZ_NUM_INSTANCES >= 1U)
extern XClk_Wiz ClkWiz_Dynamic0;
#endif

int clkwiz_set_dual_out_khz(u32 clk0_khz, u32 clk1_khz);
/*
 * Reprogram only when either rate differs from last applied values.
 * *did_reconfig: 1 if MMCM load ran (peer may glitch), 0 if skipped.
 */
int clkwiz_set_dual_out_khz_if_changed(u32 clk0_khz, u32 clk1_khz,
				       u8 *did_reconfig);
/* Pick achievable pclk (kHz): prefer >= want, within max_khz. */
u32 clkwiz_pick_pclk_khz(u32 want_khz, u32 max_khz);

#if defined (XPAR_XVTC_NUM_INSTANCES)
/* Re-bind clk_wiz + both VTC after PL reset; clears applied-rate shadow. */
int clkwiz_vtc_cfg(void);
#endif

#endif /* XPAR_XCLK_WIZ_NUM_INSTANCES */

#endif /* __CLK_WIZ_H__ */
