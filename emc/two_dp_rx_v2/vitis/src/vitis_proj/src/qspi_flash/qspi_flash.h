#ifndef __QSPI_FLASH_H__

//#define XPAR_XQSPIPSU_NUM_INSTANCES (1U)
//#define XPAR_XQSPIPS_NUM_INSTANCES (1U)
//#undef XPAR_PROCESSOR_SUBSYSTEM_AXI_QUAD_SPI_0_SPI_MODE

//#include "xparameters.h"
#if defined (XPAR_XQSPIPSU_NUM_INSTANCES) // XPAR_XQSPIPS_NUM_INSTANCES XPAR_XSPI_NUM_INSTANCES (XPAR_AXI_QUAD_SPI_0_SPI_MODE == 2U)
#define __QSPI_FLASH_H__
//#include "xparameters.h"	/* SDK generated parameters */
#include "xqspipsu.h"		/* QSPI device driver */
//#if defined (ARMR5) || (__aarch64__) || (__arm__)
//#include "xtime_l.h"
//#endif
/************************** Constant Definitions *****************************/

/*
 * The following constants define the commands which may be sent to the Flash
 * device.
 */
#define WRITE_STATUS_CMD	0x01
#define WRITE_CMD			0x02
#define READ_CMD			0x03
#define WRITE_DISABLE_CMD	0x04
#define READ_STATUS_CMD		0x05
#define WRITE_ENABLE_CMD	0x06
//#define VOLATILE_WRITE_ENABLE_CMD	0x50
#define FAST_READ_CMD		0x0B
#define DUAL_READ_CMD		0x3B
#define QUAD_READ_CMD		0x6B
#define BULK_ERASE_CMD		0xC7
#define	SEC_ERASE_CMD		0xD8
#define READ_ID				0x9F
#define READ_CONFIG_CMD		0x35
#define WRITE_CONFIG_CMD	0x01
#define ENTER_4B_ADDR_MODE	0xB7
#define EXIT_4B_ADDR_MODE	0xE9
#define EXIT_4B_ADDR_MODE_ISSI	0x29
#define WRITE_CMD_4B		0x12
#define READ_CMD_4B			0x13
#define FAST_READ_CMD_4B	0x0C
#define DUAL_READ_CMD_4B	0x3C
#define QUAD_READ_CMD_4B	0x6C
#define	SEC_ERASE_CMD_4B	0xDC
#define BANK_REG_RD			0x16
#define BANK_REG_WR			0x17
/* Bank register is called Extended Address Register in Micron */
#define EXTADD_REG_RD		0xC8
#define EXTADD_REG_WR		0xC5
#define	DIE_ERASE_CMD		0xC4
#define READ_FLAG_STATUS_CMD	0x70

/*
 * The following constants define the offsets within a FlashBuffer data
 * type for each kind of data.  Note that the read data offset is not the
 * same as the write data because the QSPIPSU driver is designed to allow full
 * duplex transfers such that the number of bytes received is the number
 * sent and received.
 */
#define COMMAND_OFFSET		0 /* Flash instruction */
#define ADDRESS_1_OFFSET	1 /* MSB byte of address to read or write */
#define ADDRESS_2_OFFSET	2 /* Middle byte of address to read or write */
#define ADDRESS_3_OFFSET	3 /* LSB byte of address to read or write */
#define ADDRESS_4_OFFSET	4 /* LSB byte of address to read or write
 * when 4 byte address
 */
#define DATA_OFFSET			5 /* Start of Data for Read/Write */
#define DUMMY_OFFSET		4 /* Dummy byte offset for fast, dual and quad
 * reads
 */
#define DUMMY_SIZE			1 /* Number of dummy bytes for fast, dual and
 * quad reads
 */
#define DUMMY_CLOCKS		8 /* Number of dummy bytes for fast */
#define RD_ID_SIZE			4 /* Read ID command + 3 bytes ID response */
#define BULK_ERASE_SIZE		1 /* Bulk Erase command size */
#define SEC_ERASE_SIZE		4 /* Sector Erase command + Sector address */
#define BANK_SEL_SIZE		2 /* BRWR or EARWR command + 1 byte bank value */
#define RD_CFG_SIZE			2 /* 1 byte Configuration register + RD CFG command*/
#define WR_CFG_SIZE			3 /* WRR command + 1 byte each Status and Config Reg*/
#define DIE_ERASE_SIZE		4 /* Die Erase command + Die address */
/*
 * The following constants specify the extra bytes which are sent to the
 * Flash on the QSPIPSu interface, that are not data, but control information
 * which includes the command and address
 */
