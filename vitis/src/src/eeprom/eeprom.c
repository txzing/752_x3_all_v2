#include "../bsp.h"
#if defined (USING_EEPROM)

#if EEPROM_INTF == EEPROM_USING_PSIIC
#if defined (XPAR_XIICPS_NUM_INSTANCES) && defined (__PS_I2C_H__)

typedef u16 AddressType;

//for at24c01/24c02 might be 0x50~0x57
//for at24c04 might be 0x50, 0x52, 0x54, 0x56, for cat24aa04 is 0x50 only
//for at24c08 might be 0x50, 0x54, for cat24aa08 is 0x50 only
//for at24c16 is 0x50 only
#define EEPROM_ADDR		0x50	// 7bit 0x50 == 8bit 0xA0

//#define I2C_MUX_ADDR    0x74  /**< I2C Mux Address */

// at24c01 has 16 pages of 8 byte each, 128 bytes in total
// at24c02 has 32 pages of 8 byte each, 256 bytes in total
// at24c04/cat24aa04 has 32 pages of 16 byte each, 512 bytes in total
// at24c08/cat24aa08 has 64 pages of 16 byte each, 1024 bytes in total
// at24c16 has 128 pages of 16 byte each, 2048 bytes in total
// at24c32 has 128 pages of 32 byte each, 4096 bytes in total
// at24c64 has 256 pages of 32 byte each, 8192 bytes in total
// cat24s64 has 128 pages of 64 byte each, 8192 bytes in total
// at24c128 has 256 pages of 64 byte each, 16384 bytes in total
// at24c256/m24256 has 512 pages of 64 byte each, 32768 bytes in total
#define MAX_SIZE		64
#define PAGE_SIZE_8		8
#define PAGE_SIZE_16	16
#define PAGE_SIZE_32	32
#define PAGE_SIZE_64	64

/*
 * Write buffer for writing a page.
 */
static u8 WriteBuffer[sizeof(AddressType) + MAX_SIZE];
static u8 ReadBuffer[MAX_SIZE];	/* Read buffer for reading a page. */

u32 PageSize = PAGE_SIZE_16;
u32 TotalPage = 64;

#if 0
int I2cMux_Eeprom(void)
{
  u8 Buffer;
  int Status;

  bsp_printf("Set i2c mux... \r\n");

  /* Select SI5324 clock generator */
  Buffer = 0x1;
  Status = XIicPs_MasterSendPolled(&ps_i2c_0, (u8 *)&Buffer, 1, I2C_MUX_ADDR);
  if(Status==XST_SUCCESS)
  {
	  bsp_printf("Set i2c mux done\r\n");
  }
  else
  {
	  return XST_FAILURE;
  }

  while (XIicPs_BusIsBusy(&ps_i2c_0));

  Status = XIicPs_MasterRecvPolled(&ps_i2c_0, &Buffer,1, I2C_MUX_ADDR);
  if (Status != XST_SUCCESS) {
	return XST_FAILURE;
  }

  usleep(250000);

  return Status;
}
#endif


#if 0
/*****************************************************************************/
/**
* This function is used to figure out page size Eeprom slave device
*
* @param	Eeprom Address
*
* @param	Pagesize pointer
*
* @return	XST_SUCCESS if successful and also update the epprom slave
* device pagesize else XST_FAILURE.
*
* @note		None.
*
******************************************************************************/
static int FindEepromPageSize(XIicPs *IicInstance, u16 EepromAddr, u32 *PageSize_ptr)
{
	u32 Index, i;
	int Status = XST_FAILURE;
//	AddressType Address = EEPROM_START_ADDRESS;
	AddressType Address = 0;
	int WrBfrOffset = 0;
	u32 ps[3] = {64, 32, 16, 8};
	u32 PageSize_test, count;
	u16 EepromSlvAddr;

	EepromSlvAddr = EEPROM_ADDR | (((Address & 0x3ff) >> 8));

	for (i = 0; i < 3; i++)
	{
		count = 0;
		PageSize_test = ps[i];
		*PageSize_ptr = PageSize_test;
		/*
		 * Initialize the data to write and the read buffer.
		 */
		if (PageSize_test == PAGE_SIZE_16) {
			WriteBuffer[0] = (u8) (Address);
			WrBfrOffset = 1;
		} else {
			WriteBuffer[0] = (u8) (Address >> 8);
			WriteBuffer[1] = (u8) (Address);
			WrBfrOffset = 2;
		}

		for (Index = 0; Index < PageSize_test; Index++) {
			WriteBuffer[WrBfrOffset + Index] = Index + i;
			ReadBuffer[Index] = 0;
		}

		/*
		 * Write to the EEPROM.
		 */
		Status = XIicPs_MasterSendPolled(IicInstance, WriteBuffer, WrBfrOffset + PageSize_test, EepromSlvAddr);
		if (Status != XST_SUCCESS) {
			return XST_FAILURE;
		}

		/*
		 * Read from the EEPROM.
		 */
		Status = EepromReadData(IicInstance, ReadBuffer, PageSize_test, 0);
		if (Status != XST_SUCCESS) {
			return XST_FAILURE;
		}

		/*
		 * Verify the data read against the data written.
		 */
		for (Index = 0; Index < PageSize_test; Index++) {
			if (ReadBuffer[Index] == Index + i) {
				count++;
			}
		}
		if (count == PageSize_test)
		{
			return XST_SUCCESS;
		}
	}
	return Status;
}

#endif


