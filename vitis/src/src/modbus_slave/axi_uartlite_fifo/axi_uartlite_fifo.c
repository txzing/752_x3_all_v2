#include "../../bsp.h"
#if defined (MODBUS_RTU_SLAVE)

#if defined (XPAR_XUARTLITE_NUM_INSTANCES) && defined (INTC_DEVICE_ID) && defined (INTC)
#include "xgpio.h"

XGpio RS485_Gpio; /* The Instance of the GPIO Driver */
XUartLite UartLite_RS485;            /* The instance of the UartLite Device */
UART_T g_tUart0;

static uint8_t g_TxBuf0[UART0_TX_BUF_SIZE];		/* 发送缓冲区 */
static uint8_t g_RxBuf0[UART0_RX_BUF_SIZE];		/* 接收缓冲区 */

void UartRxCpltIRQ(UART_T *_pUart)
{
		/* 从串口接收数据寄存器读取数据存放到接收FIFO */
		uint8_t ch;
		ch = XUartLite_ReadReg(_pUart->uart->RegBaseAddress,
									XUL_RX_FIFO_OFFSET);
		//ch = XUartPs_ReadReg(_pUart->uart->Config.BaseAddress, XUARTPS_FIFO_OFFSET);
		_pUart->pRxBuf[_pUart->usRxWrite] = ch;
		if (++_pUart->usRxWrite >= _pUart->usRxBufSize)
		{
			_pUart->usRxWrite = 0;
		}
		if (_pUart->usRxCount < _pUart->usRxBufSize)
		{
			_pUart->usRxCount++;
		}

		/* 回调函数,通知应用程序收到新数据,一般是发送1个消息或者设置一个标记 */
		//if (_pUart->usRxWrite == _pUart->usRxRead)
		//if (_pUart->usRxCount == 1)
		{
			if (_pUart->ReciveNew)
			{
				_pUart->ReciveNew(ch); /* 比如，交给MODBUS解码程序处理字节流 */
			}
		}
}

void UartTxCpltIRQ(UART_T *_pUart)
{
	if(_pUart->usTxCount != 0)
	{
		_pUart->usTxCount--;
	}
	while (_pUart->usTxCount != 0)
	{
		/* 从发送FIFO取1个字节写入串口发送数据寄存器 */
		XUartLite_Send(_pUart->uart, &_pUart->pTxBuf[_pUart->usTxRead], 1);
		//USART_SendData(_pUart->uart, _pUart->pTxBuf[_pUart->usTxRead]);
		if (++_pUart->usTxRead >= _pUart->usTxBufSize)
		{
			_pUart->usTxRead = 0;
		}
		_pUart->usTxCount--;
	}
	//else
	if (_pUart->usTxCount == 0)
	{
		u8 StatusRegister;
		StatusRegister =
					XUartLite_GetStatusReg(_pUart->uart->RegBaseAddress);
		/* 回调函数, 一般用来处理RS485通信，将RS485芯片设置为接收模式，避免抢占总线 */
		while ((StatusRegister & XUL_SR_TX_FIFO_EMPTY) == 0)
		{
			StatusRegister = XUartLite_GetStatusReg(_pUart->uart->RegBaseAddress);
		}

		if (_pUart->SendOver)
		{
			_pUart->SendOver();
		}
	}
}

void Uart_Var_Init(void)
{
	g_tUart0.Intc = &InterruptController;
	g_tUart0.uart = &UartLite_RS485;			/* 串口设备 */
	g_tUart0.pTxBuf = g_TxBuf0;					/* 发送缓冲区指针 */
	g_tUart0.pRxBuf = g_RxBuf0;					/* 接收缓冲区指针 */
	g_tUart0.usTxBufSize = UART0_TX_BUF_SIZE;	/* 发送缓冲区大小 */
	g_tUart0.usRxBufSize = UART0_RX_BUF_SIZE;	/* 接收缓冲区大小 */
	g_tUart0.usTxWrite = 0;						/* 发送FIFO写索引 */
	g_tUart0.usTxRead = 0;						/* 发送FIFO读索引 */
	g_tUart0.usRxWrite = 0;						/* 接收FIFO写索引 */
	g_tUart0.usRxRead = 0;						/* 接收FIFO读索引 */
	g_tUart0.usRxCount = 0;						/* 接收到的新数据个数 */
	g_tUart0.usTxCount = 0;						/* 待发送的数据个数 */
	g_tUart0.SendBefor = RS485_SendBefor;		/* 发送数据前的回调函数 */
	g_tUart0.SendOver = RS485_SendOver;			/* 发送完毕后的回调函数 */
	g_tUart0.ReciveNew = RS485_ReciveNew;		/* 接收到新数据后的回调函数 */
	g_tUart0.Sending = 0;						/* 正在发送中标志 */
}



