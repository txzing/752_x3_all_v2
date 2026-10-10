#ifndef __PS_GPIO_H__

#if defined (XPAR_XGPIOPS_NUM_INSTANCES)
#define __PS_GPIO_H__
#include "xgpiops.h"

#define RELAY_CUTOFF        (78+0)
#define PL_LOCK_LED			(78+1)
#define EEPROM_WP			(78+2)

#define CHIPMAX96752_1_GPIO2			(78+3)
#define CHIPMAX96752_1_GPIO4			(78+4)
#define CHIPMAX96752_2_GPIO2			(78+5)
#define CHIPMAX96752_2_GPIO4			(78+6)
#define CHIPMAX96752_3_GPIO2			(78+7)
#define CHIPMAX96752_3_GPIO4			(78+8)


extern XGpioPs Gpio;

int PsGpioSetup(XGpioPs *InstancePtr, u16 DeviceId);

#endif // XPAR_XGPIOPS_NUM_INSTANCES

#endif // __PS_GPIO_H__

