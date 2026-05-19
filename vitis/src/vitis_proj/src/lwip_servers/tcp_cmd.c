#include "../bsp.h"

#if defined (XPAR_XEMACPS_NUM_INSTANCES) || defined (XPAR_XAXIETHERNET_NUM_INSTANCES)
#if defined (TCP_COMMAND_SRV)


static struct tcp_pcb *client_pcb_cmd = NULL;   // 命令连接客户端 PCB
static struct tcp_pcb *client_pcb_video = NULL; // 视频连接客户端 PCB (if TCP_VIDEO)

struct tcp_pcb *tcp_cmd_get_client_pcb(void)
{
	return client_pcb_cmd;
}

void tcp_cmd_svr_send_msg(const char *msg)
{
	err_t err;
	size_t len;

	if (client_pcb_cmd == NULL || msg == NULL)
	{
		return;
	}
	len = strlen(msg);
	if (len == 0U)
	{
		return;
	}
	if (len > 1400U)
	{
		len = 1400U;
	}
	if ((u32)tcp_sndbuf(client_pcb_cmd) < len)
	{
		bsp_printf("tcp_cmd_svr_send_msg: sndbuf low\r\n");
		return;
	}
	err = tcp_write(client_pcb_cmd, msg, (u16_t)len, TCP_WRITE_FLAG_COPY);
	if (err != ERR_OK)
	{
		xil_printf("tcp_cmd_svr_send_msg: tcp_write %d\r\n", (int)err);
		return;
	}
	err = tcp_output(client_pcb_cmd);
	if (err != ERR_OK)
	{
		xil_printf("tcp_cmd_svr_send_msg: tcp_output %d\r\n", (int)err);
	}
}

#ifndef TCP_VIDEO_FLUSH_BATCH
#define TCP_VIDEO_FLUSH_BATCH 6U
#endif
#ifndef TCP_TCP_WRITE_MEM_RETRY_MAX
#define TCP_TCP_WRITE_MEM_RETRY_MAX 40U
#endif
#ifndef TCP_TCP_OUTPUT_MEM_RETRY_MAX
#define TCP_TCP_OUTPUT_MEM_RETRY_MAX 64U
#endif
#ifndef TCP_TCP_MEM_RETRY_DELAY_US
#define TCP_TCP_MEM_RETRY_DELAY_US 25U
#endif

static void dispatch_command(uint16_t cmd)
{
    switch (cmd)
    {
    case 0x10: msg_cmd_0x10(); break;
    case 0x11: msg_cmd_0x11(); break;
    case 0x12: msg_cmd_0x12(); break;
    case 0x13: msg_cmd_0x13(); break;
    case 0x20: msg_cmd_0x20(); break;
    case 0x21: msg_cmd_0x21(); break;
#if defined (XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES)
    case 0x30: msg_cmd_0x30(); break;
#endif
    case 0x40: msg_cmd_0x40(); break;
#if defined (XPAR_XV_TPG_NUM_INSTANCES)
    case 0x50: msg_cmd_0x50(); break;
#endif
    default: ack_fail_request(); break;
    }
}

static void send_tcp_response(struct tcp_pcb *pcb, const char *ctx)
{
    err_t err;
    if (sendlen < 8)
    {
        return;
    }
    if (tcp_sndbuf(pcb) >= sendlen)
    {
        err = tcp_write(pcb, send_buf, sendlen, TCP_WRITE_FLAG_COPY);
        if (err != ERR_OK)
        {
            xil_printf("%s: Error on tcp_write: %d\r\n", ctx, err);
        }
        err = tcp_output(pcb);
        if (err != ERR_OK)
        {
            xil_printf("%s: Error on tcp_output: %d\r\n", ctx, err);
        }
    }
    else
    {
        bsp_printf("%s no space in tcp_sndbuf\n\r", ctx);
    }
}

void print_tcp_cmd_header(void)
{
    bsp_printf("%20s %6d\r\n", "TCP CMD", TCP_CMD_SVR_PORT);
#if defined(TCP_VIDEO)
    bsp_printf("%20s %6d\r\n", "TCP VIDEO", TCP_VIDEO_SVR_PORT);
#endif
}

