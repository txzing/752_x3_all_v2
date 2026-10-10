#ifndef __AXI_UARTLITE_FIFO_H_
#if defined (MODBUS_RTU_SLAVE)
#if defined (XPAR_XUARTLITE_NUM_INSTANCES) && defined (INTC_DEVICE_ID) && defined (INTC)
#define __AXI_UARTLITE_FIFO_H_

#include "xuartlite.h"

typedef struct
{
		INTC *Intc;
		XUartLite *uart;				/* 串口设备指针 */
		uint8_t *pTxBuf;			/* 发送缓冲区 */
		uint8_t *pRxBuf;			/* 接收缓冲区 */
		uint16_t usTxBufSize;		/* 发送缓冲区大小 */
		uint16_t usRxBufSize;		/* 接收缓冲区大小 */
		volatile uint16_t usTxWrite;	/* 发送缓冲区写指针 */
		volatile uint16_t usTxRead;		/* 发送缓冲区读指针 */
		volatile uint16_t usTxCount;	/* 等待发送的数据个数 */

		volatile uint16_t usRxWrite;	/* 接收缓冲区写指针 */
		volatile uint16_t usRxRead;		/* 接收缓冲区读指针 */
		volatile uint16_t usRxCount;	/* 还未读取的新数据个数 */

		void (*SendBefor)(void); 	/* 开始发送之前的回调函数指针（主要用于RS485切换到发送模式） */
		void (*SendOver)(void); 	/* 发送完毕的回调函数指针（主要用于RS485将发送模式切换为接收模式） */
		void (*ReciveNew)(uint8_t _byte);	/* 串口收到数据的回调函数指针 */
		uint8_t Sending;			/* 正在发送中 */
}UART_T;


#define UART_DEVICE_ID      XPAR_PROCESSOR_SUBSYSTEM_RS485_HIER_AXI_UARTLITE_0_DEVICE_ID
#define UART_IRPT_INTR      XPAR_FABRIC_UARTLITE_0_VEC_ID   // 中断ID

#define UART0_BAUD			115200
#define UART0_TX_BUF_SIZE	1200
#define UART0_RX_BUF_SIZE	1200
#define RXFIFO_LEVEL 1

/*
 * 定义最大的rxfifo触发中断阈值。
 */
#define RX_DATA_MAX 1   //1-63


/* 开关全局中断的宏 */
//#define ENABLE_INT()	asm(" msr  DAIFCLR, #2")		/* 使能全局中断 */
//#define DISABLE_INT()	asm(" msr  DAIFSET, #2")	/* 使能全局中断 */
//#define ENABLE_INT() microblaze_enable_interrupts()
//#define DISABLE_INT() microblaze_disable_interrupts()
#if defined (ARMR5) || (__aarch64__) || (__arm__)
	#define ENABLE_INT()   Xil_ExceptionEnable()
	#define DISABLE_INT()  Xil_ExceptionDisable()
#else
	#define ENABLE_INT() microblaze_enable_interrupts()
	#define DISABLE_INT() microblaze_disable_interrupts()
#endif


uint8_t UartGetChar(UART_T *_pUart, uint8_t *_pByte);
void Uart_Var_Init(void);
int Uart_Init_Rs485(INTC *IntcInstancePtr, u16 DeviceId);
void RS485_SendBuf(uint8_t *_ucaBuf, uint16_t _usLen);
void RS485_SendBefor(void);
void RS485_SendOver(void);
void RS485_ReciveNew(uint8_t _byte);

#endif //defined (XPAR_XUARTLITE_NUM_INSTANCES) && defined (INTC_DEVICE_ID) && defined (INTC)
#endif //defined (MODBUS_RTU_SLAVE)
#endif //__AXI_UARTLITE_FIFO_H_
