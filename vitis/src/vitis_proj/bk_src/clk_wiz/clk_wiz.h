#ifndef __CLK_WIZ_H__

//#include "xparameters.h"
#if defined (XPAR_XCLK_WIZ_NUM_INSTANCES)
#define __CLK_WIZ_H__
#include "xil_types.h"
#include "xclk_wiz.h"

#define CLK_LOCK 	(1)
#define INPUT_FREQ_INT 200000 // 输入频率（单位：kHz，放大 1000 倍）


#if (XPAR_XCLK_WIZ_NUM_INSTANCES >= 1U)
extern XClk_Wiz ClkWiz_Dynamic0;
#endif
#if (XPAR_XCLK_WIZ_NUM_INSTANCES >= 2U)
extern XClk_Wiz ClkWiz_Dynamic1;
#endif
#if (XPAR_XCLK_WIZ_NUM_INSTANCES >= 3U)
extern XClk_Wiz ClkWiz_Dynamic3;
#endif


int Wait_For_Lock(XClk_Wiz_Config *CfgPtr_Dynamic);
int XClk_Wiz_dynamic_reconfig(XClk_Wiz * ClkWizInstance, u32 DeviceId);
int clkwiz_config(void);
int clkwiz_config_setrate(XClk_Wiz *ClkWizInstance, u32 DeviceId, float OutputFreq);
int clkwiz_config_setrate_multi(XClk_Wiz *ClkWizInstance, u32 DeviceId, float OutputFreqs[], int NumOutputs);

#endif // XPAR_XCLK_WIZ_NUM_INSTANCES

#endif // __CLK_WIZ_H__

