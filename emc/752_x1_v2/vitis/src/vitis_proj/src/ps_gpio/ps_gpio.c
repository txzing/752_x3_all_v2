/*
 * ps_gpio.c
 *
 *  Created on: 2023Äê4ÔÂ28ÈÕ
 *      Author: fengke
 */

#include "../bsp.h"
#if defined (XPAR_XGPIOPS_NUM_INSTANCES)

XGpioPs Gpio;

int PsGpioSetup(XGpioPs *InstancePtr, u16 DeviceId)
{
	XGpioPs_Config *GPIO_CONFIG ;
	int Status ;

	GPIO_CONFIG = XGpioPs_LookupConfig(DeviceId) ;

	Status = XGpioPs_CfgInitialize(InstancePtr, GPIO_CONFIG, GPIO_CONFIG->BaseAddr) ;
	if (Status != XST_SUCCESS)
	{
		bsp_printf(TXT_RED "XGpioPs_CfgInitialize failed...\r\n" TXT_RST);
		return XST_FAILURE ;
	}
	/* set as output */

	XGpioPs_SetDirectionPin(InstancePtr, RELAY_CUTOFF, 1) ;
	XGpioPs_SetOutputEnablePin(InstancePtr, RELAY_CUTOFF, 1);
	XGpioPs_WritePin(&Gpio, RELAY_CUTOFF, 0) ; // RELAY_CUTOFF: 0-keep; 1-cutoff

	XGpioPs_SetDirectionPin(InstancePtr, EEPROM_WP, 1) ;
	XGpioPs_SetOutputEnablePin(InstancePtr, EEPROM_WP, 1);
	XGpioPs_WritePin(&Gpio, EEPROM_WP, 0) ; // EEPROM write protect off

	XGpioPs_SetDirectionPin(InstancePtr, SFP_TX_DIS, 1) ;
	XGpioPs_SetOutputEnablePin(InstancePtr, SFP_TX_DIS, 1);
	XGpioPs_WritePin(&Gpio, SFP_TX_DIS, 0) ; //

	return XST_SUCCESS ;
}

#endif // XPAR_XGPIOPS_NUM_INSTANCES

