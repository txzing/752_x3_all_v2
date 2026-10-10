#ifndef __TPG_H__

#if defined (XPAR_XV_TPG_NUM_INSTANCES)
#define __TPG_H__
#include "xv_tpg.h"
#include "xvidc.h"
#include "xil_types.h"

#if (XPAR_XV_TPG_NUM_INSTANCES >= 1U)
extern XV_tpg tpg_inst0;
#endif
#if (XPAR_XV_TPG_NUM_INSTANCES >= 2U)
extern XV_tpg tpg_inst1;
#endif

/* ---- Boot: init driver + load pattern shadow (EEPROM or factory) ---- */
int tpg_config(void);
int tpg_rst_gpio_init(void);
/* Pulse shared pipe reset (both TPG ap_rst_n via rst_gpio). */
void tpg_pipe_reset_pulse(void);

/* ---- Runtime: geometry change reapplies pattern shadow ---- */
int tpg_apply_geometry(u8 ch, u32 height, u32 width);
XV_tpg *tpg_get_instance(u8 src);

/* ---- Host 0x50: pattern control (runtime only, not EEPROM) ---- */
int tpg_set_bckgnd(u8 ch, u32 bckgndId);
int tpg_set_box_motion(u8 ch, u8 mode, u8 motion_speed);
int tpg_set_box_color(u8 ch, u8 r, u8 g, u8 b);

/* ---- EEPROM VTP1 @0xB0: load at boot, save on 0x51 sub=4 ---- */
void tpg_pat_reset_defaults(void);
int tpg_nv_save(void);
u8 tpg_nv_is_dirty(void);

#endif /* XPAR_XV_TPG_NUM_INSTANCES */
#endif /* __TPG_H__ */