int Uart_Init_Rs485(INTC *IntcInstancePtr, u16 DeviceId)
{
	int Status;
	u32 StatusRegister;


	/* Initialize the GPIO driver */
	Status = XGpio_Initialize(&RS485_Gpio, XPAR_PROCESSOR_SUBSYSTEM_RS485_HIER_AXI_GPIO_0_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		xil_printf("Gpio Initialization Failed\r\n");
		return XST_FAILURE;
	}
	XGpio_SetDataDirection(&RS485_Gpio, 1, 0x0);
	XGpio_DiscreteWrite(&RS485_Gpio, 1, 0x0);//set rs485 input

	/*
	 * Initialize the UartLite driver so that it's ready to use.
	 */
	Status = XUartLite_Initialize(&UartLite_RS485, DeviceId);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

#if defined (XPAR_XSCUGIC_NUM_INSTANCES)
	XScuGic_SetPriorityTriggerType(IntcInstancePtr, UART_IRPT_INTR, 0x90, 0x3);
#endif

	/*
	 * Connect a device driver handler that will be called when an interrupt
	 * for the device occurs, the device driver handler performs the
	 * specific interrupt processing for the device.
	 */
	Status = INTC_CONNECT(IntcInstancePtr, UART_IRPT_INTR,
			   (XInterruptHandler)XUartLite_InterruptHandler,
			   (void *)&UartLite_RS485);

	XUartLite_SetRecvHandler(&UartLite_RS485, UartRxCpltIRQ, &g_tUart0);
	XUartLite_SetSendHandler(&UartLite_RS485, UartTxCpltIRQ, &g_tUart0);
	XUartLite_EnableInterrupt(&UartLite_RS485);

	/*
	 * Enter a critical region by disabling all the UART interrupts to allow
	 * this call to stop a previous operation that may be interrupt driven
	 */
	StatusRegister = XUartLite_GetStatusReg(UartLite_RS485.RegBaseAddress);
	XUartLite_WriteReg(UartLite_RS485.RegBaseAddress,
				XUL_CONTROL_REG_OFFSET, 0);
	/*
	 * Restore the interrupt enable register to it's previous value such
	 * that the critical region is exited
	 */
	StatusRegister &= XUL_CR_ENABLE_INTR;
	XUartLite_WriteReg(UartLite_RS485.RegBaseAddress,
				XUL_CONTROL_REG_OFFSET, StatusRegister);

	Xil_ExceptionEnable();
	INTC_CONNECT_ENABLE(IntcInstancePtr,UART_IRPT_INTR);

	return XST_SUCCESS;
}

uint8_t UartGetChar(UART_T *_pUart, uint8_t *_pByte)
{
	uint16_t usCount;

	/* usRxWrite 变量在中断函数中被改写，主程序读取该变量时，必须进行临界区保护 */
	DISABLE_INT();
	usCount = _pUart->usRxCount;
	ENABLE_INT();

	/* 如果读和写索引相同，则返回0 */
	//if (_pUart->usRxRead == usRxWrite)
	if (usCount == 0)	/* 已经没有数据 */
	{
		return 1;
	}
	else
	{
		*_pByte = _pUart->pRxBuf[_pUart->usRxRead];		/* 从串口接收FIFO取1个数据 */

		/* 改写FIFO读索引 */
		DISABLE_INT();
		if (++_pUart->usRxRead >= _pUart->usRxBufSize)
		{
			_pUart->usRxRead = 0;
		}
		_pUart->usRxCount--;
		ENABLE_INT();
		return 0;
	}
}

/*
*********************************************************************************************************
*	函 数 名: RS485_ReciveNew
*	功能说明: 接收到新的数据
*	形    参: _byte 接收到的新数据
*	返 回 值: 无
*********************************************************************************************************
*/
extern void MODS_ReciveNew(uint8_t _byte);
void RS485_ReciveNew(uint8_t _byte)
{
	MODS_ReciveNew(_byte);
}

