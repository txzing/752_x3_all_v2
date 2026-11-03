#ifndef __XSWITCH_H__
//#include "xparameters.h"
#if defined (XPAR_XAXIS_SWITCH_NUM_INSTANCES)
#define __XSWITCH_H__

#include "xaxis_switch.h"

#if (XPAR_XAXIS_SWITCH_NUM_INSTANCES >= 1U)
extern XAxis_Switch AxisSwitch0;
#endif
#if (XPAR_XAXIS_SWITCH_NUM_INSTANCES >= 2U)
extern XAxis_Switch AxisSwitch1;
#endif
#if (XPAR_XAXIS_SWITCH_NUM_INSTANCES >= 3U)
extern XAxis_Switch AxisSwitch2;
#endif
#if (XPAR_XAXIS_SWITCH_NUM_INSTANCES >= 4U)
extern XAxis_Switch AxisSwitch3;
#endif
#if (XPAR_XAXIS_SWITCH_NUM_INSTANCES >= 5U)
extern XAxis_Switch AxisSwitch4;
#endif
#if (XPAR_XAXIS_SWITCH_NUM_INSTANCES >= 6U)
extern XAxis_Switch AxisSwitch5;
#endif
#if (XPAR_XAXIS_SWITCH_NUM_INSTANCES >= 7U)
extern XAxis_Switch AxisSwitch6;
#endif
#if (XPAR_XAXIS_SWITCH_NUM_INSTANCES >= 8U)
extern XAxis_Switch AxisSwitch7;
#endif

int AxisSwitch(u16 DeviceId, XAxis_Switch * pAxisSwitch, u8 SiIndex, u8 MiIndex);
int axis_switch_cfg(void);

#endif // XPAR_XAXIS_SWITCH_NUM_INSTANCES

#endif // __XSWITCH_H__


