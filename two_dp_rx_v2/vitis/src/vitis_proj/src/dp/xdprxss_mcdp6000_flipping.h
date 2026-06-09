#ifndef XDPRXSS_MCDP6000_FLIPPING_H_
#define XDPRXSS_MCDP6000_FLIPPING_H_
#include "xdprxss_mcdp6000.h"

#ifdef __cplusplus
extern "C" {
#endif

void XDpRxSs_McDp6000_init_flipping(void *InstancePtr);
int XDpRxSs_MCDP6000_DpInit_flipping(XDpRxSs *DpRxSsPtr, u8 I2CSlaveAddress);
int XDpRxSs_MCDP6000_ResetDpPath_flipping(XDpRxSs *DpRxSsPtr, u8 I2CSlaveAddress);

#ifdef __cplusplus
}
#endif

#endif // XDPRXSS_MCDP6000_FLIPPING_H_
