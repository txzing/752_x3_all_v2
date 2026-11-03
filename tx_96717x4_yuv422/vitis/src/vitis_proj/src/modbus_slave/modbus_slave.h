#ifndef __MODBUY_SLAVE_H_
#include "../config.h"
#if defined (MODBUS_RTU_SLAVE)

#define __MODBUY_SLAVE_H_

#include "util_lib/util_lib.h"

#define SADDR485	1
#define SBAUD485	XPAR_AXI_UARTLITE_0_BAUDRATE

/* 01H 读强制单线圈 */
/* 05H 写强制单线圈 */
#define REG_D01		0x0101
#define REG_D02		0x0102
#define REG_D03		0x0103
#define REG_D04		0x0104
#define REG_DXX 	REG_D04

/* 02H 读取输入状态 */
#define REG_T01		0x0201
#define REG_T02		0x0202
#define REG_T03		0x0203
#define REG_TXX		REG_T03

/* 03H 读保持寄存器 */
/* 06H 写保持寄存器 */
/* 10H 写多个保存寄存器 */
#define SLAVE_REG_P00       0x0000
#define SLAVE_REG_P01		0x0001
#define SLAVE_REG_P02		0x0002
#define SLAVE_REG_P03		0x0003
#define SLAVE_REG_P04       0x0004
#define SLAVE_REG_P05		0x0005
#define SLAVE_REG_P06		0x0006
#define SLAVE_REG_P07		0x0007
#define SLAVE_REG_P08       0x0008
#define SLAVE_REG_P09		0x0009
#define SLAVE_REG_P010		0x000a
#define SLAVE_REG_P011		0x000b
#define SLAVE_REG_P012      0x000c

/* 04H 读取输入寄存器(模拟信号) */
//模式寄存器
#define REG_M00		0x0000
#define REG_M01		0x0001
#define REG_M02		0x0002
#define REG_M03		0x0003
#define REG_M04		0x0004
#define REG_M05		0x0005
#define REG_M06		0x0006
#define REG_M07		0x0007
#define REG_M08		0x0008


/* RTU 应答代码 */
#define RSP_OK				0x00		/* 成功 */
#define RSP_ERR_CMD			0x01	/* 不支持的功能码 */
#define RSP_ERR_REG_ADDR	0x02	/* 寄存器地址错误 */
#define RSP_ERR_VALUE		0x03	/* 数据值域错误 */
#define RSP_ERR_WRITE		0x04	/* 写入失败 */
#define RSP_ERR_RECV		0x05	/* 接收失败 */

#define S_RX_BUF_SIZE		1200
#define S_TX_BUF_SIZE		1200

typedef struct
{
	volatile uint8_t RxBuf[S_RX_BUF_SIZE];
	volatile uint8_t RxCount;
	volatile uint8_t RxStatus;
	volatile uint8_t RxNewFlag;

	volatile uint8_t RspCode;

	volatile uint8_t TxBuf[S_TX_BUF_SIZE];
	volatile uint8_t TxCount;
}MODS_T;

typedef struct
{
	/* 03H 06H 读写保持寄存器 */
	uint16_t P00;  //用于从机g_ID
	uint16_t P01;
	uint16_t P02;
	uint16_t P03;
	uint16_t P04;
	uint16_t P05;
	uint16_t P06;
	uint16_t P07;
	uint16_t P08;
	uint16_t P09;
	uint16_t P010;
	uint16_t P011;
	uint16_t P012;

	/* 04H 读取模拟量寄存器 */
	uint16_t A01;

	/* 01H 05H 读写单个强制线圈 */
	uint16_t D01;
	uint16_t D02;
	uint16_t D03;
	uint16_t D04;

}VAR_T;

volatile uint8_t g_mods_timeout;

void MODS_Poll(void);

extern MODS_T g_tModS;
extern VAR_T g_tVar;

#endif // #if defined (MODBUS_RTU_SLAVE)
#endif // __MODBUY_SLAVE_H_
