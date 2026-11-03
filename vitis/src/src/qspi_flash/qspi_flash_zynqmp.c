#include "../bsp.h"

#if defined (XPAR_XQSPIPSU_NUM_INSTANCES)

//#include "xparameters.h"	/* SDK generated parameters */
//#include "xqspipsu.h"		/* QSPIPSU device driver */
//#include "xil_cache.h"
//#include "xtime_l.h"
//#include "stdio.h"
//#include "qspi_flash.h"

#if (XPAR_XQSPIPSU_NUM_INSTANCES == 1U)
XQspiPsu QspiInstance;
#endif // XPAR_XQSPIPSU_NUM_INSTANCES

u8 ReadCmd;
u8 WriteCmd;
u8 StatusCmd;
u8 SectorEraseCmd;
u8 FSRFlag;

/************************** Variable Definitions *****************************/
u8 TXBUF[MAX_TX_SIZE] __attribute__ ((aligned(64)));
u8 TxBfrPtr;
u8 ReadBfrPtr[3];
//FlashInfo Flash_Config_Table[] = {
//		/* Micron */
//		{SECTOR_SIZE_64K, NUM_OF_SECTORS1024, BYTES256_PER_PAGE,
//				0x40000, 0x2000000, MICRON_ID_BYTE0,
//				MICRON_ID_BYTE2_256, 0xFFFF0000, 1}
//};
FlashInfo Flash_Config_Table[28] = {
	/* Spansion 0-8 */
	{0x10000, 0x100, 256,  0x10000, 0x1000000, SPANSION_ID_BYTE0, SPANSION_ID_BYTE2_128, 0xFFFF0000, 1},	// 0
	{0x10000, 0x200, 256,  0x20000, 0x1000000, SPANSION_ID_BYTE0, SPANSION_ID_BYTE2_128, 0xFFFF0000, 1},	// 1
	{0x20000, 0x100, 512,  0x10000, 0x1000000, SPANSION_ID_BYTE0, SPANSION_ID_BYTE2_128, 0xFFFE0000, 1},	// 2
	{0x10000, 0x200, 256,  0x20000, 0x2000000, SPANSION_ID_BYTE0, SPANSION_ID_BYTE2_256, 0xFFFF0000, 1},	// 3
	{0x10000, 0x400, 256,  0x40000, 0x2000000, SPANSION_ID_BYTE0, SPANSION_ID_BYTE2_256, 0xFFFF0000, 1},	// 4
	{0x20000, 0x200, 512,  0x20000, 0x2000000, SPANSION_ID_BYTE0, SPANSION_ID_BYTE2_256, 0xFFFE0000, 1},	// 5
	{0x40000, 0x100, 512,  0x20000, 0x4000000, SPANSION_ID_BYTE0, SPANSION_ID_BYTE2_512, 0xFFFC0000, 1},	// 6
	{0x40000, 0x200, 512,  0x40000, 0x4000000, SPANSION_ID_BYTE0, SPANSION_ID_BYTE2_512, 0xFFFC0000, 1},	// 7
	{0x80000, 0x100, 1024, 0x20000, 0x4000000, SPANSION_ID_BYTE0, SPANSION_ID_BYTE2_512, 0xFFF80000, 1},	// 8
	/* Spansion 1Gbit is handled as 512Mbit stacked */
	/* Micron */
	{0x10000, 0x100,  256, 0x10000,  0x1000000, MICRON_ID_BYTE0, MICRON_ID_BYTE2_128, 0xFFFF0000, 1},	// 9
	{0x10000, 0x200,  256, 0x20000,  0x1000000, MICRON_ID_BYTE0, MICRON_ID_BYTE2_128, 0xFFFF0000, 1},	// 10
	{0x20000, 0x100,  512, 0x10000,  0x1000000, MICRON_ID_BYTE0, MICRON_ID_BYTE2_128, 0xFFFE0000, 1},	// 11
	{0x10000, 0x200,  256, 0x20000,  0x2000000, MICRON_ID_BYTE0, MICRON_ID_BYTE2_256, 0xFFFF0000, 1},	// 12
	{0x10000, 0x400,  256, 0x40000,  0x2000000, MICRON_ID_BYTE0, MICRON_ID_BYTE2_256, 0xFFFF0000, 1},	// 13
	{0x20000, 0x200,  512, 0x20000,  0x2000000, MICRON_ID_BYTE0, MICRON_ID_BYTE2_256, 0xFFFE0000, 1},	// 14
	{0x10000, 0x400,  256, 0x40000,  0x4000000, MICRON_ID_BYTE0, MICRON_ID_BYTE2_512, 0xFFFF0000, 2},	// 15
	{0x10000, 0x800,  256, 0x80000,  0x4000000, MICRON_ID_BYTE0, MICRON_ID_BYTE2_512, 0xFFFF0000, 2},	// 16
	{0x20000, 0x400,  512, 0x40000,  0x4000000, MICRON_ID_BYTE0, MICRON_ID_BYTE2_512, 0xFFFE0000, 2},	// 17
	{0x10000, 0x800,  256, 0x80000,  0x8000000, MICRON_ID_BYTE0, MICRON_ID_BYTE2_1G,  0xFFFF0000, 4},	// 18
	{0x10000, 0x1000, 256, 0x100000, 0x8000000, MICRON_ID_BYTE0, MICRON_ID_BYTE2_1G,  0xFFFF0000, 4},	// 19
	{0x20000, 0x800,  512, 0x80000,  0x8000000, MICRON_ID_BYTE0, MICRON_ID_BYTE2_1G,  0xFFFE0000, 4},	// 20
	/* Winbond */
	{0x10000, 0x100, 256, 0x10000, 0x1000000, WINBOND_ID_BYTE0, WINBOND_ID_BYTE2_128, 0xFFFF0000, 1},	// 21
	{0x10000, 0x200, 256, 0x20000, 0x1000000, WINBOND_ID_BYTE0, WINBOND_ID_BYTE2_128, 0xFFFF0000, 1},	// 22
	{0x20000, 0x100, 512, 0x10000, 0x1000000, WINBOND_ID_BYTE0, WINBOND_ID_BYTE2_128, 0xFFFE0000, 1},	// 23
	/* Macronix */
	{0x10000, 0x800,  256, 0x80000,  0x8000000, MACRONIX_ID_BYTE0, MACRONIX_ID_BYTE2_1G, 0xFFFF0000, 4},// 24
	{0x10000, 0x1000, 256, 0x100000, 0x8000000, MACRONIX_ID_BYTE0, MACRONIX_ID_BYTE2_1G, 0xFFFF0000, 4},// 25
	{0x20000, 0x800,  512, 0x80000,  0x8000000, MACRONIX_ID_BYTE0, MACRONIX_ID_BYTE2_1G, 0xFFFE0000, 4},// 26
	/* ISSI */
	{0x10000, 0x200, 256, 0x20000, 0x2000000, ISSI_ID_BYTE0, ISSI_ID_BYTE2_256, 0xFFFF0000, 1},	//27
	{0x10000, 0x400, 256, 0x40000, 0x2000000, ISSI_ID_BYTE0, ISSI_ID_BYTE2_256, 0xFFFF0000, 1},	//28
	{0x20000, 0x200, 512, 0x20000, 0x2000000, ISSI_ID_BYTE0, ISSI_ID_BYTE2_256, 0xFFFF0000, 1}	//29
};
u32 FlashMake;
u32 FCTIndex;	/* Flash configuration table index */


/*
 * The instances to support the device drivers are global such that they
 * are initialized to zero each time the program runs. They could be local
 * but should at least be static so they are zeroed.
 */

static XQspiPsu_Msg FlashMsg[5];


u8 ReadBuffer[PAGE_SIZE + DATA_OFFSET + DUMMY_SIZE] __attribute__ ((aligned(64)));
u8 WriteBuffer[PAGE_SIZE + DATA_OFFSET] __attribute__ ((aligned(64)));

u8 CmdBfr[8];

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

    //XQspiPs_Config *QspiConfig;
	XQspiPsu_Config *QspiPsuConfig;

//    QspiConfig = XQspiPs_LookupConfig(QSPIPS_DEVICE_ID);
//    if (QspiConfig == NULL) {
//        return XST_FAILURE;
//    }
	QspiPsuConfig = XQspiPsu_LookupConfig(QSPIPSU_DEVICE_ID);
	if (QspiPsuConfig == NULL) {
		return XST_FAILURE;
	}

//    Status = XQspiPs_CfgInitialize(&QspiInstance, QspiConfig,
//            QspiConfig->BaseAddress);
//    if (Status != XST_SUCCESS) {
//        return XST_FAILURE;
//    }
	Status = XQspiPsu_CfgInitialize(&QspiInstance, QspiPsuConfig,
			QspiPsuConfig->BaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}


