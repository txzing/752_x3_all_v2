#ifndef __XGPIO_H__

//#include "xparameters.h"
#if (XPAR_XGPIO_NUM_INSTANCES >= 2U)
#define __XGPIO_H__


extern XGpio Axi_Gpio; /* The Instance of the GPIO Driver */
extern XGpio fake_i2c_Gpio; /* The Instance of the GPIO Driver */
extern XGpio XGpioOutput_oldi; /* The Instance of the GPIO Driver */

int xgpio_setup(void);


#endif

#endif
