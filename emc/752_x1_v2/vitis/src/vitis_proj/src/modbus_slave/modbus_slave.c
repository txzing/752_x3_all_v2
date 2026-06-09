#include "../bsp.h"

#if defined (MODBUS_RTU_SLAVE)

static void MODS_SendWithCRC(uint8_t *_pBuf, uint8_t _ucLen);
static void MODS_SendAckOk(void);
static void MODS_SendAckErr(uint8_t _ucErrCode);
extern void save_id(u16 id);

static void MODS_AnalyzeApp(void);

//static void MODS_RxTimeOut(void);

//static void MODS_01H(void);
//static void MODS_02H(void);
static void MODS_03H(void);
static void MODS_04H(void);
//static void MODS_05H(void);
static void MODS_06H(void);
//static void MODS_10H(void);

static uint8_t MODS_ReadRegValue(uint16_t reg_addr, uint8_t *reg_value);
static uint8_t MODS_WriteRegValue(uint16_t reg_addr, uint16_t reg_value);

void MODS_ReciveNew(uint8_t _byte);
MODS_T g_tModS;
VAR_T g_tVar;
u16 g_ID = 0x01;

extern u32 spi_channel_addr[12];

#if defined (USING_EEPROM)

static uint8_t checksum(uint8_t * ptr, int16_t cnt)
{
	int16_t i = 0;
	uint8_t calc=0;
	for(i=0;i<cnt;i++)
	{
		calc=calc + ptr[i];
	}

	return calc=~calc&0xff;
}

void save_id(u16 id)
{
	int32_t	Status;
	u8 write_buf[5] = {0};
	write_buf[0] = 0x20;
	write_buf[1] = id >> 8;
	write_buf[2] = id;
	write_buf[3] = 0x02;
	write_buf[4] = checksum(write_buf,4);
	Status = EepromWriteData(&ps_i2c_0,write_buf,5,0x100);
	if (Status != XST_SUCCESS) {
			return XST_FAILURE;
	}
}
void read_id(void)
{
	u8 read_buf[5] = {0};
	u8 i = 0;
	for(i=0;i<10;i++)
	{
		EepromReadData(&ps_i2c_0,read_buf,5,0x100);
		if(read_buf[4] == checksum(read_buf,4))
		{
			g_ID = (u16)read_buf[1] << 8 | read_buf[2];
			break;
		}
		else
		{
			g_ID = 0;
		}
	}
	xil_printf("Power_up g_ID = 0x%02x\r\n", g_ID);
}
#endif

/*
*********************************************************************************************************
*	函 数 名: MODS_ReadRegValue
*	功能说明: 读取保持寄存器的值
*	形    参: reg_addr 寄存器地址
*			  reg_value 存放寄存器结果
*	返 回 值: 1表示OK 0表示错误
*********************************************************************************************************/
static uint8_t MODS_ReadRegValue(uint16_t reg_addr, uint8_t *reg_value)
{
	uint16_t *value=NULL;

	switch (reg_addr)									/* 判断寄存器地址 */
	{
		case SLAVE_REG_P01:

			break;

		case SLAVE_REG_P02:

			break;
		case SLAVE_REG_P03:

			break;
		case SLAVE_REG_P04:

			break;
		default:
			return 0;									/* 参数异常，返回 0 */
	}
		reg_value[0] = value[0] >> 8;
		reg_value[1] = value[0];



	return 1;											/* 读取成功 */
}

