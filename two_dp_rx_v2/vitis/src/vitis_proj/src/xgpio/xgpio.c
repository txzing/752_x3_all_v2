#include "../bsp.h"

#if (XPAR_XGPIO_NUM_INSTANCES >= 2U)

XGpio Axi_Gpio; /* The Instance of the GPIO Driver */
XGpio XGpioOutput_oldi;


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

    ret32 = XGpio_DiscreteRead(&Axi_Gpio, 1);
	CLB(ret32, BIT32(0));//RELAY_CUTOFF: 0-keep; 1-cutoff
	CLB(ret32, BIT32(1));//Turn off eeprom write protection
	/* 040_CarrierBoard(CCU)_V3: NL8620* (MAX96856 path) / NL7720* (MAX96772 U22) */
	CLB(ret32, BIT32(2));//862_F_DIS_REM
	CLB(ret32, BIT32(3));//862_F_MS
	CLB(ret32, BIT32(4));//862_LOCK
	STB(ret32, BIT32(5));//NL8620PDB (DP0 deserializer enable)
	CLB(ret32, BIT32(6));//772_F_MS
	CLB(ret32, BIT32(7));//772_LOCK
	STB(ret32, BIT32(8));//NL7720PDB (DP1 MAX96772 enable)
    XGpio_DiscreteWrite(&Axi_Gpio, 1, ret32);


	/* Initialize the GPIO driver */
	Status = XGpio_Initialize(&XGpioOutput_oldi, XPAR_AXI_GPIO_LVDS_RESET_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		xil_printf("Gpio Initialization Failed\r\n");
		return XST_FAILURE;
	}
	XGpio_SetDataDirection(&XGpioOutput_oldi, 1, 0x0);
	XGpio_DiscreteWrite(&XGpioOutput_oldi, 1, 0x0);
	usleep(10*1000);
	XGpio_DiscreteWrite(&XGpioOutput_oldi, 1, 0x3);
	usleep(10*1000);
	XGpio_DiscreteWrite(&XGpioOutput_oldi, 1, 0x0);

	return Status ;
}


#endif