#define OVERHEAD_SIZE		4

/*
 * Base address of Flash1
 */
#define FLASH1BASE 0x0000000

/*
 * Sixteen MB
 */
#define SIXTEENMB 0x1000000

/*
 * Mask for quad enable bit in Flash configuration register
 */
#define FLASH_QUAD_EN_MASK 0x02

#define FLASH_SRWD_MASK 0x80

/*
 * Bank mask
 */
#define BANKMASK 0xF000000

/*
 * Identification of Flash
 * Micron:
 * Byte 0 is Manufacturer ID;
 * Byte 1 is first byte of Device ID - 0xBB or 0xBA
 * Byte 2 is second byte of Device ID describes flash size:
 * 128Mbit : 0x18; 256Mbit : 0x19; 512Mbit : 0x20
 */
#define MICRON_ID_BYTE0			0x20
#define MICRON_ID_BYTE2_128		0x18
#define MICRON_ID_BYTE2_256		0x19
#define MICRON_ID_BYTE2_512		0x20
#define MICRON_ID_BYTE2_1G		0x21
//#define MICRON_ID_BYTE2_2G		0x22	// TODO:

#define SPANSION_ID_BYTE0		0x01
#define SPANSION_ID_BYTE2_128	0x18
#define SPANSION_ID_BYTE2_256	0x19
#define SPANSION_ID_BYTE2_512	0x20

#define WINBOND_ID_BYTE0		0xEF
#define WINBOND_ID_BYTE2_128	0x18

#define MACRONIX_ID_BYTE0		0xC2
#define MACRONIX_ID_BYTE2_1G	0x1B

#define ISSI_ID_BYTE0			0x9D
#define ISSI_ID_BYTE2_256		0x19

/*
 * The index for Flash config table
 */
/* Spansion*/
#define SPANSION_INDEX_START			0
#define FLASH_CFG_TBL_SINGLE_128_SP		SPANSION_INDEX_START
#define FLASH_CFG_TBL_STACKED_128_SP	(SPANSION_INDEX_START + 1)
#define FLASH_CFG_TBL_PARALLEL_128_SP	(SPANSION_INDEX_START + 2)
#define FLASH_CFG_TBL_SINGLE_256_SP		(SPANSION_INDEX_START + 3)
#define FLASH_CFG_TBL_STACKED_256_SP	(SPANSION_INDEX_START + 4)
#define FLASH_CFG_TBL_PARALLEL_256_SP	(SPANSION_INDEX_START + 5)
#define FLASH_CFG_TBL_SINGLE_512_SP		(SPANSION_INDEX_START + 6)
#define FLASH_CFG_TBL_STACKED_512_SP	(SPANSION_INDEX_START + 7)
#define FLASH_CFG_TBL_PARALLEL_512_SP	(SPANSION_INDEX_START + 8)

/* Micron */
#define MICRON_INDEX_START				(FLASH_CFG_TBL_PARALLEL_512_SP + 1)
#define FLASH_CFG_TBL_SINGLE_128_MC		MICRON_INDEX_START
#define FLASH_CFG_TBL_STACKED_128_MC	(MICRON_INDEX_START + 1)
#define FLASH_CFG_TBL_PARALLEL_128_MC	(MICRON_INDEX_START + 2)
#define FLASH_CFG_TBL_SINGLE_256_MC		(MICRON_INDEX_START + 3)
#define FLASH_CFG_TBL_STACKED_256_MC	(MICRON_INDEX_START + 4)
#define FLASH_CFG_TBL_PARALLEL_256_MC	(MICRON_INDEX_START + 5)
#define FLASH_CFG_TBL_SINGLE_512_MC		(MICRON_INDEX_START + 6)
#define FLASH_CFG_TBL_STACKED_512_MC	(MICRON_INDEX_START + 7)
#define FLASH_CFG_TBL_PARALLEL_512_MC	(MICRON_INDEX_START + 8)
#define FLASH_CFG_TBL_SINGLE_1GB_MC		(MICRON_INDEX_START + 9)
#define FLASH_CFG_TBL_STACKED_1GB_MC	(MICRON_INDEX_START + 10)
#define FLASH_CFG_TBL_PARALLEL_1GB_MC	(MICRON_INDEX_START + 11)