/*
*********************************************************************************************************
*	函 数 名: MODS_WriteRegValue
*	功能说明: 读取保持寄存器的值
*	形    参: reg_addr 寄存器地址
*			  reg_value 寄存器值
*	返 回 值: 1表示OK 0表示错误
*********************************************************************************************************
*/
static uint8_t MODS_WriteRegValue(uint16_t reg_addr, uint16_t reg_value)
{
	switch (reg_addr)							/* 判断寄存器地址 */
	{
	    case SLAVE_REG_P00:
	    	g_tVar.P00 = reg_value;
	    	g_ID = g_tVar.P00;
	    	xil_printf("now modbus g_ID = 0x%02x\r\n", g_ID);
	    	save_id(g_ID);
        break;
		case SLAVE_REG_P01:
			g_tVar.P01 = reg_value;				/* 将值写入保存寄存器 */
			break;

		case SLAVE_REG_P02:
			g_tVar.P02 = reg_value;
			break;

		case SLAVE_REG_P03:
			g_tVar.P03 = reg_value;
			break;

		case SLAVE_REG_P04:
			g_tVar.P04 = reg_value;
			break;

		case SLAVE_REG_P05:
			g_tVar.P05 = reg_value;
			break;

		case SLAVE_REG_P06:
			g_tVar.P06 = reg_value;
			break;

		case SLAVE_REG_P07:
			g_tVar.P07 = reg_value;
			break;

		case SLAVE_REG_P08:
			g_tVar.P08 = reg_value;
			break;

		case SLAVE_REG_P09:
			g_tVar.P09 = reg_value;
			break;

		case SLAVE_REG_P010:
			g_tVar.P010 = reg_value;
			break;

		case SLAVE_REG_P011:
			g_tVar.P011 = reg_value;
			break;

		case SLAVE_REG_P012:
			g_tVar.P012 = reg_value;
			break;

		default:
			return 0;		/* 参数异常，返回 0 */
	}

	return 1;		/* 读取成功 */
}

#if 0
/*
*********************************************************************************************************
*	函 数 名: MODS_RxTimeOut
*	功能说明: 超过3.5个字符时间后执行本函数。 设置全局变量 g_mods_timeout = 1; 通知主程序开始解码。
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void MODS_RxTimeOut(void)
{
	g_mods_timeout = 1;
}
#endif

/*
*********************************************************************************************************
*	函 数 名: MODS_SendWithCRC
*	功能说明: 发送一串数据, 自动追加2字节CRC
*	形    参: _pBuf 数据；
*			  _ucLen 数据长度（不带CRC）
*	返 回 值: 无
*********************************************************************************************************
*/
static void MODS_SendWithCRC(uint8_t *_pBuf, uint8_t _ucLen)
{
	uint16_t crc;
	uint8_t buf[S_TX_BUF_SIZE];

	memcpy(buf, _pBuf, _ucLen);
	crc = CRC16_Modbus(_pBuf, _ucLen);
	buf[_ucLen++] = crc >> 8;
	buf[_ucLen++] = crc;

//	printf("_ucLen %d\n",_ucLen) ;
//	printf("crc 0x%x\n",crc) ;


	RS485_SendBuf(buf, _ucLen);
}


/*
*********************************************************************************************************
*	函 数 名: MODS_SendAckErr
*	功能说明: 发送错误应答
*	形    参: _ucErrCode : 错误代码
*	返 回 值: 无
*********************************************************************************************************
*/
static void MODS_SendAckErr(uint8_t _ucErrCode)
{
	uint8_t txbuf[3];

	txbuf[0] = g_tModS.RxBuf[0];					/* 485地址 */
	txbuf[1] = g_tModS.RxBuf[1] | 0x80;				/* 异常的功能码 */
	txbuf[2] = _ucErrCode;							/* 错误代码(01,02,03,04) */

	MODS_SendWithCRC(txbuf, 3);
}

