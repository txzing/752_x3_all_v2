/*
 * xrotate_stream_accel.c — lightweight driver for rotate_stream_accel
 * Prefers xparameters.h base address when present.
 */
#include "xrotate_stream_accel.h"
#include "xil_io.h"

#ifndef XPAR_ROTATE_STREAM_ACCEL_0_S_AXI_CONTROL_BASEADDR
#define XPAR_ROTATE_STREAM_ACCEL_0_S_AXI_CONTROL_BASEADDR 0x800D0000U
#endif

int XRotate_stream_accel_Initialize(XRotate_stream_accel *InstancePtr, u16 DeviceId)
{
	(void)DeviceId;
	InstancePtr->ControlBaseAddress = XPAR_ROTATE_STREAM_ACCEL_0_S_AXI_CONTROL_BASEADDR;
	InstancePtr->IsReady = 1U;
	return XST_SUCCESS;
}

void XRotate_stream_accel_Start(XRotate_stream_accel *InstancePtr)
{
	u32 Data = Xil_In32(InstancePtr->ControlBaseAddress + XROTATE_STREAM_CTRL_OFFSET);
	Xil_Out32(InstancePtr->ControlBaseAddress + XROTATE_STREAM_CTRL_OFFSET,
		  Data | XROTATE_STREAM_CTRL_AP_START);
}

void XRotate_stream_accel_EnableAutoRestart(XRotate_stream_accel *InstancePtr)
{
	u32 Data = Xil_In32(InstancePtr->ControlBaseAddress + XROTATE_STREAM_CTRL_OFFSET);
	Xil_Out32(InstancePtr->ControlBaseAddress + XROTATE_STREAM_CTRL_OFFSET,
		  Data | XROTATE_STREAM_CTRL_AUTO_RESTART);
}

void XRotate_stream_accel_Set_height(XRotate_stream_accel *InstancePtr, u32 Data)
{
	Xil_Out32(InstancePtr->ControlBaseAddress + XROTATE_STREAM_HEIGHT_OFFSET, Data);
}

void XRotate_stream_accel_Set_width(XRotate_stream_accel *InstancePtr, u32 Data)
{
	Xil_Out32(InstancePtr->ControlBaseAddress + XROTATE_STREAM_WIDTH_OFFSET, Data);
}

void XRotate_stream_accel_Set_direction(XRotate_stream_accel *InstancePtr, u32 Data)
{
	Xil_Out32(InstancePtr->ControlBaseAddress + XROTATE_STREAM_DIRECTION_OFFSET, Data);
}

void XRotate_stream_accel_Set_fb_bytes(XRotate_stream_accel *InstancePtr, u32 Data)
{
	Xil_Out32(InstancePtr->ControlBaseAddress + XROTATE_STREAM_FB_BYTES_OFFSET, Data);
}

void XRotate_stream_accel_Set_fb_base(XRotate_stream_accel *InstancePtr, u64 Data)
{
	Xil_Out32(InstancePtr->ControlBaseAddress + XROTATE_STREAM_FB_BASE_OFFSET, (u32)Data);
	Xil_Out32(InstancePtr->ControlBaseAddress + XROTATE_STREAM_FB_BASE_OFFSET + 4,
		  (u32)(Data >> 32));
}
