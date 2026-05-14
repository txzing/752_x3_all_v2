#ifndef __UDP_CMD_H__

//#include "xparameters.h"
#if defined (XPAR_XEMACPS_NUM_INSTANCES) || defined (XPAR_XAXIETHERNET_NUM_INSTANCES)
#if defined (UDP_COMMAND_SRV)
#define __UDP_CMD_H__

#define UDP_CMD_SVR_PORT            (5555)


void udp_cmd_svr_send_msg(const char *msg);
void transfer_udp_cmd_data(void);
int start_udp_cmd_application(void);
void print_udp_cmd_header(void);

#if defined (UDP_VIDEO)
int sendpic_udp(uint8_t *pic,  uint16_t piclen);
#endif

#endif // UDP_COMMAND_SRV
#endif // XPAR_XEMACPS_NUM_INSTANCES || XPAR_XAXIETHERNET_NUM_INSTANCES
#endif // __UDP_CMD_H__