//    XQspiPs_SetOptions(&QspiInstance, XQSPIPS_FORCE_SSELECT_OPTION | XQSPIPS_HOLD_B_DRIVE_OPTION);
    XQspiPsu_SetOptions(&QspiInstance, XQSPIPSU_MANUAL_START_OPTION);

//    XQspiPs_SetClkPrescaler(&QspiInstance, XQSPIPS_CLK_PRESCALE_8);
	XQspiPsu_SetClkPrescaler(&QspiInstance, XQSPIPSU_CLK_PRESCALE_8);

//    XQspiPs_SetSlaveSelect(&QspiInstance);
	XQspiPsu_SelectFlash(&QspiInstance,
			XQSPIPSU_SELECT_FLASH_CS_LOWER,
			XQSPIPSU_SELECT_FLASH_BUS_LOWER);


//    FlashReadID();
	Status = FlashReadID(&QspiInstance);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	bsp_printf("Flash connection mode : %d \n\r",
			QspiPsuConfig->ConnectionMode);
	bsp_printf("where 0 - Single; 1 - Stacked; 2 - Parallel \n\r");
	bsp_printf("FCTIndex: %d \n\r", FCTIndex);

	show_flash_info(FCTIndex);

//    FlashQuadEnable(&QspiInstance);
	/*
	 * Address size and read command selection
	 * Micron flash on REMUS doesn't support this 4B write/erase cmd
	 */
	if (Flash_Config_Table[FCTIndex].FlashDeviceSize > SIXTEENMB)
	{
		ReadCmd=QUAD_READ_CMD_4B;
	}
	else
	{
		ReadCmd = QUAD_READ_CMD;
	}
	WriteCmd = WRITE_CMD;
	SectorEraseCmd = SEC_ERASE_CMD;

	/* Status cmd - SR or FSR selection  */
	if((Flash_Config_Table[FCTIndex].NumDie > 1) && (FlashMake == MICRON_ID_BYTE0))
	{
		StatusCmd = READ_FLAG_STATUS_CMD;
		FSRFlag = 1;
	} else {
		StatusCmd = READ_STATUS_CMD;
		FSRFlag = 0;
	}

	bsp_printf("ReadCmd: 0x%x, WriteCmd: 0x%x, StatusCmd: 0x%x, FSRFlag: %d\n\r",
			ReadCmd, WriteCmd, StatusCmd, FSRFlag);

	if (Flash_Config_Table[FCTIndex].FlashDeviceSize > SIXTEENMB) {
		Status = FlashEnterExit4BAddMode(&QspiInstance, EXIT_4B);
		if (Status != XST_SUCCESS) {
			return XST_FAILURE;
		}
	}
    return XST_SUCCESS;
}
#if defined (UDP_UPDATE) || defined (TCP_UPDATE)
//int qspi_update(u32 total_bytes, const u8 *flash_data)
int qspi_update(u32 total_bytes, const u8 *flash_data, int type)
{
	int Status;
    u8 *BufferPtr;
    u8 pre_precent = -1;
    u8 process_percent = 0;
    u32 writed_len = 0;
    u32 readed_len = 0;
    u32 write_addr = 0;
    u32 read_addr = 0;
    u32 erase_addr = 0;
    char msg[60];
    float start_time, over_time;
    float elapsed_time;
    int i;
    int total_page = total_bytes / PAGE_SIZE + 1;

    if(type==0)
    {
    	write_addr = 0x00800000;
    	read_addr  = 0x00800000;
    	erase_addr = 0x00800000;
    }

	if (Flash_Config_Table[FCTIndex].FlashDeviceSize > SIXTEENMB) {
		Status = FlashEnterExit4BAddMode(&QspiInstance, ENTER_4B);
		if (Status != XST_SUCCESS) {
			return XST_FAILURE;
		}
	}

    bsp_printf("Performing Erase Operation...\r\n");
    send_msg("Performing Erase Operation...\r\n");
    start_time = get_time_s();
	Status = FlashErase(&QspiInstance, erase_addr, total_bytes, CmdBfr);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
    over_time = get_time_s();
    elapsed_time = over_time - start_time;
    bsp_printf("Erase Operation Successful.\r\n");
    bsp_printf("INFO:Elapsed time = %2.3f sec.\r\n", elapsed_time);
    send_msg("Erase Operation Successful.\r\n");
    sprintf(msg, "INFO:Elapsed time = %2.3f sec.\r\n",elapsed_time);
    send_msg(msg);

    bsp_printf("Performing Program Operation...\r\n");
    send_msg("Performing Program Operation...\r\n");
    start_time = get_time_s();
    for (i = 0; i < total_page; i++) {
        process_percent = writed_len / (float) total_bytes * 10 + (float)1/2;
        if (process_percent != pre_precent)
            process_print(process_percent);
        pre_precent = process_percent;

        memcpy(&WriteBuffer[0], &flash_data[writed_len], PAGE_SIZE);
        FlashWrite(&QspiInstance, write_addr, PAGE_SIZE, WRITE_CMD, WriteBuffer);
        writed_len += PAGE_SIZE;
        write_addr += PAGE_SIZE;
    }
    over_time = get_time_s();
    elapsed_time = over_time - start_time;
    bsp_printf("Program Operation Successful.\r\n");
    printf("INFO:Elapsed time = %2.3f sec.\r\n", elapsed_time);
    send_msg("Program Operation Successful.\r\n");
    sprintf(msg, "INFO:Elapsed time = %2.3f sec.\r\n",elapsed_time);
    send_msg(msg);


//    BufferPtr = &ReadBuffer[DATA_OFFSET + DUMMY_SIZE];
    printf("Performing Verify Operation...\r\n");
    send_msg("Performing Verify Operation...\r\n");
    memset(ReadBuffer, 0x00, sizeof(ReadBuffer));
    start_time = get_time_s();
    while (readed_len < total_bytes) {
        process_percent = readed_len / (float) total_bytes * 10 + (float)1/2;
        if (process_percent != pre_precent)
            process_print(process_percent);
        pre_precent = process_percent;
        if (Flash_Config_Table[FCTIndex].FlashDeviceSize > SIXTEENMB)
        {
        	FlashRead(&QspiInstance, read_addr, PAGE_SIZE, QUAD_READ_CMD_4B, CmdBfr, ReadBuffer);
        }
        else
        {
        	FlashRead(&QspiInstance, read_addr, PAGE_SIZE, QUAD_READ_CMD, CmdBfr, ReadBuffer);
        }
        if ((readed_len + PAGE_SIZE) <= total_bytes) {
            for (i = 0; i < PAGE_SIZE; i++)
                if (ReadBuffer[i] != flash_data[readed_len + i])
                    goto error_printf;
        } else {
            for (i = 0; i < total_bytes - readed_len; i++)
                if (ReadBuffer[i] != flash_data[readed_len + i])
                    goto error_printf;
            over_time = get_time_s();
            elapsed_time = over_time - start_time;
            bsp_printf("Verify Operation Successful.\r\n");
            printf("INFO:Elapsed time = %2.3f sec.\r\n", elapsed_time);
            send_msg("Verify Operation Successful.\r\n");
            sprintf(msg, "INFO:Elapsed time = %2.3f sec.\r\n",elapsed_time);
            send_msg(msg);
        }
        readed_len += PAGE_SIZE;
        read_addr += PAGE_SIZE;
    }

	if (Flash_Config_Table[FCTIndex].FlashDeviceSize > SIXTEENMB) {
		Status = FlashEnterExit4BAddMode(&QspiInstance, EXIT_4B);
		if (Status != XST_SUCCESS) {
			return XST_FAILURE;
		}
	}

    return XST_SUCCESS;

error_printf:
	bsp_printf("Verify data error at address 0x%lx\tSend Data is 0x%x\tRead Data is 0x%x\r\n",
            read_addr + i, flash_data[readed_len + i], BufferPtr[i]);
    sprintf(msg, "Verify data error at address 0x%lx.\r\n",read_addr + i);
    send_msg(msg);

	if (Flash_Config_Table[FCTIndex].FlashDeviceSize > SIXTEENMB) {
		Status = FlashEnterExit4BAddMode(&QspiInstance, EXIT_4B);
		if (Status != XST_SUCCESS) {
			return XST_FAILURE;
		}
	}

    return XST_FAILURE;
}
#endif // UDP_UPDATE || TCP_UPDATE

/*****************************************************************************/
/**
 *Update flash data
 *
 * @param	None.
 *
 * @return	XST_SUCCESS if successful, else XST_FAILURE.
 *
 * @note	None.
 *
 *****************************************************************************/
