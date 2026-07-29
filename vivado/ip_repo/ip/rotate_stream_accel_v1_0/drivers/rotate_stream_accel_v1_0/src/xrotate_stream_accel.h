#ifndef XROTATE_STREAM_ACCEL_H
#define XROTATE_STREAM_ACCEL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "xil_types.h"
#include "xstatus.h"

/* Register offsets (aligned with HLS s_axilite layout intent) */
#define XROTATE_STREAM_CTRL_OFFSET      0x00
#define XROTATE_STREAM_GIER_OFFSET      0x04
#define XROTATE_STREAM_HEIGHT_OFFSET    0x10
#define XROTATE_STREAM_WIDTH_OFFSET     0x18
#define XROTATE_STREAM_DIRECTION_OFFSET 0x20
#define XROTATE_STREAM_FB_BYTES_OFFSET  0x28
#define XROTATE_STREAM_FB_BASE_OFFSET   0x30

#define XROTATE_STREAM_CTRL_AP_START    (1u << 0)
#define XROTATE_STREAM_CTRL_AP_DONE     (1u << 1)
#define XROTATE_STREAM_CTRL_AP_IDLE     (1u << 2)
#define XROTATE_STREAM_CTRL_AP_READY    (1u << 3)
#define XROTATE_STREAM_CTRL_AUTO_RESTART (1u << 7)

typedef struct {
	UINTPTR ControlBaseAddress;
	u32 IsReady;
} XRotate_stream_accel;

int XRotate_stream_accel_Initialize(XRotate_stream_accel *InstancePtr, u16 DeviceId);
void XRotate_stream_accel_Start(XRotate_stream_accel *InstancePtr);
void XRotate_stream_accel_EnableAutoRestart(XRotate_stream_accel *InstancePtr);
void XRotate_stream_accel_Set_height(XRotate_stream_accel *InstancePtr, u32 Data);
void XRotate_stream_accel_Set_width(XRotate_stream_accel *InstancePtr, u32 Data);
void XRotate_stream_accel_Set_direction(XRotate_stream_accel *InstancePtr, u32 Data);
void XRotate_stream_accel_Set_fb_bytes(XRotate_stream_accel *InstancePtr, u32 Data);
void XRotate_stream_accel_Set_fb_base(XRotate_stream_accel *InstancePtr, u64 Data);

#ifdef __cplusplus
}
#endif
#endif
