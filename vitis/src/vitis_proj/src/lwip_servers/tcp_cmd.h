#ifndef __TCP_CMD_H__

//#include "xparameters.h"
#if defined (XPAR_XEMACPS_NUM_INSTANCES) || defined (XPAR_XAXIETHERNET_NUM_INSTANCES)
#if defined (TCP_COMMAND_SRV)
#define __TCP_CMD_H__

#define TCP_CMD_SVR_PORT              (5555)
#define TCP_VIDEO_SVR_PORT            (7777)

struct tcp_pcb;
struct tcp_pcb *tcp_cmd_get_client_pcb(void);

void tcp_cmd_svr_send_msg(const char *msg);
void transfer_tcp_cmd_data(void);
int start_tcp_cmd_application(void);
void print_tcp_cmd_header(void);

#if defined (TCP_VIDEO)
/*
 * sendpic_tcp 返回值（与 lwip_video 中 video_link_send 配合）:
 *   0  = 成功（数据已入发送队列，必要时已 tcp_output）
 *  -1 = 无视频 TCP 连接 (client_pcb_video == NULL)
 *  -2 = tcp_write 失败（非 ERR_MEM，不可恢复）
 *  -3 = tcp_output 在多次重试后仍失败
 *  TCP_SEND_TRY_LATER = 发送窗口不足，未调用 tcp_write；上层应保留 pkg_cnt 下周期重试
 */
#define TCP_SEND_TRY_LATER      (-4)
int sendpic_tcp(uint8_t *pic,  uint16_t piclen);
#endif

#endif // TCP_COMMAND_SRV
#endif // XPAR_XEMACPS_NUM_INSTANCES || XPAR_XAXIETHERNET_NUM_INSTANCES
#endif // __TCP_CMD_H__