//int update_qspi(XQspiPsu *QspiPsuInstancePtr, u16 QspiPsuDeviceId, unsigned int TotoalLen, char *FlashDataToSend)
//{
//	int Status;
//	int i ;
//	unsigned int HasSendNum = 0 ;
//	unsigned int WriteAddr = 0 ;
//	unsigned int HasRecvNum = 0 ;
//	unsigned int ReadAddr = 0 ;
//	XTime TimerStart, TimerEnd;
//	float elapsed_time ;
//
//	int PercentCurr = -1 ;
//	int PercentLast = -1 ;
//
//
//	XQspiPsu_Config *QspiPsuConfig;
//
//	/*
//	 * Initialize the QSPIPSU driver so that it's ready to use
//	 */
//	QspiPsuConfig = XQspiPsu_LookupConfig(QspiPsuDeviceId);
//	if (QspiPsuConfig == NULL) {
//		return XST_FAILURE;
//	}
//
//	/* To test, change connection mode here if not obtained from HDF */
//
//	Status = XQspiPsu_CfgInitialize(QspiPsuInstancePtr, QspiPsuConfig,
//			QspiPsuConfig->BaseAddress);
//	if (Status != XST_SUCCESS) {
//		return XST_FAILURE;
//	}
//
//	/*
//	 * Set Manual Start
//	 */
//	XQspiPsu_SetOptions(QspiPsuInstancePtr, XQSPIPSU_MANUAL_START_OPTION);
//
//	/*
//	 * Set the prescaler for QSPIPSU clock
//	 */
//	XQspiPsu_SetClkPrescaler(QspiPsuInstancePtr, XQSPIPSU_CLK_PRESCALE_8);
//
//	XQspiPsu_SelectFlash(QspiPsuInstancePtr,
//			XQSPIPSU_SELECT_FLASH_CS_LOWER,
//			XQSPIPSU_SELECT_FLASH_BUS_LOWER);
//
//	/*
//	 * Read flash ID and obtain all flash related information
//	 * It is important to call the read id function before
//	 * performing proceeding to any operation, including
//	 * preparing the WriteBuffer
//	 */
//	Status = FlashReadID(QspiPsuInstancePtr);
//	if (Status != XST_SUCCESS) {
//		return XST_FAILURE;
//	}
//
//	bsp_printf("Flash connection mode : %d\n\r",
//			QspiPsuConfig->ConnectionMode);
//	bsp_printf("where 0 - Single; 1 - Stacked; 2 - Parallel\n\r");
//	bsp_printf("FCTIndex: %d\n\r", FCTIndex);
//	/*
//	 * Address size and read command selection
//	 * Micron flash on REMUS doesn't support this 4B write/erase cmd
//	 */
//	ReadCmd = QUAD_READ_CMD;
//	WriteCmd = WRITE_CMD;
//	SectorEraseCmd = SEC_ERASE_CMD;
//
//	/* Status cmd  */
//
//	StatusCmd = READ_STATUS_CMD;
//	FSRFlag = 0;
//
//
//	bsp_printf("ReadCmd: 0x%x, WriteCmd: 0x%x,"
//			" StatusCmd: 0x%x, FSRFlag: %d\n\r",
//			ReadCmd, WriteCmd, StatusCmd, FSRFlag);
//
//	if (Flash_Config_Table[FCTIndex].FlashDeviceSize > SIXTEENMB) {
//		Status = FlashEnterExit4BAddMode(QspiPsuInstancePtr, ENTER_4B);
//		if (Status != XST_SUCCESS) {
//			return XST_FAILURE;
//		}
//	}
//
//
//	/*
//	 * Erase the flash.
//	 */
//	printf("Performing Erase Operation...\r\n") ;
//
//	XTime_SetTime(0) ;
//	XTime_GetTime(&TimerStart) ;
//	Status = FlashErase(QspiPsuInstancePtr, 0, TotoalLen, CmdBfr);
//	if (Status != XST_SUCCESS) {
//		return XST_FAILURE;
//	}
//	XTime_GetTime(&TimerEnd) ;
//	printf("100%%\r\n") ;
//	elapsed_time = ((float)(TimerEnd-TimerStart))/((float)COUNTS_PER_SECOND) ;
//	printf("INFO:Elapsed time = %.2f sec\r\n", elapsed_time) ;
//	printf("Erase Operation Successful.\r\n") ;
//
//	/*
//	 * Program the flash.
//	 */
//	printf("Performing Program Operation...\r\n") ;
//	XTime_SetTime(0) ;
//	XTime_GetTime(&TimerStart) ;
//	do{
//		PercentCurr = (int)(((float)HasSendNum/(float)TotoalLen)*10) ;
//
//		if (PercentCurr != PercentLast)
//			print_percent(PercentCurr) ;
//
//		PercentLast = PercentCurr ;
//
//		if ((HasSendNum+Flash_Config_Table[FCTIndex].PageSize) >= TotoalLen)
//		{
//			for (i = 0 ; i < Flash_Config_Table[FCTIndex].PageSize ; i++)
//			{
//				if (i >= TotoalLen-HasSendNum)
//				{
//					WriteBuffer[i] = 0 ;
//				}
//				else
//				{
//					WriteBuffer[i] = (u8)(FlashDataToSend[HasSendNum+i]);
//				}
//			}
//			FlashWrite(QspiPsuInstancePtr, WriteAddr, Flash_Config_Table[FCTIndex].PageSize, WriteCmd, WriteBuffer);
//			printf("100%%\r\n") ;
//			XTime_GetTime(&TimerEnd) ;
//			elapsed_time = (float)(TimerEnd-TimerStart)/(COUNTS_PER_SECOND) ;
//			printf("INFO:Elapsed time = %.2f sec\r\n", elapsed_time) ;
//			printf("Program Operation Successful.\r\n") ;
//			HasSendNum+= Flash_Config_Table[FCTIndex].PageSize ;
//		}
//		else
//		{
//			for (i = 0 ; i < Flash_Config_Table[FCTIndex].PageSize ; i++)
//			{
//				WriteBuffer[i] = (u8)(FlashDataToSend[HasSendNum+i]);
//			}
//			FlashWrite(QspiPsuInstancePtr, WriteAddr, Flash_Config_Table[FCTIndex].PageSize, WriteCmd, WriteBuffer);
//			HasSendNum+= Flash_Config_Table[FCTIndex].PageSize ;
//			WriteAddr+= Flash_Config_Table[FCTIndex].PageSize ;
//		}
//	}while(HasSendNum < TotoalLen) ;
//
//	HasSendNum = 0 ;
//	WriteAddr = 0 ;
//
//	/*
//	 * Verify the flash.
//	 */
//	printf("Performing Verify Operation...\r\n") ;
//	memset(ReadBuffer, 0x00, sizeof(ReadBuffer));
//	XTime_SetTime(0) ;
//	XTime_GetTime(&TimerStart) ;
//	do{
//		PercentCurr = (int)(((float)HasRecvNum/(float)TotoalLen)*10) ;
//
//		if (PercentCurr != PercentLast)
//			print_percent(PercentCurr) ;
//
//		PercentLast = PercentCurr ;
//
//		if ((HasRecvNum+Flash_Config_Table[FCTIndex].PageSize) >= TotoalLen)
//		{
//			FlashRead(QspiPsuInstancePtr, ReadAddr, Flash_Config_Table[FCTIndex].PageSize, ReadCmd, CmdBfr, ReadBuffer);
//			for (i = 0 ; i < TotoalLen-HasRecvNum; i++)
//			{
//				if (ReadBuffer[i] != (u8)(FlashDataToSend[HasRecvNum+i]))
//				{
//					printf("Verify data error, address is 0x%x\tSend Data is 0x%x\tRead Data is 0x%x\r\n",
//							ReadAddr+i+1,FlashDataToSend[HasRecvNum+i], ReadBuffer[i]) ;
//					break ;
//				}
//			}
//			HasRecvNum+= Flash_Config_Table[FCTIndex].PageSize ;
//			printf("100%%\r\n") ;
//			XTime_GetTime(&TimerEnd) ;
//			elapsed_time = (float)(TimerEnd-TimerStart)/(COUNTS_PER_SECOND) ;
//			printf("INFO:Elapsed time = %.2f sec\r\n", elapsed_time) ;
//			printf("Verify Operation Successful.\r\n") ;
//		}
//		else
//		{
//			FlashRead(QspiPsuInstancePtr, ReadAddr, Flash_Config_Table[FCTIndex].PageSize, ReadCmd, CmdBfr, ReadBuffer);
//			for (i = 0 ; i < Flash_Config_Table[FCTIndex].PageSize ; i++)
//			{
//				if (ReadBuffer[i] != (u8)(FlashDataToSend[HasRecvNum+i]))
//				{
//					printf("Verify data error, address is 0x%x\tSend Data is 0x%x\tRead Data is 0x%x\r\n",
//							ReadAddr+i+1,FlashDataToSend[HasRecvNum+i], ReadBuffer[i]) ;
//					break ;
//				}
//			}
//			HasRecvNum+= Flash_Config_Table[FCTIndex].PageSize ;
//			ReadAddr+= Flash_Config_Table[FCTIndex].PageSize ;
//		}
//	}while(HasRecvNum < TotoalLen) ;
//
//	HasRecvNum = 0 ;
//	ReadAddr = 0 ;
//
//
//	if (Flash_Config_Table[FCTIndex].FlashDeviceSize > SIXTEENMB) {
//		Status = FlashEnterExit4BAddMode(QspiPsuInstancePtr, EXIT_4B);
//		if (Status != XST_SUCCESS) {
//			return XST_FAILURE;
//		}
//	}
//
//	return XST_SUCCESS;
//}

