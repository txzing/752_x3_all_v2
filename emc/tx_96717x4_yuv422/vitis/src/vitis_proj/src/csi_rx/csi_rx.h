#ifndef __CSI_RX_H__

//#include "xparameters.h"
#if defined (XPAR_XCSISS_NUM_INSTANCES)
#define __CSI_RX_H__
//#include "xcsi.h"
#include "xcsiss.h"

#if (XPAR_XCSISS_NUM_INSTANCES >= 1U)
extern XCsiSs Csi2RxSsInst_0;
#endif
#if (XPAR_XCSISS_NUM_INSTANCES >= 2U)
extern XCsiSs Csi2RxSsInst_1;
#endif
#if (XPAR_XCSISS_NUM_INSTANCES >= 3U)
extern XCsiSs Csi2RxSsInst_2;
#endif
#if (XPAR_XCSISS_NUM_INSTANCES >= 4U)
extern XCsiSs Csi2RxSsInst_3;
#endif

int Csi2RxSs_Init(XCsiSs *InstancePtr, u32 DeviceId);
int csi_rx_config(void);
void print_csi_rx_reg_info(XCsiSs *InstancePtr, u8 verbose);
void print_csi_rx_reg_inf(UINTPTR Addr);

#endif // XPAR_XCSI_NUM_INSTANCES

#endif // __CSI_RX_H__