/*****************************************************************************/
/**
* This function writes a buffer of data continuously to the IIC serial EEPROM.
*
* @param	ByteCount contains the number of bytes in the buffer to be
*		written.
*
* @return	XST_SUCCESS if successful else XST_FAILURE.
*
******************************************************************************/
s32 EepromWriteData(XIicPs *IicInstance, u8 * BufferPtr, u16 ByteCount, u16 start_addr)
{
	s32 Status;
	u8 start_page_offset,end_page_offset;
//	u16 end_addr;
	u16 start_page,end_page,page_cnt;
	u16 byte_cnt;
	u16 EepromSlvAddr;
	u32 WrBfrOffset;

	u8 WriteBuffer[PageSize+2]; // page 大小 加 最多2byte 片内地址
//	if (PageSize == PAGE_SIZE_16 || PageSize == PAGE_SIZE_8) {
//		WrBfrOffset = 1;
//	} else {
//		WrBfrOffset = 2;
//	}

	Xil_AssertVoid(IicInstance != NULL);
	Xil_AssertVoid(IicInstance->IsReady == XIL_COMPONENT_IS_READY);

	//EepromSlvAddr = EepromSlvAddr & ((start_addr & 0x3ff) >> 8);
	//WriteBuffer[0] = (u8)(start_addr & 0xff);// m24c08起始地址1byte
	//memcpy(WriteBuffer+1, BufferPtr, PAGE_SIZE_16);

	/*
	 * Send the Data.
	 */
	if(ByteCount>1)
	{
		start_page_offset = start_addr % PageSize; // 起始地址在一页内的偏移, 0~15 for 16 byte page
		end_page_offset = (start_addr + ByteCount-1) % PageSize; // 0~15 for 16 byte page
		start_page = start_addr / PageSize + 1; // 从1开始算  (0~15+16)/16 = 1, (16+16)/16 = 2, => (addr+pagesize)/pagesize=addr/pagesize+1, for 16 byte page devices
		// 计算结束的页，就是last page.从1开始算
		end_page = (start_addr + ByteCount-1) / PageSize + 1; // 从1开始算

		for(page_cnt=start_page; page_cnt<=end_page; )
		{
			if(page_cnt==start_page) // 第一个page，可能不是从头开始的
			{
				if (PageSize == PAGE_SIZE_16 || PageSize == PAGE_SIZE_8) {
					EepromSlvAddr = EEPROM_ADDR | (((start_addr & 0x3ff) >> 8));
					WriteBuffer[0] = (u8) (start_addr);
					WrBfrOffset = 1;
				} else {
					EepromSlvAddr = EEPROM_ADDR;
					WriteBuffer[0] = (u8) (start_addr >> 8);
					WriteBuffer[1] = (u8) (start_addr);
					WrBfrOffset = 2;
				}
				if(start_page_offset + ByteCount < PageSize) // 起始地址 到 这一页尾(0~15 for 16 byte page) 能放得下数据buffer
				{
					memcpy(WriteBuffer+WrBfrOffset, BufferPtr, ByteCount);
					Status = XIicPs_MasterSendPolled(IicInstance, WriteBuffer, ByteCount+WrBfrOffset, EepromSlvAddr);
				}
				else
				{
					memcpy(WriteBuffer+WrBfrOffset, BufferPtr, PageSize-start_page_offset); // PageSize-start_page_offset 范围是 1~16
					Status = XIicPs_MasterSendPolled(IicInstance, WriteBuffer, PageSize-start_page_offset+WrBfrOffset, EepromSlvAddr);
				}
			}
			else if((page_cnt==end_page)&&(end_page!=start_page)&&(end_page_offset!=(PageSize-1))) // last page handle
			{
				if (PageSize == PAGE_SIZE_16 || PageSize == PAGE_SIZE_8) {
					EepromSlvAddr = EEPROM_ADDR | (((((page_cnt-1)*PageSize) & 0x3ff) >> 8));
					WriteBuffer[0] = (u8) (((page_cnt-start_page)*PageSize));
					WrBfrOffset = 1;
				} else {
					EepromSlvAddr = EEPROM_ADDR;
					WriteBuffer[0] = (u8) (((page_cnt-start_page)*PageSize) >> 8);
					WriteBuffer[1] = (u8) (((page_cnt-start_page)*PageSize));
					WrBfrOffset = 2;
				}
				// BufferPtr+(PageSize-start_page_offset) 是去掉写入到第一页的数据
				// end_page_offset+1 范围是 1~16
				memcpy(WriteBuffer+WrBfrOffset, BufferPtr+(PageSize-start_page_offset)+(page_cnt-start_page-1)*PageSize, end_page_offset+1);
				Status = XIicPs_MasterSendPolled(IicInstance, WriteBuffer, end_page_offset+1+WrBfrOffset, EepromSlvAddr);
			}
			else
			{
				if (PageSize == PAGE_SIZE_16 || PageSize == PAGE_SIZE_8) {
					EepromSlvAddr = EEPROM_ADDR | (((((page_cnt-1)*PageSize) & 0x3ff) >> 8));
					WriteBuffer[0] = (u8) (((page_cnt-start_page)*PageSize));
					WrBfrOffset = 1;
				} else {
					EepromSlvAddr = EEPROM_ADDR;
					WriteBuffer[0] = (u8) (((page_cnt-start_page)*PageSize) >> 8);
					WriteBuffer[1] = (u8) (((page_cnt-start_page)*PageSize));
					WrBfrOffset = 2;
				}

				memcpy(WriteBuffer+WrBfrOffset, BufferPtr+(PageSize-start_page_offset)+(page_cnt-start_page-1)*PageSize, PageSize);
				Status = XIicPs_MasterSendPolled(IicInstance, WriteBuffer, PageSize+WrBfrOffset, EepromSlvAddr);
			}

			if (Status != XST_SUCCESS)
			{
				return XST_FAILURE;
			}
			while (XIicPs_BusIsBusy(IicInstance));
			usleep(250000);
			page_cnt++;
		}

		usleep(250000);
	}
	else
	{
		if (PageSize == PAGE_SIZE_16 || PageSize == PAGE_SIZE_8) {
			EepromSlvAddr = EEPROM_ADDR | (((start_addr & 0x3ff) >> 8));
			WriteBuffer[0] = (u8) (start_addr);
			WrBfrOffset = 1;
		} else {
			EepromSlvAddr = EEPROM_ADDR;
			WriteBuffer[0] = (u8) (start_addr >> 8);
			WriteBuffer[1] = (u8) (start_addr);
			WrBfrOffset = 2;
		}
		Status = XIicPs_MasterSendPolled(IicInstance, WriteBuffer, WrBfrOffset, EepromSlvAddr);

		if (Status != XST_SUCCESS)
		{
			return XST_FAILURE;
		}

		while (XIicPs_BusIsBusy(IicInstance));
	}

	return XST_SUCCESS;
}