/*
*********************************************************************************************************
*	函 数 名: MODS_SendAckOk
*	功能说明: 发送正确的应答.
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void MODS_SendAckOk(void)
{
	uint8_t txbuf[6];
	uint8_t i;

	for (i = 0; i < 6; i++)
	{
		txbuf[i] = g_tModS.RxBuf[i];
	}
	MODS_SendWithCRC(txbuf, 6);
}

/*
*********************************************************************************************************
*	函 数 名: MODS_03H
*	功能说明: 读取保持寄存器 在一个或多个保持寄存器中取得当前的二进制值
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void MODS_03H(void)
{
	/*
		从机地址为11H。保持寄存器的起始地址为006BH，结束地址为006DH。该次查询总共访问3个保持寄存器。

		主机发送:
			11 从机地址
			03 功能码
			00 寄存器地址高字节
			6B 寄存器地址低字节
			00 寄存器数量高字节
			03 寄存器数量低字节
			76 CRC高字节
			87 CRC低字节

		从机应答: 	保持寄存器的长度为2个字节。对于单个保持寄存器而言，寄存器高字节数据先被传输，
					低字节数据后被传输。保持寄存器之间，低地址寄存器先被传输，高地址寄存器后被传输。
			11 从机地址
			03 功能码
			06 字节数
			00 数据1高字节(006BH)
			6B 数据1低字节(006BH)
			00 数据2高字节(006CH)
			13 数据2 低字节(006CH)
			00 数据3高字节(006DH)
			00 数据3低字节(006DH)
			38 CRC高字节
			B9 CRC低字节

		例子:
			01 03 30 06 00 01  6B0B      ---- 读 3006H, 触发电流
			01 03 4000 0010 51C6         ---- 读 4000H 倒数第1条浪涌记录 32字节
			01 03 4001 0010 0006         ---- 读 4001H 倒数第1条浪涌记录 32字节

			01 03 F000 0008 770C         ---- 读 F000H 倒数第1条告警记录 16字节
			01 03 F001 0008 26CC         ---- 读 F001H 倒数第2条告警记录 16字节

			01 03 7000 0020 5ED2         ---- 读 7000H 倒数第1条波形记录第1段 64字节
			01 03 7001 0020 0F12         ---- 读 7001H 倒数第1条波形记录第2段 64字节

			01 03 7040 0020 5F06         ---- 读 7040H 倒数第2条波形记录第1段 64字节
	*/
	uint16_t reg;
	uint16_t num;
	uint16_t i;
	uint8_t reg_value[64];

	g_tModS.RspCode = RSP_OK;

	if (g_tModS.RxCount != 8)								/* 03H命令必须是8个字节 */
	{
		g_tModS.RspCode = RSP_ERR_VALUE;					/* 数据值域错误 */
		goto err_ret;
	}


	reg = BEBufToUint16(&g_tModS.RxBuf[2]); 				/* 寄存器号 */
	num = BEBufToUint16(&g_tModS.RxBuf[4]);					/* 寄存器个数 */
//	xil_printf("\r\nreg =0x%2x\r\n",reg);
//	xil_printf("num =0x%2x\r\n",num);

	if(num == 0 ||  num > 200) // H03功能码本项目只允许一个地址，即只有H0001寄存器地址合法, 那么只有一个寄存器数量是合法的
	{
		g_tModS.RspCode = RSP_ERR_VALUE;					/* 数据值域错误 */
		goto err_ret;
	}

	for (i = 0; i < num; i++)
	{
		if (MODS_ReadRegValue(reg, &reg_value[2 * i]) == 0)	/* 读出寄存器值放入reg_value */
		{

			g_tModS.RspCode = RSP_ERR_REG_ADDR;				/* 寄存器地址错误 */
			break;
		}
		reg++;
	}

err_ret:
	if (g_tModS.RspCode == RSP_OK)							/* 正确应答 */
	{
		g_tModS.TxCount = 0;
		g_tModS.TxBuf[g_tModS.TxCount++] = g_tModS.RxBuf[0];
		g_tModS.TxBuf[g_tModS.TxCount++] = g_tModS.RxBuf[1];
		g_tModS.TxBuf[g_tModS.TxCount++] = num * 2;			/* 返回字节数 */

		for (i = 0; i < num; i++)
		{
			g_tModS.TxBuf[g_tModS.TxCount++] = reg_value[2*i];
			g_tModS.TxBuf[g_tModS.TxCount++] = reg_value[2*i+1];
		}
		MODS_SendWithCRC(g_tModS.TxBuf, g_tModS.TxCount);	/* 发送正确应答 */
	}
	else
	{
		MODS_SendAckErr(g_tModS.RspCode);					/* 发送错误应答 */
	}
}

/*
*********************************************************************************************************
*	函 数 名: MODS_04H
*	功能说明: 读取输入寄存器（对应A01/A02） SMA
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void MODS_04H(void)
{
	/*
		主机发送:
			11 从机地址
			04 功能码
			00 寄存器起始地址高字节
			08 寄存器起始地址低字节
			00 寄存器个数高字节
			02 寄存器个数低字节
			F2 CRC高字节
			99 CRC低字节

		从机应答:  输入寄存器长度为2个字节。对于单个输入寄存器而言，寄存器高字节数据先被传输，
				低字节数据后被传输。输入寄存器之间，低地址寄存器先被传输，高地址寄存器后被传输。
			11 从机地址
			04 功能码
			04 字节数
			00 数据1高字节(0008H)
			0A 数据1低字节(0008H)
			00 数据2高字节(0009H)
			0B 数据2低字节(0009H)
			8B CRC高字节
			80 CRC低字节

		例子:

			01 04 2201 0006 2BB0  --- 读 2201H A01通道模拟量 开始的6个数据
			01 04 2201 0001 6A72  --- 读 2201H

	*/
	uint16_t reg;
	uint16_t num;
	uint16_t i;
	uint16_t status[10];

	memset(status, 0, 10);

	g_tModS.RspCode = RSP_OK;

	if (g_tModS.RxCount != 8)
	{
		g_tModS.RspCode = RSP_ERR_VALUE;	/* 数据值域错误 */
		goto err_ret;
	}

	reg = BEBufToUint16(&g_tModS.RxBuf[2]); 	/* 寄存器号 */
	num = BEBufToUint16(&g_tModS.RxBuf[4]);	   /* 寄存器个数 */

	// 对H04功能码本项目只有一个合法地址0x0001
	if(reg != REG_M00)
	{
		g_tModS.RspCode = RSP_ERR_REG_ADDR;		/* 寄存器地址域错误 */
		goto err_ret;
	}

	if(num != 1)
	{
		g_tModS.RspCode = RSP_ERR_REG_ADDR;		/* 寄存器地址域错误 */
	}