/** Close a tcp session */
static void tcp_server_close(struct tcp_pcb *pcb)
{
	err_t err;

	if (pcb != NULL) {
		tcp_recv(pcb, NULL);
		tcp_err(pcb, NULL);
		err = tcp_close(pcb);
		if (err != ERR_OK) {
			/* Free memory with abort */
			tcp_abort(pcb);
		}
	}
}

/* Forward declarations for accept callbacks */
static err_t accept_cmd_callback(void *arg, struct tcp_pcb *newpcb, err_t err);
#if defined(TCP_VIDEO)
static err_t accept_video_callback(void *arg, struct tcp_pcb *newpcb, err_t err);
#endif

static err_t tcp_cmd_recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
	/* do not read the packet if we are not in ESTABLISHED state */
    if (!p)
    {
    	tcp_close(tpcb);
    	tcp_recv(tpcb, NULL);
    	xil_printf("tcp connection closed\r\n");
    	return ERR_OK;
    }


	receivelen = p->tot_len;
	pbuf_copy_partial(p, receivebuf, p->tot_len, 0);
    pbuf_free(p);

    client_pcb_cmd  = tpcb;


//    xil_printf("receive data : %x\r\n",receivebuf[4]);
//    xil_printf("msg_cmd : %x\r\n",*(receivebuf+4));

    msg_len = 0;
    msg_cmd = 0;

    memcpy(&msg_len,receivebuf,2);
    memcpy(&msg_cmd,receivebuf+4,1);

//    xil_printf("msg_len : %d\r\n",msg_len);
//    xil_printf("msg_cmd : %x\r\n",msg_cmd);

    dispatch_command(msg_cmd);
    if (msg_cmd != 0x80u)
    {
    	send_tcp_response(client_pcb_cmd, "tcp_cmd_recv_callback");
    }

	return ERR_OK;
}

/** Error callback for command server, tcp session aborted */
static void tcp_cmd_server_err(void *arg, err_t err)
{
	LWIP_UNUSED_ARG(err);
	tcp_server_close(client_pcb_cmd);
	client_pcb_cmd = NULL;
	bsp_printf("tcp_cmd_ser: Command connection aborted\n\r");
}


/* Video receive callback (optional — here we just handle remote close) */
#if defined(TCP_VIDEO)
static err_t tcp_video_recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
    if (!p)
    {
    	tcp_close(tpcb);
    	tcp_recv(tpcb, NULL);
    	xil_printf("tcp video connection closed\r\n");
    	return ERR_OK;
    }

	receivelen = p->tot_len;
	pbuf_copy_partial(p, receivebuf, p->tot_len, 0);
    pbuf_free(p);

    client_pcb_video = tpcb;


//    xil_printf("receive data : %x\r\n",receivebuf[4]);
//    xil_printf("msg_cmd : %x\r\n",*(receivebuf+4));

    msg_len = 0;
    msg_cmd = 0;

    memcpy(&msg_len,receivebuf,2);
    memcpy(&msg_cmd,receivebuf+4,1);

//    xil_printf("msg_len : %d\r\n",msg_len);
//    xil_printf("msg_cmd : %x\r\n",msg_cmd);

    if(msg_cmd == 0x80u) // 与 UDP 一致：0x80 仅改状态，不在本链路自动回包
    {
    	msg_cmd_0x80();
    }
    else
    {
        send_tcp_response(client_pcb_video, "tcp_video_recv_callback");
    }

	return ERR_OK;

}

/** Error callback for video server */
static void tcp_video_server_err(void *arg, err_t err)
{
	LWIP_UNUSED_ARG(err);
	tcp_server_close(client_pcb_video);
	client_pcb_video = NULL;
	bsp_printf("tcp_video_ser: Video connection aborted\n\r");
}
#endif


