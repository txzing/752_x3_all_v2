#include "../bsp.h"

#if defined (XPAR_XEMACPS_NUM_INSTANCES) || defined (XPAR_XAXIETHERNET_NUM_INSTANCES)
#if defined (UDP_UPDATE)

static struct udp_pcb *client_pcb = NULL;

static int start_update_flag = 0 ;
static int start_update_type = 0 ;
static uint8_t md5sum[16];
static uint32_t tsize;

static void reset_update_state(void)
{
	memset(md5sum, 0, 16);
	tsize = 0;
	total_bytes = 0;
}

void print_udp_update_header(void)
{
    bsp_printf("%20s %6d\r\n", "UDP Update", UDP_UPDATE_SVR_PORT);
}

void udp_update_svr_send_msg(const char *msg)
{
#if (UDP_UPDATE_SVR_SEND_MSG == 1U)
	if(client_pcb != NULL)
	{
		static struct pbuf *pbuf2sent;

		pbuf2sent = pbuf_alloc(PBUF_TRANSPORT, strlen(msg), PBUF_POOL);
		if (!pbuf2sent)
			bsp_printf("Error allocating pbuf\r\n");

		memcpy(pbuf2sent->payload, msg, strlen(msg));

		if (udp_send(client_pcb, pbuf2sent) != ERR_OK)
			bsp_printf("UDP send error\r\n");

		pbuf_free(pbuf2sent);
	}
#endif
}

void udp_update_process_print(u8 percent)
{
	char msg[16];
	if (percent <= 9U)
	{
		sprintf(msg, "%u0%%..", percent);
		udp_update_svr_send_msg(msg);
		bsp_printf("%u0%%..", percent);
	}
	else if (percent == 10U)
	{
		udp_update_svr_send_msg("100%\r\n");
		bsp_printf("100%%\r\n");
	}
}


void udp_update_recv_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p,
		const ip_addr_t *ip, u16_t port)
{
    struct pbuf *q;
    q = p;
    upcb->remote_ip = *ip;
    upcb->remote_port = port;
    client_pcb = upcb;

    if (!(memcmp("md5sum", p->payload, 6)))
    {
    	memcpy(md5sum, p->payload + 6, 16);
    	memcpy(&tsize, p->payload + 6 + 16, 4);
    	NOP();
	}
    else if(q->tot_len == 6 && !(memcmp("update", p->payload, 6)))
    {
		uint8_t cacl_md5sum[16];
		md5_sum(rxbuffer, total_bytes, cacl_md5sum);
		if(memcmp(md5sum, cacl_md5sum, 16)==0)
		{
			start_update_type = 0;
			start_update_flag = 1;
			udp_update_svr_send_msg("Start QSPI Update\r\n");
		}
		else
		{
			reset_update_state();
			udp_update_svr_send_msg("md5sum != cacl_md5sum\r\n");
		}
        NOP();
    }
    else if(q->tot_len == 6 && !(memcmp("golden", p->payload, 6)))
    {
		uint8_t cacl_md5sum[16];
		md5_sum(rxbuffer, total_bytes, cacl_md5sum);
		if(memcmp(md5sum, cacl_md5sum, 16)==0)
		{
			start_update_flag = 1;
			start_update_type = 1;
			udp_update_svr_send_msg("Start QSPI Update\r\n");
		}
		else
		{
			reset_update_state();
			udp_update_svr_send_msg("md5sum != cacl_md5sum\r\n");
		}
		NOP();
	}
    else if(q->tot_len == 5 && !(memcmp("clear", p->payload, 5)))
    {
        start_update_flag = 0;
        total_bytes = 0;
        udp_update_svr_send_msg("Clear received data\r\n");
        bsp_printf("Clear received data\r\n");
    }
    else
    {
        while(q->tot_len != q->len)
        {
            memcpy(&rxbuffer[total_bytes], q->payload, q->len);
            total_bytes += q->len;
            q = q->next;
        }
        memcpy(&rxbuffer[total_bytes], q->payload, q->len);
        total_bytes += q->len;
    }

    pbuf_free(p);
}
/*
 * Create new pcb, bind pcb and port, set call back function
 */
int start_udp_update_application(void)
{
	struct udp_pcb *pcb;
	err_t err;

	pcb = udp_new();
	if (!pcb)
	{
		bsp_printf("Error creating PCB. Out of Memory\n\r");
		return -1;
	}
	/* bind to specified @port */
	err = udp_bind(pcb, IP_ADDR_ANY, UDP_UPDATE_SVR_PORT);
	if (err != ERR_OK)
	{
		bsp_printf("Unable to bind to port %d: err = %d\n\r", UDP_UPDATE_SVR_PORT, err);
		udp_remove(pcb);
		return -2;
	}
	udp_recv(pcb, udp_update_recv_callback, NULL);
//	IP4_ADDR(&target_addr, 192,168,1,35);
//	bsp_printf("UDP server started @ port %d\n\r", UDP_UPDATE_SVR_PORT);

	return 0;
}

void transfer_udp_update_data(void)
{
    char msg[60];
	if (start_update_flag)
	{
        bsp_printf("Start QSPI Update!\r\n");
        bsp_printf("tsize = %d\r\n", tsize);
        bsp_printf("file size of BOOT.bin is %lu Bytes\r\n", total_bytes);
        sprintf(msg, "file size of BOOT.bin is %lu Bytes\r\n", total_bytes);
        udp_update_svr_send_msg(msg);
        if (qspi_update(total_bytes, rxbuffer, start_update_type) != XST_SUCCESS)
        {
        	udp_update_svr_send_msg("Update Qspi Error!\r\n");
            bsp_printf("Update Qspi Error!\r\n");
        }
        else
        {
            total_bytes = 0;
        }
    	memset(md5sum, 0, 16);
    	tsize = 0;
    	client_pcb = NULL;
	}
    start_update_flag = 0;
}


#endif // UDP_UPDATE


#endif // XPAR_XEMACPS_NUM_INSTANCES || XPAR_XAXIETHERNET_NUM_INSTANCES

/*
usage:

call udp_server_setup() and platform_enable_interrupts() before the main_loop
call udp_transfer_data() in the main_loop
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
