#ifndef __CSI_TX_H__

//#include "xparameters.h"
#if defined (XPAR_XCSI2TX_NUM_INSTANCES)
#define __CSI_TX_H__
#include "xcsi2txss.h"

#if (XPAR_XCSI2TX_NUM_INSTANCES >= 1U)
extern XCsi2TxSs Csi2TxSsInst_0;
#endif
#if (XPAR_XCSI2TX_NUM_INSTANCES >= 2U)
extern XCsi2TxSs Csi2TxSsInst_1;
#endif
#if (XPAR_XCSI2TX_NUM_INSTANCES >= 3U)
extern XCsi2TxSs Csi2TxSsInst_2;
#endif
#if (XPAR_XCSI2TX_NUM_INSTANCES >= 4U)
extern XCsi2TxSs Csi2TxSsInst_3;
#endif


#if (XPAR_XCSI2TX_NUM_INSTANCES >= 1U)
u32 Csi2TxSs_Init_0(u32 DeviceId);
#endif
#if (XPAR_XCSI2TX_NUM_INSTANCES >= 2U)
u32 Csi2TxSs_Init_1(u32 DeviceId);
#endif
#if (XPAR_XCSI2TX_NUM_INSTANCES >= 3U)
u32 Csi2TxSs_Init_2(u32 DeviceId);
#endif
#if (XPAR_XCSI2TX_NUM_INSTANCES >= 4U)
u32 Csi2TxSs_Init_3(u32 DeviceId);
#endif



#endif // XPAR_XCSI2TX_NUM_INSTANCES

#endif // __CSI_TX_H__

