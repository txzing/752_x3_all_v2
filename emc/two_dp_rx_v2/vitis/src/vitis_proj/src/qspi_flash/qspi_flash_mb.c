#include "../bsp.h"

// #if (XPAR_AXI_QUAD_SPI_0_SPI_MODE == 2U)
// #if (XPAR_PROCESSOR_SUBSYSTEM_AXI_QUAD_SPI_0_SPI_MODE == 2U)
#if (XPAR_SPI_0_SPI_MODE == 2U)

#if defined (UDP_UPDATE) || defined (TCP_UPDATE)
XSpi  XSpiInstance;

u8 ReadBuffer[PAGE_SIZE + DATA_OFFSET + OVERHEAD_SIZE + DUMMY_SIZE + DUMMY_SIZE];
u8 WriteBuffer[PAGE_SIZE + DATA_OFFSET + DUMMY_SIZE];

FlashInfo Flash_Config_Table[28] = {
	/* Spansion 0-8 */
	{0x10000, 0x100, 256,  0x10000, 0x1000000, SPANSION_ID_BYTE0, SPANSION_ID_BYTE2_128, 0xFFFF0000, 1},	// 0
	{0x10000, 0x200, 256,  0x20000, 0x2000000, SPANSION_ID_BYTE0, SPANSION_ID_BYTE2_256, 0xFFFF0000, 1},	// 1
	{0x40000, 0x100, 512,  0x20000, 0x4000000, SPANSION_ID_BYTE0, SPANSION_ID_BYTE2_512, 0xFFFC0000, 1},	// 2
	/* Spansion 1Gbit is handled as 512Mbit stacked */
	/* Micron */
	{0x10000, 0x100,  256, 0x10000,  0x1000000, MICRON_ID_BYTE0, MICRON_ID_BYTE2_128, 0xFFFF0000, 1},	// 3
	{0x10000, 0x200,  256, 0x20000,  0x2000000, MICRON_ID_BYTE0, MICRON_ID_BYTE2_256, 0xFFFF0000, 1},	// 4
	{0x10000, 0x400,  256, 0x40000,  0x4000000, MICRON_ID_BYTE0, MICRON_ID_BYTE2_512, 0xFFFF0000, 2},	// 5
	{0x10000, 0x800,  256, 0x80000,  0x8000000, MICRON_ID_BYTE0, MICRON_ID_BYTE2_1G,  0xFFFF0000, 4},	// 6
	/* Winbond */
	{0x10000, 0x100, 256, 0x10000, 0x1000000, WINBOND_ID_BYTE0, WINBOND_ID_BYTE2_128, 0xFFFF0000, 1},	// 7
	{0x10000, 0x200, 256, 0x20000, 0x2000000, WINBOND_ID_BYTE0, WINBOND_ID_BYTE2_256, 0xFFFF0000, 1},	// 8
	/* Macronix */
	{0x10000, 0x800,  256, 0x80000,  0x8000000, MACRONIX_ID_BYTE0, MACRONIX_ID_BYTE2_1G, 0xFFFF0000, 4},// 9
	/* ISSI */
	{0x10000, 0x200, 256, 0x20000, 0x2000000, ISSI_ID_BYTE0, ISSI_ID_BYTE2_256, 0xFFFF0000, 1} //10
};
u32 FlashMake;
u32 FCTIndex;	/* Flash configuration table index */
u8 FSRFlag;


int FlashEnterExit4BAddMode(XSpi *QspiPtr, unsigned int Enable)
{
	int Status;
	u8 WriteDisableCmd = { WRITE_DISABLE_CMD };
	u8 WriteEnableCmd = { WRITE_ENABLE_CMD };
	u8 BankRegWREnCmd[] = { BANK_REG_WR,  (1 << 7)};
	u8 BankRegWRDisCmd[] = { BANK_REG_WR, (0 << 7)};
	u8 Cmd = { ENTER_4B_ADDR_MODE };
	u8 ReadStatusCmd[] = { READ_STATUS_CMD, 0 }; /* must send 2 bytes */
	u8 FlashStatus[2];

	if(Enable) {
		Cmd = ENTER_4B_ADDR_MODE;
	} else {
		if(FlashMake == ISSI_ID_BYTE0)
			Cmd = EXIT_4B_ADDR_MODE_ISSI;
		else
			Cmd = EXIT_4B_ADDR_MODE;
	}
	switch (FlashMake){
		case ISSI_ID_BYTE0:
		case MICRON_ID_BYTE0:
			Status = XSpi_Transfer(QspiPtr, &WriteEnableCmd, NULL, sizeof(WriteEnableCmd));
			if (Status != XST_SUCCESS) {
				return XST_FAILURE;
			}
			break;

		case SPANSION_ID_BYTE0:
			if(Enable)
			{
				Status = XSpi_Transfer(QspiPtr, &BankRegWREnCmd, NULL, sizeof(BankRegWREnCmd));
			}
			else
			{
				Status = XSpi_Transfer(QspiPtr, &BankRegWRDisCmd, NULL, sizeof(BankRegWRDisCmd));
			}
			if (Status != XST_SUCCESS) {
				return XST_FAILURE;
			}
			return Status;

		default:
			/*
			 * For Macronix and Winbond flash parts
			 * Write enable command is not required.
			 */
			break;
	}

	Status = XSpi_Transfer(QspiPtr, &Cmd, NULL, sizeof(Cmd));
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

    while (1) {
        /*
         * Poll the status register of the device to determine
         * when it completes, by sending a read status command
         * and receiving the status byte
         */
    	XSpi_Transfer(QspiPtr, ReadStatusCmd, FlashStatus, sizeof(ReadStatusCmd));

        /*
         * If the status indicates the write is done, then stop
         * waiting, if a value of 0xFF in the status byte is
         * read from the device and this loop never exits, the
         * device slave select is possibly incorrect such that
         * the device status is not being read
         */
        if(FSRFlag) {
        	if ((FlashStatus[1] & 0x80) != 0) {
        		break;
        	}
        } else {
        	if ((FlashStatus[1] & 0x01) == 0) {
        		break;
        	}
        }
    }

	Status = XSpi_Transfer(QspiPtr, &WriteDisableCmd, NULL, sizeof(WriteDisableCmd));
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
}


