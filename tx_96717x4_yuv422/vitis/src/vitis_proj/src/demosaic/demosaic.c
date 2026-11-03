#include "../bsp.h"
#if defined (XPAR_XV_DEMOSAIC_NUM_INSTANCES)

int demosaic_init(u16 deviceid,u16 bayer_phase,u32 Hsize,u32 Vsize) {
	XV_demosaic demosaic;
	XV_demosaic_Config *demosaic_config;

	if(bayer_phase >3)return XST_FAILURE;

	if ( (demosaic_config = XV_demosaic_LookupConfig(deviceid)) == NULL) {
		bsp_printf("XV_demosaic_LookupConfig() failed\r\n");
		return XST_FAILURE;
	}
	if (XV_demosaic_CfgInitialize(&demosaic, demosaic_config, demosaic_config->BaseAddress)
			!= XST_SUCCESS) {
		bsp_printf("XV_demosaic_CfgInitialize() failed\r\n");
		return XST_FAILURE;
	}

	XV_demosaic_Set_HwReg_width(&demosaic, Hsize);
	XV_demosaic_Set_HwReg_height(&demosaic, Vsize);
	XV_demosaic_Set_HwReg_bayer_phase(&demosaic, bayer_phase);

	if (!XV_demosaic_IsReady(&demosaic)) {
		bsp_printf("demosaic core not ready\r\n");
		return XST_FAILURE;
	}
	XV_demosaic_EnableAutoRestart(&demosaic);
	XV_demosaic_Start(&demosaic);

	bsp_printf("Demosiac module initialized\r\n");

	return XST_SUCCESS;
}

#endif // XPAR_XV_DEMOSAIC_NUM_INSTANCES