/* Winbond */
#define WINBOND_INDEX_START				(FLASH_CFG_TBL_PARALLEL_1GB_MC + 1)
#define FLASH_CFG_TBL_SINGLE_128_WB		WINBOND_INDEX_START
#define FLASH_CFG_TBL_STACKED_128_WB	(WINBOND_INDEX_START + 1)
#define FLASH_CFG_TBL_PARALLEL_128_WB	(WINBOND_INDEX_START + 2)

/* Macronix */
#define MACRONIX_INDEX_START			(FLASH_CFG_TBL_PARALLEL_128_WB + 1)
#define FLASH_CFG_TBL_SINGLE_1G_MX		MACRONIX_INDEX_START
#define FLASH_CFG_TBL_STACKED_1G_MX		(MACRONIX_INDEX_START + 1)
#define FLASH_CFG_TBL_PARALLEL_1G_MX	(MACRONIX_INDEX_START + 2)

/* ISSI */
#define ISSI_INDEX_START				(FLASH_CFG_TBL_PARALLEL_1G_MX + 1)
#define FLASH_CFG_TBL_SINGLE_256_ISSI	ISSI_INDEX_START
#define FLASH_CFG_TBL_STACKED_256_ISSI	(ISSI_INDEX_START + 1)
#define FLASH_CFG_TBL_PARALLEL_256_ISSI	(ISSI_INDEX_START + 2)

/*
 * The following constants map to the XPAR parameters created in the
 * xparameters.h file. They are defined here such that a user can easily
 * change all the needed parameters in one place.
 */
#define QSPIPSU_DEVICE_ID		XPAR_XQSPIPSU_0_DEVICE_ID

/*
 * Max page size to initialize write and read buffer
 */
#define PAGE_SIZE 256
#define MAX_FLASH_LEN   32*1024*1024
// u32  FLASH_UPGRADE_ADDRESS =0x800000;   //  8MB

#define ENTER_4B	1
#define EXIT_4B		0

#define MAX_TX_SIZE 256*10000
/**************************** Type Definitions *******************************/

typedef struct{
	u32 SectSize;			/* Individual sector size or combined sector size in case of parallel config */
	u32 NumSect;			/* Total no. of sectors in one/two flash devices */
	u32 PageSize;			/* Individual page size or combined page size in case of parallel config */
	u32 NumPage;			/* Total no. of pages in one/two flash devices */
	u32 FlashDeviceSize;	/* This is the size of one flash device NOT the combination of both devices, if present */
	u8 ManufacturerID;		/* Manufacturer ID - used to identify make */
	u8 DeviceIDMemSize;		/* Byte of device ID indicating the memory size */
	u32 SectMask;			/* Mask to get sector start address */
	u8 NumDie;				/* No. of die forming a single flash */
} FlashInfo;



/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/

int qspi_init(void);
#if defined (UDP_UPDATE) || defined (TCP_UPDATE)
//int qspi_update(u32 total_bytes, const u8 *flash_data);
int qspi_update(u32 total_bytes, const u8 *flash_data, int type);
#endif // UDP_UPDATE || TCP_UPDATE
//int update_qspi(XQspiPsu *QspiPsuInstancePtr, u16 QspiPsuDeviceId, unsigned int TotoalLen, char *FlashDataToSend) ;

int FlashReadID(XQspiPsu *QspiPsuPtr);
int FlashErase(XQspiPsu *QspiPsuPtr, u32 Address, u32 ByteCount, u8 *WriteBfrPtr);
int FlashWrite(XQspiPsu *QspiPsuPtr, u32 Address, u32 ByteCount, u8 Command,
		u8 *WriteBfrPtr);
int FlashRead(XQspiPsu *QspiPsuPtr, u32 Address, u32 ByteCount, u8 Command,
		u8 *WriteBfrPtr, u8 *ReadBfrPtr);