/*****************************************************************************/
/**
* This function reads data from the IIC serial EEPROM into a specified buffer.
*
* @param	BufferPtr contains the address of the data buffer to be filled.
* @param	ByteCount contains the number of bytes in the buffer to be read.
*
* @return	XST_SUCCESS if successful else XST_FAILURE.
*
* @note		None.
*
******************************************************************************/
s32 EepromReadData(XIicPs *IicInstance, u8 *BufferPtr, u16 ByteCount, u16 start_addr)
{
	s32 Status;
	AddressType Address = start_addr;
	u32 WrBfrOffset;
	u16 EepromSlvAddr;

	Xil_AssertVoid(IicInstance != NULL);
	Xil_AssertVoid(IicInstance->IsReady == XIL_COMPONENT_IS_READY);

	/*
	 * Position the Pointer in EEPROM.
	 */
	if (PageSize == PAGE_SIZE_16 || PageSize == PAGE_SIZE_8) {
		EepromSlvAddr = EEPROM_ADDR | (((start_addr & 0x3ff) >> 8));
		WriteBuffer[0] = (u8) (Address);
		WrBfrOffset = 1;
	} else {
		EepromSlvAddr = EEPROM_ADDR;
		WriteBuffer[0] = (u8) (Address >> 8);
		WriteBuffer[1] = (u8) (Address);
		WrBfrOffset = 2;
	}

//	Status = XIicPs_MasterSendPolled(IicInstance, WriteBuffer, WrBfrOffset, EepromSlvAddr);
//	if (Status != XST_SUCCESS) {
//		return XST_FAILURE;
//	}
//	while (XIicPs_BusIsBusy(IicInstance));
	Status = EepromWriteData(IicInstance, WriteBuffer, 0, start_addr);

	/*
	 * Receive the Data.
	 */

	Status = XIicPs_MasterRecvPolled(IicInstance, BufferPtr,
						  ByteCount, EepromSlvAddr);
	if (Status != XST_SUCCESS) {
			return XST_FAILURE;
	}
	/*
	 * Wait until bus is idle to start another transfer.
	 */
	while (XIicPs_BusIsBusy(IicInstance));

	return XST_SUCCESS;
}

#endif // XPAR_XIICPS_NUM_INSTANCES && __PS_I2C_H__
#endif // EEPROM_USING_PSIIC


#if EEPROM_INTF == EEPROM_USING_XIIC
#if defined (XPAR_XIIC_NUM_INSTANCES)

#define IIC_BASE_ADDRESS	XPAR_IIC_0_BASEADDR

typedef u16 AddressType;

//for at24c01/24c02 might be 0x50~0x57
//for at24c04 might be 0x50, 0x52, 0x54, 0x56, for cat24aa04 is 0x50 only
//for at24c08 might be 0x50, 0x54, for cat24aa08 is 0x50 only
//for at24c16 is 0x50 only
#define EEPROM_ADDR		0x50	// 7bit 0x50 == 8bit 0xA0

//#define I2C_MUX_ADDR    0x74  /**< I2C Mux Address */

// at24c01 has 16 pages of 8 byte each, 128 bytes in total
// at24c02 has 32 pages of 8 byte each, 256 bytes in total
// at24c04/cat24aa04 has 32 pages of 16 byte each, 512 bytes in total
// at24c08/cat24aa08 has 64 pages of 16 byte each, 1024 bytes in total
// at24c16 has 128 pages of 16 byte each, 2048 bytes in total
// at24c32 has 128 pages of 32 byte each, 4096 bytes in total
// at24c64 has 256 pages of 32 byte each, 8192 bytes in total
// cat24s64 has 128 pages of 64 byte each, 8192 bytes in total
// at24c128 has 256 pages of 64 byte each, 16384 bytes in total
// at24c256/m24256 has 512 pages of 64 byte each, 32768 bytes in total
#define MAX_SIZE		64
#define PAGE_SIZE_8		8
#define PAGE_SIZE_16	16
#define PAGE_SIZE_32	32
#define PAGE_SIZE_64	64

/*
 * Write buffer for writing a page.
 */
static u8 WriteBuffer[sizeof(AddressType) + MAX_SIZE];
static u8 ReadBuffer[MAX_SIZE];	/* Read buffer for reading a page. */

u32 PageSize = PAGE_SIZE_16;
u32 TotalPage = 64;

