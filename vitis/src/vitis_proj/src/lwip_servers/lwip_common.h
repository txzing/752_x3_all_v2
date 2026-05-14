#ifndef __LWIP_COMMON_H__

//#include "xparameters.h"
#if defined (XPAR_XEMACPS_NUM_INSTANCES) || defined (XPAR_XAXIETHERNET_NUM_INSTANCES)
#if defined (UDP_UPDATE) || defined (TCP_UPDATE) || defined (TCP_COMMAND_SRV) || defined (UDP_COMMAND_SRV)
#define __LWIP_COMMON_H__
#include "lwipopts.h"
#include "lwip/priv/tcp_priv.h"
#include "lwip/init.h"
#include "lwip/inet.h"
#include "lwip/err.h"
#include "netif/xadapter.h"
#include "lwip/udp.h"
#include "lwip/tcp.h"
#include "settings.h"

#if LWIP_IPV6==1
	#include "lwip/ip6_addr.h"
	#include "lwip/ip6.h"
#else
	#if LWIP_DHCP==1
		#include "lwip/dhcp.h"
		extern volatile int dhcp_timoutcntr;
		err_t dhcp_start(struct netif *netif);
	#endif // LWIP_DHCP
	#define DEFAULT_IP_ADDRESS  "192.168.1.10"
	#define DEFAULT_IP_MASK     "255.255.255.0"
	#define DEFAULT_GW_ADDRESS  "192.168.1.1"
#endif // LWIP_IPV6

//#define SEND_MSG	(0U)		// 1 - enable, 0 - disable

extern volatile int TcpFastTmrFlag;
extern volatile int TcpSlowTmrFlag;

#if defined (XPAR_XEMACPS_NUM_INSTANCES)
#define PLATFORM_EMAC_BASEADDR XPAR_XEMACPS_0_BASEADDR
#endif // XPAR_XEMACPS_NUM_INSTANCES
#if defined (XPAR_XAXIETHERNET_NUM_INSTANCES)
#define PLATFORM_EMAC_BASEADDR XPAR_AXIETHERNET_0_BASEADDR
#endif // XPAR_XAXIETHERNET_NUM_INSTANCES

//#define MAX_FLASH_LEN   32*1024*1024
#if defined (UDP_UPDATE) || defined (TCP_UPDATE)
extern u8 rxbuffer[MAX_FLASH_LEN];
extern u32 total_bytes;
extern config_Settings_t config;
#endif // #if defined (UDP_UPDATE) || defined (TCP_UPDATE)


#if defined (UDP_COMMAND_SRV) || defined (TCP_COMMAND_SRV)
extern uint8_t receivebuf[1500];
extern uint8_t send_buf[1500];
extern int receivelen;
extern int sendlen;
extern uint16_t msg_len;
extern uint16_t msg_cmd;
extern u8 current_ch;
#endif

extern struct netif server_netif;
extern u8 reset_pl;

#if LWIP_IPV6==1
void print_ip6(char *msg, ip_addr_t *ip);
#else
void print_ip(char *msg, ip_addr_t *ip);
void print_ip_settings(ip_addr_t *ip, ip_addr_t *mask, ip_addr_t *gw);
#endif

void send_msg(const char *msg);
void process_print(u8 percent);
int lwip_common_init(struct netif *netif);
void transfer_data(struct netif *netif);

#if defined (UDP_COMMAND_SRV) || defined (TCP_COMMAND_SRV)
void msg_cmd_0x10(void);
void msg_cmd_0x11(void);
void msg_cmd_0x12(void);
void msg_cmd_0x13(void);
void msg_cmd_0x20(void);
void msg_cmd_0x21(void);
void msg_cmd_0x30(void);
void msg_cmd_0x40(void);
void msg_cmd_0x80(void);
#if defined (XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES)
void err_auto_send_func(u8 ch);
#endif
#endif


#endif // #if defined (UDP_UPDATE) || defined (TCP_UPDATE) || defined (TCP_COMMAND_SRV) || defined (UDP_COMMAND_SRV)
#endif // XPAR_XEMACPS_NUM_INSTANCES || XPAR_XAXIETHERNET_NUM_INSTANCES
#endif // #ifndef __LWIP_COMMON_H__
