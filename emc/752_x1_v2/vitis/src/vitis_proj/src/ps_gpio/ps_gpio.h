#ifndef __PS_GPIO_H__

#if defined (XPAR_XGPIOPS_NUM_INSTANCES)
#define __PS_GPIO_H__
#include "xgpiops.h"

#define RELAY_CUTOFF        (78+0)
#define EEPROM_WP			(78+1)

#define SFP_TX_DIS			(78+8)
#define SFP_TX_LOSS			(78+9)



extern XGpioPs Gpio;

int PsGpioSetup(XGpioPs *InstancePtr, u16 DeviceId);

#endif // XPAR_XGPIOPS_NUM_INSTANCES

#endif // __PS_GPIO_H__