s32 EepromWriteData(u8 *BufferPtr, u16 ByteCount, u16 start_addr)
{
//	s32 Status;
	u8 start_page_offset,end_page_offset;
//	u16 end_addr;
	u16 start_page,end_page,page_cnt;
	u16 byte_cnt;
	u16 EepromSlvAddr;
	u32 WrBfrOffset;

	u8 WriteBuffer[PageSize+2]; // page 大小 加 最多2byte 片内地址
//	if (PageSize == PAGE_SIZE_16 || PageSize == PAGE_SIZE_8) {
//		WrBfrOffset = 1;
//	} else {
//		WrBfrOffset = 2;
//	}

	//EepromSlvAddr = EepromSlvAddr & ((start_addr & 0x3ff) >> 8);
	//WriteBuffer[0] = (u8)(start_addr & 0xff);// m24c08起始地址1byte
	//memcpy(WriteBuffer+1, BufferPtr, PAGE_SIZE_16);

	/*
	 * Send the Data.
	 */
	if(ByteCount>1)
	{
		start_page_offset = start_addr % PageSize; // 起始地址在一页内的偏移, 0~15 for 16 byte page
		end_page_offset = (start_addr + ByteCount-1) % PageSize; // 0~15 for 16 byte page
		start_page = start_addr / PageSize + 1; // 从1开始算  (0~15+16)/16 = 1, (16+16)/16 = 2, => (addr+pagesize)/pagesize=addr/pagesize+1, for 16 byte page devices
		// 计算结束的页，就是last page.从1开始算
		end_page = (start_addr + ByteCount-1) / PageSize + 1; // 从1开始算

		for(page_cnt=start_page; page_cnt<=end_page; )
		{
			if(page_cnt==start_page) // 第一个page，可能不是从头开始的
			{
				if (PageSize == PAGE_SIZE_16 || PageSize == PAGE_SIZE_8) {
					EepromSlvAddr = EEPROM_ADDR | (((start_addr & 0x3ff) >> 8));
					WriteBuffer[0] = (u8) (start_addr);
					WrBfrOffset = 1;
				} else {
					EepromSlvAddr = EEPROM_ADDR;
					WriteBuffer[0] = (u8) (start_addr >> 8);
					WriteBuffer[1] = (u8) (start_addr);
					WrBfrOffset = 2;
				}
				if(start_page_offset + ByteCount < PageSize) // 起始地址 到 这一页尾(0~15 for 16 byte page) 能放得下数据buffer
				{
					memcpy(WriteBuffer+WrBfrOffset, BufferPtr, ByteCount);
					ByteCount=XIic_Send(IIC_BASE_ADDRESS, EepromSlvAddr, WriteBuffer, ByteCount+WrBfrOffset, XIIC_STOP);
					if(ByteCount!=(ByteCount+WrBfrOffset))
					{
						return XST_FAILURE;
					}
				}
				else
				{
					memcpy(WriteBuffer+WrBfrOffset, BufferPtr, PageSize-start_page_offset); // PageSize-start_page_offset 范围是 1~16
					ByteCount=XIic_Send(IIC_BASE_ADDRESS, EepromSlvAddr, WriteBuffer, PageSize-start_page_offset+WrBfrOffset, XIIC_STOP);
					if(ByteCount!=(PageSize-start_page_offset+WrBfrOffset))
					{
						return XST_FAILURE;
					}
				}
			}
			else if((page_cnt==end_page)&&(end_page!=start_page)&&(end_page_offset!=(PageSize-1))) // last page handle
			{
				if (PageSize == PAGE_SIZE_16 || PageSize == PAGE_SIZE_8) {
					EepromSlvAddr = EEPROM_ADDR | (((((page_cnt-1)*PageSize) & 0x3ff) >> 8));
					WriteBuffer[0] = (u8) (((page_cnt-start_page)*PageSize));
					WrBfrOffset = 1;
				} else {
					EepromSlvAddr = EEPROM_ADDR;
					WriteBuffer[0] = (u8) (((page_cnt-start_page)*PageSize) >> 8);
					WriteBuffer[1] = (u8) (((page_cnt-start_page)*PageSize));
					WrBfrOffset = 2;
				}
				// BufferPtr+(PageSize-start_page_offset) 是去掉写入到第一页的数据
				// end_page_offset+1 范围是 1~16
				memcpy(WriteBuffer+WrBfrOffset, BufferPtr+(PageSize-start_page_offset)+(page_cnt-start_page-1)*PageSize, end_page_offset+1);
				ByteCount=XIic_Send(IIC_BASE_ADDRESS, EepromSlvAddr, WriteBuffer, end_page_offset+1+WrBfrOffset, XIIC_STOP);
				if(ByteCount!=(end_page_offset+1+WrBfrOffset))
				{
					return XST_FAILURE;
				}
			}
			else
			{
				if (PageSize == PAGE_SIZE_16 || PageSize == PAGE_SIZE_8) {
					EepromSlvAddr = EEPROM_ADDR | (((((page_cnt-1)*PageSize) & 0x3ff) >> 8));
					WriteBuffer[0] = (u8) (((page_cnt-start_page)*PageSize));
					WrBfrOffset = 1;
				} else {
					EepromSlvAddr = EEPROM_ADDR;
					WriteBuffer[0] = (u8) (((page_cnt-start_page)*PageSize) >> 8);
					WriteBuffer[1] = (u8) (((page_cnt-start_page)*PageSize));
					WrBfrOffset = 2;
				}

				memcpy(WriteBuffer+WrBfrOffset, BufferPtr+(PageSize-start_page_offset)+(page_cnt-start_page-1)*PageSize, PageSize);
				ByteCount=XIic_Send(IIC_BASE_ADDRESS, EepromSlvAddr, WriteBuffer, PageSize+WrBfrOffset, XIIC_STOP);
				if(ByteCount!=(PageSize+WrBfrOffset))
				{
					return XST_FAILURE;
				}
			}

			if(ByteCount==0)
			{
				return XST_FAILURE;
			}
			usleep(250000);
			page_cnt++;
		}

		usleep(250000);
	}
	else
	{
		if (PageSize == PAGE_SIZE_16 || PageSize == PAGE_SIZE_8) {
			EepromSlvAddr = EEPROM_ADDR | (((start_addr & 0x3ff) >> 8));
			WriteBuffer[0] = (u8) (start_addr);
			WrBfrOffset = 1;
		} else {
			EepromSlvAddr = EEPROM_ADDR;
			WriteBuffer[0] = (u8) (start_addr >> 8);
			WriteBuffer[1] = (u8) (start_addr);
			WrBfrOffset = 2;
		}
		ByteCount=XIic_Send(IIC_BASE_ADDRESS, EepromSlvAddr, WriteBuffer, WrBfrOffset, XIIC_REPEATED_START);
		if(ByteCount!=(WrBfrOffset))
		{
			return XST_FAILURE;
		}
	}

	return XST_SUCCESS;
}

