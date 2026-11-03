#ifndef __HOT_RESET_H__

#if defined (PLATFORM_ZYNQ) || defined (PLATFORM_ZYNQMP)
#define __HOT_RESET_H__

void PsSoftwareReset(void);
void getResetReason(void);
void pl_resetn(void);
void ModifyMultiBoot(void);

extern u8 reset_pl;

#endif // #if defined (PLATFORM_ZYNQ) || defined (PLATFORM_ZYNQMP)
#endif // #ifndef __HOT_RESET_H__