/*****************************************************************************/
/**
 *
 * Reads the flash ID and identifies the flash in FCT table.
 *
 * @param	None.
 *
 * @return	XST_SUCCESS if successful, else XST_FAILURE.
 *
 * @note	None.
 *
 *****************************************************************************/
int FlashReadID(XQspiPsu *QspiPsuPtr)
{
	int Status;
	int StartIndex;

	/*
	 * Read ID
	 */
	TxBfrPtr = READ_ID;
	FlashMsg[0].TxBfrPtr = &TxBfrPtr;
	FlashMsg[0].RxBfrPtr = NULL;
	FlashMsg[0].ByteCount = 1;
	FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
	FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

	FlashMsg[1].TxBfrPtr = NULL;
	FlashMsg[1].RxBfrPtr = ReadBfrPtr;
	FlashMsg[1].ByteCount = 3;
	FlashMsg[1].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
	FlashMsg[1].Flags = XQSPIPSU_MSG_FLAG_RX;

	Status = XQspiPsu_PolledTransfer(QspiPsuPtr, FlashMsg, 2);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	bsp_printf("FlashID=0x%x 0x%x 0x%x\n\r", ReadBfrPtr[0], ReadBfrPtr[1],
			ReadBfrPtr[2]);

	/* In case of dual, read both and ensure they are same make/size */

	/*
	 * Deduce flash make
	 */
	if(ReadBfrPtr[0] == MICRON_ID_BYTE0) {
		FlashMake = MICRON_ID_BYTE0;
		StartIndex = MICRON_INDEX_START;
	}else if(ReadBfrPtr[0] == SPANSION_ID_BYTE0) {
		FlashMake = SPANSION_ID_BYTE0;
		StartIndex = SPANSION_INDEX_START;
	}else if(ReadBfrPtr[0] == WINBOND_ID_BYTE0) {
		FlashMake = WINBOND_ID_BYTE0;
		StartIndex = WINBOND_INDEX_START;
	} else if(ReadBfrPtr[0] == MACRONIX_ID_BYTE0) {
		FlashMake = MACRONIX_ID_BYTE0;
		StartIndex = MACRONIX_INDEX_START;
	} else if(ReadBfrPtr[0] == ISSI_ID_BYTE0) {
		FlashMake = ISSI_ID_BYTE0;
		StartIndex = ISSI_INDEX_START;
	}

	/*
	 * If valid flash ID, then check connection mode & size and
	 * assign corresponding index in the Flash configuration table
	 */
	if(((FlashMake == MICRON_ID_BYTE0) || (FlashMake == SPANSION_ID_BYTE0)||
			(FlashMake == WINBOND_ID_BYTE0)) &&
			(ReadBfrPtr[2] == MICRON_ID_BYTE2_128)) {

		switch(QspiPsuPtr->Config.ConnectionMode)
		{
			case XQSPIPSU_CONNECTION_MODE_SINGLE:
				FCTIndex = FLASH_CFG_TBL_SINGLE_128_SP + StartIndex;
				break;
			case XQSPIPSU_CONNECTION_MODE_PARALLEL:
				FCTIndex = FLASH_CFG_TBL_PARALLEL_128_SP + StartIndex;
				break;
			case XQSPIPSU_CONNECTION_MODE_STACKED:
				FCTIndex = FLASH_CFG_TBL_STACKED_128_SP + StartIndex;
				break;
			default:
				FCTIndex = 0;
				break;
		}
	}

	/* 256 and 512Mbit supported only for Micron and Spansion, not Winbond */
	if(((FlashMake == MICRON_ID_BYTE0) || (FlashMake == SPANSION_ID_BYTE0)) &&
			(ReadBfrPtr[2] == MICRON_ID_BYTE2_256)) {

		switch(QspiPsuPtr->Config.ConnectionMode)
		{
			case XQSPIPSU_CONNECTION_MODE_SINGLE:
				FCTIndex = FLASH_CFG_TBL_SINGLE_256_SP + StartIndex;
				break;
			case XQSPIPSU_CONNECTION_MODE_PARALLEL:
				FCTIndex = FLASH_CFG_TBL_PARALLEL_256_SP + StartIndex;
				break;
			case XQSPIPSU_CONNECTION_MODE_STACKED:
				FCTIndex = FLASH_CFG_TBL_STACKED_256_SP + StartIndex;
				break;
			default:
				FCTIndex = 0;
				break;
		}
	}
	if((FlashMake == ISSI_ID_BYTE0) &&
			(ReadBfrPtr[2] == MICRON_ID_BYTE2_256)) {
		switch(QspiPsuPtr->Config.ConnectionMode)
		{
			case XQSPIPSU_CONNECTION_MODE_SINGLE:
				FCTIndex = FLASH_CFG_TBL_SINGLE_256_ISSI;
				break;
			case XQSPIPSU_CONNECTION_MODE_PARALLEL:
				FCTIndex = FLASH_CFG_TBL_PARALLEL_256_ISSI;
				break;
			case XQSPIPSU_CONNECTION_MODE_STACKED:
				FCTIndex = FLASH_CFG_TBL_STACKED_256_ISSI;
				break;
			default:
				FCTIndex = 0;
				break;
		}
	}
	if(((FlashMake == MICRON_ID_BYTE0) || (FlashMake == SPANSION_ID_BYTE0)) &&
			(ReadBfrPtr[2] == MICRON_ID_BYTE2_512)) {

		switch(QspiPsuPtr->Config.ConnectionMode)
		{
			case XQSPIPSU_CONNECTION_MODE_SINGLE:
				FCTIndex = FLASH_CFG_TBL_SINGLE_512_SP + StartIndex;
				break;
			case XQSPIPSU_CONNECTION_MODE_PARALLEL:
				FCTIndex = FLASH_CFG_TBL_PARALLEL_512_SP + StartIndex;
				break;
			case XQSPIPSU_CONNECTION_MODE_STACKED:
				FCTIndex = FLASH_CFG_TBL_STACKED_512_SP + StartIndex;
				break;
			default:
				FCTIndex = 0;
				break;
		}
	}
	/*
	 * 1Gbit Single connection supported for Spansion.
	 * The ConnectionMode will indicate stacked as this part has 2 SS
	 * The device ID will indicate 512Mbit.
	 * This configuration is handled as the above 512Mbit stacked configuration
	 */
	/* 1Gbit single, parallel and stacked supported for Micron */
	if((FlashMake == MICRON_ID_BYTE0) &&
			(ReadBfrPtr[2] == MICRON_ID_BYTE2_1G)) {

		switch(QspiPsuPtr->Config.ConnectionMode)
		{
			case XQSPIPSU_CONNECTION_MODE_SINGLE:
				FCTIndex = FLASH_CFG_TBL_SINGLE_1GB_MC;
				break;
			case XQSPIPSU_CONNECTION_MODE_PARALLEL:
				FCTIndex = FLASH_CFG_TBL_PARALLEL_1GB_MC;
				break;
			case XQSPIPSU_CONNECTION_MODE_STACKED:
				FCTIndex = FLASH_CFG_TBL_STACKED_1GB_MC;
				break;
			default:
				FCTIndex = 0;
				break;
		}
	}
	/* 1Gbit single, parallel and stacked supported for Macronix */
	if(((FlashMake == MACRONIX_ID_BYTE0) &&
			(ReadBfrPtr[2] == MACRONIX_ID_BYTE2_1G))) {

		switch(QspiPsuPtr->Config.ConnectionMode) {
			case XQSPIPSU_CONNECTION_MODE_SINGLE:
				FCTIndex = FLASH_CFG_TBL_SINGLE_1G_MX;
				break;
			case XQSPIPSU_CONNECTION_MODE_PARALLEL:
				FCTIndex = FLASH_CFG_TBL_PARALLEL_1G_MX;
				break;
			case XQSPIPSU_CONNECTION_MODE_STACKED:
				FCTIndex = FLASH_CFG_TBL_STACKED_1G_MX;
				break;
			default:
				FCTIndex = 0;
				break;
		}
	}

	return XST_SUCCESS;
}