void show_flash_info(int idx)
{
	FlashInfo*flash_ptr=&Flash_Config_Table[idx];
	bsp_printf("SectSize:%d\r\n",flash_ptr->SectSize);
	bsp_printf("NumSect:%d\r\n",flash_ptr->NumSect);
	bsp_printf("SectMask:%d\r\n",flash_ptr->SectMask);
	bsp_printf("NumPage:%d\r\n",flash_ptr->NumPage);
	bsp_printf("PageSize:%d\r\n",flash_ptr->PageSize);
	bsp_printf("DeviceIDMemSize:%d\r\n",flash_ptr->DeviceIDMemSize);
	bsp_printf("FlashDeviceSize:%d\r\n",flash_ptr->FlashDeviceSize);
	bsp_printf("ManufacturerID:%d\r\n",flash_ptr->ManufacturerID);
	bsp_printf("NumDie:%d\r\n",flash_ptr->NumDie);
}

int qspi_init()
{
    int Status;

    XSpi_Config *XSpiConfigPtr;

    //??????QSPI????
    XSpiConfigPtr = XSpi_LookupConfig(QSPI_DEVICE_ID);
    if (XSpiConfigPtr == NULL) {
        return XST_FAILURE;
    }

    Status = XSpi_CfgInitialize(&XSpiInstance, XSpiConfigPtr, XSpiConfigPtr->BaseAddress);
    if (Status != XST_SUCCESS) {
        return XST_FAILURE;
    }

	if (XSpiInstance.SpiMode != XSP_QUAD_MODE) {
		return XST_FAILURE;
	}

	/*
	 * Set the Spi device as a master mode.
	 */
	Status = XSpi_SetOptions(&XSpiInstance, XSP_MASTER_OPTION | XSP_MANUAL_SSELECT_OPTION);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	// ¥¹¥ì?¥Ö¤ò??¤¹¤ë
	Status = XSpi_SetSlaveSelect(&XSpiInstance, 1);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Start the SPI driver so that the device is enabled.
	 */
	XSpi_Start(&XSpiInstance);

	/*
	 * Disable Global interrupt to use polled mode operation
	 */
	XSpi_IntrGlobalDisable(&XSpiInstance);


    FlashReadID();
    bsp_printf("FCTIndex: %d \n\r", FCTIndex);
    show_flash_info(FCTIndex);
    FlashQuadEnable(&XSpiInstance);

	/* Status cmd - SR or FSR selection */
	if((Flash_Config_Table[FCTIndex].NumDie > 1) && (FlashMake == MICRON_ID_BYTE0)) {
		FSRFlag = 1;
	} else {
		FSRFlag = 0;
	}

	if (Flash_Config_Table[FCTIndex].FlashDeviceSize > SIXTEENMB)
	{
		FlashEnterExit4BAddMode(&XSpiInstance, EXIT_4B);
	}

    return XST_SUCCESS;
}

