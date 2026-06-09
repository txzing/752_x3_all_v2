/*******************************************************************************
* Copyright (C) 2018 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
*******************************************************************************/

/*****************************************************************************/
/**
*
* @file dppt.h
*
* This file contains functions to configure Video Pattern Generator core.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who    Date     Changes
* ----- ------ -------- --------------------------------------------------
* 1.00  KI    07/13/17 Initial release.
* </pre>
*
******************************************************************************/

#ifndef DP_APP_H_

#if defined (XPAR_XDPRXSS_NUM_INSTANCES) || defined (XPAR_XDPTXSS_NUM_INSTANCES)
#define DP_APP_H_

#if defined (XPAR_XDPTXSS_NUM_INSTANCES)
#include "xdptxss.h"
#endif

#if defined (XPAR_XDPRXSS_NUM_INSTANCES)
#include "xdprxss.h"
#endif


#include "xdprxss_mcdp6000_flipping.h"
#include "xdp.h"
#include "stdlib.h"
#include "microblaze_sleep.h"
#include "dppt_vid_phy_config.h"
#include "edid_info.h"
#include "video_timing_table.h"

/* ************************************************************************** */

int DPPtIntrInitialize();
int DpPt_SetupIntrSystem();
void DpPt_HpdEventHandler(void *InstancePtr);
void DpPt_HpdPulseHandler(void *InstancePtr);
void DpPt_LinkrateChgHandler (void *InstancePtr);
void Dprx_InterruptHandlerPllReset(void *InstancePtr);
void Dprx_InterruptHandlerLinkBW(void *InstancePtr);
void Dprx_InterruptHandlerTrainingDone(void *InstancePtr);
void Dprx_InterruptHandlerBwChange(void *InstancePtr);
void Dprx_InterruptHandlerInfoPkt(void *InstancePtr);
void Dprx_InterruptHandlerExtPkt(void *InstancePtr);
void Dprx_InterruptHandlerUplug(void *InstancePtr);
void Dprx_InterruptHandlerPwr(void *InstancePtr);

void DpRxSs_AccessLaneSetHandler(void *InstancePtr);
void DpRxSs_AccessLinkQualHandler(void *InstancePtr);
void DpRxSs_AccessErrorCounterHandler(void *InstancePtr);
void DpRxSs_CRCTestEventHandler(void *InstancePtr);
void DpRxSs_InfoPacketHandler(void *InstancePtr);
void DpRxSs_ExtPacketHandler(void *InstancePtr);

void Dprx_HdcpAuthCallback(void *InstancePtr);
void Dprx_HdcpUnAuthCallback(void *InstancePtr);
void DpPt_CustomWaitUs(void *InstancePtr, u32 MicroSeconds);
int DPRxInitialize();
int dp_init_peripherals(void);

/* Introduced to address reduced blanking 
 * linereset issue from 16.4 release
 * */


void video_change_detect(u32 *count_track, u32 *rxMsamisc0_track,
		u32 *bpc_track, u32 *recv_clk_freq_track, float *recv_frame_clk_track,
		u32 *recv_frame_clk_int_track, int *track_count, void *InstancePtr);
void detect_rx_video(int *track_count1, void *InstancePtr);
void sink_power_cycle(u32 power_down_time);
void Print_InfoPkt();
void Print_ExtPkt();

void dp_init(void);

#if XPAR_XVPHY_NUM_INSTANCES >= 1
extern XVphy rx_VPhy_Instance_0;
extern XDpRxSs DpRxSsInst_0;		/* The DPRX Subsystem instance.*/
#endif
#if XPAR_XVPHY_NUM_INSTANCES >= 2
extern XVphy rx_VPhy_Instance_1;
extern XDpRxSs DpRxSsInst_1;		/* The DPRX Subsystem instance.*/
#endif

#endif

#endif /* DPAPP_H_ */
