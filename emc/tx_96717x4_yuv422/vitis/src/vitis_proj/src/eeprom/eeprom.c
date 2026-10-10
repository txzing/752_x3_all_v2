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

u32 PageSize = PAGE_SIZE_16;

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
	 * ByteCount > 0: write payload (includes 1-byte writes).
	 * ByteCount == 0: address-only seek (used by EepromReadData).
	 */
	if (ByteCount > 0U)
	{
		start_page_offset = start_addr % PageSize; // 起始地址在一页内的偏移, 0~15 for 16 byte page
		end_page_offset = (start_addr + ByteCount-1) % PageSize; // 0~15 for 16 byte page
		start_page = start_addr / PageSize + 1; // 从1开始算  (0~15+16)/16 = 1, (16+16)/16 = 2, => (addr+pagesize)/pagesize=addr/pagesize+1, for 16 byte page devices
		// 计算结束的页-就是last page.从1开始算
		end_page = (start_addr + ByteCount-1) / PageSize + 1; // 从1开始算

		for(page_cnt=start_page; page_cnt<=end_page; )
		{
			if(page_cnt==start_page) // 第一个page-可能不是从头开始的
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
					WriteBuffer[0] = (u8) (((page_cnt - 1U) * PageSize));
					WrBfrOffset = 1;
				} else {
					EepromSlvAddr = EEPROM_ADDR;
					WriteBuffer[0] = (u8) ((((page_cnt - 1U) * PageSize) >> 8));
					WriteBuffer[1] = (u8) (((page_cnt - 1U) * PageSize));
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
					WriteBuffer[0] = (u8) (((page_cnt - 1U) * PageSize));
					WrBfrOffset = 1;
				} else {
					EepromSlvAddr = EEPROM_ADDR;
					WriteBuffer[0] = (u8) ((((page_cnt - 1U) * PageSize) >> 8));
					WriteBuffer[1] = (u8) (((page_cnt - 1U) * PageSize));
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
			usleep(EEPROM_PAGE_WRITE_DELAY_US);
			page_cnt++;
		}

		usleep(EEPROM_PAGE_WRITE_DELAY_US);
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
s32 EepromWriteVerify(XIicPs *IicInstance, u8 *BufferPtr, u16 ByteCount,
		      u16 start_addr)
{
	s32 Status;
	u16 off;
	u16 chunk;
	u8 rb[MAX_SIZE];

	if ((IicInstance == NULL) || (BufferPtr == NULL) || (ByteCount == 0U))
	{
		return XST_FAILURE;
	}

	Status = EepromWriteData(IicInstance, BufferPtr, ByteCount, start_addr);
	if (Status != XST_SUCCESS)
	{
		return Status;
	}

	/* Read back in PageSize-sized chunks (rb[] is MAX_SIZE). */
	for (off = 0U; off < ByteCount; off = (u16)(off + chunk))
	{
		chunk = (u16)(ByteCount - off);
		if (chunk > (u16)PageSize)
		{
			chunk = (u16)PageSize;
		}
		if (chunk > (u16)MAX_SIZE)
		{
			chunk = (u16)MAX_SIZE;
		}
		Status = EepromReadData(IicInstance, rb, chunk,
					(u16)(start_addr + off));
		if (Status != XST_SUCCESS)
		{
			return Status;
		}
		if (memcmp(rb, &BufferPtr[off], chunk) != 0)
		{
			xil_printf("eeprom: verify fail @0x%04X len=%u\r\n",
				   (unsigned)(start_addr + off),
				   (unsigned)chunk);
			return XST_FAILURE;
		}
	}
	return XST_SUCCESS;
}

s32 EepromReadData(XIicPs *IicInstance, u8 *BufferPtr, u16 ByteCount, u16 start_addr)
{
	s32 Status;
	u16 EepromSlvAddr;

	Xil_AssertVoid(IicInstance != NULL);
	Xil_AssertVoid(IicInstance->IsReady == XIL_COMPONENT_IS_READY);

	/* Slave address high bits only needed for short-address (8/16B page) devices. */
	if (PageSize == PAGE_SIZE_16 || PageSize == PAGE_SIZE_8) {
		EepromSlvAddr = EEPROM_ADDR | (((start_addr & 0x3ff) >> 8));
	} else {
		EepromSlvAddr = EEPROM_ADDR;
	}

	/* Address pointer setup (ByteCount=0); payload of WriteBuffer unused. */
	Status = EepromWriteData(IicInstance, WriteBuffer, 0, start_addr);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	Status = XIicPs_MasterRecvPolled(IicInstance, BufferPtr,
						  ByteCount, EepromSlvAddr);
	if (Status != XST_SUCCESS) {
			return XST_FAILURE;
	}
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
		// 计算结束的页-就是last page.从1开始算
		end_page = (start_addr + ByteCount-1) / PageSize + 1; // 从1开始算

		for(page_cnt=start_page; page_cnt<=end_page; )
		{
			if(page_cnt==start_page) // 第一个page-可能不是从头开始的
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
					WriteBuffer[0] = (u8) (((page_cnt - 1U) * PageSize));
					WrBfrOffset = 1;
				} else {
					EepromSlvAddr = EEPROM_ADDR;
					WriteBuffer[0] = (u8) ((((page_cnt - 1U) * PageSize) >> 8));
					WriteBuffer[1] = (u8) (((page_cnt - 1U) * PageSize));
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
					WriteBuffer[0] = (u8) (((page_cnt - 1U) * PageSize));
					WrBfrOffset = 1;
				} else {
					EepromSlvAddr = EEPROM_ADDR;
					WriteBuffer[0] = (u8) ((((page_cnt - 1U) * PageSize) >> 8));
					WriteBuffer[1] = (u8) (((page_cnt - 1U) * PageSize));
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
			usleep(EEPROM_PAGE_WRITE_DELAY_US);
			page_cnt++;
		}

		usleep(EEPROM_PAGE_WRITE_DELAY_US);
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
// 确保有设备-且要调用xgpio_i2c.c里的初始化
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
		// 计算结束的页-就是last page.从1开始算
		end_page = (start_addr + ByteCount-1) / PageSize + 1; // 从1开始算

		for(page_cnt=start_page; page_cnt<=end_page; )
		{
			if(page_cnt==start_page) // 第一个page-可能不是从头开始的
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
					WriteBuffer[0] = (u8) (((page_cnt - 1U) * PageSize));
					WrBfrOffset = 1;
				} else {
					EepromSlvAddr = EEPROM_ADDR;
					WriteBuffer[0] = (u8) ((((page_cnt - 1U) * PageSize) >> 8));
					WriteBuffer[1] = (u8) (((page_cnt - 1U) * PageSize));
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
					WriteBuffer[0] = (u8) (((page_cnt - 1U) * PageSize));
					WrBfrOffset = 1;
				} else {
					EepromSlvAddr = EEPROM_ADDR;
					WriteBuffer[0] = (u8) ((((page_cnt - 1U) * PageSize) >> 8));
					WriteBuffer[1] = (u8) (((page_cnt - 1U) * PageSize));
					WrBfrOffset = 2;
				}

				memcpy(WriteBuffer+WrBfrOffset, BufferPtr+(PageSize-start_page_offset)+(page_cnt-start_page-1)*PageSize, PageSize);
				Status = xgpio_i2c_write(EEPROM_XGPIO_I2C_NO, EepromSlvAddr, WriteBuffer, PageSize+WrBfrOffset, BUS_STOP);
			}

			if (Status != XST_SUCCESS)
			{
				return XST_FAILURE;
			}
			usleep(EEPROM_PAGE_WRITE_DELAY_US);
			page_cnt++;
		}

		usleep(EEPROM_PAGE_WRITE_DELAY_US);
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
// 确保有设备-且要调用emio_i2c.c里的初始化
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
		// 计算结束的页-就是last page.从1开始算
		end_page = (start_addr + ByteCount-1) / PageSize + 1; // 从1开始算

		for(page_cnt=start_page; page_cnt<=end_page; )
		{
			if(page_cnt==start_page) // 第一个page-可能不是从头开始的
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
					WriteBuffer[0] = (u8) (((page_cnt - 1U) * PageSize));
					WrBfrOffset = 1;
				} else {
					EepromSlvAddr = EEPROM_ADDR;
					WriteBuffer[0] = (u8) ((((page_cnt - 1U) * PageSize) >> 8));
					WriteBuffer[1] = (u8) (((page_cnt - 1U) * PageSize));
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
					WriteBuffer[0] = (u8) (((page_cnt - 1U) * PageSize));
					WrBfrOffset = 1;
				} else {
					EepromSlvAddr = EEPROM_ADDR;
					WriteBuffer[0] = (u8) ((((page_cnt - 1U) * PageSize) >> 8));
					WriteBuffer[1] = (u8) (((page_cnt - 1U) * PageSize));
					WrBfrOffset = 2;
				}

				memcpy(WriteBuffer+WrBfrOffset, BufferPtr+(PageSize-start_page_offset)+(page_cnt-start_page-1)*PageSize, PageSize);
				Status = emio_i2c_write(EEPROM_EMIO_I2C_NO, EepromSlvAddr, WriteBuffer, PageSize+WrBfrOffset, BUS_STOP);
			}

			if (Status != XST_SUCCESS)
			{
				return XST_FAILURE;
			}
			usleep(EEPROM_PAGE_WRITE_DELAY_US);
			page_cnt++;
		}

		usleep(EEPROM_PAGE_WRITE_DELAY_US);
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

#endif // USING_EEPROM