/* accept callback for command server */
static err_t accept_cmd_callback(void *arg, struct tcp_pcb *newpcb, err_t err)
{
	if ((err != ERR_OK) || (newpcb == NULL))
	{
		return ERR_VAL;
	}

    bsp_printf("tcp_cmd_ser: Command Connection Accepted\r\n");
    client_pcb_cmd = newpcb;
    tcp_recv(client_pcb_cmd, tcp_cmd_recv_callback);
    tcp_arg(client_pcb_cmd, NULL);
    tcp_err(client_pcb_cmd, tcp_cmd_server_err);

    return ERR_OK;
}

#if defined(TCP_VIDEO)
/* accept callback for video server */
static err_t accept_video_callback(void *arg, struct tcp_pcb *newpcb, err_t err)
{
	if ((err != ERR_OK) || (newpcb == NULL))
	{
		return ERR_VAL;
	}

    bsp_printf("tcp_video_ser: Video Connection Accepted\r\n");
    client_pcb_video = newpcb;

    /* 如果需要在 video 链路上接收来自 PC 的消息，可设置 tcp_recv */
    tcp_recv(client_pcb_video, tcp_video_recv_callback);
    tcp_arg(client_pcb_video, NULL);
    tcp_err(client_pcb_video, tcp_video_server_err);

    return ERR_OK;
}
#endif

/*
 * Create new pcb, bind pcb and port, set call back function
 */
int start_tcp_cmd_application(void)
{
	struct tcp_pcb *pcb_cmd, *lpcb_cmd;
	err_t err;

	/* Create a new TCP PCB structure for command */
    pcb_cmd = tcp_new_ip_type(IPADDR_TYPE_ANY);
    if (!pcb_cmd) {
        bsp_printf("Error creating CMD PCB. Out of Memory\n\r");
        return -1;
    }

	/* Bind the command pcb to the TCP_CMD_SVR_PORT */
    err = tcp_bind(pcb_cmd, IP_ANY_TYPE, TCP_CMD_SVR_PORT);
    if (err != ERR_OK) {
        bsp_printf("Unable to bind to CMD port %d: err = %d\n\r", TCP_CMD_SVR_PORT, err);
        tcp_close(pcb_cmd);
        return -2;
    }

    lpcb_cmd = tcp_listen(pcb_cmd);
    if (!lpcb_cmd) {
        bsp_printf("Out of memory while tcp_listen (cmd)\n\r");
        return -3;
    }

    tcp_arg(lpcb_cmd, NULL);
    tcp_accept(lpcb_cmd, accept_cmd_callback);

#if defined (TCP_VIDEO)
    /* Create and bind video listener if TCP_VIDEO enabled */
    {
        struct tcp_pcb *pcb_vid, *lpcb_vid;
        pcb_vid = tcp_new_ip_type(IPADDR_TYPE_ANY);
        if (!pcb_vid) {
            bsp_printf("Error creating VIDEO PCB. Out of Memory\n\r");
            // don't return error; COMMAND server is already up
        } else {
            err = tcp_bind(pcb_vid, IP_ANY_TYPE, TCP_VIDEO_SVR_PORT);
            if (err != ERR_OK) {
                bsp_printf("Unable to bind to VIDEO port %d: err = %d\n\r", TCP_VIDEO_SVR_PORT, err);
                tcp_close(pcb_vid);
            } else {
                lpcb_vid = tcp_listen(pcb_vid);
                if (!lpcb_vid) {
                    bsp_printf("Out of memory while tcp_listen (video)\n\r");
                    tcp_close(pcb_vid);
                } else {
                    tcp_arg(lpcb_vid, NULL);
                    tcp_accept(lpcb_vid, accept_video_callback);
                }
            }
        }
    }
#endif

#if defined (TCP_VIDEO)
    for (int i = 0; i < ETH_VIDEO_NUM; i++) // 遍历所有通道
    {
    	VdmaChannels[i].send_pic_start = 0;
    	VdmaChannels[i].send_video_start = 0;
    	VdmaChannels[i].video_sending = 0;
    }
#endif

    return 0;
}

void transfer_tcp_cmd_data(void)
{
	return;
}

#if defined (TCP_VIDEO)