//	if ((reg >= REG_A01) && (num > 0) && (reg + num <= REG_AXX + 1))
//	{
		for (i = 0; i < num; i++)
		{
			switch (reg)
			{
				/* 测试参数 */
				case REG_M00:
					status[i] = g_tVar.A01;
					break;

				default:
					status[i] = 0;
					break;
			}
			reg++;
		}
//	}
//	else
//	{
//		g_tModS.RspCode = RSP_ERR_REG_ADDR;		/* 寄存器地址错误 */
//	}

err_ret:
	if (g_tModS.RspCode == RSP_OK)		/* 正确应答 */
	{
		g_tModS.TxCount = 0;
		g_tModS.TxBuf[g_tModS.TxCount++] = g_tModS.RxBuf[0];
		g_tModS.TxBuf[g_tModS.TxCount++] = g_tModS.RxBuf[1];
		g_tModS.TxBuf[g_tModS.TxCount++] = num * 2;			/* 返回字节数 */

		for (i = 0; i < num; i++)
		{
			g_tModS.TxBuf[g_tModS.TxCount++] = status[i] >> 8;
			g_tModS.TxBuf[g_tModS.TxCount++] = status[i] & 0xFF;
		}
		MODS_SendWithCRC(g_tModS.TxBuf, g_tModS.TxCount);
	}
	else
	{
		MODS_SendAckErr(g_tModS.RspCode);	/* 告诉主机命令错误 */
	}
}

/*
*********************************************************************************************************
*	函 数 名: MODS_05H
*	功能说明: 设置连续读取多个通道
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void MODS_05H(void)
{

}



/*
*********************************************************************************************************
*	函 数 名: MODS_06H
*	功能说明: 写单个寄存器
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void MODS_06H(void)
{

	uint16_t reg_addr;
	uint16_t reg_value;

	g_tModS.RspCode = RSP_OK;

	if (g_tModS.RxCount != 8)
	{
		g_tModS.RspCode = RSP_ERR_VALUE;		/* 数据值域错误 */
		goto err_ret;
	}

	reg_addr = BEBufToUint16(&g_tModS.RxBuf[2]); 	/* 寄存器号 */
	reg_value = BEBufToUint16(&g_tModS.RxBuf[4]);	/* 寄存器值 */

	xil_printf("reg_addr = %d\r\n",reg_addr);
	xil_printf("reg_value =  %d\r\n",reg_value);


	if(reg_addr > SLAVE_REG_P012) //
	{
		g_tModS.RspCode = RSP_ERR_WRITE;		/* 寄存器地址域错误 */
		goto err_ret;
	}


	if (MODS_WriteRegValue(reg_addr, reg_value) == 1)	/* 该函数会把写入的值存入寄存器 */
	{
		;
	}
	else
	{
		g_tModS.RspCode = RSP_ERR_REG_ADDR;		/* 寄存器地址错误 */
	}

err_ret:
	if (g_tModS.RspCode == RSP_OK)				/* 正确应答 */
	{
//		MODS_SendAckOk();
		RS485_SendBuf(g_tModS.RxBuf, g_tModS.RxCount);
	}
	else
	{
		MODS_SendAckErr(g_tModS.RspCode);		/* 告诉主机命令错误 */
	}
}

