/******************************************************************************
 * @file axis_passthrough_monitor.c
 * @brief Driver helper implementations.
 *
 * Version: 1.1 (must match spirit:version in component.xml and OPTION VERSION in
 *          drivers/axis_passthrough_monitor_v1_0/data/axis_passthrough_monitor.mdd)
 * Revision date: 2026-05-13
 *
 * Revision highlights:
 * - XAxisPassthroughMonitor_LookupConfig() walks XAxisPassthroughMonitor_ConfigTable[]
 *   using XPAR_AXI_PASSTHROUGH_MONITOR_NUM_INSTANCES.
 *****************************************************************************/

/***************************** Include Files *******************************/
#include "axis_passthrough_monitor.h"

/************************** Function Definitions ***************************/

const XAxisPassthroughMonitor_Config *XAxisPassthroughMonitor_LookupConfig(u16 DeviceId)
{
#if defined(XPAR_AXI_PASSTHROUGH_MONITOR_NUM_INSTANCES)
	u32 i;

	for (i = 0U; i < (u32)XPAR_AXI_PASSTHROUGH_MONITOR_NUM_INSTANCES; i++) {
		if (XAxisPassthroughMonitor_ConfigTable[i].DeviceId == DeviceId) {
			return &XAxisPassthroughMonitor_ConfigTable[i];
		}
	}
#endif
	(void)DeviceId;
	return NULL;
}
