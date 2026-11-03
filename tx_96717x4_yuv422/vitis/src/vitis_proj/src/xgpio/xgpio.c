#include "../bsp.h"

#if (XPAR_XGPIO_NUM_INSTANCES >= 2U)
#if 0

XGpio Axi_Gpio; /* The Instance of the GPIO Driver */


int xgpio_setup(void)
{
	int Status ;

	/* Initialize the GPIO driver */
	Status = XGpio_Initialize(&Axi_Gpio, XPAR_PROCESSOR_SUBSYSTEM_AXI_GPIO_0_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		xil_printf("Gpio Initialization Failed\r\n");
		return XST_FAILURE;
	}
	XGpio_SetDataDirection(&Axi_Gpio, 1, 0x0);
	XGpio_DiscreteWrite(&Axi_Gpio, 1, 0x0);

	return Status ;
}

#endif

#endif
