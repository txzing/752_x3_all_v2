#ifndef __UDP_UPDATE_H__

//#include "xparameters.h"
#if defined (XPAR_XEMACPS_NUM_INSTANCES) || defined (XPAR_XAXIETHERNET_NUM_INSTANCES)
#if defined (UDP_UPDATE)
#define __UDP_UPDATE_H__

#define UDP_UPDATE_SVR_SEND_MSG	(1U)		// 1 - enable, 0 - disable

#define UDP_UPDATE_SVR_PORT            (6789)

void udp_update_svr_send_msg(const char *msg);
void transfer_udp_update_data(void);
int start_udp_update_application(void);
void print_udp_update_header(void);

#endif // UDP_UPDATE
#endif // XPAR_XEMACPS_NUM_INSTANCES || XPAR_XAXIETHERNET_NUM_INSTANCES
#endif // __UDP_UPDATE_H__
