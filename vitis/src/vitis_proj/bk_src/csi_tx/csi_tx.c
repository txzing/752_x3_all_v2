#include "../bsp.h"
#if defined (XPAR_XCSI2TX_NUM_INSTANCES)

#if (XPAR_XCSI2TX_NUM_INSTANCES >= 1U)
XCsi2TxSs Csi2TxSsInst_0;
#endif
#if (XPAR_XCSI2TX_NUM_INSTANCES >= 2U)
XCsi2TxSs Csi2TxSsInst_1;
#endif
#if (XPAR_XCSI2TX_NUM_INSTANCES >= 3U)
XCsi2TxSs Csi2TxSsInst_2;
#endif
#if (XPAR_XCSI2TX_NUM_INSTANCES >= 4U)
XCsi2TxSs Csi2TxSsInst_3;
#endif

int XGpioSetup(XGpio *InstancePtr, u16 DeviceId)
{
	int Status ;

	Status = XGpio_Initialize(InstancePtr, DeviceId) ;
	if (Status != XST_SUCCESS)
	{
		bsp_printf(TXT_RED "\r\n__FILE__:%s, __LINE__:%d\r\n" TXT_RST,__FILE__, __LINE__);
		return XST_FAILURE ;
	}
	/* set as output */
    XGpio_SetDataDirection(InstancePtr, 1, 0x0);
    XGpio_SetDataDirection(InstancePtr, 2, 0x0);

	return XST_SUCCESS ;
}

#if (XPAR_XCSI2TX_NUM_INSTANCES >= 1U)
u32 Csi2TxSs_Init_0(u32 DeviceId)
{
	u32 Status;
	XCsi2TxSs_Config *ConfigPtr;

	XGpio XGpioOutput;//control csi tx dt&wc
	XGpioSetup(&XGpioOutput, XPAR_TPG_RGB888_AXI_GPIO_0_DEVICE_ID) ;
	XGpio_DiscreteWrite(&XGpioOutput, 1, 0x24); // RGB888
//	XGpio_DiscreteWrite(&XGpioOutput, 1, 0x2B); // RAW10
//    XGpio_DiscreteWrite(&XGpioOutput, 1, 0x1E); // YUV422_8bit
#if (defined R1080)
	XGpio_DiscreteWrite(&XGpioOutput, 2, 1920*24/8<<16); // WC RGB888
#else
	XGpio_DiscreteWrite(&XGpioOutput, 2, 3840*24/8<<16); // WC RGB888
#endif

	/* Obtain the device configuration for the MIPI CSI2 TX Subsystem */
	ConfigPtr = XCsi2TxSs_LookupConfig(DeviceId);
	if (!ConfigPtr)
	{
		bsp_printf(TXT_RED "\r\n__FILE__:%s, __LINE__:%d\r\n" TXT_RST,__FILE__, __LINE__);
		return XST_FAILURE;
	}

	ConfigPtr->DataType = 0x24; // RGB888
//	ConfigPtr->DataType = 0x2A; // RAW8
//	ConfigPtr->DataType = 0x2B; // RAW10
//	ConfigPtr->DataType = 0x2C; // RAW12
//	ConfigPtr->DataType = 0x1E; // YUV422
	/* Copy the device configuration into the Csi2TxSsInst's Config
	 * structure. */
	Status = XCsi2TxSs_CfgInitialize(&Csi2TxSsInst_0, ConfigPtr,
					ConfigPtr->BaseAddr);
	if (Status != XST_SUCCESS)
	{
		xil_printf("MIPI CSI2 TX SS config initialization failed.\n\r");
		return XST_FAILURE;
	}

	XCsi2TxSs_SetClkMode(&Csi2TxSsInst_0, 0);
	Status = XCsi2TxSs_Activate(&Csi2TxSsInst_0, XCSI2TX_ENABLE);

	return Status;
}

#endif

