/******************************************************************************
 * @file axis_pixel_compare.c
 * @brief Driver helper implementations.
 *
 * Version: 2.5 (must match spirit:version in component.xml and OPTION VERSION in
 *          drivers/axis_pixel_compare_v1_0/data/axis_pixel_compare.mdd)
 * Revision date: 2026-05-13
 *
 * Revision highlights:
 * - XAxisPixelCompare_LookupConfig() walks XAxisPixelCompare_ConfigTable[]
 *   using XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES.
 *****************************************************************************/

/***************************** Include Files *******************************/
#include "axis_pixel_compare.h"

/************************** Function Definitions ***************************/

const XAxisPixelCompare_Config *XAxisPixelCompare_LookupConfig(u16 DeviceId)
{
#if defined(XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES)
	u32 i;

	for (i = 0U; i < (u32)XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES; i++) {
		if (XAxisPixelCompare_ConfigTable[i].DeviceId == DeviceId) {
			return &XAxisPixelCompare_ConfigTable[i];
		}
	}
#endif
	(void)DeviceId;
	return NULL;
}
