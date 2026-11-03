#include "../bsp.h"

#if defined (XPAR_XIICPS_NUM_INSTANCES)

#if (XPAR_XIICPS_NUM_INSTANCES >= 1U)
XIicPs ps_i2c_0;
#endif
#if (XPAR_XIICPS_NUM_INSTANCES >= 2U)
XIicPs ps_i2c_1;
#endif

int ps_i2c_reg8_write(XIicPs *InstancePtr, char IIC_ADDR, char Addr, char Data)
{
	int Status;
	u8 SendBuffer[2];

	Xil_AssertVoid(InstancePtr != NULL);
	Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

	SendBuffer[0] = Addr;
	SendBuffer[1] = Data;
	Status = XIicPs_MasterSendPolled(InstancePtr, SendBuffer, 2, IIC_ADDR);
	while (XIicPs_BusIsBusy(InstancePtr));

	return Status;
}

char ps_i2c_reg8_read(XIicPs *InstancePtr, char IIC_ADDR, char Addr)
{
	u8 wr_data, rd_data;

	Xil_AssertVoid(InstancePtr != NULL);
	Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

	wr_data = Addr;
	XIicPs_MasterSendPolled(InstancePtr, &wr_data, 1, IIC_ADDR);
	XIicPs_MasterRecvPolled(InstancePtr, &rd_data, 1, IIC_ADDR);
	while (XIicPs_BusIsBusy(InstancePtr));
	return rd_data;
}

int ps_i2c_reg16_write(XIicPs *InstancePtr, char IIC_ADDR, unsigned short Addr, char Data)
{
	int Status;
	u8 SendBuffer[3];

	Xil_AssertVoid(InstancePtr != NULL);
	Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

	SendBuffer[0] = Addr>>8;
	SendBuffer[1] = Addr;
	SendBuffer[2] = Data;
	Status = XIicPs_MasterSendPolled(InstancePtr, SendBuffer, 3, IIC_ADDR);
	while (XIicPs_BusIsBusy(InstancePtr));

	return Status;
}

char ps_i2c_reg16_read(XIicPs *InstancePtr, char IIC_ADDR, unsigned short Addr)
{
	u8 rd_data;
	u8 SendBuffer[2];

	Xil_AssertVoid(InstancePtr != NULL);
	Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

	SendBuffer[0] = Addr>>8;
	SendBuffer[1] = Addr;
	XIicPs_MasterSendPolled(InstancePtr, SendBuffer, 2, IIC_ADDR);
	XIicPs_MasterRecvPolled(InstancePtr, &rd_data, 1, IIC_ADDR);
	while (XIicPs_BusIsBusy(InstancePtr));
	return rd_data;
}


int ps_i2c_init(XIicPs *Iic,short DeviceID ,u32 IIC_SCLK_RATE)
{
	int Status;
	XIicPs_Config *Config;

	Config = XIicPs_LookupConfig(DeviceID);
	if (NULL == Config) {
		bsp_printf(TXT_RED "XIicPs_LookupConfig failure\r\n" TXT_RST);
		return XST_FAILURE;
	}

	Status = XIicPs_CfgInitialize(Iic, Config, Config->BaseAddress);
	if (Status != XST_SUCCESS) {
		bsp_printf(TXT_RED "XIicPs_CfgInitialize failure\r\n" TXT_RST);
		return XST_FAILURE;
	}
	XIicPs_SetSClk(Iic, IIC_SCLK_RATE);
	while (XIicPs_BusIsBusy(Iic));	// Wait
	return XST_SUCCESS;
}


int ps_i2c_config(void)
{
	int Status;

	Status = ps_i2c_init(&ps_i2c_0, XPAR_PSU_I2C_0_DEVICE_ID, 100000);
	if (Status != XST_SUCCESS)
	{
		//Xil_Assert(__FILE__, __LINE__);
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

#endif // XPAR_XIICPS_NUM_INSTANCES


/*
usage:

call follows before main_loop,
```
	ps_i2c_config();
```
*/
