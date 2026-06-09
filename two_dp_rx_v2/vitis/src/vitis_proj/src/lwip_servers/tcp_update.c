#include "../bsp.h"

#if defined (XPAR_XEMACPS_NUM_INSTANCES) || defined (XPAR_XAXIETHERNET_NUM_INSTANCES)
#if defined (TCP_UPDATE)

static struct tcp_pcb *client_pcb = NULL;

//static u8 rxbuffer[MAX_FLASH_LEN];
//static u32 total_bytes = 0;
static int start_update_flag = 0 ;
static int start_update_type = 0 ;
static uint8_t md5sum[16];
static uint32_t tsize;
//static int send_msg_enabled = 0 ;

void print_tcp_update_header(void)
{
    bsp_printf("%20s %6d\r\n", "TCP Update", TCP_UPDATE_SVR_PORT);
}


void tcp_update_svr_send_msg(const char *msg)
{
#if (TCP_UPDATE_SVR_SEND_MSG == 1U)
	if(client_pcb != NULL)
	{
		err_t err;
		tcp_nagle_disable(client_pcb);
	    if (tcp_sndbuf(client_pcb) > strlen(msg))
	    {
	        err = tcp_write(client_pcb, msg, strlen(msg), TCP_WRITE_FLAG_COPY);
	        if (err != ERR_OK)
	            xil_printf("tcp_server: Error on tcp_write: %d\r\n", err);
	        err = tcp_output(client_pcb);
	        if (err != ERR_OK)
	            xil_printf("tcp_server: Error on tcp_output: %d\r\n", err);
	    }
	    else
	        xil_printf("no space in tcp_sndbuf\r\n");
	}
#endif
}

void tcp_update_process_print(u8 percent)
{
    switch (percent)
    {
		case 0:
			tcp_update_svr_send_msg("0%..");
			bsp_printf("0%%..");
			break;
		case 1:
			tcp_update_svr_send_msg("10%..");
			bsp_printf("10%%..");
			break;
		case 2:
			tcp_update_svr_send_msg("20%..");
			bsp_printf("20%%..");
			break;
		case 3:
			tcp_update_svr_send_msg("30%..");
			bsp_printf("30%%..");
			break;
		case 4:
			tcp_update_svr_send_msg("40%..");
			bsp_printf("40%%..");
			break;
		case 5:
			tcp_update_svr_send_msg("50%..");
			bsp_printf("50%%..");
			break;
		case 6:
			tcp_update_svr_send_msg("60%..");
			bsp_printf("60%%..");
			break;
		case 7:
			tcp_update_svr_send_msg("70%..");
			bsp_printf("70%%..");
			break;
		case 8:
			tcp_update_svr_send_msg("80%..");
			bsp_printf("80%%..");
			break;
		case 9:
			tcp_update_svr_send_msg("90%..");
			bsp_printf("90%%..");
			break;
		case 10:
			tcp_update_svr_send_msg("100%\r\n");
			bsp_printf("100%%\r\n");
		default:
			break;
    }
}

/** Close a tcp session */
static void tcp_server_close(struct tcp_pcb *pcb)
{
	err_t err;

	if (pcb != NULL)
	{
		tcp_recv(pcb, NULL);
		tcp_err(pcb, NULL);
		err = tcp_close(pcb);
		if (err != ERR_OK)
		{
			/* Free memory with abort */
			tcp_abort(pcb);
		}
	}
}

static err_t tcp_update_recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
    struct pbuf *q;

    if (!p)
    {
    	tcp_close(tpcb);
    	tcp_recv(tpcb, NULL);
    	xil_printf("tcp connection closed\r\n");
    	return ERR_OK;
    }

    q = p;

    if(!(memcmp("md5sum", p->payload, 6)))
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
			tcp_update_svr_send_msg("\r\nStart QSPI Update\r\n");
		}
		else
		{
			memset(md5sum, 0, 16);
			tsize = 0;
			total_bytes = 0;
			tcp_update_svr_send_msg("md5sum != cacl_md5sum\r\n");
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
			tcp_update_svr_send_msg("Start QSPI Update\r\n");
		}
		else
		{
			memset(md5sum, 0, 16);
			tsize = 0;
			total_bytes = 0;
			tcp_update_svr_send_msg("md5sum != cacl_md5sum\r\n");
		}
		NOP();
	}
    else if(q->tot_len == 5 && !(memcmp("clear", p->payload, 5)))
    {
        start_update_flag = 0;
        total_bytes = 0;
        tcp_update_svr_send_msg("Clear received data\r\n");
        bsp_printf("Clear received data\r\n");
    }
    else
    {
        while (q->tot_len != q->len)
        {
            memcpy(&rxbuffer[total_bytes], q->payload, q->len);
            total_bytes += q->len;
            q = q->next;
        }
        memcpy(&rxbuffer[total_bytes], q->payload, q->len);
        total_bytes += q->len;
    }

    tcp_recved(tpcb, p->tot_len);
    pbuf_free(p);

    return ERR_OK;

}



