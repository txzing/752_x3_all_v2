#ifndef __PS_GPIO_H__

#if defined (XPAR_XGPIOPS_NUM_INSTANCES)
#define __PS_GPIO_H__
#include "xgpiops.h"

#define RELAY_CUTOFF        (78+0)
#define EEPROM_WP			(78+1)


#define CHIPMAX96752_1_GPIO2			(78+5)
#define CHIPMAX96752_1_GPIO4			(78+6)
#define CHIPMAX96752_2_GPIO2			(78+10)
#define CHIPMAX96752_2_GPIO4			(78+11)

#define SFP_TX_DIS			(78+12)
#define SFP_TX_LOSS			(78+13)

extern XGpioPs Gpio;

int PsGpioSetup(XGpioPs *InstancePtr, u16 DeviceId);

#endif // XPAR_XGPIOPS_NUM_INSTANCES

#endif // __PS_GPIO_H__

