#ifndef __XSWITCH_H__
#define __XSWITCH_H__

#include "xparameters.h"

#if defined (XPAR_XAXIS_SWITCH_NUM_INSTANCES)

#include "xaxis_switch.h"

/*
 * Top-level axis_switch_0 (CSI3 mux) - matches current BD:
 *   S00 = PathA (axis_broadcaster M03)
 *   S01 = PathB (st_1/m_axis)
 *   M00 = CSI3
 */
#if defined (XPAR_ST_1_AXIS_SWITCH_0_DEVICE_ID)
#define AXIS_SWITCH_CSI3_DEVICE_ID	XPAR_ST_1_AXIS_SWITCH_0_DEVICE_ID
#elif defined (XPAR_AXIS_SWITCH_0_DEVICE_ID)
#define AXIS_SWITCH_CSI3_DEVICE_ID	XPAR_AXIS_SWITCH_0_DEVICE_ID
#endif

#define AXIS_SWITCH_CSI3_SI_PATHA	0U
#define AXIS_SWITCH_CSI3_SI_PATHB	1U
#define AXIS_SWITCH_CSI3_MI		0U

int axis_switch_cfg(void);
/* same_wh_fps!=0 -> CSI3 from PathA; else CSI3 from PathB. */
int axis_switch_csi3_select(u8 same_wh_fps);

#endif /* XPAR_XAXIS_SWITCH_NUM_INSTANCES */

#endif /* __XSWITCH_H__ */
