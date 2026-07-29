#include "../bsp.h"
#include "rotate_stream_cfg.h"

#if defined (XPAR_XROTATE_STREAM_ACCEL_NUM_INSTANCES)

#include "xrotate_stream_accel.h"

static XRotate_stream_accel RotateStreamInst;
static u8 RotateStreamConfigured;

int rotate_stream_init_once(void)
{
	u32 fb_bytes;
	int Status;

	if (RotateStreamConfigured) {
		return XST_SUCCESS;
	}

	Status = XRotate_stream_accel_Initialize(&RotateStreamInst, 0);
	if (Status != XST_SUCCESS) {
		xil_printf("rotate_stream init failed: %d\r\n", Status);
		return Status;
	}

	fb_bytes = (u32)ROTATE_STREAM_IN_WIDTH * (u32)ROTATE_STREAM_IN_HEIGHT * 3u;

	XRotate_stream_accel_Set_height(&RotateStreamInst, ROTATE_STREAM_IN_HEIGHT);
	XRotate_stream_accel_Set_width(&RotateStreamInst, ROTATE_STREAM_IN_WIDTH);
	XRotate_stream_accel_Set_direction(&RotateStreamInst, ROTATE_STREAM_DIRECTION);
	XRotate_stream_accel_Set_fb_bytes(&RotateStreamInst, fb_bytes);
	XRotate_stream_accel_Set_fb_base(&RotateStreamInst, (u64)ROTATE_STREAM_FB_BASE);

	XRotate_stream_accel_EnableAutoRestart(&RotateStreamInst);
	XRotate_stream_accel_Start(&RotateStreamInst);

	RotateStreamConfigured = 1;
	xil_printf("rotate_stream armed once: %ux%u dir=%u fb=0x%08x\r\n",
		   ROTATE_STREAM_IN_WIDTH, ROTATE_STREAM_IN_HEIGHT,
		   ROTATE_STREAM_DIRECTION, (u32)ROTATE_STREAM_FB_BASE);
	return XST_SUCCESS;
}

#else

int rotate_stream_init_once(void)
{
	return XST_SUCCESS;
}

#endif