u32 GetRealAddr(XQspiPsu *QspiPsuPtr, u32 Address);
int BulkErase(XQspiPsu *QspiPsuPtr, u8 *WriteBfrPtr);
int DieErase(XQspiPsu *QspiPsuPtr, u8 *WriteBfrPtr);
int FlashEnterExit4BAddMode(XQspiPsu *QspiPsuPtr,unsigned int Enable);
int FlashEnableQuadMode(XQspiPsu *QspiPsuPtr);


void print_percent(int percent) ;

#if (XPAR_XQSPIPSU_NUM_INSTANCES == 1U)
extern XQspiPsu QspiInstance;
#endif


//int update_qspi(XQspiPsu *QspiPsuInstancePtr, u16 QspiPsuDeviceId, unsigned int TotoalLen, char *FlashDataToSend) ;


#endif // XPAR_XQSPIPSU_NUM_INSTANCES

#if defined (XPAR_XQSPIPS_NUM_INSTANCES)
#define __QSPI_FLASH_H__
#include "xqspips.h"
#define QSPI_DEVICE_ID		XPAR_XQSPIPS_0_DEVICE_ID

//发送到FLASH器件的指令
#define WRITE_STATUS_CMD	0x01
#define WRITE_CMD			0x02
#define READ_CMD			0x03
#define WRITE_DISABLE_CMD	0x04
#define READ_STATUS_CMD		0x05
#define WRITE_ENABLE_CMD	0x06
#define FAST_READ_CMD		0x0B
#define DUAL_READ_CMD		0x3B
#define QUAD_READ_CMD		0x6B
#define BULK_ERASE_CMD		0xC7
#define	SEC_ERASE_CMD		0xD8
#define READ_ID				0x9F
#define WRITE_CMD_4B		0x12
#define READ_CMD_4B			0x13
#define	SEC_ERASE_CMD_4B	0xDC
#define FAST_READ_CMD_4B	0x0C
#define DUAL_READ_CMD_4B	0x3C
#define QUAD_READ_CMD_4B	0x6C
#define ENTER_4B_ADDR_MODE	0xB7
#define EXIT_4B_ADDR_MODE	0xE9
#define EXIT_4B_ADDR_MODE_ISSI	0x29
#define BANK_REG_RD			0x16
#define BANK_REG_WR			0x17
/* Bank register is called Extended Address Register in Micron */
#define EXTADD_REG_RD		0xC8
#define EXTADD_REG_WR		0xC5
#define	DIE_ERASE_CMD		0xC4
#define READ_FLAG_STATUS_CMD	0x70

//FLASH BUFFER
#define COMMAND_OFFSET		0  // FLASH instruction */
#define ADDRESS_1_OFFSET	1  // MSB byte of address to read or write */
#define ADDRESS_2_OFFSET	2  // Middle byte of address to read or write */
#define ADDRESS_3_OFFSET	3  // LSB byte of address to read or write */
#define ADDRESS_4_OFFSET	4 /* LSB byte of address to read or write when 4 byte address */
#define DATA_OFFSET			4  // Start of Data for Read/Write */
#define DUMMY_OFFSET		4  // Dummy byte offset for fast, dual and quad reads
#define DUMMY_SIZE			1  // Number of dummy bytes for fast, dual and quad reads
#define RD_ID_SIZE			4  // Read ID command + 3 bytes ID response */
#define READ_STATUS_SIZE	2
#define WRITE_STATUS_SIZE	2
#define BULK_ERASE_SIZE		1  // Bulk Erase command size */
#define SEC_ERASE_SIZE		4  // Sector Erase command + Sector address */
#define OVERHEAD_SIZE		4  // control information: command and address
#define FLASH_SR_IS_READY_MASK		0x01 /* Ready mask */

#define PAGE_SIZE		256			// modify if needed！！！！
#define MAX_FLASH_LEN   32*1024*1024
/*
 * Sixteen MB
 */
#define SIXTEENMB 0x1000000

/*
 * Identification of Flash
 * Micron:
 * Byte 0 is Manufacturer ID;
 * Byte 1 is first byte of Device ID - 0xBB or 0xBA
 * Byte 2 is second byte of Device ID describes flash size:
 * 128Mbit : 0x18; 256Mbit : 0x19; 512Mbit : 0x20
 */
#define MICRON_ID_BYTE0			0x20
#define MICRON_ID_BYTE2_128		0x18
#define MICRON_ID_BYTE2_256		0x19
#define MICRON_ID_BYTE2_512		0x20
#define MICRON_ID_BYTE2_1G		0x21
//#define MICRON_ID_BYTE2_2G		0x22	// TODO:

