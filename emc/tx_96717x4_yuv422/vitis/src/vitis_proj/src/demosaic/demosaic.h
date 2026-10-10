#ifndef __DEMOSAIC_H__

//#include "xparameters.h"
#if defined (XPAR_XV_DEMOSAIC_NUM_INSTANCES)
#define __DEMOSAIC_H__
#include "xv_demosaic.h"

int demosaic_init(u16 deviceid,u16 bayer_phase,u32 Hsize,u32 Vsize);

#endif // XPAR_XV_DEMOSAIC_NUM_INSTANCES

#endif // __DEMOSAIC_H__