/*****************************************************************************/
/**
 *
 * This function writes to the  serial Flash connected to the QSPIPSU interface.
 * All the data put into the buffer must be in the same page of the device with
 * page boundaries being on 256 byte boundaries.
 *
 * @param	QspiPsuPtr is a pointer to the QSPIPSU driver component to use.
 * @param	Address contains the address to write data to in the Flash.
 * @param	ByteCount contains the number of bytes to write.
 * @param	Command is the command used to write data to the flash. QSPIPSU
 *		device supports only Page Program command to write data to the
 *		flash.
 * @param	Pointer to the write buffer (which is to be transmitted)
 *
 * @return	XST_SUCCESS if successful, else XST_FAILURE.
 *
 * @note	None.
 *
 ******************************************************************************/
int FlashWrite(XQspiPsu *QspiPsuPtr, u32 Address, u32 ByteCount, u8 Command,
		u8 *WriteBfrPtr)
{
	u8 WriteEnableCmd;
	u8 ReadStatusCmd;
	u8 FlashStatus[2];
	u8 WriteCmd[5];
	u32 RealAddr;
	u32 CmdByteCount;
	int Status;

	WriteEnableCmd = WRITE_ENABLE_CMD;
	/*
	 * Translate address based on type of connection
	 * If stacked assert the slave select based on address
	 */
	RealAddr = GetRealAddr(QspiPsuPtr, Address);

	/*
	 * Send the write enable command to the Flash so that it can be
	 * written to, this needs to be sent as a separate transfer before
	 * the write
	 */
	FlashMsg[0].TxBfrPtr = &WriteEnableCmd;
	FlashMsg[0].RxBfrPtr = NULL;
	FlashMsg[0].ByteCount = 1;
	FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
	FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

	Status = XQspiPsu_PolledTransfer(QspiPsuPtr, FlashMsg, 1);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	WriteCmd[COMMAND_OFFSET]   = Command;

	/* To be used only if 4B address program cmd is supported by flash */
	if (Flash_Config_Table[FCTIndex].FlashDeviceSize > SIXTEENMB) {
		WriteCmd[ADDRESS_1_OFFSET] = (u8)((RealAddr & 0xFF000000) >> 24);
		WriteCmd[ADDRESS_2_OFFSET] = (u8)((RealAddr & 0xFF0000) >> 16);
		WriteCmd[ADDRESS_3_OFFSET] = (u8)((RealAddr & 0xFF00) >> 8);
		WriteCmd[ADDRESS_4_OFFSET] = (u8)(RealAddr & 0xFF);
		CmdByteCount = 5;
	} else {
		WriteCmd[ADDRESS_1_OFFSET] = (u8)((RealAddr & 0xFF0000) >> 16);
		WriteCmd[ADDRESS_2_OFFSET] = (u8)((RealAddr & 0xFF00) >> 8);
		WriteCmd[ADDRESS_3_OFFSET] = (u8)(RealAddr & 0xFF);
		CmdByteCount = 4;
	}

	FlashMsg[0].TxBfrPtr = WriteCmd;
	FlashMsg[0].RxBfrPtr = NULL;
	FlashMsg[0].ByteCount = CmdByteCount;
	FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
	FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

	FlashMsg[1].TxBfrPtr = WriteBfrPtr;
	FlashMsg[1].RxBfrPtr = NULL;
	FlashMsg[1].ByteCount = ByteCount;
	FlashMsg[1].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
	FlashMsg[1].Flags = XQSPIPSU_MSG_FLAG_TX;
	if (QspiPsuPtr->Config.ConnectionMode == XQSPIPSU_CONNECTION_MODE_PARALLEL) {
		FlashMsg[1].Flags |= XQSPIPSU_MSG_FLAG_STRIPE;
	}

	Status = XQspiPsu_PolledTransfer(QspiPsuPtr, FlashMsg, 2);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Wait for the write command to the Flash to be completed, it takes
	 * some time for the data to be written
	 */
	while (1) {
		ReadStatusCmd = StatusCmd;
		FlashMsg[0].TxBfrPtr = &ReadStatusCmd;
		FlashMsg[0].RxBfrPtr = NULL;
		FlashMsg[0].ByteCount = 1;
		FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
		FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

		FlashMsg[1].TxBfrPtr = NULL;
		FlashMsg[1].RxBfrPtr = FlashStatus;
		FlashMsg[1].ByteCount = 2;
		FlashMsg[1].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
		FlashMsg[1].Flags = XQSPIPSU_MSG_FLAG_RX;
		if (QspiPsuPtr->Config.ConnectionMode == XQSPIPSU_CONNECTION_MODE_PARALLEL) {
			FlashMsg[1].Flags |= XQSPIPSU_MSG_FLAG_STRIPE;
		}

		Status = XQspiPsu_PolledTransfer(QspiPsuPtr, FlashMsg, 2);
		if (Status != XST_SUCCESS) {
			return XST_FAILURE;
		}

		if (QspiPsuPtr->Config.ConnectionMode == XQSPIPSU_CONNECTION_MODE_PARALLEL) {
			if (FSRFlag) {
				FlashStatus[1] &= FlashStatus[0];
			} else {
				FlashStatus[1] |= FlashStatus[0];
			}
		}

		if (FSRFlag) {
			if ((FlashStatus[1] & 0x80) != 0) {
				break;
			}
		} else {
			if ((FlashStatus[1] & 0x01) == 0) {
				break;
			}
		}
	}

	return 0;
}

/*****************************************************************************/
/**
 *
 * This function erases the sectors in the  serial Flash connected to the
 * QSPIPSU interface.
 *
 * @param	QspiPtr is a pointer to the QSPIPSU driver component to use.
 * @param	Address contains the address of the first sector which needs to
 *		be erased.
 * @param	ByteCount contains the total size to be erased.
 * @param	Pointer to the write buffer (which is to be transmitted)
 *
 * @return	XST_SUCCESS if successful, else XST_FAILURE.
 *
 * @note	None.
 *
 ******************************************************************************/