#define SPANSION_ID_BYTE0		0x01
#define SPANSION_ID_BYTE2_128	0x18
#define SPANSION_ID_BYTE2_256	0x19
#define SPANSION_ID_BYTE2_512	0x20

#define WINBOND_ID_BYTE0		0xEF
#define WINBOND_ID_BYTE2_128	0x18
#define WINBOND_ID_BYTE2_256	0x19

#define MACRONIX_ID_BYTE0		0xC2
#define MACRONIX_ID_BYTE2_1G	0x1B

#define ISSI_ID_BYTE0			0x9D
#define ISSI_ID_BYTE2_256		0x19

/*
 * The index for Flash config table
 */
/* Spansion*/
#define SPANSION_INDEX_START			0
#define FLASH_CFG_TBL_SINGLE_128_SP		SPANSION_INDEX_START
#define FLASH_CFG_TBL_SINGLE_256_SP		(SPANSION_INDEX_START + 1)
#define FLASH_CFG_TBL_SINGLE_512_SP		(SPANSION_INDEX_START + 2)
/* Micron */
#define MICRON_INDEX_START			(FLASH_CFG_TBL_SINGLE_512_SP + 1)
#define FLASH_CFG_TBL_SINGLE_128_MC		MICRON_INDEX_START
#define FLASH_CFG_TBL_SINGLE_256_MC		(MICRON_INDEX_START + 1)
#define FLASH_CFG_TBL_SINGLE_512_MC		(MICRON_INDEX_START + 2)
#define FLASH_CFG_TBL_SINGLE_1GB_MC		(MICRON_INDEX_START + 3)

/* Winbond */
#define WINBOND_INDEX_START			(FLASH_CFG_TBL_SINGLE_1GB_MC + 1)
#define FLASH_CFG_TBL_SINGLE_128_WB		WINBOND_INDEX_START
#define FLASH_CFG_TBL_SINGLE_256_WB		(WINBOND_INDEX_START+1)

/* Macronix */
#define MACRONIX_INDEX_START			(FLASH_CFG_TBL_SINGLE_256_WB + 1)
#define FLASH_CFG_TBL_SINGLE_1G_MX		MACRONIX_INDEX_START

/* ISSI */
#define ISSI_INDEX_START			(FLASH_CFG_TBL_SINGLE_1G_MX + 1)
#define FLASH_CFG_TBL_SINGLE_256_ISSI		ISSI_INDEX_START

#define ENTER_4B	1
#define EXIT_4B		0

typedef struct{
	u32 SectSize;			/* Individual sector size or combined sector size in case of parallel config */
	u32 NumSect;			/* Total no. of sectors in one/two flash devices */
	u32 PageSize;			/* Individual page size or combined page size in case of parallel config */
	u32 NumPage;			/* Total no. of pages in one/two flash devices */
	u32 FlashDeviceSize;	/* This is the size of one flash device NOT the combination of both devices, if present */
	u8 ManufacturerID;		/* Manufacturer ID - used to identify make */
	u8 DeviceIDMemSize;		/* Byte of device ID indicating the memory size */
	u32 SectMask;			/* Mask to get sector start address */
	u8 NumDie;				/* No. of die forming a single flash */
} FlashInfo;

#if (XPAR_XQSPIPS_NUM_INSTANCES == 1U)
extern XQspiPs QspiInstance;
#endif // XPAR_XQSPIPS_NUM_INSTANCES

int qspi_init(void);
#if defined (UDP_UPDATE) || defined (TCP_UPDATE)
//int qspi_update(u32 total_bytes, const u8 *flash_data);
int qspi_update(u32 total_bytes, const u8 *flash_data, int type);
#endif // UDP_UPDATE || TCP_UPDATE
void FlashErase(XQspiPs *QspiPtr, u32 Address, u32 ByteCount);
void FlashWrite(XQspiPs *QspiPtr, u32 Address, u32 ByteCount, u8 Command);
void FlashRead(XQspiPs *QspiPtr, u32 Address, u32 ByteCount, u8 Command);
int FlashReadID(void);
void FlashQuadEnable(XQspiPs *QspiPtr);

#endif // XPAR_XQSPIPS_NUM_INSTANCES