s32 EepromReadData(u8 *BufferPtr, u16 ByteCount, u16 start_addr)
{
//	s32 Status;
	AddressType Address = start_addr;
	u32 WrBfrOffset;
	u16 EepromSlvAddr;
//	volatile unsigned SentByteCount;

	/*
	 * Position the Pointer in EEPROM.
	 */
	if (PageSize == PAGE_SIZE_16 || PageSize == PAGE_SIZE_8) {
		EepromSlvAddr = EEPROM_ADDR | (((start_addr & 0x3ff) >> 8));
		WriteBuffer[0] = (u8) (Address);
		WrBfrOffset = 1;
	} else {
		EepromSlvAddr = EEPROM_ADDR;
		WriteBuffer[0] = (u8) (Address >> 8);
		WriteBuffer[1] = (u8) (Address);
		WrBfrOffset = 2;
	}

//	SentByteCount = XIic_Send(IIC_BASE_ADDRESS, EepromSlvAddr, WriteBuffer, WrBfrOffset, XIIC_STOP);
	ByteCount=XIic_Send(IIC_BASE_ADDRESS, EepromSlvAddr, WriteBuffer, WrBfrOffset, XIIC_REPEATED_START);
	if(ByteCount!=(WrBfrOffset))
	{
		return XST_FAILURE;
	}

	/*
	 * Receive the Data.
	 */
	ByteCount=XIic_Recv(IIC_BASE_ADDRESS, EepromSlvAddr, BufferPtr, ByteCount, XIIC_STOP);
	if(ByteCount!=ByteCount)
	{
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

#endif // XPAR_XIIC_NUM_INSTANCES
#endif // EEPROM_USING_XIIC

#if EEPROM_INTF == EEPROM_USING_XGPIO
// 确保有设备，且要调用xgpio_i2c.c里的初始化
#if defined(XPAR_XGPIO_I2C_0_AXI_GPIO_0_DEVICE_ID) && defined (__XGPIO_I2C_H__)

typedef u16 AddressType;

//for at24c01/24c02 might be 0x50~0x57
//for at24c04 might be 0x50, 0x52, 0x54, 0x56, for cat24aa04 is 0x50 only
//for at24c08 might be 0x50, 0x54, for cat24aa08 is 0x50 only
//for at24c16 is 0x50 only
#define EEPROM_ADDR		0x50	// 7bit 0x50 == 8bit 0xA0

//#define I2C_MUX_ADDR    0x74  /**< I2C Mux Address */

// at24c01 has 16 pages of 8 byte each, 128 bytes in total
// at24c02 has 32 pages of 8 byte each, 256 bytes in total
// at24c04/cat24aa04 has 32 pages of 16 byte each, 512 bytes in total
// at24c08/cat24aa08 has 64 pages of 16 byte each, 1024 bytes in total
// at24c16 has 128 pages of 16 byte each, 2048 bytes in total
// at24c32 has 128 pages of 32 byte each, 4096 bytes in total
// at24c64 has 256 pages of 32 byte each, 8192 bytes in total
// cat24s64 has 128 pages of 64 byte each, 8192 bytes in total
// at24c128 has 256 pages of 64 byte each, 16384 bytes in total
// at24c256/m24256 has 512 pages of 64 byte each, 32768 bytes in total
#define MAX_SIZE		64
#define PAGE_SIZE_8		8
#define PAGE_SIZE_16	16
#define PAGE_SIZE_32	32
#define PAGE_SIZE_64	64

/*
 * Write buffer for writing a page.
 */
static u8 WriteBuffer[sizeof(AddressType) + MAX_SIZE];
static u8 ReadBuffer[MAX_SIZE];	/* Read buffer for reading a page. */

u32 PageSize = PAGE_SIZE_16;
u32 TotalPage = 64;

static int xgpio_i2c_read(i2c_no i2c, u16 EepromSlvAddr, u8 *BufferPtr, u16 ByteCount)
{
	u8 rxd;
	u8 ack=0;
	u16 i;

	i2c_start(i2c);

  	i2c_send_byte(i2c, EepromSlvAddr<<1 | 0x01);
  	ack=i2c_recv_ack(i2c, STRETCH_OFF);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}

	for(i=0;i<ByteCount-1;i++)
	{
		*(BufferPtr+i) = i2c_recv_byte(i2c);
		i2c_ack(i2c);
	}

	*(BufferPtr+i) = i2c_recv_byte(i2c);
	i2c_nack(i2c);

  	i2c_stop(i2c);

  	return XST_SUCCESS;
}

static int xgpio_i2c_write(i2c_no i2c, u16 EepromSlvAddr, u8 *BufferPtr, u16 ByteCount, opt_mode mode)
{
	u8 ack=0;
	u16 i;

	i2c_start(i2c);

	i2c_send_byte(i2c, EepromSlvAddr<<1);
	ack=i2c_recv_ack(i2c, STRETCH_OFF);
	if(ack)
	{
		i2c_stop(i2c);
		return XST_FAILURE;
	}

	for(i=0;i<ByteCount;i++)
	{
		i2c_send_byte(i2c, *(BufferPtr+i));
		ack=i2c_recv_ack(i2c, STRETCH_OFF);
		if(ack)
		{
			i2c_stop(i2c);
			return XST_FAILURE;
		}

	}

	if(mode)
	{
		i2c_stop(i2c);
	}

	return XST_SUCCESS;
}

s32 EepromWriteData(u8 *BufferPtr, u16 ByteCount, u16 start_addr)
{
	s32 Status;
	u8 start_page_offset,end_page_offset;
//	u16 end_addr;
	u16 start_page,end_page,page_cnt;
	u16 byte_cnt;
	u16 EepromSlvAddr;
	u32 WrBfrOffset;

	u8 WriteBuffer[PageSize+2]; // page 大小 加 最多2byte 片内地址
//	if (PageSize == PAGE_SIZE_16 || PageSize == PAGE_SIZE_8) {
//		WrBfrOffset = 1;
//	} else {
//		WrBfrOffset = 2;
//	}

	//EepromSlvAddr = EepromSlvAddr & ((start_addr & 0x3ff) >> 8);
	//WriteBuffer[0] = (u8)(start_addr & 0xff);// m24c08起始地址1byte
	//memcpy(WriteBuffer+1, BufferPtr, PAGE_SIZE_16);

	/*
	 * Send the Data.
	 */
	if(ByteCount>1)
	{
		start_page_offset = start_addr % PageSize; // 起始地址在一页内的偏移, 0~15 for 16 byte page
		end_page_offset = (start_addr + ByteCount-1) % PageSize; // 0~15 for 16 byte page
		start_page = start_addr / PageSize + 1; // 从1开始算  (0~15+16)/16 = 1, (16+16)/16 = 2, => (addr+pagesize)/pagesize=addr/pagesize+1, for 16 byte page devices
		// 计算结束的页，就是last page.从1开始算
		end_page = (start_addr + ByteCount-1) / PageSize + 1; // 从1开始算

		for(page_cnt=start_page; page_cnt<=end_page; )
		{
			if(page_cnt==start_page) // 第一个page，可能不是从头开始的
			{
				if (PageSize == PAGE_SIZE_16 || PageSize == PAGE_SIZE_8) {
					EepromSlvAddr = EEPROM_ADDR | (((start_addr & 0x3ff) >> 8));
					WriteBuffer[0] = (u8) (start_addr);
					WrBfrOffset = 1;
				} else {
					EepromSlvAddr = EEPROM_ADDR;
					WriteBuffer[0] = (u8) (start_addr >> 8);
					WriteBuffer[1] = (u8) (start_addr);
					WrBfrOffset = 2;
				}
				if(start_page_offset + ByteCount < PageSize) // 起始地址 到 这一页尾(0~15 for 16 byte page) 能放得下数据buffer
				{
					memcpy(WriteBuffer+WrBfrOffset, BufferPtr, ByteCount);
					Status = xgpio_i2c_write(EEPROM_XGPIO_I2C_NO, EepromSlvAddr, WriteBuffer, ByteCount+WrBfrOffset, BUS_STOP);
				}
				else
				{
					memcpy(WriteBuffer+WrBfrOffset, BufferPtr, PageSize-start_page_offset); // PageSize-start_page_offset 范围是 1~16
					Status = xgpio_i2c_write(EEPROM_XGPIO_I2C_NO, EepromSlvAddr, WriteBuffer, PageSize-start_page_offset+WrBfrOffset, BUS_STOP);
				}
			}
			else if((page_cnt==end_page)&&(end_page!=start_page)&&(end_page_offset!=(PageSize-1))) // last page handle
			{
				if (PageSize == PAGE_SIZE_16 || PageSize == PAGE_SIZE_8) {
					EepromSlvAddr = EEPROM_ADDR | (((((page_cnt-1)*PageSize) & 0x3ff) >> 8));
					WriteBuffer[0] = (u8) (((page_cnt-start_page)*PageSize));
					WrBfrOffset = 1;
				} else {
					EepromSlvAddr = EEPROM_ADDR;
					WriteBuffer[0] = (u8) (((page_cnt-start_page)*PageSize) >> 8);
					WriteBuffer[1] = (u8) (((page_cnt-start_page)*PageSize));
					WrBfrOffset = 2;
				}
				// BufferPtr+(PageSize-start_page_offset) 是去掉写入到第一页的数据
				// end_page_offset+1 范围是 1~16
				memcpy(WriteBuffer+WrBfrOffset, BufferPtr+(PageSize-start_page_offset)+(page_cnt-start_page-1)*PageSize, end_page_offset+1);
				Status = xgpio_i2c_write(EEPROM_XGPIO_I2C_NO, EepromSlvAddr, WriteBuffer, end_page_offset+1+WrBfrOffset, BUS_STOP);
			}
			else
			{
				if (PageSize == PAGE_SIZE_16 || PageSize == PAGE_SIZE_8) {
					EepromSlvAddr = EEPROM_ADDR | (((((page_cnt-1)*PageSize) & 0x3ff) >> 8));
					WriteBuffer[0] = (u8) (((page_cnt-start_page)*PageSize));
					WrBfrOffset = 1;
				} else {
					EepromSlvAddr = EEPROM_ADDR;
					WriteBuffer[0] = (u8) (((page_cnt-start_page)*PageSize) >> 8);
					WriteBuffer[1] = (u8) (((page_cnt-start_page)*PageSize));
					WrBfrOffset = 2;
				}

				memcpy(WriteBuffer+WrBfrOffset, BufferPtr+(PageSize-start_page_offset)+(page_cnt-start_page-1)*PageSize, PageSize);
				Status = xgpio_i2c_write(EEPROM_XGPIO_I2C_NO, EepromSlvAddr, WriteBuffer, PageSize+WrBfrOffset, BUS_STOP);
			}

			if (Status != XST_SUCCESS)
			{
				return XST_FAILURE;
			}
			usleep(250000);
			page_cnt++;
		}

		usleep(250000);
	}
	else
	{
		if (PageSize == PAGE_SIZE_16 || PageSize == PAGE_SIZE_8) {
			EepromSlvAddr = EEPROM_ADDR | (((start_addr & 0x3ff) >> 8));
			WriteBuffer[0] = (u8) (start_addr);
			WrBfrOffset = 1;
		} else {
			EepromSlvAddr = EEPROM_ADDR;
			WriteBuffer[0] = (u8) (start_addr >> 8);
			WriteBuffer[1] = (u8) (start_addr);
			WrBfrOffset = 2;
		}
		Status = xgpio_i2c_write(EEPROM_XGPIO_I2C_NO, EepromSlvAddr, WriteBuffer, WrBfrOffset, BUS_STOP);

		if (Status != XST_SUCCESS)
		{
			return XST_FAILURE;
		}
	}

	return XST_SUCCESS;
}

s32 EepromReadData(u8 *BufferPtr, u16 ByteCount, u16 start_addr)
{
	s32 Status;
	AddressType Address = start_addr;
	u32 WrBfrOffset;
	u16 EepromSlvAddr;
//	volatile unsigned SentByteCount;

	/*
	 * Position the Pointer in EEPROM.
	 */
	if (PageSize == PAGE_SIZE_16 || PageSize == PAGE_SIZE_8) {
		EepromSlvAddr = EEPROM_ADDR | (((start_addr & 0x3ff) >> 8));
		WriteBuffer[0] = (u8) (Address);
		WrBfrOffset = 1;
	} else {
		EepromSlvAddr = EEPROM_ADDR;
		WriteBuffer[0] = (u8) (Address >> 8);
		WriteBuffer[1] = (u8) (Address);
		WrBfrOffset = 2;
	}

	Status = xgpio_i2c_write(EEPROM_XGPIO_I2C_NO, EepromSlvAddr, WriteBuffer, WrBfrOffset, NO_STOP);

	/*
	 * Receive the Data.
	 */
	Status = xgpio_i2c_read(EEPROM_XGPIO_I2C_NO, EepromSlvAddr, BufferPtr, ByteCount);
	if(ByteCount!=ByteCount)
	{
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

#endif // XPAR_XGPIO_I2C_0_AXI_GPIO_0_DEVICE_ID
#endif // EEPROM_USING_XGPIO


#if EEPROM_INTF == EEPROM_USING_EMIO
// 确保有设备，且要调用emio_i2c.c里的初始化
#if defined(XPAR_XGPIOPS_0_DEVICE_ID) && defined (__EMIO_I2C_H__)

typedef u16 AddressType;

//for at24c01/24c02 might be 0x50~0x57
//for at24c04 might be 0x50, 0x52, 0x54, 0x56, for cat24aa04 is 0x50 only
//for at24c08 might be 0x50, 0x54, for cat24aa08 is 0x50 only
//for at24c16 is 0x50 only
#define EEPROM_ADDR		0x50	// 7bit 0x50 == 8bit 0xA0

//#define I2C_MUX_ADDR    0x74  /**< I2C Mux Address */

// at24c01 has 16 pages of 8 byte each, 128 bytes in total
// at24c02 has 32 pages of 8 byte each, 256 bytes in total
// at24c04/cat24aa04 has 32 pages of 16 byte each, 512 bytes in total
// at24c08/cat24aa08 has 64 pages of 16 byte each, 1024 bytes in total
// at24c16 has 128 pages of 16 byte each, 2048 bytes in total
// at24c32 has 128 pages of 32 byte each, 4096 bytes in total
// at24c64 has 256 pages of 32 byte each, 8192 bytes in total
// cat24s64 has 128 pages of 64 byte each, 8192 bytes in total
// at24c128 has 256 pages of 64 byte each, 16384 bytes in total
// at24c256/m24256 has 512 pages of 64 byte each, 32768 bytes in total
#define MAX_SIZE		64
#define PAGE_SIZE_8		8
#define PAGE_SIZE_16	16
#define PAGE_SIZE_32	32
#define PAGE_SIZE_64	64

/*
 * Write buffer for writing a page.
 */
static u8 WriteBuffer[sizeof(AddressType) + MAX_SIZE];
static u8 ReadBuffer[MAX_SIZE];	/* Read buffer for reading a page. */

u32 PageSize = PAGE_SIZE_16;
u32 TotalPage = 64;

static int emio_i2c_read(emio_i2c_no i2c, u16 EepromSlvAddr, u8 *BufferPtr, u16 ByteCount)
{
	u8 rxd;
	u8 ack=0;
	u16 i;

	emio_i2c_start(i2c);

	emio_i2c_send_byte(i2c, EepromSlvAddr<<1 | 0x01);
  	ack=emio_i2c_recv_ack(i2c, EMIO_STRETCH_OFF);
	if(ack)
	{
		emio_i2c_stop(i2c);
		return XST_FAILURE;
	}

	for(i=0;i<ByteCount-1;i++)
	{
		*(BufferPtr+i) = emio_i2c_recv_byte(i2c);
		emio_i2c_ack(i2c);
	}

	*(BufferPtr+i) = emio_i2c_recv_byte(i2c);
	emio_i2c_nack(i2c);

	emio_i2c_stop(i2c);

  	return XST_SUCCESS;
}

static int emio_i2c_write(emio_i2c_no i2c, u16 EepromSlvAddr, u8 *BufferPtr, u16 ByteCount, opt_mode mode)
{
	u8 ack=0;
	u16 i;

	emio_i2c_start(i2c);

	emio_i2c_send_byte(i2c, EepromSlvAddr<<1);
	ack=emio_i2c_recv_ack(i2c, EMIO_STRETCH_OFF);
	if(ack)
	{
		emio_i2c_stop(i2c);
		return XST_FAILURE;
	}

	for(i=0;i<ByteCount;i++)
	{
		emio_i2c_send_byte(i2c, *(BufferPtr+i));
		ack=emio_i2c_recv_ack(i2c, EMIO_STRETCH_OFF);
		if(ack)
		{
			emio_i2c_stop(i2c);
			return XST_FAILURE;
		}

	}

	if(mode)
	{
		emio_i2c_stop(i2c);
	}

	return XST_SUCCESS;
}

s32 EepromWriteData(u8 *BufferPtr, u16 ByteCount, u16 start_addr)
{
	s32 Status;
	u8 start_page_offset,end_page_offset;
//	u16 end_addr;
	u16 start_page,end_page,page_cnt;
	u16 byte_cnt;
	u16 EepromSlvAddr;
	u32 WrBfrOffset;

	u8 WriteBuffer[PageSize+2]; // page 大小 加 最多2byte 片内地址
//	if (PageSize == PAGE_SIZE_16 || PageSize == PAGE_SIZE_8) {
//		WrBfrOffset = 1;
//	} else {
//		WrBfrOffset = 2;
//	}

	//EepromSlvAddr = EepromSlvAddr & ((start_addr & 0x3ff) >> 8);
	//WriteBuffer[0] = (u8)(start_addr & 0xff);// m24c08起始地址1byte
	//memcpy(WriteBuffer+1, BufferPtr, PAGE_SIZE_16);

	/*
	 * Send the Data.
	 */
	if(ByteCount>1)
	{
		start_page_offset = start_addr % PageSize; // 起始地址在一页内的偏移, 0~15 for 16 byte page
		end_page_offset = (start_addr + ByteCount-1) % PageSize; // 0~15 for 16 byte page
		start_page = start_addr / PageSize + 1; // 从1开始算  (0~15+16)/16 = 1, (16+16)/16 = 2, => (addr+pagesize)/pagesize=addr/pagesize+1, for 16 byte page devices
		// 计算结束的页，就是last page.从1开始算
		end_page = (start_addr + ByteCount-1) / PageSize + 1; // 从1开始算

		for(page_cnt=start_page; page_cnt<=end_page; )
		{
			if(page_cnt==start_page) // 第一个page，可能不是从头开始的
			{
				if (PageSize == PAGE_SIZE_16 || PageSize == PAGE_SIZE_8) {
					EepromSlvAddr = EEPROM_ADDR | (((start_addr & 0x3ff) >> 8));
					WriteBuffer[0] = (u8) (start_addr);
					WrBfrOffset = 1;
				} else {
					EepromSlvAddr = EEPROM_ADDR;
					WriteBuffer[0] = (u8) (start_addr >> 8);
					WriteBuffer[1] = (u8) (start_addr);
					WrBfrOffset = 2;
				}
				if(start_page_offset + ByteCount < PageSize) // 起始地址 到 这一页尾(0~15 for 16 byte page) 能放得下数据buffer
				{
					memcpy(WriteBuffer+WrBfrOffset, BufferPtr, ByteCount);
					Status = emio_i2c_write(EEPROM_EMIO_I2C_NO, EepromSlvAddr, WriteBuffer, ByteCount+WrBfrOffset, BUS_STOP);
				}
				else
				{
					memcpy(WriteBuffer+WrBfrOffset, BufferPtr, PageSize-start_page_offset); // PageSize-start_page_offset 范围是 1~16
					Status = emio_i2c_write(EEPROM_EMIO_I2C_NO, EepromSlvAddr, WriteBuffer, PageSize-start_page_offset+WrBfrOffset, BUS_STOP);
				}
			}
			else if((page_cnt==end_page)&&(end_page!=start_page)&&(end_page_offset!=(PageSize-1))) // last page handle
			{
				if (PageSize == PAGE_SIZE_16 || PageSize == PAGE_SIZE_8) {
					EepromSlvAddr = EEPROM_ADDR | (((((page_cnt-1)*PageSize) & 0x3ff) >> 8));
					WriteBuffer[0] = (u8) (((page_cnt-start_page)*PageSize));
					WrBfrOffset = 1;
				} else {
					EepromSlvAddr = EEPROM_ADDR;
					WriteBuffer[0] = (u8) (((page_cnt-start_page)*PageSize) >> 8);
					WriteBuffer[1] = (u8) (((page_cnt-start_page)*PageSize));
					WrBfrOffset = 2;
				}
				// BufferPtr+(PageSize-start_page_offset) 是去掉写入到第一页的数据
				// end_page_offset+1 范围是 1~16
				memcpy(WriteBuffer+WrBfrOffset, BufferPtr+(PageSize-start_page_offset)+(page_cnt-start_page-1)*PageSize, end_page_offset+1);
				Status = emio_i2c_write(EEPROM_EMIO_I2C_NO, EepromSlvAddr, WriteBuffer, end_page_offset+1+WrBfrOffset, BUS_STOP);
			}
			else
			{
				if (PageSize == PAGE_SIZE_16 || PageSize == PAGE_SIZE_8) {
					EepromSlvAddr = EEPROM_ADDR | (((((page_cnt-1)*PageSize) & 0x3ff) >> 8));
					WriteBuffer[0] = (u8) (((page_cnt-start_page)*PageSize));
					WrBfrOffset = 1;
				} else {
					EepromSlvAddr = EEPROM_ADDR;
					WriteBuffer[0] = (u8) (((page_cnt-start_page)*PageSize) >> 8);
					WriteBuffer[1] = (u8) (((page_cnt-start_page)*PageSize));
					WrBfrOffset = 2;
				}

				memcpy(WriteBuffer+WrBfrOffset, BufferPtr+(PageSize-start_page_offset)+(page_cnt-start_page-1)*PageSize, PageSize);
				Status = emio_i2c_write(EEPROM_EMIO_I2C_NO, EepromSlvAddr, WriteBuffer, PageSize+WrBfrOffset, BUS_STOP);
			}

			if (Status != XST_SUCCESS)
			{
				return XST_FAILURE;
			}
			usleep(250000);
			page_cnt++;
		}

		usleep(250000);
	}
	else
	{
		if (PageSize == PAGE_SIZE_16 || PageSize == PAGE_SIZE_8) {
			EepromSlvAddr = EEPROM_ADDR | (((start_addr & 0x3ff) >> 8));
			WriteBuffer[0] = (u8) (start_addr);
			WrBfrOffset = 1;
		} else {
			EepromSlvAddr = EEPROM_ADDR;
			WriteBuffer[0] = (u8) (start_addr >> 8);
			WriteBuffer[1] = (u8) (start_addr);
			WrBfrOffset = 2;
		}
		Status = emio_i2c_write(EEPROM_EMIO_I2C_NO, EepromSlvAddr, WriteBuffer, WrBfrOffset, BUS_STOP);

		if (Status != XST_SUCCESS)
		{
			return XST_FAILURE;
		}
	}

	return XST_SUCCESS;
}

s32 EepromReadData(u8 *BufferPtr, u16 ByteCount, u16 start_addr)
{
	s32 Status;
	AddressType Address = start_addr;
	u32 WrBfrOffset;
	u16 EepromSlvAddr;
//	volatile unsigned SentByteCount;

	/*
	 * Position the Pointer in EEPROM.
	 */
	if (PageSize == PAGE_SIZE_16 || PageSize == PAGE_SIZE_8) {
		EepromSlvAddr = EEPROM_ADDR | (((start_addr & 0x3ff) >> 8));
		WriteBuffer[0] = (u8) (Address);
		WrBfrOffset = 1;
	} else {
		EepromSlvAddr = EEPROM_ADDR;
		WriteBuffer[0] = (u8) (Address >> 8);
		WriteBuffer[1] = (u8) (Address);
		WrBfrOffset = 2;
	}

	Status = emio_i2c_write(EEPROM_EMIO_I2C_NO, EepromSlvAddr, WriteBuffer, WrBfrOffset, NO_STOP);

	/*
	 * Receive the Data.
	 */
	Status = emio_i2c_read(EEPROM_EMIO_I2C_NO, EepromSlvAddr, BufferPtr, ByteCount);
	if(ByteCount!=ByteCount)
	{
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

#endif // XPAR_XGPIOPS_0_DEVICE_ID
#endif // EEPROM_USING_XGPIO

/* main.c for eeprom test
#include "bsp.h"

u8 WriteBuffer_g[1024];
u8 ReadBuffer_g[1024];

int main()
{
	int Status ;

    init_platform();
	
	#if defined(XPAR_XGPIO_I2C_0_AXI_GPIO_0_DEVICE_ID)
    Status = xgpio_i2c_init();
    if (Status != XST_SUCCESS)
	{
		Xil_Assert(__FILE__, __LINE__);
		return XST_FAILURE ;
	}
	#endif // XPAR_XGPIO_I2C_0_AXI_GPIO_0_DEVICE_ID
	
	#if defined (XPAR_XIICPS_NUM_INSTANCES)
    ps_i2c_config();
	#endif // XPAR_XIICPS_NUM_INSTANCES
	
	#if defined(XPAR_AXI_IIC_0_DEVICE_ID)
    XIic_WriteReg(XPAR_IIC_0_BASEADDR, XIIC_GPO_REG_OFFSET, 0);
#endif // XPAR_AXI_IIC_0_DEVICE_ID

#if defined (XPAR_XGPIOPS_NUM_INSTANCES)
    emio_init();
#endif // XPAR_XGPIOPS_NUM_INSTANCES

#if 1
    u16 i=0;
    for(i=0;i<sizeof(WriteBuffer_g);i++)
    {
    	WriteBuffer_g[i] = i;
    	if(0 == (i%256))
    	{
    		WriteBuffer_g[i] = i/256;
    	}
    }

	EepromWriteData(WriteBuffer_g, sizeof(WriteBuffer_g), 0);
	EepromReadData(ReadBuffer_g, sizeof(ReadBuffer_g), 0);

#endif


#if 1


//	EepromReadData(ReadBuffer_g, 768, 256);
	EepromReadData(ReadBuffer_g, 512+3, 512-3);

#endif


#if 0
	WriteBuffer_g[0] = 0x0a;
	WriteBuffer_g[1] = 0xa0;
	WriteBuffer_g[2] = 0x50;
	WriteBuffer_g[3] = 0x05;

	EepromWriteData(WriteBuffer_g, 4, 518);
	EepromReadData(ReadBuffer_g, 512, 512);


#endif

#if 0
	WriteBuffer_g[0] = 0x0a;
	WriteBuffer_g[1] = 0xa0;
	WriteBuffer_g[2] = 0x51;
	WriteBuffer_g[3] = 0x25;
	WriteBuffer_g[4] = 0x3a;
	WriteBuffer_g[5] = 0xa4;
	WriteBuffer_g[6] = 0x55;
	WriteBuffer_g[7] = 0x65;
	WriteBuffer_g[8] = 0x7a;
	WriteBuffer_g[9] = 0xa7;
	WriteBuffer_g[10] = 0x58;
	WriteBuffer_g[11] = 0x95;
	WriteBuffer_g[12] = 0xaa;
	WriteBuffer_g[13] = 0xab;
	WriteBuffer_g[14] = 0x5c;
	WriteBuffer_g[15] = 0xd5;
	WriteBuffer_g[16] = 0x1a;
	WriteBuffer_g[17] = 0xa1;
	WriteBuffer_g[18] = 0x54;
	WriteBuffer_g[19] = 0x45;

	EepromWriteData(WriteBuffer_g, 20, 518);
	EepromReadData(ReadBuffer_g, 512, 512);

#endif
	while(1);
	cleanup_platform();
    return 0;
}
*/

#endif // USING_EEPROM