/** Error callback, tcp session aborted */
static void tcp_server_err(void *arg, err_t err)
{
	LWIP_UNUSED_ARG(err);
//	u64_t now = get_time_ms();
//	u64_t diff_ms = now - server.start_time;
	tcp_server_close(client_pcb);
	client_pcb = NULL;
//	tcp_conn_report(diff_ms, TCP_ABORTED_REMOTE);
	bsp_printf("tcp_update_ser: Connection aborted\n\r");
}

static err_t accept_callback(void *arg, struct tcp_pcb *newpcb, err_t err)
{

	if ((err != ERR_OK) || (newpcb == NULL))
	{
		return ERR_VAL;
	}

    bsp_printf("tcp_update_ser: Connection Accepted\r\n");
    //保存连接的客户端 PCB
    client_pcb = newpcb;
    tcp_recv(client_pcb, tcp_update_recv_callback);
    tcp_arg(client_pcb, NULL);
    tcp_err(client_pcb, tcp_server_err);

    return ERR_OK;
}

/*
 * Create new pcb, bind pcb and port, set call back function
 */
int start_tcp_update_application(void)
{
	struct tcp_pcb *pcb,*lpcb;
	err_t err;

	/* Create a new TCP PCB structure  */
    // pcb = tcp_new_ip_type(IPADDR_TYPE_ANY);
    pcb = tcp_new();
    if (!pcb) {
        bsp_printf("Error creating PCB. Out of Memory\n\r");
        return -1;
    }

	/* Bind the pcb to the TCP_PORT port */
	/* Using IP_ANY_TYPE==IP_ADDR_ANY allow the pcb to be used by any local interface */
    err = tcp_bind(pcb, IP_ANY_TYPE, TCP_UPDATE_SVR_PORT);
    if (err != ERR_OK) {
        bsp_printf("Unable to bind to port %d: err = %d\n\r", TCP_UPDATE_SVR_PORT, err);
        tcp_close(pcb);
        return -2;
    }

    lpcb = tcp_listen(pcb);
    if (!lpcb) {
        bsp_printf("Out of memory while tcp_listen\n\r");
        return -3;
    }
    //此处不需要回调函数的任何参数
    tcp_arg(lpcb, NULL);

    tcp_accept(lpcb, accept_callback);

    return 0;
}


void transfer_tcp_update_data(void)
{
	char msg[60];
	if (start_update_flag)
	{
        bsp_printf("Start QSPI Update!\r\n");
        bsp_printf("tsize = %d\r\n", tsize);
        bsp_printf("file size of BOOT.bin is %lu Bytes\r\n", total_bytes);
        sprintf(msg, "file size of BOOT.bin is %lu Bytes\r\n", total_bytes);
        tcp_update_svr_send_msg(msg);
        if (qspi_update(total_bytes, rxbuffer, start_update_type) != XST_SUCCESS)
        {
        	tcp_update_svr_send_msg("Update Qspi Error!\r\n");
            bsp_printf("Update Qspi Error!\r\n");
        }
        else
        {
            total_bytes = 0;
        }
    	memset(md5sum, 0, 16);
    	tsize = 0;
	}
    start_update_flag = 0;
}

#endif // TCP_UPDATE

#endif // XPAR_XEMACPS_NUM_INSTANCES || XPAR_XAXIETHERNET_NUM_INSTANCES


/*
usage:
call tcp_server_setup() and platform_enable_interrupts() before the main_loop
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
