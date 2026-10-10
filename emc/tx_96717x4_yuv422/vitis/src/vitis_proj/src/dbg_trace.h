#ifndef __DEBUG_TRACE_H__
#define __DEBUG_TRACE_H__

#define TRACE_ON 		(1)			// 是否打印的全局开关 0 or 1
#define FLOAT_ON		(0)			// 是否使能浮点数打印 0 or 1

/* ANSI Colors */
#define TXT_RED     "\x1b[31m"
#define TXT_GREEN   "\x1b[32m"
#define TXT_YELLOW  "\x1b[33m"
#define TXT_BLUE    "\x1b[34m"
#define TXT_MAGENTA "\x1b[35m"
#define TXT_CYAN    "\x1b[36m"
#define TXT_RST   "\x1b[0m"

#define bsp_printf(fmt, arg...) 			\
	do { 									\
		if (TRACE_ON) {						\
			if(FLOAT_ON)					\
				printf(fmt , ## arg); 	\
			else							\
				xil_printf(fmt , ## arg);		\
		}									\
	} while (0)

#define bsp_info(fmt, arg...)				\
	do { 									\
		if (TRACE_ON) {						\
			if(FLOAT_ON)					\
				printf("%s(%d): "fmt, __FILE__, __LINE__, ## arg); \
			else							\
				xil_printf("%s(%d): "fmt, __FILE__, __LINE__, ## arg); \
		}									\
	} while (0)

#define bsp_dbg(fmt, arg...)				\
	do { 									\
		if (TRACE_ON) {						\
			if(FLOAT_ON)					\
				printf("%s(%d)-<%s>: "fmt, __FILE__, __LINE__, __FUNCTION__, ## arg); \
			else							\
				xil_printf("%s(%d)-<%s>: "fmt, __FILE__, __LINE__, __FUNCTION__, ## arg); \
		}									\
	} while (0)

#endif // __DEBUG_TRACE_H__


/*
note:
find . -type f -name "*.[ch]" -o -name "*.cxx"  | xargs sed -i 's/xil_printf/bsp_printf/g'
find . ! \( -name "dbg_trace.h" -o -name "bsp.h" \) -a \( -name "*.[ch]" -o -name "*.cxx" \) | xargs sed -i 's/xil_printf/bsp_printf/g'

usage:
	bsp_printf("CSI Rx Ss 0 Init failed status = %x.\r\n", Status);
	bsp_printf(TXT_RED "CSI Rx Ss 0 Init failed status = %x.\r\n" TXT_RST, Status);
*/