/** 在 tcp_write 已成功排队后，仅重试 tcp_output，避免上层重复 tcp_write 造成重复载荷 */
static err_t tcp_video_flush_output(void)
{
	unsigned n;
	err_t err;

	for (n = 0U; n < TCP_TCP_OUTPUT_MEM_RETRY_MAX; n++)
	{
		err = tcp_output(client_pcb_video);
		if (err == ERR_OK)
		{
			return ERR_OK;
		}
		if (err != ERR_MEM)
		{
			return err;
		}
		usleep(TCP_TCP_MEM_RETRY_DELAY_US);
	}
	return ERR_MEM;
}

/*
 * Send frame data with tcp (to video connection)
 *
 * @param pic is frame pointer will be send
 * @param piclen is frame length in one package
 *
 * Format uses global packet_p and HEADER_SIZE
 */
int sendpic_tcp(uint8_t *pic,  uint16_t piclen)
{
	static u8 pending_pkts = 0U;
	unsigned wr_try;
	err_t err;
	struct tcp_pcb *pcb;

	pcb = client_pcb_video;
	if (pcb == NULL)
	{
		pending_pkts = 0U;
	    for (int i = 0; i < ETH_VIDEO_NUM; i++) // 遍历所有通道
	    {
	    	VdmaChannels[i].send_pic_start = 0;
	    	VdmaChannels[i].send_video_start = 0;
	    	memset(VdmaChannels[i].send_err_start, 0, sizeof(VdmaChannels[i].send_err_start));
	    }
		xil_printf("Error on sendpic_tcp client_pcb_video == NULL\r\n");
		return -1;
	}

	memcpy(send_buf, &packet_p, HEADER_SIZE);
	memcpy(send_buf + HEADER_SIZE, pic, piclen);

	sendlen = HEADER_SIZE + piclen;

	if (tcp_sndbuf(pcb) < sendlen)
	{
		err = tcp_video_flush_output();
		if (err != ERR_OK)
		{
			xil_printf("sendpic_tcp: flush before write err %d\r\n", (int)err);
			return -3;
		}
		pending_pkts = 0U;
		if (tcp_sndbuf(pcb) < sendlen)
		{
			return TCP_SEND_TRY_LATER;
		}
	}

	for (wr_try = 0U; wr_try < TCP_TCP_WRITE_MEM_RETRY_MAX; wr_try++)
	{
		err = tcp_write(pcb, send_buf, sendlen,
		                (u8_t)(TCP_WRITE_FLAG_COPY | TCP_WRITE_FLAG_MORE));
		if (err == ERR_OK)
		{
			break;
		}
		if (err != ERR_MEM)
		{
			xil_printf("sendpic_tcp: tcp_write err %d\r\n", (int)err);
			return -2;
		}
		usleep(TCP_TCP_MEM_RETRY_DELAY_US);
	}
	if (err != ERR_OK)
	{
		return -2;
	}

	pending_pkts++;
	if (((u16)pending_pkts >= (u16)TCP_VIDEO_FLUSH_BATCH) || (tcp_sndbuf(pcb) < (int)sendlen))
	{
		err = tcp_video_flush_output();
		if (err != ERR_OK)
		{
			xil_printf("sendpic_tcp: tcp_output after write err %d\r\n", (int)err);
			return -3;
		}
		pending_pkts = 0U;
	}

	return 0;
}
#endif //#if defined (TCP_VIDEO)

#endif // TCP_COMMAND_SRV

#endif // XPAR_XEMACPS_NUM_INSTANCES || XPAR_XAXIETHERNET_NUM_INSTANCES
/*
usage:

call udp_server_setup() and platform_enable_interrupts() before the main_loop
call tcp_transfer_data() in the main_loop
run
```
    if (TcpFastTmrFlag) {
        tcp_fasttmr();
        TcpFastTmrFlag = 0;
    }
    if (TcpSlowTmrFlag) {
        tcp_slowtmr();
        TcpSlowTmrFlag = 0;
    }
```
in the main_loop, if tcp_transfer_data() not content it
keep a single `server_netif` definition, if you have more than one lwip process.

*/