int FlashErase(XQspiPsu *QspiPsuPtr, u32 Address, u32 ByteCount, u8 *WriteBfrPtr)
{
	u8 WriteEnableCmd;
	u8 ReadStatusCmd;
	u8 FlashStatus[2];
	int Sector;
	u32 RealAddr;
	u32 NumSect;
	int Status;

	int PercentCurr = -1 ;
	int PercentLast = -1 ;

	WriteEnableCmd = WRITE_ENABLE_CMD;
	/*
	 * If erase size is same as the total size of the flash, use bulk erase
	 * command or die erase command multiple times as required
	 */
	if (ByteCount == ((Flash_Config_Table[FCTIndex]).NumSect * (Flash_Config_Table[FCTIndex]).SectSize))
	{
		if (QspiPsuPtr->Config.ConnectionMode == XQSPIPSU_CONNECTION_MODE_STACKED)
		{
			XQspiPsu_SelectFlash(QspiPsuPtr, XQSPIPSU_SELECT_FLASH_CS_LOWER, XQSPIPSU_SELECT_FLASH_BUS_LOWER);
		}

		if (Flash_Config_Table[FCTIndex].NumDie == 1)
		{
			/*
			 * Call Bulk erase
			 */
			BulkErase(QspiPsuPtr, WriteBfrPtr);
		}

		if (Flash_Config_Table[FCTIndex].NumDie > 1)
		{
			/*
			 * Call Die erase
			 */
			DieErase(QspiPsuPtr, WriteBfrPtr);
		}
		/*
		 * If stacked mode, bulk erase second flash
		 */
		if (QspiPsuPtr->Config.ConnectionMode == XQSPIPSU_CONNECTION_MODE_STACKED)
		{
			XQspiPsu_SelectFlash(QspiPsuPtr, XQSPIPSU_SELECT_FLASH_CS_UPPER, XQSPIPSU_SELECT_FLASH_BUS_LOWER);

			if (Flash_Config_Table[FCTIndex].NumDie == 1)
			{
				/*
				 * Call Bulk erase
				 */
				BulkErase(QspiPsuPtr, WriteBfrPtr);
			}

			if (Flash_Config_Table[FCTIndex].NumDie > 1)
			{
				/*
				 * Call Die erase
				 */
				DieErase(QspiPsuPtr, WriteBfrPtr);
			}
		}

		return 0;
	}

	/*
	 * If the erase size is less than the total size of the flash, use sector erase command
	 */

	/*
	 * Calculate no. of sectors to erase based on byte count
	 */
	NumSect = ByteCount/(Flash_Config_Table[FCTIndex].SectSize) + 1;

	/*
	 * If ByteCount to k sectors,
	 * but the address range spans from N to N+k+1 sectors, then
	 * increment no. of sectors to be erased
	 */

	if (((Address + ByteCount) & Flash_Config_Table[FCTIndex].SectMask) ==
			((Address + (NumSect * Flash_Config_Table[FCTIndex].SectSize)) &
					Flash_Config_Table[FCTIndex].SectMask)) {
		NumSect++;
	}

	bsp_printf("Erase Size is %u Bytes\r\n", NumSect*Flash_Config_Table[FCTIndex].SectSize) ;

	for (Sector = 0; Sector < NumSect; Sector++) {

		PercentCurr = (int)(((float)Sector/(float)NumSect)*10) ;

		if (PercentCurr != PercentLast)
		{
			print_percent(PercentCurr) ;
		}

		PercentLast = PercentCurr ;

		/*
		 * Translate address based on type of connection
		 * If stacked assert the slave select based on address
		 */
		RealAddr = GetRealAddr(QspiPsuPtr, Address);

		/*
		 * Send the write enable command to the Flash so that it can be
		 * written to, this needs to be sent as a separate
		 * transfer before the write
		 */
		FlashMsg[0].TxBfrPtr = &WriteEnableCmd;
		FlashMsg[0].RxBfrPtr = NULL;
		FlashMsg[0].ByteCount = 1;
		FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
		FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

		Status = XQspiPsu_PolledTransfer(QspiPsuPtr, FlashMsg, 1);
		if (Status != XST_SUCCESS) {
			return XST_FAILURE;
		}

		WriteBfrPtr[COMMAND_OFFSET]   = SectorEraseCmd;

		/*
		 * To be used only if 4B address sector erase cmd is
		 * supported by flash
		 */
		if (Flash_Config_Table[FCTIndex].FlashDeviceSize > SIXTEENMB) {
			WriteBfrPtr[ADDRESS_1_OFFSET] = (u8)((RealAddr & 0xFF000000) >> 24);
			WriteBfrPtr[ADDRESS_2_OFFSET] = (u8)((RealAddr & 0xFF0000) >> 16);
			WriteBfrPtr[ADDRESS_3_OFFSET] = (u8)((RealAddr & 0xFF00) >> 8);
			WriteBfrPtr[ADDRESS_4_OFFSET] = (u8)(RealAddr & 0xFF);
			FlashMsg[0].ByteCount = 5;
		} else {
			WriteBfrPtr[ADDRESS_1_OFFSET] = (u8)((RealAddr & 0xFF0000) >> 16);
			WriteBfrPtr[ADDRESS_2_OFFSET] = (u8)((RealAddr & 0xFF00) >> 8);
			WriteBfrPtr[ADDRESS_3_OFFSET] = (u8)(RealAddr & 0xFF);
			FlashMsg[0].ByteCount = 4;
		}

		FlashMsg[0].TxBfrPtr = WriteBfrPtr;
		FlashMsg[0].RxBfrPtr = NULL;
		FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
		FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

		Status = XQspiPsu_PolledTransfer(QspiPsuPtr, FlashMsg, 1);
		if (Status != XST_SUCCESS) {
			return XST_FAILURE;
		}

		/*
		 * Wait for the erase command to be completed
		 */
		while (1) {
			ReadStatusCmd = StatusCmd;
			FlashMsg[0].TxBfrPtr = &ReadStatusCmd;
			FlashMsg[0].RxBfrPtr = NULL;
			FlashMsg[0].ByteCount = 1;
			FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
			FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

			FlashMsg[1].TxBfrPtr = NULL;
			FlashMsg[1].RxBfrPtr = FlashStatus;
			FlashMsg[1].ByteCount = 2;
			FlashMsg[1].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
			FlashMsg[1].Flags = XQSPIPSU_MSG_FLAG_RX;
			if (QspiPsuPtr->Config.ConnectionMode == XQSPIPSU_CONNECTION_MODE_PARALLEL)
			{
				FlashMsg[1].Flags |= XQSPIPSU_MSG_FLAG_STRIPE;
			}

			Status = XQspiPsu_PolledTransfer(QspiPsuPtr, FlashMsg, 2);
			if (Status != XST_SUCCESS)
			{
				return XST_FAILURE;
			}

			if (QspiPsuPtr->Config.ConnectionMode == XQSPIPSU_CONNECTION_MODE_PARALLEL)
			{
				if (FSRFlag) {
					FlashStatus[1] &= FlashStatus[0];
				} else {
					FlashStatus[1] |= FlashStatus[0];
				}
			}

			if (FSRFlag) {
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

	return 0;
}


/*****************************************************************************/
/**
 *
 * This function performs read. DMA is the default setting.
 *
 * @param	QspiPtr is a pointer to the QSPIPSU driver component to use.
 * @param	Address contains the address of the first sector which needs to
 *			be erased.
 * @param	ByteCount contains the total size to be erased.
 * @param	Command is the command used to read data from the flash.
 *		Supports normal, fast, dual and quad read commands.
 * @param	Pointer to the write buffer which contains data to be
 *		transmitted
 * @param	Pointer to the read buffer to which valid received data
 *		should be written
 *
 * @return	XST_SUCCESS if successful, else XST_FAILURE.
 *
 * @note	None.
 *
 ******************************************************************************/
int FlashRead(XQspiPsu *QspiPsuPtr, u32 Address, u32 ByteCount, u8 Command,
		u8 *WriteBfrPtr, u8 *ReadBfrPtr)
{
	u32 RealAddr;
	u32 DiscardByteCnt;
	u32 FlashMsgCnt;
	int Status;

	/* Check die boundary conditions if required for any flash */

	/* For Dual Stacked, split and read for boundary crossing */
	/*
	 * Translate address based on type of connection
	 * If stacked assert the slave select based on address
	 */
	RealAddr = GetRealAddr(QspiPsuPtr, Address);

	WriteBfrPtr[COMMAND_OFFSET]   = Command;
	if (Flash_Config_Table[FCTIndex].FlashDeviceSize > SIXTEENMB) {
		WriteBfrPtr[ADDRESS_1_OFFSET] = (u8)((RealAddr & 0xFF000000) >> 24);
		WriteBfrPtr[ADDRESS_2_OFFSET] = (u8)((RealAddr & 0xFF0000) >> 16);
		WriteBfrPtr[ADDRESS_3_OFFSET] = (u8)((RealAddr & 0xFF00) >> 8);
		WriteBfrPtr[ADDRESS_4_OFFSET] = (u8)(RealAddr & 0xFF);
		DiscardByteCnt = 5;
	} else {
		WriteBfrPtr[ADDRESS_1_OFFSET] = (u8)((RealAddr & 0xFF0000) >> 16);
		WriteBfrPtr[ADDRESS_2_OFFSET] = (u8)((RealAddr & 0xFF00) >> 8);
		WriteBfrPtr[ADDRESS_3_OFFSET] = (u8)(RealAddr & 0xFF);
		DiscardByteCnt = 4;
	}

	FlashMsg[0].TxBfrPtr = WriteBfrPtr;
	FlashMsg[0].RxBfrPtr = NULL;
	FlashMsg[0].ByteCount = DiscardByteCnt;
	FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
	FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

	FlashMsgCnt = 1;

	/* It is recommended to have a separate entry for dummy */
	if ((Command == FAST_READ_CMD) || (Command == DUAL_READ_CMD) ||
		(Command == QUAD_READ_CMD) || (Command == FAST_READ_CMD_4B) ||
		(Command == DUAL_READ_CMD_4B) || (Command == QUAD_READ_CMD_4B)) {
		/* Update Dummy cycles as per flash specs for QUAD IO */

		/*
		 * It is recommended that Bus width value during dummy
		 * phase should be same as data phase
		 */
		if ((Command == FAST_READ_CMD) || (Command == FAST_READ_CMD_4B)) {
			FlashMsg[1].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
		}

		if ((Command == DUAL_READ_CMD) || (Command == DUAL_READ_CMD_4B)) {
			FlashMsg[1].BusWidth = XQSPIPSU_SELECT_MODE_DUALSPI;
		}

		if ((Command == QUAD_READ_CMD) || (Command == QUAD_READ_CMD_4B)) {
			FlashMsg[1].BusWidth = XQSPIPSU_SELECT_MODE_QUADSPI;
		}

		FlashMsg[1].TxBfrPtr = NULL;
		FlashMsg[1].RxBfrPtr = NULL;
		FlashMsg[1].ByteCount = DUMMY_CLOCKS;
		FlashMsg[1].Flags = 0;

		FlashMsgCnt++;
	}

	if ((Command == FAST_READ_CMD) || (Command == FAST_READ_CMD_4B)) {
		FlashMsg[FlashMsgCnt].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
	}

	if ((Command == DUAL_READ_CMD) || (Command == DUAL_READ_CMD_4B)) {
		FlashMsg[FlashMsgCnt].BusWidth = XQSPIPSU_SELECT_MODE_DUALSPI;
	}

	if ((Command == QUAD_READ_CMD) || (Command == QUAD_READ_CMD_4B)) {
		FlashMsg[FlashMsgCnt].BusWidth = XQSPIPSU_SELECT_MODE_QUADSPI;
	}

	FlashMsg[FlashMsgCnt].TxBfrPtr = NULL;
	FlashMsg[FlashMsgCnt].RxBfrPtr = ReadBfrPtr;
	FlashMsg[FlashMsgCnt].ByteCount = ByteCount;
	FlashMsg[FlashMsgCnt].Flags = XQSPIPSU_MSG_FLAG_RX;

	if (QspiPsuPtr->Config.ConnectionMode == XQSPIPSU_CONNECTION_MODE_PARALLEL) {
		FlashMsg[FlashMsgCnt].Flags |= XQSPIPSU_MSG_FLAG_STRIPE;
	}

	Status = XQspiPsu_PolledTransfer(QspiPsuPtr, FlashMsg, FlashMsgCnt+1);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	return 0;
}


/*****************************************************************************/
/**
 *
 * This functions performs a bulk erase operation when the
 * flash device has a single die. Works for both Spansion and Micron
 *
 * @param	QspiPsuPtr is a pointer to the QSPIPSU driver component to use.
 * @param	WritBfrPtr is the pointer to command+address to be sent
 *
 * @return	XST_SUCCESS if successful, else XST_FAILURE.
 *
 * @note	None.
 *
 ******************************************************************************/
int BulkErase(XQspiPsu *QspiPsuPtr, u8 *WriteBfrPtr)
{
	u8 WriteEnableCmd;
	u8 ReadStatusCmd;
	u8 FlashStatus[2];
	int Status;

	WriteEnableCmd = WRITE_ENABLE_CMD;
	/*
	 * Send the write enable command to the Flash so that it can be
	 * written to, this needs to be sent as a separate transfer before
	 * the write
	 */
	FlashMsg[0].TxBfrPtr = &WriteEnableCmd;
	FlashMsg[0].RxBfrPtr = NULL;
	FlashMsg[0].ByteCount = 1;
	FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
	FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

	Status = XQspiPsu_PolledTransfer(QspiPsuPtr, FlashMsg, 1);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	WriteBfrPtr[COMMAND_OFFSET]   = BULK_ERASE_CMD;
	FlashMsg[0].TxBfrPtr = WriteBfrPtr;
	FlashMsg[0].RxBfrPtr = NULL;
	FlashMsg[0].ByteCount = 1;
	FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
	FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

	Status = XQspiPsu_PolledTransfer(QspiPsuPtr, FlashMsg, 1);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Wait for the write command to the Flash to be completed, it takes
	 * some time for the data to be written
	 */
	while (1) {
		ReadStatusCmd = StatusCmd;
		FlashMsg[0].TxBfrPtr = &ReadStatusCmd;
		FlashMsg[0].RxBfrPtr = NULL;
		FlashMsg[0].ByteCount = 1;
		FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
		FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

		FlashMsg[1].TxBfrPtr = NULL;
		FlashMsg[1].RxBfrPtr = FlashStatus;
		FlashMsg[1].ByteCount = 2;
		FlashMsg[1].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
		FlashMsg[1].Flags = XQSPIPSU_MSG_FLAG_RX;
		if (QspiPsuPtr->Config.ConnectionMode == XQSPIPSU_CONNECTION_MODE_PARALLEL) {
			FlashMsg[1].Flags |= XQSPIPSU_MSG_FLAG_STRIPE;
		}


		Status = XQspiPsu_PolledTransfer(QspiPsuPtr, FlashMsg, 2);
		if (Status != XST_SUCCESS) {
			return XST_FAILURE;
		}
		if (QspiPsuPtr->Config.ConnectionMode == XQSPIPSU_CONNECTION_MODE_PARALLEL) {
			if (FSRFlag) {
				FlashStatus[1] &= FlashStatus[0];
			} else {
				FlashStatus[1] |= FlashStatus[0];
			}
		}

		if (FSRFlag) {
			if ((FlashStatus[1] & 0x80) != 0) {
				break;
			}
		} else {
			if ((FlashStatus[1] & 0x01) == 0) {
				break;
			}
		}
	}

	return 0;
}

/*****************************************************************************/
/**
 *
 * This functions performs a die erase operation on all the die in
 * the flash device. This function uses the die erase command for
 * Micron 512Mbit and 1Gbit
 *
 * @param	QspiPsuPtr is a pointer to the QSPIPSU driver component to use.
 * @param	WritBfrPtr is the pointer to command+address to be sent
 *
 * @return	XST_SUCCESS if successful, else XST_FAILURE.
 *
 * @note	None.
 *
 ******************************************************************************/
int DieErase(XQspiPsu *QspiPsuPtr, u8 *WriteBfrPtr)
{
	u8 WriteEnableCmd;
	u8 DieCnt;
	u8 ReadStatusCmd;
	u8 FlashStatus[2];
	int Status;

	WriteEnableCmd = WRITE_ENABLE_CMD;
	for (DieCnt = 0;
			DieCnt < Flash_Config_Table[FCTIndex].NumDie;
			DieCnt++) {
		/*
		 * Send the write enable command to the Flash so that it can be
		 * written to, this needs to be sent as a separate transfer
		 * before the write
		 */
		FlashMsg[0].TxBfrPtr = &WriteEnableCmd;
		FlashMsg[0].RxBfrPtr = NULL;
		FlashMsg[0].ByteCount = 1;
		FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
		FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

		Status = XQspiPsu_PolledTransfer(QspiPsuPtr, FlashMsg, 1);
		if (Status != XST_SUCCESS) {
			return XST_FAILURE;
		}

		WriteBfrPtr[COMMAND_OFFSET]   = DIE_ERASE_CMD;
		/* Check these number of address bytes as per flash device */
		WriteBfrPtr[ADDRESS_1_OFFSET] = 0;
		WriteBfrPtr[ADDRESS_2_OFFSET] = 0;
		WriteBfrPtr[ADDRESS_3_OFFSET] = 0;

		FlashMsg[0].TxBfrPtr = WriteBfrPtr;
		FlashMsg[0].RxBfrPtr = NULL;
		FlashMsg[0].ByteCount = 4;
		FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
		FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

		Status = XQspiPsu_PolledTransfer(QspiPsuPtr, FlashMsg, 1);
		if (Status != XST_SUCCESS) {
			return XST_FAILURE;
		}

		/*
		 * Wait for the write command to the Flash to be completed,
		 * it takes some time for the data to be written
		 */
		while (1) {
			ReadStatusCmd = StatusCmd;
			FlashMsg[0].TxBfrPtr = &ReadStatusCmd;
			FlashMsg[0].RxBfrPtr = NULL;
			FlashMsg[0].ByteCount = 1;
			FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
			FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

			FlashMsg[1].TxBfrPtr = NULL;
			FlashMsg[1].RxBfrPtr = FlashStatus;
			FlashMsg[1].ByteCount = 2;
			FlashMsg[1].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
			FlashMsg[1].Flags = XQSPIPSU_MSG_FLAG_RX;
			if (QspiPsuPtr->Config.ConnectionMode == XQSPIPSU_CONNECTION_MODE_PARALLEL) {
				FlashMsg[1].Flags |= XQSPIPSU_MSG_FLAG_STRIPE;
			}

			Status = XQspiPsu_PolledTransfer(QspiPsuPtr,
					FlashMsg, 2);
			if (Status != XST_SUCCESS) {
				return XST_FAILURE;
			}
			if (QspiPsuPtr->Config.ConnectionMode == XQSPIPSU_CONNECTION_MODE_PARALLEL) {
				if (FSRFlag) {
					FlashStatus[1] &= FlashStatus[0];
				} else {
					FlashStatus[1] |= FlashStatus[0];
				}
			}

			if (FSRFlag) {
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

	return 0;
}

/*****************************************************************************/
/**
 *
 * This functions translates the address based on the type of interconnection.
 * In case of stacked, this function asserts the corresponding slave select.
 *
 * @param	QspiPsuPtr is a pointer to the QSPIPSU driver component to use.
 * @param	Address which is to be accessed (for erase, write or read)
 *
 * @return	RealAddr is the translated address - for single it is unchanged;
 *		for stacked, the lower flash size is subtracted;
 *		for parallel the address is divided by 2.
 *
 * @note	In addition to get the actual address to work on flash this
 *		function also selects the CS and BUS based on the configuration
 *		detected.
 *
 ******************************************************************************/
u32 GetRealAddr(XQspiPsu *QspiPsuPtr, u32 Address)
{
	u32 RealAddr;

	switch (QspiPsuPtr->Config.ConnectionMode) {
	case XQSPIPSU_CONNECTION_MODE_SINGLE:
		XQspiPsu_SelectFlash(QspiPsuPtr, XQSPIPSU_SELECT_FLASH_CS_LOWER, XQSPIPSU_SELECT_FLASH_BUS_LOWER);
		RealAddr = Address;
		break;
	case XQSPIPSU_CONNECTION_MODE_STACKED:
		/* Select lower or upper Flash based on sector address */
		if (Address & Flash_Config_Table[FCTIndex].FlashDeviceSize) {

			XQspiPsu_SelectFlash(QspiPsuPtr, XQSPIPSU_SELECT_FLASH_CS_UPPER, XQSPIPSU_SELECT_FLASH_BUS_LOWER);
			/*
			 * Subtract first flash size when accessing second flash
			 */
			RealAddr = Address & (~Flash_Config_Table[FCTIndex].FlashDeviceSize);
		} else {
			/*
			 * Set selection to L_PAGE
			 */
			XQspiPsu_SelectFlash(QspiPsuPtr, XQSPIPSU_SELECT_FLASH_CS_LOWER, XQSPIPSU_SELECT_FLASH_BUS_LOWER);

			RealAddr = Address;

		}
		break;
	case XQSPIPSU_CONNECTION_MODE_PARALLEL:
		/*
		 * The effective address in each flash is the actual
		 * address / 2
		 */
		XQspiPsu_SelectFlash(QspiPsuPtr, XQSPIPSU_SELECT_FLASH_CS_BOTH, XQSPIPSU_SELECT_FLASH_BUS_BOTH);
		RealAddr = Address / 2;
		break;
	default:
		/* RealAddr wont be assigned in this case; */
		break;

	}

	return(RealAddr);
}


/*****************************************************************************/
/**
 * @brief
 * This API enters the flash device into 4 bytes addressing mode.
 * As per the Micron and ISSI spec, before issuing the command to enter
 * into 4 byte addr mode, a write enable command is issued.
 * For Macronix and Winbond flash parts write
 * enable is not required.
 *
 * @param	QspiPtr is a pointer to the QSPIPSU driver component to use.
 * @param	Enable is a either 1 or 0 if 1 then enters 4 byte if 0 exits.
 *
 * @return
 *		- XST_SUCCESS if successful.
 *		- XST_FAILURE if it fails.
 *
 *
 ******************************************************************************/
int FlashEnterExit4BAddMode(XQspiPsu *QspiPsuPtr, unsigned int Enable)
{
	int Status;
	u8 WriteEnableCmd;
	u8 Cmd;
	u8 WriteDisableCmd;
	u8 ReadStatusCmd;
	u8 WriteBuffer[2] = {0};
	u8 FlashStatus[2] = {0};

	if (Enable) {
		Cmd = ENTER_4B_ADDR_MODE;
	} else {
		if(FlashMake == ISSI_ID_BYTE0)
			Cmd = EXIT_4B_ADDR_MODE_ISSI;
		else
			Cmd = EXIT_4B_ADDR_MODE;
	}


	switch (FlashMake) {
		case ISSI_ID_BYTE0:
		case MICRON_ID_BYTE0:
			WriteEnableCmd = WRITE_ENABLE_CMD;
			/*
			 * Send the write enable command to the Flash so that it can be
			 * written to, this needs to be sent as a separate transfer before
			 * the write
			 */
			FlashMsg[0].TxBfrPtr = &WriteEnableCmd;
			FlashMsg[0].RxBfrPtr = NULL;
			FlashMsg[0].ByteCount = 1;
			FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
			FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

			Status = XQspiPsu_PolledTransfer(QspiPsuPtr, FlashMsg, 1);
			if (Status != XST_SUCCESS) {
				return XST_FAILURE;
			}
			break;

		case SPANSION_ID_BYTE0:

			if(Enable) {
				WriteBuffer[0] = BANK_REG_WR;
				WriteBuffer[1] = 1 << 7;
			} else {
				WriteBuffer[0] = BANK_REG_WR;
				WriteBuffer[1] = 0 << 7;
			}

			FlashMsg[0].TxBfrPtr = WriteBuffer;
			FlashMsg[0].RxBfrPtr = NULL;
			FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
			FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;
			FlashMsg[0].ByteCount = 2;

			Status = XQspiPsu_PolledTransfer(QspiPsuPtr, FlashMsg, 1);
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

	GetRealAddr(QspiPsuPtr, 0);

	FlashMsg[0].TxBfrPtr = &Cmd;
	FlashMsg[0].RxBfrPtr = NULL;
	FlashMsg[0].ByteCount = 1;
	FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
	FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

	Status = XQspiPsu_PolledTransfer(QspiPsuPtr, FlashMsg, 1);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	while (1) {
		ReadStatusCmd = StatusCmd;

		FlashMsg[0].TxBfrPtr = &ReadStatusCmd;
		FlashMsg[0].RxBfrPtr = NULL;
		FlashMsg[0].ByteCount = 1;
		FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
		FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

		FlashMsg[1].TxBfrPtr = NULL;
		FlashMsg[1].RxBfrPtr = FlashStatus;
		FlashMsg[1].ByteCount = 2;
		FlashMsg[1].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
		FlashMsg[1].Flags = XQSPIPSU_MSG_FLAG_RX;

		if (QspiPsuPtr->Config.ConnectionMode == XQSPIPSU_CONNECTION_MODE_PARALLEL) {
			FlashMsg[1].Flags |= XQSPIPSU_MSG_FLAG_STRIPE;
		}

		Status = XQspiPsu_PolledTransfer(QspiPsuPtr, FlashMsg, 2);
		if (Status != XST_SUCCESS) {
			return XST_FAILURE;
		}

		if (QspiPsuPtr->Config.ConnectionMode == XQSPIPSU_CONNECTION_MODE_PARALLEL) {
			if (FSRFlag) {
				FlashStatus[1] &= FlashStatus[0];
			} else {
				FlashStatus[1] |= FlashStatus[0];
			}
		}

		if (FSRFlag) {
			if ((FlashStatus[1] & 0x80) != 0) {
				break;
			}
		} else {
			if ((FlashStatus[1] & 0x01) == 0) {
				break;
			}
		}
	}

	switch (FlashMake) {
		case ISSI_ID_BYTE0:
		case MICRON_ID_BYTE0:
			WriteDisableCmd = WRITE_DISABLE_CMD;
			/*
			 * Send the write enable command to the Flash so that it can be
			 * written to, this needs to be sent as a separate transfer before
			 * the write
			 */
			FlashMsg[0].TxBfrPtr = &WriteDisableCmd;
			FlashMsg[0].RxBfrPtr = NULL;
			FlashMsg[0].ByteCount = 1;
			FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
			FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

			Status = XQspiPsu_PolledTransfer(QspiPsuPtr, FlashMsg, 1);
			if (Status != XST_SUCCESS) {
				return XST_FAILURE;
			}
			break;

		default:
			/*
			 * For Macronix and Winbond flash parts
			 * Write disable command is not required.
			 */
			break;
	}

	return Status;
}



void print_percent(int percent)
{
	switch(percent)
	{
	case 0 : bsp_printf("0%%..") ; break ;
	case 1 : bsp_printf("10%%..") ; break ;
	case 2 : bsp_printf("20%%..") ; break ;
	case 3 : bsp_printf("30%%..") ; break ;
	case 4 : bsp_printf("40%%..") ; break ;
	case 5 : bsp_printf("50%%..") ; break ;
	case 6 : bsp_printf("60%%..") ; break ;
	case 7 : bsp_printf("70%%..") ; break ;
	case 8 : bsp_printf("80%%..") ; break ;
	case 9 : bsp_printf("90%%..") ; break ;
	case 10 : bsp_printf("100%..") ; break ;
	default : break ;
	}
}

#endif // XPAR_XQSPIPSU_NUM_INSTANCES