#if (XPAR_XCSI2TX_NUM_INSTANCES >= 2U)
u32 Csi2TxSs_Init_1(u32 DeviceId)
{
	u32 Status;
	XCsi2TxSs_Config *ConfigPtr;

	XGpio XGpioOutput;//control csi tx dt&wc
	XGpioSetup(&XGpioOutput, XPAR_TPG_YUV422_AXI_GPIO_0_DEVICE_ID) ;
//	XGpio_DiscreteWrite(&XGpioOutput, 1, 0x24); // RGB888
//	XGpio_DiscreteWrite(&XGpioOutput, 1, 0x2B); // RAW10
//	XGpio_DiscreteWrite(&XGpioOutput, 1, 0x2A); // RAW8
    XGpio_DiscreteWrite(&XGpioOutput, 1, 0x1E); // YUV422_8bit
#if (defined R1080)
//	XGpio_DiscreteWrite(&XGpioOutput, 2, 1920*24/8<<16); // WC RGB888
	XGpio_DiscreteWrite(&XGpioOutput, 2, 1920*16/8<<16); // WC YUV422_8bit
#else
	XGpio_DiscreteWrite(&XGpioOutput, 2, 3840*24/8<<16); // WC RGB888
#endif

	/* Obtain the device configuration for the MIPI CSI2 TX Subsystem */
	ConfigPtr = XCsi2TxSs_LookupConfig(DeviceId);
	if (!ConfigPtr)
	{
		bsp_printf(TXT_RED "\r\n__FILE__:%s, __LINE__:%d\r\n" TXT_RST,__FILE__, __LINE__);
		return XST_FAILURE;
	}

//	ConfigPtr->DataType = 0x24; // RGB888
//	ConfigPtr->DataType = 0x2A; // RAW8
//	ConfigPtr->DataType = 0x2B; // RAW10
//	ConfigPtr->DataType = 0x2C; // RAW12
	ConfigPtr->DataType = 0x1E; // YUV422
	/* Copy the device configuration into the Csi2TxSsInst's Config
	 * structure. */
	Status = XCsi2TxSs_CfgInitialize(&Csi2TxSsInst_1, ConfigPtr,
			ConfigPtr->BaseAddr);
	if (Status != XST_SUCCESS)
	{
		xil_printf("MIPI CSI2 TX SS config initialization failed.\n\r");
		return XST_FAILURE;
	}

	XCsi2TxSs_SetClkMode(&Csi2TxSsInst_1, 0);
	Status = XCsi2TxSs_Activate(&Csi2TxSsInst_1, XCSI2TX_ENABLE);

	return Status;
}

#endif

