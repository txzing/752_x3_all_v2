#ifndef __ROTATE_STREAM_CFG_H__
#define __ROTATE_STREAM_CFG_H__

#include "xil_types.h"
#include "xstatus.h"

/*
 * Stream rotate: configure once, enable auto-restart; PL keeps rotating
 * each AXIS frame without further CPU kicks.
 */
#ifndef ROTATE_STREAM_DIRECTION
#define ROTATE_STREAM_DIRECTION 0
#endif
#ifndef ROTATE_STREAM_IN_WIDTH
#define ROTATE_STREAM_IN_WIDTH 1920
#endif
#ifndef ROTATE_STREAM_IN_HEIGHT
#define ROTATE_STREAM_IN_HEIGHT 3840
#endif
#ifndef ROTATE_STREAM_FB_BASE
#define ROTATE_STREAM_FB_BASE (XPAR_DDR_MEM_BASEADDR + 0x20000000U + 0x2000000U)
#endif

int rotate_stream_init_once(void);

#endif /* __ROTATE_STREAM_CFG_H__ */