int FlashGetStatus(XSpi *SpiPtr)
{
	int Status;

	/*
	 * Prepare the Write Buffer.
	 */
	WriteBuffer[0] = READ_STATUS_CMD;

	/*
	 * Initiate the Transfer.
	 */
//	TransferInProgress = TRUE;
	Status = XSpi_Transfer(SpiPtr, WriteBuffer, ReadBuffer, READ_STATUS_SIZE);
	if(Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Wait till the Transfer is complete and check if there are any errors
	 * in the transaction..
	 */
//	while(TransferInProgress);
//	if(ErrorCount != 0) {
//		ErrorCount = 0;
//		return XST_FAILURE;
//	}

	return XST_SUCCESS;
}

int FlashWaitForFlashReady(void)
{
	int Status;
	u8 StatusReg;

	while(1) {

		/*
		 * Get the Status Register. The status register content is
		 * stored at the second byte pointed by the ReadBuffer.
		 */
		Status = FlashGetStatus(&XSpiInstance);
		if(Status != XST_SUCCESS) {
			return XST_FAILURE;
		}

		/*
		 * Check if the flash is ready to accept the next command.
		 * If so break.
		 */
		StatusReg = ReadBuffer[1];
		if((StatusReg & FLASH_SR_IS_READY_MASK) == 0) {
			break;
		}
	}

	return XST_SUCCESS;
}

int qspi_update(u32 total_bytes, const u8 *flash_data, int type)
{
    u8 *BufferPtr;
    u8 pre_precent = -1;
    u8 process_percent = 0;
    u32 writed_len = 0;
    u32 readed_len = 0;
    u32 write_addr = 0;
    u32 read_addr  = 0;
    u32 erase_addr = 0;
    char msg[60];
    float start_time, over_time;
    float elapsed_time;
    int i;
    int total_page = total_bytes / PAGE_SIZE + 1;

    if(type==0)
    {
    	write_addr = 0x00900000;
    	read_addr  = 0x00900000;
    	erase_addr = 0x00900000;
    }

	if (Flash_Config_Table[FCTIndex].FlashDeviceSize > SIXTEENMB)
	{
		FlashEnterExit4BAddMode(&XSpiInstance, ENTER_4B);
	}

#if 1
    printf("Performing Erase Operation...\r\n");
    send_msg("Performing Erase Operation...\r\n");
//    start_time = get_time_s();
    FlashErase(&XSpiInstance, erase_addr, total_bytes);
//    over_time = get_time_s();
//    elapsed_time = over_time - start_time;
    printf("Erase Operation Successful.\r\n");
//    printf("INFO:Elapsed time = %2.3f sec.\r\n", elapsed_time);
    send_msg("Erase Operation Successful.\r\n");
//    sprintf(msg, "INFO:Elapsed time = %2.3f sec.\r\n",elapsed_time);
//    send_msg(msg);
    //?¨°FLASH??????????
#endif
#if 1
    printf("Performing Program Operation...\r\n");
    send_msg("Performing Program Operation...\r\n");
//    start_time = get_time_s();
    for (i = 0; i < total_page; i++) {
        process_percent = writed_len / (float) total_bytes * 10 + (float)1/2;
        if (process_percent != pre_precent)
            process_print(process_percent);
        pre_precent = process_percent;
		// Micron flash on REMUS doesn't support this 4B write/erase cmd
        if (Flash_Config_Table[FCTIndex].FlashDeviceSize > SIXTEENMB)
        {
        	memcpy(&WriteBuffer[DATA_OFFSET + DUMMY_SIZE], &flash_data[writed_len], PAGE_SIZE);
			if(Flash_Config_Table[FCTIndex].ManufacturerID == MICRON_ID_BYTE0)
			{
				FlashWrite(&XSpiInstance, write_addr, PAGE_SIZE, WRITE_CMD);
			}
			else
			{
				FlashWrite(&XSpiInstance, write_addr, PAGE_SIZE, WRITE_CMD_4B);
			}
        }
        else
        {
        	memcpy(&WriteBuffer[DATA_OFFSET], &flash_data[writed_len], PAGE_SIZE);
			FlashWrite(&XSpiInstance, write_addr, PAGE_SIZE, WRITE_CMD);
        }
        writed_len += PAGE_SIZE;
        write_addr += PAGE_SIZE;
    }
//    over_time = get_time_s();
//    elapsed_time = over_time - start_time;
    printf("Program Operation Successful.\r\n");
//    printf("INFO:Elapsed time = %2.3f sec.\r\n", elapsed_time);
    send_msg("Program Operation Successful.\r\n");
//    sprintf(msg, "INFO:Elapsed time = %2.3f sec.\r\n",elapsed_time);
//    send_msg(msg);
#endif
    //????QUAD??????FLASH???????????????????¨¦
    if (Flash_Config_Table[FCTIndex].FlashDeviceSize > SIXTEENMB)
    {
    	BufferPtr = &ReadBuffer[DATA_OFFSET + 4 + DUMMY_SIZE];
		//BufferPtr = &ReadBuffer[DATA_OFFSET + OVERHEAD_SIZE]; // tested on zynq for WINBOND, WRITE_CMD_4B and READ_CMD_4B
    }
    else
    {
    	BufferPtr = &ReadBuffer[DATA_OFFSET + 4];
    }
    printf("Performing Verify Operation...\r\n");
    send_msg("Performing Verify Operation...\r\n");
    memset(ReadBuffer, 0x00, sizeof(ReadBuffer));
//    start_time = get_time_s();
    while (readed_len < total_bytes) {
        process_percent = readed_len / (float) total_bytes * 10 + (float)1/2;
        if (process_percent != pre_precent)
            process_print(process_percent);
        pre_precent = process_percent;
        if (Flash_Config_Table[FCTIndex].FlashDeviceSize > SIXTEENMB)
        {
        	if(Flash_Config_Table[FCTIndex].ManufacturerID == WINBOND_ID_BYTE0)
        	{
        		FlashRead(&XSpiInstance, read_addr, PAGE_SIZE, READ_CMD_4B);
        	}
        	else
        	{
        		FlashRead(&XSpiInstance, read_addr, PAGE_SIZE, QUAD_READ_CMD_4B);
        	}
        }
        else
        {
        	FlashRead(&XSpiInstance, read_addr, PAGE_SIZE, QUAD_READ_CMD);
        }
        if ((readed_len + PAGE_SIZE) <= total_bytes) {
            //??¡À?????FLASH????FLASH????????????
            for (i = 0; i < PAGE_SIZE; i++)
                if (BufferPtr[i] != flash_data[readed_len + i])
                    goto error_printf;
        } else {
            for (i = 0; i < total_bytes - readed_len; i++)
                if (BufferPtr[i] != flash_data[readed_len + i])
                    goto error_printf;
//            over_time = get_time_s();
//            elapsed_time = over_time - start_time;
            printf("Verify Operation Successful.\r\n");
//            printf("INFO:Elapsed time = %2.3f sec.\r\n", elapsed_time);
            send_msg("Verify Operation Successful.\r\n");
//            sprintf(msg, "INFO:Elapsed time = %2.3f sec.\r\n",elapsed_time);
//            send_msg(msg);
        }
        readed_len += PAGE_SIZE;
        read_addr += PAGE_SIZE;
    }

	if (Flash_Config_Table[FCTIndex].FlashDeviceSize > SIXTEENMB)
	{
		FlashEnterExit4BAddMode(&XSpiInstance, EXIT_4B);
	}

    return XST_SUCCESS;

error_printf:
    printf("Verify data error at address 0x%lx\tSend Data is 0x%x\tRead Data is 0x%x\r\n",
            read_addr + i, flash_data[readed_len + i], BufferPtr[i]);
    sprintf(msg, "Verify data error at address 0x%lx.\r\n",read_addr + i);
    send_msg(msg);

	if (Flash_Config_Table[FCTIndex].FlashDeviceSize > SIXTEENMB)
	{
		FlashEnterExit4BAddMode(&XSpiInstance, EXIT_4B);
	}

    return XST_FAILURE;
}

void FlashWrite(XSpi *QspiPtr, u32 Address, u32 ByteCount, u8 Command)
{
    u8 WriteEnableCmd = { WRITE_ENABLE_CMD };
    u8 ReadStatusCmd[] = { READ_STATUS_CMD, 0 }; /* must send 2 bytes */
    u8 FlashStatus[2];
	u8 WriteDisableCmd = { WRITE_DISABLE_CMD };
	/* Status cmd - SR or FSR selection */
	if((Flash_Config_Table[FCTIndex].NumDie > 1) &&
			(FlashMake == MICRON_ID_BYTE0)) {
		ReadStatusCmd[0] = READ_FLAG_STATUS_CMD;
	} else {
		ReadStatusCmd[0] = READ_STATUS_CMD;
	}
    /*
     * Send the write enable command to the FLASH so that it can be
     * written to, this needs to be sent as a seperate transfer before
     * the write
     */
    XSpi_Transfer(QspiPtr, &WriteEnableCmd, NULL, sizeof(WriteEnableCmd));

    /*
     * Setup the write command with the specified address and data for the
     * FLASH
     */
    WriteBuffer[COMMAND_OFFSET] = Command;
	if (Flash_Config_Table[FCTIndex].FlashDeviceSize > SIXTEENMB)
    {
        WriteBuffer[ADDRESS_1_OFFSET] = (u8) ((Address & 0xFF000000) >> 24);
        WriteBuffer[ADDRESS_2_OFFSET] = (u8) ((Address & 0xFF0000) >> 16);
        WriteBuffer[ADDRESS_3_OFFSET] = (u8) ((Address & 0xFF00) >> 8);
        WriteBuffer[ADDRESS_4_OFFSET] = (u8) (Address & 0xFF);
        ByteCount += DUMMY_SIZE;
    }
    else
    {
        WriteBuffer[ADDRESS_1_OFFSET] = (u8) ((Address & 0xFF0000) >> 16);
        WriteBuffer[ADDRESS_2_OFFSET] = (u8) ((Address & 0xFF00) >> 8);
        WriteBuffer[ADDRESS_3_OFFSET] = (u8) (Address & 0xFF);
    }

    /*
     * Send the write command, address, and data to the FLASH to be
     * written, no receive buffer is specified since there is nothing to
     * receive
     */
    XSpi_Transfer(QspiPtr, WriteBuffer, NULL, ByteCount + OVERHEAD_SIZE);

    /*
     * Wait for the write command to the FLASH to be completed, it takes
     * some time for the data to be written
     */
    while (1) {
        /*
         * Poll the status register of the FLASH to determine when it
         * completes, by sending a read status command and receiving the
         * status byte
         */
    	XSpi_Transfer(QspiPtr, ReadStatusCmd, FlashStatus, sizeof(ReadStatusCmd));

        /*
         * If the status indicates the write is done, then stop waiting,
         * if a value of 0xFF in the status byte is read from the
         * device and this loop never exits, the device slave select is
         * possibly incorrect such that the device status is not being
         * read
         */
        if(FSRFlag) {
        	if ((FlashStatus[1] & 0x80) != 0) {
        		break;
        	}
        } else {
        	if ((FlashStatus[1] & 0x01) == 0) {
        		break;
        	}
        }
    }

	XSpi_Transfer(QspiPtr, &WriteDisableCmd, NULL, sizeof(WriteDisableCmd));
}

void FlashRead(XSpi *QspiPtr, u32 Address, u32 ByteCount, u8 Command)
{
	/*
	 * Wait while the Flash is busy.
	 */
	FlashWaitForFlashReady();
    /*
     * Setup the write command with the specified address and data for the
     * FLASH
     */
    WriteBuffer[COMMAND_OFFSET] = Command;
    if (Flash_Config_Table[FCTIndex].FlashDeviceSize > SIXTEENMB)
    {
        WriteBuffer[ADDRESS_1_OFFSET] = (u8) ((Address & 0xFF000000) >> 24);
        WriteBuffer[ADDRESS_2_OFFSET] = (u8) ((Address & 0xFF0000) >> 16);
        WriteBuffer[ADDRESS_3_OFFSET] = (u8) ((Address & 0xFF00) >> 8);
        WriteBuffer[ADDRESS_4_OFFSET] = (u8) (Address & 0xFF);
        ByteCount += DUMMY_SIZE;
    }
    else
    {
        WriteBuffer[ADDRESS_1_OFFSET] = (u8) ((Address & 0xFF0000) >> 16);
        WriteBuffer[ADDRESS_2_OFFSET] = (u8) ((Address & 0xFF00) >> 8);
        WriteBuffer[ADDRESS_3_OFFSET] = (u8) (Address & 0xFF);
    }

    if ((Command == FAST_READ_CMD) || (Command == FAST_READ_CMD_4B)) {
        ByteCount += DUMMY_SIZE;
    } else if ((Command == DUAL_READ_CMD) || (Command == DUAL_READ_CMD_4B)) {
    	ByteCount += 2;
    } else if ((Command == QUAD_READ_CMD) || (Command == QUAD_READ_CMD_4B)) {
    	ByteCount += 4;
    } else if ((READ_CMD_4B) || (Flash_Config_Table[FCTIndex].ManufacturerID == WINBOND_ID_BYTE0))
    {
    	ByteCount += 4;
    }

    /*
     * Send the read command to the FLASH to read the specified number
     * of bytes from the FLASH, send the read command and address and
     * receive the specified number of bytes of data in the data buffer
     */
    XSpi_Transfer(QspiPtr, WriteBuffer, ReadBuffer, ByteCount + OVERHEAD_SIZE);
}

int DieErase(XSpi *QspiPtr)
{
	u8 WriteEnableCmd = { WRITE_ENABLE_CMD };
	u8 ReadStatusCmd[] = { READ_STATUS_CMD, 0 }; /* must send 2 bytes */
	u8 FlashStatus[2];
	u8 DieCnt;
	int Status;

	/* Status cmd - SR or FSR selection */
	if((Flash_Config_Table[FCTIndex].NumDie > 1) && (FlashMake == MICRON_ID_BYTE0)) {
		ReadStatusCmd[0] = READ_FLAG_STATUS_CMD;
	} else {
		ReadStatusCmd[0] = READ_STATUS_CMD;
	}

	for(DieCnt = 0; DieCnt < Flash_Config_Table[FCTIndex].NumDie; DieCnt++) {
		Status = XSpi_Transfer(QspiPtr, &WriteEnableCmd, NULL, sizeof(WriteEnableCmd));
		if (Status != XST_SUCCESS) {
			return XST_FAILURE;
		}

		WriteBuffer[COMMAND_OFFSET] = DIE_ERASE_CMD;
	    Status = XSpi_Transfer(QspiPtr, WriteBuffer, NULL, BULK_ERASE_SIZE);
		if (Status != XST_SUCCESS) {
			return XST_FAILURE;
		}

	    /* Wait for the erase command to the FLASH to be completed*/
	    while (1) {
	        /*
	         * Poll the status register of the device to determine
	         * when it completes, by sending a read status command
	         * and receiving the status byte
	         */
	    	Status = XSpi_Transfer(QspiPtr, ReadStatusCmd, FlashStatus, sizeof(ReadStatusCmd));
	    	if (Status != XST_SUCCESS) {
	    		return XST_FAILURE;
	    	}
	        /*
	         * If the status indicates the write is done, then stop
	         * waiting; if a value of 0xFF in the status byte is
	         * read from the device and this loop never exits, the
	         * device slave select is possibly incorrect such that
	         * the device status is not being read
	         */
	        if(FSRFlag) {
	        	if ((FlashStatus[1] & 0x80) != 0) {
	        		break;
	        	}
	        } else {
	        	if ((FlashStatus[1] & 0x01) == 0) {
	        		break;
	        	}
	        }
	    }
	}
}

int BulkErase(XSpi *QspiPtr)
{
	u8 WriteEnableCmd = { WRITE_ENABLE_CMD };
	u8 ReadStatusCmd[] = { READ_STATUS_CMD, 0 }; /* must send 2 bytes */
	u8 FlashStatus[2];
	int Status;

	/* Status cmd - SR or FSR selection */
	if((Flash_Config_Table[FCTIndex].NumDie > 1) && (FlashMake == MICRON_ID_BYTE0)) {
		ReadStatusCmd[0] = READ_FLAG_STATUS_CMD;
	} else {
		ReadStatusCmd[0] = READ_STATUS_CMD;
	}

	Status = XSpi_Transfer(QspiPtr, &WriteEnableCmd, NULL, sizeof(WriteEnableCmd));
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

    /* Setup the bulk erase command*/
    WriteBuffer[COMMAND_OFFSET] = BULK_ERASE_CMD;

    /*
     * Send the bulk erase command; no receive buffer is specified
     * since there is nothing to receive
     */
    Status = XSpi_Transfer(QspiPtr, WriteBuffer, NULL, BULK_ERASE_SIZE);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

    /* Wait for the erase command to the FLASH to be completed*/
    while (1) {
        /*
         * Poll the status register of the device to determine
         * when it completes, by sending a read status command
         * and receiving the status byte
         */
    	Status = XSpi_Transfer(QspiPtr, ReadStatusCmd, FlashStatus, sizeof(ReadStatusCmd));
    	if (Status != XST_SUCCESS) {
    		return XST_FAILURE;
    	}
        /*
         * If the status indicates the write is done, then stop
         * waiting; if a value of 0xFF in the status byte is
         * read from the device and this loop never exits, the
         * device slave select is possibly incorrect such that
         * the device status is not being read
         */
        if(FSRFlag) {
        	if ((FlashStatus[1] & 0x80) != 0) {
        		break;
        	}
        } else {
        	if ((FlashStatus[1] & 0x01) == 0) {
        		break;
        	}
        }
    }

	return XST_SUCCESS;
}

void FlashErase(XSpi *QspiPtr, u32 Address, u32 ByteCount)
{
    u8 WriteEnableCmd = { WRITE_ENABLE_CMD };
    u8 ReadStatusCmd[] = { READ_STATUS_CMD, 0 }; /* must send 2 bytes */
    u8 FlashStatus[2];
    u8 WriteDisableCmd = { WRITE_DISABLE_CMD };
    int Sector;
    int total_sector;
    u8 pre_precent = -1;
    u8 process_percent = 0;

	/* Status cmd - SR or FSR selection */
	if((Flash_Config_Table[FCTIndex].NumDie > 1) &&
			(FlashMake == MICRON_ID_BYTE0)) {
		ReadStatusCmd[0] = READ_FLAG_STATUS_CMD;
	} else {
		ReadStatusCmd[0] = READ_STATUS_CMD;
	}

    /*
     * If erase size is same as the total size of the flash, use bulk erase
     * command
     */
    if (ByteCount == ((Flash_Config_Table[FCTIndex]).NumSect * (Flash_Config_Table[FCTIndex]).SectSize)) {
        /*
         * Send the write enable command to the FLASH so that it can be
         * written to, this needs to be sent as a seperate transfer
         * before the erase
         */
		if(Flash_Config_Table[FCTIndex].NumDie == 1) {
			/*
			 * Call Bulk erase
			 */
			BulkErase(QspiPtr);
		}

		if(Flash_Config_Table[FCTIndex].NumDie > 1) {
			/*
			 * Call Die erase
			 */
			DieErase(QspiPtr);
		}
        return;
    }

    /*
     * If the erase size is less than the total size of the flash, use
     * sector erase command
     */
    total_sector = (ByteCount / Flash_Config_Table[FCTIndex].SectSize) + 1;
    for (Sector = 0; Sector < total_sector; Sector++) {
        process_percent = Sector / (float) total_sector * 10 + (float)1/2;;
        if (process_percent != pre_precent)
            process_print(process_percent);
        pre_precent = process_percent;

        /*
         * Send the write enable command to the SEEPOM so that it can be
         * written to, this needs to be sent as a seperate transfer
         * before the write
         */
        XSpi_Transfer(QspiPtr, &WriteEnableCmd, NULL, sizeof(WriteEnableCmd));

        /*
         * Setup the write command with the specified address and data
         * for the FLASH
         */

        if (Flash_Config_Table[FCTIndex].FlashDeviceSize > SIXTEENMB)
        {
		// Micron flash on REMUS doesn't support this 4B write/erase cmd
            if(Flash_Config_Table[FCTIndex].ManufacturerID == MICRON_ID_BYTE0)
            {
            	WriteBuffer[COMMAND_OFFSET] = SEC_ERASE_CMD;
            }
            else
            {
				WriteBuffer[COMMAND_OFFSET] = SEC_ERASE_CMD_4B;
			}
			WriteBuffer[ADDRESS_1_OFFSET] = (u8) ((Address & 0xFF000000) >> 24);
			WriteBuffer[ADDRESS_2_OFFSET] = (u8) ((Address & 0xFF0000) >> 16);
			WriteBuffer[ADDRESS_3_OFFSET] = (u8) ((Address & 0xFF00) >> 8);
			WriteBuffer[ADDRESS_4_OFFSET] = (u8) (Address & 0xFF);
	        /*
	         * Send the sector erase command and address; no receive buffer
	         * is specified since there is nothing to receive
	         */
	        XSpi_Transfer(QspiPtr, WriteBuffer, NULL, SEC_ERASE_SIZE + DUMMY_SIZE);
        }
        else
        {
        	WriteBuffer[COMMAND_OFFSET] = SEC_ERASE_CMD;
        	WriteBuffer[ADDRESS_1_OFFSET] = (u8) (Address >> 16);
			WriteBuffer[ADDRESS_2_OFFSET] = (u8) (Address >> 8);
			WriteBuffer[ADDRESS_3_OFFSET] = (u8) (Address & 0xFF);
			/*
			 * Send the sector erase command and address; no receive buffer
			 * is specified since there is nothing to receive
			 */
			XSpi_Transfer(QspiPtr, WriteBuffer, NULL, SEC_ERASE_SIZE);
        }

        /*
         * Wait for the sector erse command to the
         * FLASH to be completed
         */
        while (1) {
            /*
             * Poll the status register of the device to determine
             * when it completes, by sending a read status command
             * and receiving the status byte
             */
        	XSpi_Transfer(QspiPtr, ReadStatusCmd, FlashStatus, sizeof(ReadStatusCmd));

            /*
             * If the status indicates the write is done, then stop
             * waiting, if a value of 0xFF in the status byte is
             * read from the device and this loop never exits, the
             * device slave select is possibly incorrect such that
             * the device status is not being read
             */
            if(FSRFlag) {
            	if ((FlashStatus[1] & 0x80) != 0) {
            		break;
            	}
            } else {
            	if ((FlashStatus[1] & 0x01) == 0) {
            		break;
            	}
            }
        }

        Address += Flash_Config_Table[FCTIndex].SectSize;
    }

    XSpi_Transfer(QspiPtr, &WriteDisableCmd, NULL, sizeof(WriteDisableCmd));
}

int FlashReadID(void)
{
    int Status;
    int StartIndex;

	Status = FlashWaitForFlashReady();
	if(Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

    /* Read ID in Auto mode.*/
    WriteBuffer[COMMAND_OFFSET] = READ_ID;
    WriteBuffer[ADDRESS_1_OFFSET] = 0x23; /* 3 dummy bytes */
    WriteBuffer[ADDRESS_2_OFFSET] = 0x08;
    WriteBuffer[ADDRESS_3_OFFSET] = 0x09;

    Status = XSpi_Transfer(&XSpiInstance, WriteBuffer, ReadBuffer, RD_ID_SIZE);
    if (Status != XST_SUCCESS) {
        return XST_FAILURE;
    }

    printf("FlashID=0x%x 0x%x 0x%x\n\r", ReadBuffer[1], ReadBuffer[2], ReadBuffer[3]);

	/*
	 * Deduce flash make
	 */
	if(ReadBuffer[1] == MICRON_ID_BYTE0) {
		FlashMake = MICRON_ID_BYTE0;
		StartIndex = MICRON_INDEX_START;
	}else if(ReadBuffer[1] == SPANSION_ID_BYTE0) {
		FlashMake = SPANSION_ID_BYTE0;
		StartIndex = SPANSION_INDEX_START;
	}else if(ReadBuffer[1] == WINBOND_ID_BYTE0) {
		FlashMake = WINBOND_ID_BYTE0;
		StartIndex = WINBOND_INDEX_START;
	} else if(ReadBuffer[1] == MACRONIX_ID_BYTE0) {
		FlashMake = MACRONIX_ID_BYTE0;
		StartIndex = MACRONIX_INDEX_START;
	} else if(ReadBuffer[1] == ISSI_ID_BYTE0) {
		FlashMake = ISSI_ID_BYTE0;
		StartIndex = ISSI_INDEX_START;
	}

	/*
	 * If valid flash ID, then check connection mode & size and
	 * assign corresponding index in the Flash configuration table
	 */
	if(((FlashMake == MICRON_ID_BYTE0) || (FlashMake == SPANSION_ID_BYTE0)||
			(FlashMake == WINBOND_ID_BYTE0)) &&
			(ReadBuffer[3] == MICRON_ID_BYTE2_128)) {
		FCTIndex = FLASH_CFG_TBL_SINGLE_128_SP + StartIndex;
	}

	/* 256 and 512Mbit supported only for Micron, Spansion and  Winbond */
	if(((FlashMake == MICRON_ID_BYTE0) || (FlashMake == SPANSION_ID_BYTE0) ||
			(FlashMake == WINBOND_ID_BYTE0)) &&
			(ReadBuffer[3] == MICRON_ID_BYTE2_256)) {
		FCTIndex = FLASH_CFG_TBL_SINGLE_256_SP + StartIndex;
	}
	if((FlashMake == ISSI_ID_BYTE0) &&
			(ReadBuffer[3] == MICRON_ID_BYTE2_256)) {
		FCTIndex = FLASH_CFG_TBL_SINGLE_256_ISSI;
	}
	if(((FlashMake == MICRON_ID_BYTE0) || (FlashMake == SPANSION_ID_BYTE0)) &&
			(ReadBuffer[3] == MICRON_ID_BYTE2_512)) {
		FCTIndex = FLASH_CFG_TBL_SINGLE_512_SP + StartIndex;
	}
	/*
	 * 1Gbit Single connection supported for Spansion.
	 * The ConnectionMode will indicate stacked as this part has 2 SS
	 * The device ID will indicate 512Mbit.
	 * This configuration is handled as the above 512Mbit stacked configuration
	 */
	/* 1Gbit single supported for Micron */
	if((FlashMake == MICRON_ID_BYTE0) &&
			(ReadBuffer[3] == MICRON_ID_BYTE2_1G)) {
		FCTIndex = FLASH_CFG_TBL_SINGLE_1GB_MC;
	}
	/* 1Gbit single supported for Macronix */
	if(((FlashMake == MACRONIX_ID_BYTE0) &&
			(ReadBuffer[3] == MACRONIX_ID_BYTE2_1G))) {
		FCTIndex = FLASH_CFG_TBL_SINGLE_1G_MX;
	}

    return XST_SUCCESS;
}

void FlashQuadEnable(XSpi *QspiPtr)
{
    u8 WriteEnableCmd = { WRITE_ENABLE_CMD };
    u8 ReadStatusCmd[] = { READ_STATUS_CMD, 0 };
    u8 QuadEnableCmd[] = { WRITE_STATUS_CMD, 0 };
    u8 FlashStatus[2];

    if (FlashMake == ISSI_ID_BYTE0)
    {

    	XSpi_Transfer(QspiPtr, ReadStatusCmd, FlashStatus, sizeof(ReadStatusCmd));

        QuadEnableCmd[1] = FlashStatus[1] | 1 << 6;

        XSpi_Transfer(QspiPtr, &WriteEnableCmd, NULL, sizeof(WriteEnableCmd));

        XSpi_Transfer(QspiPtr, QuadEnableCmd, NULL, sizeof(QuadEnableCmd));
    }
    else if (FlashMake == WINBOND_ID_BYTE0)
    {
    	XSpi_Transfer(QspiPtr, &WriteEnableCmd, NULL,
    	                sizeof(WriteEnableCmd));
    	FlashWaitForFlashReady();

		WriteBuffer[COMMAND_OFFSET] = 0x01;
		WriteBuffer[ADDRESS_1_OFFSET] = 0x00;
		WriteBuffer[ADDRESS_2_OFFSET] = 0x02;
		XSpi_Transfer(QspiPtr, WriteBuffer, NULL, 3);

		FlashWaitForFlashReady();
		WriteBuffer[COMMAND_OFFSET] = 0x35;
		XSpi_Transfer(QspiPtr, WriteBuffer, ReadBuffer, 2);
    }
}
#endif  // #if defined (UDP_UPDATE) || defined (TCP_UPDATE)

//#endif // #if (XPAR_AXI_QUAD_SPI_0_SPI_MODE == 2U)
//#endif // #if (XPAR_PROCESSOR_SUBSYSTEM_AXI_QUAD_SPI_0_SPI_MODE == 2U)
#endif // #if (XPAR_SPI_0_SPI_MODE == 2U)