#if (XPAR_XCSI2TX_NUM_INSTANCES >= 3U)
u32 Csi2TxSs_Init_2(u32 DeviceId)
{
	u32 Status;
	XCsi2TxSs_Config *ConfigPtr;

	XGpio XGpioOutput;//control csi tx dt&wc
	XGpioSetup(&XGpioOutput, XPAR_TPG_RAW8_AXI_GPIO_0_DEVICE_ID) ;
//	XGpio_DiscreteWrite(&XGpioOutput, 1, 0x24); // RGB888
//	XGpio_DiscreteWrite(&XGpioOutput, 1, 0x2B); // RAW10
	XGpio_DiscreteWrite(&XGpioOutput, 1, 0x2A); // RAW8
//    XGpio_DiscreteWrite(&XGpioOutput, 1, 0x1E); // YUV422_8bit
#if (defined R1080)
//	XGpio_DiscreteWrite(&XGpioOutput, 2, 1920*24/8<<16); // WC RGB888
//	XGpio_DiscreteWrite(&XGpioOutput, 2, 1920*16/8<<16); // WC YUV422_8bit
	XGpio_DiscreteWrite(&XGpioOutput, 2, 1920*8/8<<16); // WC RAW8
#else
	XGpio_DiscreteWrite(&XGpioOutput, 2, 3840*24/8<<16); // WC RGB888
#endif

	/* Obtain the device configuration for the MIPI CSI2 TX Subsystem */
	ConfigPtr = XCsi2TxSs_LookupConfig(DeviceId);
	if (!ConfigPtr)
	{
		bsp_printf(TXT_RED "\r\n__FILE__:%s, __LINE__:%d\r\n" TXT_RST,__FILE__, __LINE__);
		return XST_FAILURE;
	}

//	ConfigPtr->DataType = 0x24; // RGB888
	ConfigPtr->DataType = 0x2A; // RAW8
//	ConfigPtr->DataType = 0x2B; // RAW10
//	ConfigPtr->DataType = 0x2C; // RAW12
//	ConfigPtr->DataType = 0x1E; // YUV422
	/* Copy the device configuration into the Csi2TxSsInst's Config
	 * structure. */
	Status = XCsi2TxSs_CfgInitialize(&Csi2TxSsInst_2, ConfigPtr,
			ConfigPtr->BaseAddr);
	if (Status != XST_SUCCESS)
	{
		xil_printf("MIPI CSI2 TX SS config initialization failed.\n\r");
		return XST_FAILURE;
	}

	XCsi2TxSs_SetClkMode(&Csi2TxSsInst_2, 0);
	Status = XCsi2TxSs_Activate(&Csi2TxSsInst_2, XCSI2TX_ENABLE);

	return Status;
}

#endif

#if (XPAR_XCSI2TX_NUM_INSTANCES >= 4U)
u32 Csi2TxSs_Init_3(u32 DeviceId)
{
	u32 Status;
	XCsi2TxSs_Config *ConfigPtr;

	XGpio XGpioOutput;//control csi tx dt&wc
	XGpioSetup(&XGpioOutput, XPAR_ST_3_AXI_GPIO_0_DEVICE_ID) ;
	XGpio_DiscreteWrite(&XGpioOutput, 1, 0x24); // RGB888
//	XGpio_DiscreteWrite(&XGpioOutput, 1, 0x2B); // RAW10
//    XGpio_DiscreteWrite(&XGpioOutput, 1, 0x1E); // YUV422_8bit
#if (defined R1080)
	XGpio_DiscreteWrite(&XGpioOutput, 2, 1920*24/8<<16); // WC RGB888
#else
	XGpio_DiscreteWrite(&XGpioOutput, 2, 3840*24/8<<16); // WC RGB888
#endif

	/* Obtain the device configuration for the MIPI CSI2 TX Subsystem */
	ConfigPtr = XCsi2TxSs_LookupConfig(DeviceId);
	if (!ConfigPtr)
	{
		bsp_printf(TXT_RED "\r\n__FILE__:%s, __LINE__:%d\r\n" TXT_RST,__FILE__, __LINE__);
		return XST_FAILURE;
	}

//	ConfigPtr->DataType = 0x24; // RGB888
//	ConfigPtr->DataType = 0x2A; // RAW8
//	ConfigPtr->DataType = 0x2B; // RAW10
	ConfigPtr->DataType = 0x2C; // RAW12
//	ConfigPtr->DataType = 0x1E; // YUV422
	/* Copy the device configuration into the Csi2TxSsInst's Config
	 * structure. */
	Status = XCsi2TxSs_CfgInitialize(&Csi2TxSsInst_3, ConfigPtr,
			ConfigPtr->BaseAddr);
	if (Status != XST_SUCCESS)
	{
		xil_printf("MIPI CSI2 TX SS config initialization failed.\n\r");
		return XST_FAILURE;
	}

	XCsi2TxSs_SetClkMode(&Csi2TxSsInst_3, 0);
	Status = XCsi2TxSs_Activate(&Csi2TxSsInst_3, XCSI2TX_ENABLE);

	return Status;
}
#endif

#endif