//#if (XPAR_AXI_QUAD_SPI_0_SPI_MODE == 2U)
//#if (XPAR_PROCESSOR_SUBSYSTEM_AXI_QUAD_SPI_0_SPI_MODE == 2U)
#if (XPAR_SPI_0_SPI_MODE == 2U)
#define __QSPI_FLASH_H__
#include "xspi.h"		/* SPI device driver */
#include "xspi_l.h"
//#define QSPI_DEVICE_ID 		XPAR_AXI_QUAD_SPI_0_DEVICE_ID
//#define QSPI_DEVICE_ID 		XPAR_PROCESSOR_SUBSYSTEM_AXI_QUAD_SPI_0_DEVICE_ID
#define QSPI_DEVICE_ID 		XPAR_SPI_0_DEVICE_ID

//??????FLASH?÷????????
#define WRITE_STATUS_CMD	0x01
#define WRITE_CMD			0x02
#define READ_CMD			0x03
#define WRITE_DISABLE_CMD	0x04
#define READ_STATUS_CMD		0x05
#define WRITE_ENABLE_CMD	0x06
#define FAST_READ_CMD		0x0B
#define DUAL_READ_CMD		0x3B
#define QUAD_READ_CMD		0x6B
#define BULK_ERASE_CMD		0xC7
#define	SEC_ERASE_CMD		0xD8
#define READ_ID				0x9F
#define WRITE_CMD_4B		0x12
#define READ_CMD_4B			0x13
#define	SEC_ERASE_CMD_4B	0xDC
#define FAST_READ_CMD_4B	0x0C
#define DUAL_READ_CMD_4B	0x3C
#define QUAD_READ_CMD_4B	0x6C
#define ENTER_4B_ADDR_MODE	0xB7
#define EXIT_4B_ADDR_MODE	0xE9
#define EXIT_4B_ADDR_MODE_ISSI	0x29
#define BANK_REG_RD			0x16
#define BANK_REG_WR			0x17
/* Bank register is called Extended Address Register in Micron */
#define EXTADD_REG_RD		0xC8
#define EXTADD_REG_WR		0xC5
#define	DIE_ERASE_CMD		0xC4
#define READ_FLAG_STATUS_CMD	0x70

//FLASH BUFFER
#define COMMAND_OFFSET		0  // FLASH instruction */
#define ADDRESS_1_OFFSET	1  // MSB byte of address to read or write */
#define ADDRESS_2_OFFSET	2  // Middle byte of address to read or write */
#define ADDRESS_3_OFFSET	3  // LSB byte of address to read or write */
#define ADDRESS_4_OFFSET	4 /* LSB byte of address to read or write when 4 byte address */
#define DATA_OFFSET			4  // Start of Data for Read/Write */
#define DUMMY_OFFSET		4  // Dummy byte offset for fast, dual and quad reads
#define DUMMY_SIZE			1  // Number of dummy bytes for fast, dual and quad reads
#define RD_ID_SIZE			4  // Read ID command + 3 bytes ID response */
#define READ_STATUS_SIZE	2
#define WRITE_STATUS_SIZE	2
#define BULK_ERASE_SIZE		1  // Bulk Erase command size */
#define SEC_ERASE_SIZE		4  // Sector Erase command + Sector address */
#define OVERHEAD_SIZE		4  // control information: command and address
#define FLASH_SR_IS_READY_MASK		0x01 /* Ready mask */

#define PAGE_SIZE		256			// modify if needed！！！！
#define MAX_FLASH_LEN   32*1024*1024
/*
 * Sixteen MB
 */
#define SIXTEENMB 0x1000000

/*
 * Identification of Flash
 * Micron:
 * Byte 0 is Manufacturer ID;
 * Byte 1 is first byte of Device ID - 0xBB or 0xBA
 * Byte 2 is second byte of Device ID describes flash size:
 * 128Mbit : 0x18; 256Mbit : 0x19; 512Mbit : 0x20
 */
#define MICRON_ID_BYTE0			0x20
#define MICRON_ID_BYTE2_128		0x18
#define MICRON_ID_BYTE2_256		0x19
#define MICRON_ID_BYTE2_512		0x20
#define MICRON_ID_BYTE2_1G		0x21
//#define MICRON_ID_BYTE2_2G		0x22	// TODO:

#define SPANSION_ID_BYTE0		0x01
#define SPANSION_ID_BYTE2_128	0x18
#define SPANSION_ID_BYTE2_256	0x19
#define SPANSION_ID_BYTE2_512	0x20

#define WINBOND_ID_BYTE0		0xEF
#define WINBOND_ID_BYTE2_128	0x18
#define WINBOND_ID_BYTE2_256	0x19

#define MACRONIX_ID_BYTE0		0xC2
#define MACRONIX_ID_BYTE2_1G	0x1B

#define ISSI_ID_BYTE0			0x9D
#define ISSI_ID_BYTE2_256		0x19

/*
 * The index for Flash config table
 */
/* Spansion*/
#define SPANSION_INDEX_START			0
#define FLASH_CFG_TBL_SINGLE_128_SP		SPANSION_INDEX_START
#define FLASH_CFG_TBL_SINGLE_256_SP		(SPANSION_INDEX_START + 1)
#define FLASH_CFG_TBL_SINGLE_512_SP		(SPANSION_INDEX_START + 2)
/* Micron */
#define MICRON_INDEX_START			(FLASH_CFG_TBL_SINGLE_512_SP + 1)
#define FLASH_CFG_TBL_SINGLE_128_MC		MICRON_INDEX_START
#define FLASH_CFG_TBL_SINGLE_256_MC		(MICRON_INDEX_START + 1)
#define FLASH_CFG_TBL_SINGLE_512_MC		(MICRON_INDEX_START + 2)
#define FLASH_CFG_TBL_SINGLE_1GB_MC		(MICRON_INDEX_START + 3)

/* Winbond */
#define WINBOND_INDEX_START			(FLASH_CFG_TBL_SINGLE_1GB_MC + 1)
#define FLASH_CFG_TBL_SINGLE_128_WB		WINBOND_INDEX_START
#define FLASH_CFG_TBL_SINGLE_256_WB		(WINBOND_INDEX_START+1)

/* Macronix */
#define MACRONIX_INDEX_START			(FLASH_CFG_TBL_SINGLE_256_WB + 1)
#define FLASH_CFG_TBL_SINGLE_1G_MX		MACRONIX_INDEX_START

/* ISSI */
#define ISSI_INDEX_START			(FLASH_CFG_TBL_SINGLE_1G_MX + 1)
#define FLASH_CFG_TBL_SINGLE_256_ISSI		ISSI_INDEX_START

#define ENTER_4B	1
#define EXIT_4B		0

typedef struct{
	u32 SectSize;			/* Individual sector size or combined sector size in case of parallel config */
	u32 NumSect;			/* Total no. of sectors in one/two flash devices */
	u32 PageSize;			/* Individual page size or combined page size in case of parallel config */
	u32 NumPage;			/* Total no. of pages in one/two flash devices */
	u32 FlashDeviceSize;	/* This is the size of one flash device NOT the combination of both devices, if present */
	u8 ManufacturerID;		/* Manufacturer ID - used to identify make */
	u8 DeviceIDMemSize;		/* Byte of device ID indicating the memory size */
	u32 SectMask;			/* Mask to get sector start address */
	u8 NumDie;				/* No. of die forming a single flash */
} FlashInfo;

extern XSpi XSpiInstance;

int qspi_init(void);
#if defined (UDP_UPDATE) || defined (TCP_UPDATE)
//int qspi_update(u32 total_bytes, const u8 *flash_data);
int qspi_update(u32 total_bytes, const u8 *flash_data, int type);
#endif // UDP_UPDATE || TCP_UPDATE
void FlashErase(XSpi *XSpiPtr, u32 Address, u32 ByteCount);
void FlashWrite(XSpi *XSpiPtr, u32 Address, u32 ByteCount, u8 Command);
void FlashRead(XSpi *XSpiPtr, u32 Address, u32 ByteCount, u8 Command);
int FlashReadID(void);
void FlashQuadEnable(XSpi *XSpiPtr);

//#endif // #if (XPAR_AXI_QUAD_SPI_0_SPI_MODE == 2U)
//#endif // #if (XPAR_PROCESSOR_SUBSYSTEM_AXI_QUAD_SPI_0_SPI_MODE == 2U)
#endif // #if (XPAR_SPI_0_SPI_MODE == 2U)

#endif // __QSPI_FLASH_H__
