#ifndef __TPG_H__

//#include "xparameters.h"
#if defined (XPAR_XV_TPG_NUM_INSTANCES)
#define __TPG_H__
#include "xv_tpg.h"
#include "xvidc.h"


#if (XPAR_XV_TPG_NUM_INSTANCES >= 1U)
extern XV_tpg tpg_inst0;
#endif // XPAR_XV_TPG_NUM_INSTANCES == 1U
#if (XPAR_XV_TPG_NUM_INSTANCES >= 2U)
extern XV_tpg tpg_inst1;
#endif // XPAR_XV_TPG_NUM_INSTANCES == 2U
#if (XPAR_XV_TPG_NUM_INSTANCES >= 3U)
extern XV_tpg tpg_inst2;
#endif // XPAR_XV_TPG_NUM_INSTANCES == 3U
#if (XPAR_XV_TPG_NUM_INSTANCES >= 4U)
extern XV_tpg tpg_inst3;
#endif // XPAR_XV_TPG_NUM_INSTANCES == 4U

void tpg_cfg(XV_tpg *InstancePtr, u32 height, u32 width, u32 colorFormat, u32 bckgndId);
void tpg_box(XV_tpg *InstancePtr, u32 boxSize, u32 motionSpeed);
void tpg_box_static(XV_tpg *InstancePtr, u32 boxSize);
void tpg_box_disable(XV_tpg *InstancePtr);
int tpg_config(void);
int smart_tpg_config(u8 ch, u32 height, u32 width, u32 colorFormat, u32 bckgndId);
XV_tpg *tpg_get_instance(u8 ch);
int tpg_set_bckgnd(u8 ch, u32 bckgndId);
int tpg_set_box_motion(u8 ch, u8 mode, u8 motion_speed);
int tpg_set_box_color(u8 ch, u8 r, u8 g, u8 b);

#endif // XPAR_XV_TPG_NUM_INSTANCES

#endif // __TPG_H__

