#ifndef __TCP_UPDATE_H__

//#include "xparameters.h"
#if defined (XPAR_XEMACPS_NUM_INSTANCES) || defined (XPAR_XAXIETHERNET_NUM_INSTANCES)
#if defined (TCP_UPDATE)
#define __TCP_UPDATE_H__

#define TCP_UPDATE_SVR_SEND_MSG	(1U)		// 1 - enable, 0 - disable

#define TCP_UPDATE_SVR_PORT            (6789)

void tcp_update_svr_send_msg(const char *msg);
void transfer_tcp_update_data(void);
int start_tcp_update_application(void);
void print_tcp_update_header(void);

#endif // TCP_UPDATE
#endif // XPAR_XEMACPS_NUM_INSTANCES && TCP_UPDATE
#endif // __TCP_UPDATE_H__
