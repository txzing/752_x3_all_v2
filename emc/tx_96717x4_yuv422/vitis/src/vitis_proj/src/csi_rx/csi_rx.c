#include "../bsp.h"
#if defined (XPAR_XCSI_NUM_INSTANCES)

#if (XPAR_XCSI_NUM_INSTANCES >= 1U)
XCsiSs Csi2RxSsInst_0;
#endif
#if (XPAR_XCSI_NUM_INSTANCES >= 2U)
XCsiSs Csi2RxSsInst_1;
#endif
#if (XPAR_XCSI_NUM_INSTANCES >= 3U)
XCsiSs Csi2RxSsInst_2;
#endif
#if (XPAR_XCSI_NUM_INSTANCES >= 4U)
XCsiSs Csi2RxSsInst_3;
#endif

int Csi2RxSs_Init(XCsiSs *InstancePtr, u32 DeviceId)
{
	int Status;
	XCsiSs_Config *ConfigPtr;

	ConfigPtr = XCsiSs_LookupConfig(DeviceId);
	if (!ConfigPtr)
	{
		bsp_printf(TXT_RED "CSI2RxSs LookupCfg failed\r\n" TXT_RST);
		return XST_FAILURE;
	}

	Status = XCsiSs_CfgInitialize(InstancePtr, ConfigPtr,
			ConfigPtr->BaseAddr);
	if (Status != XST_SUCCESS)
	{
		bsp_printf(TXT_RED "CsiRxSs Cfg init failed - %x\r\n" TXT_RST, Status);
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

int csi_rx_config(void)
{
	int Status;
#if (XPAR_XCSI_NUM_INSTANCES >= 1U)
	Status = Csi2RxSs_Init(&Csi2RxSsInst_0, XPAR_CSI_0_DEVICE_ID);
	if (Status != XST_SUCCESS)
	{
		//Xil_Assert(__FILE__, __LINE__);
		bsp_printf(TXT_RED "CSI Rx Ss 0 Init failed status = %x.\r\n"
						 TXT_RST, Status);
		return XST_FAILURE;
	}

//	XCsiSs_ReportCoreInfo(&Csi2RxSsInst_0);
	Status = XCsiSs_Reset(&Csi2RxSsInst_0);
	if (Status != XST_SUCCESS)
	{
		Xil_Assert(__FILE__, __LINE__);
		return XST_FAILURE;
	}

	Status = XCsiSs_Activate(&Csi2RxSsInst_0, XCSI_ENABLE);
	if (Status != XST_SUCCESS)
	{
		//Xil_Assert(__FILE__, __LINE__);
		bsp_printf(TXT_RED "CSI Rx Ss 0 Activate failed status = %x.\r\n"
						 TXT_RST, Status);
		return XST_FAILURE;
	}
//	bsp_printf(TXT_RED "CSI Rx Ss 0 config done!\r\n" TXT_RST);
#endif

#if (XPAR_XCSI_NUM_INSTANCES >= 2U)
	Status = Csi2RxSs_Init(&Csi2RxSsInst_1, XPAR_CSI_1_DEVICE_ID);
	if (Status != XST_SUCCESS)
	{
		//Xil_Assert(__FILE__, __LINE__);
		bsp_printf(TXT_RED "CSI Rx Ss 1 Init failed status = %x.\r\n"
						 TXT_RST, Status);
		return XST_FAILURE;
	}
//	XCsi_SetSoftReset(&Csi2RxSsInst_1);
//	XCsi_SetActiveLaneCount(&Csi2RxSsInst_1, 4);
//	XCsi_ClearSoftReset(&Csi2RxSsInst_1);
	Status = XCsi_Activate(&Csi2RxSsInst_1, XCSI_ENABLE);
	if (Status != XST_SUCCESS)
	{
		//Xil_Assert(__FILE__, __LINE__);
		bsp_printf(TXT_RED "CSI Rx Ss 1 Activate failed status = %x.\r\n"
						 TXT_RST, Status);
		return XST_FAILURE;
	}
#endif

#if (XPAR_XCSI_NUM_INSTANCES >= 3U)
	Status = Csi2RxSs_Init(&Csi2RxSsInst_2, XPAR_CSI_2_DEVICE_ID);
	if (Status != XST_SUCCESS)
	{
		//Xil_Assert(__FILE__, __LINE__);
		bsp_printf(TXT_RED "CSI Rx Ss 2 Init failed status = %x.\r\n"
						 TXT_RST, Status);
		return XST_FAILURE;
	}
//	XCsi_SetSoftReset(&Csi2RxSsInst_2);
//	XCsi_SetActiveLaneCount(&Csi2RxSsInst_2, 4);
//	XCsi_ClearSoftReset(&Csi2RxSsInst_2);
	Status = XCsi_Activate(&Csi2RxSsInst_2, XCSI_ENABLE);
	if (Status != XST_SUCCESS)
	{
		//Xil_Assert(__FILE__, __LINE__);
		bsp_printf(TXT_RED "CSI Rx Ss 2 Activate failed status = %x.\r\n"
						 TXT_RST, Status);
		return XST_FAILURE;
	}
#endif

#if (XPAR_XCSI_NUM_INSTANCES >= 4U)
	Status = Csi2RxSs_Init(&Csi2RxSsInst_3, XPAR_CSI_3_DEVICE_ID);
	if (Status != XST_SUCCESS)
	{
		//Xil_Assert(__FILE__, __LINE__);
		bsp_printf(TXT_RED "CSI Rx Ss 3 Init failed status = %x.\r\n"
						 TXT_RST, Status);
		return XST_FAILURE;
	}
//	XCsi_SetSoftReset(&Csi2RxSsInst_3);
//	XCsi_SetActiveLaneCount(&Csi2RxSsInst_3, 4);
//	XCsi_ClearSoftReset(&Csi2RxSsInst_3);
	Status = XCsi_Activate(&Csi2RxSsInst_3, XCSI_ENABLE);
	if (Status != XST_SUCCESS)
	{
		//Xil_Assert(__FILE__, __LINE__);
		bsp_printf(TXT_RED "CSI Rx Ss 3 Activate failed status = %x.\r\n"
						 TXT_RST, Status);
		return XST_FAILURE;
	}
#endif

	return XST_SUCCESS;
}

void print_csi_rx_reg_inf(UINTPTR Addr)
{
	Xil_AssertNonvoid(Addr != NULL);

	bsp_printf("(0x00)Core Configuration Register     = 0x%08x\r\n", Xil_In32(Addr+0x0));
	bsp_printf("(0x04)Protocol Configuration Register = 0x%08x\r\n", Xil_In32(Addr+0x04));
	bsp_printf("(0x10)Core Status Register            = 0x%08x\r\n", Xil_In32(Addr+0x10));
	bsp_printf("(0x20)Global Interrupt Enable Register= 0x%08x\r\n", Xil_In32(Addr+0x20));
	bsp_printf("(0x24)Interrupt Status Register       = 0x%08x\r\n", Xil_In32(Addr+0x24));
	bsp_printf("(0x28)Interrupt Enable Register       = 0x%08x\r\n", Xil_In32(Addr+0x28));
	bsp_printf("(0x30)Generic Short Packet Register   = 0x%08x\r\n", Xil_In32(Addr+0x30));
	bsp_printf("(0x34)VCX Frame Error Register        = 0x%08x\r\n", Xil_In32(Addr+0x34));
	bsp_printf("(0x3C)Clock Lane Information Register = 0x%08x\r\n", Xil_In32(Addr+0x3C));
	bsp_printf("(0x40)Lane0 Information               = 0x%08x\r\n", Xil_In32(Addr+0x40));
	bsp_printf("(0x44)Lane1 Information               = 0x%08x\r\n", Xil_In32(Addr+0x44));
	bsp_printf("(0x48)Lane2 Information               = 0x%08x\r\n", Xil_In32(Addr+0x48));
	bsp_printf("(0x4C)Lane3 Information               = 0x%08x\r\n", Xil_In32(Addr+0x4C));
	bsp_printf("(0x60)Image Information 1 for VC0     = 0x%08x\r\n", Xil_In32(Addr+0x60));
	bsp_printf("(0x64)Image Information 2 for VC0     = 0x%08x\r\n", Xil_In32(Addr+0x64));
	bsp_printf("(0x68)Image Information 1 for VC1     = 0x%08x\r\n", Xil_In32(Addr+0x68));
	bsp_printf("(0x6C)Image Information 2 for VC1     = 0x%08x\r\n", Xil_In32(Addr+0x6C));
	bsp_printf("(0x70)Image Information 1 for VC2     = 0x%08x\r\n", Xil_In32(Addr+0x70));
	bsp_printf("(0x74)Image Information 2 for VC2     = 0x%08x\r\n", Xil_In32(Addr+0x74));
	bsp_printf("(0x78)Image Information 1 for VC3     = 0x%08x\r\n", Xil_In32(Addr+0x78));
	bsp_printf("(0x7C)Image Information 2 for VC3     = 0x%08x\r\n", Xil_In32(Addr+0x7C));
	bsp_printf("(0x80)Image Information 1 for VC4     = 0x%08x\r\n", Xil_In32(Addr+0x80));
	bsp_printf("(0x84)Image Information 2 for VC4     = 0x%08x\r\n", Xil_In32(Addr+0x84));
	bsp_printf("(0x88)Image Information 1 for VC5     = 0x%08x\r\n", Xil_In32(Addr+0x88));
	bsp_printf("(0x8C)Image Information 2 for VC5     = 0x%08x\r\n", Xil_In32(Addr+0x8C));
	bsp_printf("(0x90)Image Information 1 for VC6     = 0x%08x\r\n", Xil_In32(Addr+0x90));
	bsp_printf("(0x94)Image Information 2 for VC6     = 0x%08x\r\n", Xil_In32(Addr+0x94));
	bsp_printf("(0x98)Image Information 1 for VC7     = 0x%08x\r\n", Xil_In32(Addr+0x98));
	bsp_printf("(0x9C)Image Information 2 for VC7     = 0x%08x\r\n", Xil_In32(Addr+0x9C));
	bsp_printf("(0xA0)Image Information 1 for VC8     = 0x%08x\r\n", Xil_In32(Addr+0xA0));
	bsp_printf("(0xA4)Image Information 2 for VC8     = 0x%08x\r\n", Xil_In32(Addr+0xA4));
	bsp_printf("(0xA8)Image Information 1 for VC9     = 0x%08x\r\n", Xil_In32(Addr+0xA8));
	bsp_printf("(0xAC)Image Information 2 for VC9     = 0x%08x\r\n", Xil_In32(Addr+0xAC));
	bsp_printf("(0xB0)Image Information 1 for VC10    = 0x%08x\r\n", Xil_In32(Addr+0xB0));
	bsp_printf("(0xB4)Image Information 2 for VC10    = 0x%08x\r\n", Xil_In32(Addr+0xB4));
	bsp_printf("(0xB8)Image Information 1 for VC11    = 0x%08x\r\n", Xil_In32(Addr+0xB8));
	bsp_printf("(0xBC)Image Information 2 for VC11    = 0x%08x\r\n", Xil_In32(Addr+0xBC));
	bsp_printf("(0xC0)Image Information 1 for VC12    = 0x%08x\r\n", Xil_In32(Addr+0xC0));
	bsp_printf("(0xC4)Image Information 2 for VC12    = 0x%08x\r\n", Xil_In32(Addr+0xC4));
	bsp_printf("(0xC8)Image Information 1 for VC13    = 0x%08x\r\n", Xil_In32(Addr+0xC8));
	bsp_printf("(0xCC)Image Information 2 for VC13    = 0x%08x\r\n", Xil_In32(Addr+0xCC));
	bsp_printf("(0xD0)Image Information 1 for VC14    = 0x%08x\r\n", Xil_In32(Addr+0xD0));
	bsp_printf("(0xD4)Image Information 2 for VC14    = 0x%08x\r\n", Xil_In32(Addr+0xD4));
	bsp_printf("(0xD8)Image Information 1 for VC15    = 0x%08x\r\n", Xil_In32(Addr+0xD8));
	bsp_printf("(0xDC)Image Information 2 for VC15    = 0x%08x\r\n", Xil_In32(Addr+0xDC));

	bsp_printf("(0x00)D-PHY CONTROL                   = 0x%08x\r\n", Xil_In32(Addr+0x1000+0x00));
	bsp_printf("(0x04)D-PHY IDELAY_TAP_VALUE          = 0x%08x\r\n", Xil_In32(Addr+0x1000+0x04));
	bsp_printf("(0x08)D-PHY INIT                      = 0x%08x\r\n", Xil_In32(Addr+0x1000+0x08));
	bsp_printf("(0x10)D-PHY HS_TIMEOUT                = 0x%08x\r\n", Xil_In32(Addr+0x1000+0x10));
	bsp_printf("(0x14)D-PHY ESC_TIMEOUT               = 0x%08x\r\n", Xil_In32(Addr+0x1000+0x14));
	bsp_printf("(0x18)D-PHY CL_STATUS                 = 0x%08x\r\n", Xil_In32(Addr+0x1000+0x18));
	bsp_printf("(0x1C)D-PHY DL0_STATUS                = 0x%08x\r\n", Xil_In32(Addr+0x1000+0x1C));
	bsp_printf("(0x20)D-PHY DL1_STATUS                = 0x%08x\r\n", Xil_In32(Addr+0x1000+0x20));
	bsp_printf("(0x24)D-PHY DL2_STATUS                = 0x%08x\r\n", Xil_In32(Addr+0x1000+0x24));
	bsp_printf("(0x28)D-PHY DL3_STATUS                = 0x%08x\r\n", Xil_In32(Addr+0x1000+0x28));
	bsp_printf("(0x30)D-PHY HS_SETTLE L0              = 0x%08x\r\n", Xil_In32(Addr+0x1000+0x30));
	bsp_printf("(0x48)D-PHY HS_SETTLE L1              = 0x%08x\r\n", Xil_In32(Addr+0x1000+0x48));
}

void print_csi_rx_reg_info(XCsiSs *InstancePtr, u8 verbose)
{
	UINTPTR Addr;

	Xil_AssertNonvoid(InstancePtr != NULL);
	Addr = InstancePtr->Config.BaseAddr;

	if(verbose == 1)
	{
		bsp_printf("(0x00)Core Configuration Register     = 0x%08x\r\n", Xil_In32(Addr+0x0));
		bsp_printf("(0x04)Protocol Configuration Register = 0x%08x\r\n", Xil_In32(Addr+0x04));
		bsp_printf("(0x10)Core Status Register            = 0x%08x\r\n", Xil_In32(Addr+0x10));
		bsp_printf("(0x20)Global Interrupt Enable Register= 0x%08x\r\n", Xil_In32(Addr+0x20));
		bsp_printf("(0x24)Interrupt Status Register       = 0x%08x\r\n", Xil_In32(Addr+0x24));
		bsp_printf("(0x28)Interrupt Enable Register       = 0x%08x\r\n", Xil_In32(Addr+0x28));
		bsp_printf("(0x30)Generic Short Packet Register   = 0x%08x\r\n", Xil_In32(Addr+0x30));
		bsp_printf("(0x34)VCX Frame Error Register        = 0x%08x\r\n", Xil_In32(Addr+0x34));
		bsp_printf("(0x3C)Clock Lane Information Register = 0x%08x\r\n", Xil_In32(Addr+0x3C));
		bsp_printf("(0x40)Lane0 Information               = 0x%08x\r\n", Xil_In32(Addr+0x40));
		bsp_printf("(0x44)Lane1 Information               = 0x%08x\r\n", Xil_In32(Addr+0x44));
		bsp_printf("(0x48)Lane2 Information               = 0x%08x\r\n", Xil_In32(Addr+0x48));
		bsp_printf("(0x4C)Lane3 Information               = 0x%08x\r\n", Xil_In32(Addr+0x4C));
		bsp_printf("(0x60)Image Information 1 for VC0     = 0x%08x\r\n", Xil_In32(Addr+0x60));
		bsp_printf("(0x64)Image Information 2 for VC0     = 0x%08x\r\n", Xil_In32(Addr+0x64));
		bsp_printf("(0x68)Image Information 1 for VC1     = 0x%08x\r\n", Xil_In32(Addr+0x68));
		bsp_printf("(0x6C)Image Information 2 for VC1     = 0x%08x\r\n", Xil_In32(Addr+0x6C));
		bsp_printf("(0x70)Image Information 1 for VC2     = 0x%08x\r\n", Xil_In32(Addr+0x70));
		bsp_printf("(0x74)Image Information 2 for VC2     = 0x%08x\r\n", Xil_In32(Addr+0x74));
		bsp_printf("(0x78)Image Information 1 for VC3     = 0x%08x\r\n", Xil_In32(Addr+0x78));
		bsp_printf("(0x7C)Image Information 2 for VC3     = 0x%08x\r\n", Xil_In32(Addr+0x7C));
		bsp_printf("(0x80)Image Information 1 for VC4     = 0x%08x\r\n", Xil_In32(Addr+0x80));
		bsp_printf("(0x84)Image Information 2 for VC4     = 0x%08x\r\n", Xil_In32(Addr+0x84));
		bsp_printf("(0x88)Image Information 1 for VC5     = 0x%08x\r\n", Xil_In32(Addr+0x88));
		bsp_printf("(0x8C)Image Information 2 for VC5     = 0x%08x\r\n", Xil_In32(Addr+0x8C));
		bsp_printf("(0x90)Image Information 1 for VC6     = 0x%08x\r\n", Xil_In32(Addr+0x90));
		bsp_printf("(0x94)Image Information 2 for VC6     = 0x%08x\r\n", Xil_In32(Addr+0x94));
		bsp_printf("(0x98)Image Information 1 for VC7     = 0x%08x\r\n", Xil_In32(Addr+0x98));
		bsp_printf("(0x9C)Image Information 2 for VC7     = 0x%08x\r\n", Xil_In32(Addr+0x9C));
		bsp_printf("(0xA0)Image Information 1 for VC8     = 0x%08x\r\n", Xil_In32(Addr+0xA0));
		bsp_printf("(0xA4)Image Information 2 for VC8     = 0x%08x\r\n", Xil_In32(Addr+0xA4));
		bsp_printf("(0xA8)Image Information 1 for VC9     = 0x%08x\r\n", Xil_In32(Addr+0xA8));
		bsp_printf("(0xAC)Image Information 2 for VC9     = 0x%08x\r\n", Xil_In32(Addr+0xAC));
		bsp_printf("(0xB0)Image Information 1 for VC10    = 0x%08x\r\n", Xil_In32(Addr+0xB0));
		bsp_printf("(0xB4)Image Information 2 for VC10    = 0x%08x\r\n", Xil_In32(Addr+0xB4));
		bsp_printf("(0xB8)Image Information 1 for VC11    = 0x%08x\r\n", Xil_In32(Addr+0xB8));
		bsp_printf("(0xBC)Image Information 2 for VC11    = 0x%08x\r\n", Xil_In32(Addr+0xBC));
		bsp_printf("(0xC0)Image Information 1 for VC12    = 0x%08x\r\n", Xil_In32(Addr+0xC0));
		bsp_printf("(0xC4)Image Information 2 for VC12    = 0x%08x\r\n", Xil_In32(Addr+0xC4));
		bsp_printf("(0xC8)Image Information 1 for VC13    = 0x%08x\r\n", Xil_In32(Addr+0xC8));
		bsp_printf("(0xCC)Image Information 2 for VC13    = 0x%08x\r\n", Xil_In32(Addr+0xCC));
		bsp_printf("(0xD0)Image Information 1 for VC14    = 0x%08x\r\n", Xil_In32(Addr+0xD0));
		bsp_printf("(0xD4)Image Information 2 for VC14    = 0x%08x\r\n", Xil_In32(Addr+0xD4));
		bsp_printf("(0xD8)Image Information 1 for VC15    = 0x%08x\r\n", Xil_In32(Addr+0xD8));
		bsp_printf("(0xDC)Image Information 2 for VC15    = 0x%08x\r\n", Xil_In32(Addr+0xDC));

		bsp_printf("(0x00)D-PHY CONTROL                   = 0x%08x\r\n", Xil_In32(Addr+0x1000+0x00));
		bsp_printf("(0x04)D-PHY IDELAY_TAP_VALUE          = 0x%08x\r\n", Xil_In32(Addr+0x1000+0x04));
		bsp_printf("(0x08)D-PHY INIT                      = 0x%08x\r\n", Xil_In32(Addr+0x1000+0x08));
		bsp_printf("(0x10)D-PHY HS_TIMEOUT                = 0x%08x\r\n", Xil_In32(Addr+0x1000+0x10));
		bsp_printf("(0x14)D-PHY ESC_TIMEOUT               = 0x%08x\r\n", Xil_In32(Addr+0x1000+0x14));
		bsp_printf("(0x18)D-PHY CL_STATUS                 = 0x%08x\r\n", Xil_In32(Addr+0x1000+0x18));
		bsp_printf("(0x1C)D-PHY DL0_STATUS                = 0x%08x\r\n", Xil_In32(Addr+0x1000+0x1C));
		bsp_printf("(0x20)D-PHY DL1_STATUS                = 0x%08x\r\n", Xil_In32(Addr+0x1000+0x20));
		bsp_printf("(0x24)D-PHY DL2_STATUS                = 0x%08x\r\n", Xil_In32(Addr+0x1000+0x24));
		bsp_printf("(0x28)D-PHY DL3_STATUS                = 0x%08x\r\n", Xil_In32(Addr+0x1000+0x28));
		bsp_printf("(0x30)D-PHY HS_SETTLE L0              = 0x%08x\r\n", Xil_In32(Addr+0x1000+0x30));
		bsp_printf("(0x48)D-PHY HS_SETTLE L1              = 0x%08x\r\n", Xil_In32(Addr+0x1000+0x48));
	}
}

#endif // XPAR_XCSI_NUM_INSTANCES


/*
usage:
assume you have a xgpio to set 96 bit user data

declare a globle variable before main()
```
XGpio XGpioOutput;
```

call follows before main_loop, especially call csi_tx_config() after axis prepared(tpg,vdma,vpss)

    Status = xgpio_setup(&XGpioOutput, XPAR_GPIO_0_DEVICE_ID, 0, 0) ;
    if (Status != XST_SUCCESS)
	{
    	Xil_Assert(__FILE__, __LINE__);
		return XST_FAILURE ;
	}
	XGpio_DiscreteWrite(&XGpioOutput, 1, 0x24); // RGB888
//	XGpio_DiscreteWrite(&XGpioOutput, 1, 0x2A); // RAW8
//	XGpio_DiscreteWrite(&XGpioOutput, 1, 0x2B); // RAW10
//	XGpio_DiscreteWrite(&XGpioOutput, 1, 0x2C); // RAW12
//	XGpio_DiscreteWrite(&XGpioOutput, 1, 0x1E); // YUV422_8bit
//	XGpio_DiscreteWrite(&XGpioOutput, 2, VIDEO_COLUMNS*24/8<<16); // WC RGB888
//	XGpio_DiscreteWrite(&XGpioOutput, 2, 3840*24/8<<16); // WC RGB888
//	XGpio_DiscreteWrite(&XGpioOutput, 2, (1920*12/8)<<16); // WC RAW12
	XGpio_DiscreteWrite(&XGpioOutput, 2, (1920*24/8)<<16); // WC RGB888
//	XGpio_DiscreteWrite(&XGpioOutput, 2, (VIDEO_COLUMNS*10/8)<<16); // WC RAW10
//	XGpio_DiscreteWrite(&XGpioOutput, 2, (1920*16/8)<<16); // WC YUV422_8bit

	//after axis prepared
	Status = csi_tx_config();
	if (Status != XST_SUCCESS)
	{
		Xil_Assert(__FILE__, __LINE__);
		return XST_FAILURE ;
	}
*/
