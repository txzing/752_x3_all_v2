/*******************************************************************************
* Copyright (C) 2018 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
*******************************************************************************/

/*****************************************************************************/
/**
*
* @file xedid_print_example.h
*
* This file contains functions to configure Video Pattern Generator core.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who    Date     Changes
* ----- ------ -------- --------------------------------------------------
* 1.00  YB    07/01/15 Initial release.
* </pre>
*
******************************************************************************/

#ifndef EDID_INFO_H_
#define EDID_INFO_H_

#include "xdp.h"
#include "xvidc_edid.h"
#include "string.h"
#include "xil_printf.h"
#include "xstatus.h"
#include "xparameters.h"
#include "xuartlite_l.h"





u32 Edid_PrintDecodeBase(u8 *EdidRaw);
void Edid_Print_Supported_VideoModeTable(u8 *EdidRaw);
void XDptx_DbgPrintEdid(XDp *InstancePtr);

void edid_default(u32 baseaddr);
void edid_change(int page, u32 baseaddr);
void edid_monitor_songren(u32 baseaddr);

#endif /* XEDID_PRINT_EXAMPLE_H_ */