/*
*********************************************************************************************************
*	函 数 名: RS485_SendBefor
*	功能说明: 发送数据前的准备工作。对于RS485通信，请设置RS485芯片为发送状态，
*			  并修改 UartVarInit()中的函数指针等于本函数名，比如 g_tUart2.SendBefor = RS485_SendBefor
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void RS485_SendBefor(void)
{
	u32 ret;
//	RS485_TX_EN();	/* 切换RS485收发芯片为发送模式 */
	ret = XGpio_DiscreteRead(&RS485_Gpio, 1);
	STB(ret, BIT32(0));
	XGpio_DiscreteWrite(&RS485_Gpio, 1, ret);
}

/*
*********************************************************************************************************
*	函 数 名: RS485_SendOver
*	功能说明: 发送一串数据结束后的善后处理。对于RS485通信，请设置RS485芯片为接收状态，
*			  并修改 UartVarInit()中的函数指针等于本函数名，比如 g_tUart2.SendOver = RS485_SendOver
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void RS485_SendOver(void)
{
	u32 ret;
//	RS485_RX_EN();	/* 切换RS485收发芯片为接收模式 */
	ret = XGpio_DiscreteRead(&RS485_Gpio, 1);
	CLB(ret, BIT32(0));
	XGpio_DiscreteWrite(&RS485_Gpio, 1, ret);
}

/*
*********************************************************************************************************
*	函 数 名: UartSend
*	功能说明: 填写数据到UART发送缓冲区,并启动发送中断。中断处理函数发送完毕后，自动关闭发送中断
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
static void UartSend(UART_T *_pUart, uint8_t *_ucaBuf, uint16_t _usLen)
{
	uint16_t i;
	u32 StatusRegister;

	for (i = 0; i < _usLen; i++)
	{
		/* 如果发送缓冲区已经满了，则等待缓冲区空 */
		while (1)
		{
			volatile uint16_t usCount;

			DISABLE_INT();
			usCount = _pUart->usTxCount;
			ENABLE_INT();
//			XUartLite_Send(_pUart->uart, &_pUart->pTxBuf[_pUart->usTxRead], 1);
			if (usCount < _pUart->usTxBufSize)
			{
				break;
			}
		}

		/* 将新数据填入发送缓冲区 */
		_pUart->pTxBuf[_pUart->usTxWrite] = _ucaBuf[i];

		DISABLE_INT();
		if (++_pUart->usTxWrite >= _pUart->usTxBufSize)
		{
			_pUart->usTxWrite = 0;
		}
		_pUart->usTxCount++;
		ENABLE_INT();
//		XUartLite_Send(_pUart->uart, &_pUart->pTxBuf[_pUart->usTxRead], 1);
//		if (++_pUart->usTxRead >= _pUart->usTxBufSize)
//		{
//			_pUart->usTxRead = 0;
//		}
//		_pUart->usTxCount--;
	}
	XUartLite_Send(_pUart->uart, &_pUart->pTxBuf[_pUart->usTxRead], 1);
	if (++_pUart->usTxRead >= _pUart->usTxBufSize)
	{
		_pUart->usTxRead = 0;
	}
	//_pUart->usTxCount--;
}


/*
*********************************************************************************************************
*	函 数 名: comSendBuf
*	功能说明: 向串口发送一组数据。数据放到发送缓冲区后立即返回，由中断服务程序在后台完成发送
*	形    参: _ucPort: 端口号(COM1 - COM6)
*			  _ucaBuf: 待发送的数据缓冲区
*			  _usLen : 数据长度
*	返 回 值: 无
*********************************************************************************************************
*/
void comSendBuf(uint8_t *_ucaBuf, uint16_t _usLen)
{
	UART_T *pUart;

	//pUart = ComToUart(_ucPort);
	pUart = &g_tUart0;
	if (pUart == 0)
	{
		return;
	}

	if (pUart->SendBefor != 0)
	{
		pUart->SendBefor();		/* 如果是RS485通信，可以在这个函数中将RS485设置为发送模式 */
	}

//	UartSend(pUart, _ucaBuf, _usLen);//有问题
	XUartLite_Send(pUart->uart, _ucaBuf, _usLen);
}

/*
*********************************************************************************************************
*	函 数 名: RS485_SendBuf
*	功能说明: 通过RS485芯片发送一串数据。注意，本函数不等待发送完毕。
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void RS485_SendBuf(uint8_t *_ucaBuf, uint16_t _usLen)
{
	comSendBuf(_ucaBuf, _usLen);
}


#endif // #if defined (XPAR_XUARTLITE_NUM_INSTANCES) && defined (INTC_DEVICE_ID) && defined (INTC)
#endif //defined (MODBUS_RTU_SLAVE)
