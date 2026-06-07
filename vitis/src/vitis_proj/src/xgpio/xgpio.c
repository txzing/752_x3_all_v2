#include "../bsp.h"

#if (XPAR_XGPIO_NUM_INSTANCES >= 2U)
#if 1

XGpio Axi_Gpio; /* The Instance of the GPIO Driver */
XGpio XGpioOutput_oldi;

int xgpio_setup(void)
{
	int Status ;

//	/* Initialize the GPIO driver */
//	Status = XGpio_Initialize(&Axi_Gpio, XPAR_AXI_GPIO_LVDS_SEL_DEVICE_ID);
//	if (Status != XST_SUCCESS) {
//		xil_printf("Gpio Initialization Failed\r\n");
//		return XST_FAILURE;
//	}
//	XGpio_SetDataDirection(&Axi_Gpio, 1, 0x0);
//	XGpio_DiscreteWrite(&Axi_Gpio, 1, 0x0);


	/* Initialize the GPIO driver */
	Status = XGpio_Initialize(&XGpioOutput_oldi, XPAR_AXI_GPIO_LVDS_RESET_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		xil_printf("Gpio Initialization Failed\r\n");
		return XST_FAILURE;
	}
	XGpio_SetDataDirection(&XGpioOutput_oldi, 1, 0x0);
	XGpio_DiscreteWrite(&XGpioOutput_oldi, 1, 0x0);
	usleep(50*1000);
	XGpio_DiscreteWrite(&XGpioOutput_oldi, 1, 0xf);
	usleep(50*1000);
	XGpio_DiscreteWrite(&XGpioOutput_oldi, 1, 0x0);
	return Status ;
}

#endif

#endif