/*
*********************************************************************************************************
*	函 数 名: MODS_10H
*	功能说明: 写多个寄存器
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void MODS_10H(void)
{

}

/*
*********************************************************************************************************
*	函 数 名: MODS_AnalyzeApp
*	功能说明: 分析应用层协议
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void MODS_AnalyzeApp(void)
{
	switch (g_tModS.RxBuf[1])				/* 第2个字节 功能码 */
	{
//		case 0x01:							/* 读取线圈状态（此例程用led代替）*/
//			MODS_01H();
//			break;

		//case 0x02:							/* 读取输入状态（按键状态）*/
			//MODS_02H();
			//break;

		case 0x03:							/* 读取保持寄存器（此例程存在g_tVar中）*/
			MODS_03H();
			break;

		case 0x04:							/* 读取输入寄存器（ADC的值）*/
			MODS_04H();
			break;

		case 0x05:							/*连续读*/
			MODS_05H();
			break;

		case 0x06:							/* 写单个保存寄存器（此例程改写g_tVar中的参数）*/
			MODS_06H();
			break;

		case 0x10:						/* 写多个保存寄存器（此例程存在g_tVar中的参数）*/
			MODS_10H();
			break;

		default:
			g_tModS.RspCode = RSP_ERR_CMD;
			MODS_SendAckErr(g_tModS.RspCode);	/* 告诉主机命令错误 */
			break;
	}
}

void MODS_Poll(void)
{
	uint16_t addr;
	uint16_t crc_data;
	/* 超过3.5个字符时间后执行MODH_RxTimeOut()函数。全局变量 g_rtu_timeout = 1; 通知主程序开始解码 */
	if (g_mods_timeout == 0)
	{
		return;								/* 没有超时，即没有接受完，继续接收。不要清零 g_tModS.RxCount */
	}

	g_mods_timeout = 0;	 					/* 清标志 */

	if (g_tModS.RxCount < 4)				/* 接收到的数据小于4个字节就认为错误 */
	{
		g_tModS.RxCount = 0;
		return;
	}
//	printf("g_tModS.RxCount  %d\n", g_tModS.RxCount);
	/* 计算CRC校验和 */
	crc_data = CRC16_Modbus(g_tModS.RxBuf, g_tModS.RxCount);
	if (crc_data != 0)
	{
		printf("crc error  %x\n", crc_data) ;
		g_tModS.RspCode = RSP_ERR_RECV;
		MODS_SendAckErr(g_tModS.RspCode);					/* 发送错误应答 */
		g_tModS.RxCount = 0;
		return;
	}

	/* 站地址 (1字节） */
	addr = g_tModS.RxBuf[0];				/* 第1字节 站号 */
	if (addr != g_ID)	 			/* 判断主机发送的命令地址是否符合 */
	{
		if(addr == 0x00)
		{
			MODS_AnalyzeApp();
		}
		g_tModS.RxCount = 0;
		return;
	}

	/* 分析应用层协议 */
	MODS_AnalyzeApp();

	g_tModS.RxCount = 0;					/* 必须清零计数器，方便下次帧同步 */


}

void MODS_ReciveNew(uint8_t _byte)
{
	/*
		3.5个字符的时间间隔，只是用在RTU模式下面，因为RTU模式没有开始符和结束符，
		两个数据包之间只能靠时间间隔来区分，Modbus定义在不同的波特率下，间隔时间是不一样的，
		所以就是3.5个字符的时间，波特率高，这个时间间隔就小，波特率低，这个时间间隔相应就大

			4800  = 7.297ms
			9600  = 3.646ms
			19200  = 1.771ms
			38400  = 0.885ms
		*/
	uint32_t timeout;
	g_mods_timeout = 0;
//	timeout = 35000000 / SBAUD485;
//	timeout = 275;
//	timeout = 10;

#if 0
	// 如果收出现错误，可以适当增大间隔
	#if (UART0_BAUD == 4800U)
		modbus_timer_start(7297*modbus_us);
	#elif (UART0_BAUD == 9600U)
		modbus_timer_start(3646*modbus_us);
	#elif (UART0_BAUD == 19200U)
		modbus_timer_start(1771*modbus_us);
	#elif (UART0_BAUD == 38400U)
		modbus_timer_start(885*modbus_us);
	#else
		modbus_timer_start(20*modbus_us);
	#endif
#else
		modbus_timer_start(20*modbus_ms);
#endif


	if (g_tModS.RxCount < S_RX_BUF_SIZE)
	{
		g_tModS.RxBuf[g_tModS.RxCount++] = _byte;
	}
}

#endif //defined (MODBUS_RTU_SLAVE)
