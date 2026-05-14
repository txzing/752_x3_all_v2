#include "../bsp.h"

#if defined (XPAR_XEMACPS_NUM_INSTANCES) || defined (XPAR_XAXIETHERNET_NUM_INSTANCES)
#if defined (UDP_COMMAND_SRV)

static struct udp_pcb *client_pcb = NULL;
static struct pbuf *pbuf_to_be_sent = NULL;
ip_addr_t target_addr;

void print_udp_cmd_header(void)
{
    bsp_printf("%20s %6d\r\n", "UDP CMD", UDP_CMD_SVR_PORT);
}


void udp_cmd_recv_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p,
		const ip_addr_t *addr, u16_t port)
{
	receivelen = p->tot_len;
	memset(receivebuf, 0, sizeof(receivebuf));
	memset(send_buf, 0, sizeof(send_buf));
	pbuf_copy_partial(p, receivebuf, p->tot_len, 0);
    pbuf_free(p);

    upcb->remote_ip = *addr;
    upcb->remote_port = port;
    client_pcb = upcb;


//	xil_printf("%d.%d.%d.%d\r\n", ip4_addr1(addr), ip4_addr2(addr), ip4_addr3(addr), ip4_addr4(addr));
//	xil_printf("port  : %d\r\n",port);
//    xil_printf("receive data : %x\r\n",receivebuf[4]);
//    xil_printf("msg_cmd : %x\r\n",*(receivebuf+4));

    msg_len = 0;
    msg_cmd = 0;

    memcpy(&msg_len,receivebuf,2);
    memcpy(&msg_cmd,receivebuf+4,1);

//    xil_printf("msg_len : %d\r\n",msg_len);
//    xil_printf("msg_cmd : %x\r\n",msg_cmd);


    if(msg_cmd == 0x10)	// read mem addr
    {
    	msg_cmd_0x10();
    }
    else if(msg_cmd == 0x11)	// write mem addr
	{
    	msg_cmd_0x11();
	}
    else if(msg_cmd == 0x12)	// read mem bulk
	{
    	msg_cmd_0x12();
	}
    else if(msg_cmd == 0x13)	// write mem bulk
	{
    	msg_cmd_0x13();
	}
    else if(msg_cmd == 0x20)
	{
    	msg_cmd_0x20();
	}
    else if(msg_cmd == 0x21)
	{
    	msg_cmd_0x21();
	}
#if defined (XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES)
    else if(msg_cmd == 0x30) //
    {
    	msg_cmd_0x30();
    }
#endif//defined (XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES)
    else if(msg_cmd == 0x40) // variables get and set
    {
    	msg_cmd_0x40();
    }

//需要分隔开
    if(msg_cmd == 0x80)	// video control
	{
    	msg_cmd_0x80();
	}
    else
    {

    	static struct pbuf *pbuf2sent;
//        pbuf2sent = pbuf_alloc(PBUF_TRANSPORT,sendlen,PBUF_POOL);
		pbuf2sent = pbuf_alloc(PBUF_TRANSPORT, sendlen, PBUF_RAM);
		pbuf_take(pbuf2sent,(char*)send_buf,sendlen);
		if (!pbuf2sent)
			bsp_printf("udp_cmd Error allocating pbuf\r\n");

		if (udp_send(client_pcb, pbuf2sent) != ERR_OK)
			bsp_printf("udp_cmd UDP send error\r\n");

		pbuf_free(pbuf2sent);

    }

}
/*
 * Create new pcb, bind pcb and port, set call back function
 */
int start_udp_cmd_application(void)
{
	struct udp_pcb *pcb;
	err_t err;

	/* Create a new UDP PCB structure  */
	pcb = udp_new();
	if (!pcb)
	{
		bsp_printf("Error creating PCB. Out of Memory\n\r");
		return -1;
	}

	/* Bind the upcb to the UDP_PORT port */
	/* Using IP_ANY_TYPE==IP_ADDR_ANY allow the upcb to be used by any local interface */
	err = udp_bind(pcb, IP_ADDR_ANY, UDP_CMD_SVR_PORT);
	if (err != ERR_OK)
	{
		xil_printf("UDP server: Unable to bind to port");
		bsp_printf("Unable to bind to port %d: err = %d\n\r", UDP_CMD_SVR_PORT, err);
		udp_remove(pcb);
		return -2;
	}

    /* Set a receive callback for the upcb */
	udp_recv(pcb, udp_cmd_recv_callback, NULL);
//	bsp_printf("UDP server started @ port %d\n\r", UDP_CMD_SVR_PORT);

//	IP4_ADDR(&target_addr, 192,168,1,77);

#if defined (UDP_VIDEO)
    for (int i = 0; i < ETH_VIDEO_NUM; i++) // 遍历所有通道
    {
    	VdmaChannels[i].send_pic_start = 0;
    	VdmaChannels[i].video_sending = 0;
    	VdmaChannels[i].send_video_start = 0;
    }
#endif

	return 0;
}

void transfer_udp_cmd_data(void)
{
	return;
}

#if defined (UDP_VIDEO)
/*
 * Send frame data with udp
 *
 * @param pic is frame pointer will be send
 * @param piclen is frame length in one package
 * @param sn is Serial number for each ethernet package
 *
 * @format
 * @HeaderFormat
 *
 */

int sendpic_udp(uint8_t *pic,  uint16_t piclen)
{
	if(client_pcb != NULL)
	{
		err_t err;

		/*  申请pbuf资源  */
		pbuf_to_be_sent = pbuf_alloc(PBUF_TRANSPORT, piclen + HEADER_SIZE, PBUF_POOL);
		if(pbuf_to_be_sent == NULL)
		{
			xil_printf("pbuf_alloc %d fail\n\r", piclen + HEADER_SIZE);
			return -3;
		}

		memcpy(pbuf_to_be_sent->payload, &packet_p, HEADER_SIZE);
		memcpy(pbuf_to_be_sent->payload+HEADER_SIZE, pic, piclen);

		pbuf_to_be_sent->len = HEADER_SIZE + piclen;
		pbuf_to_be_sent->tot_len = HEADER_SIZE + piclen;

		err = udp_send(client_pcb,pbuf_to_be_sent);
		pbuf_free(pbuf_to_be_sent);
		//发送报文
		 if (err != ERR_OK)
		{
			xil_printf("pbuf_to_be_sent->len %d\n\r", pbuf_to_be_sent->len);
			xil_printf(TXT_RED "Error on udp send : %d\r\n" TXT_RST, err);
			return -2;
		}

	}
	else
	{
		xil_printf("Error on client_pcb == NULL\r\n");
		return -1;
	}
	return 0;
}

#if defined (XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES)
void err_auto_send_func(u8 ch)
{
	if (ch >= XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES)
	{
	    return;
	}
	else if(client_pcb != NULL)
	{
		err_t err;
		struct pbuf *pq;

		sendlen = 8 + (unsigned)sizeof(vcmp_message);
		pq = pbuf_alloc(PBUF_TRANSPORT, sendlen, PBUF_POOL);
		if(pq == NULL)
		{
			xil_printf("err_auto_send pbuf_alloc %d fail\r\n", sendlen);
			return;
		}

		xil_printf("err_auto_send %d\r\n", ch + 1);

		memcpy(send_buf,&sendlen,2);
		send_buf[2] = 0x00;
		send_buf[3] = 0x00;
		send_buf[4] = 0x30;
		send_buf[5] = 0x05;
		send_buf[6] = 0xff;

		memcpy(send_buf+7,&vcmp_m[ch],(unsigned)sizeof(vcmp_message));

		send_buf[sendlen-1] = checksum(send_buf,sendlen-1);

		pbuf_take(pq,(char*)send_buf,sendlen);
		err = udp_send(client_pcb,pq);
		pbuf_free(pq);
		if (err != ERR_OK)
		{
			xil_printf(TXT_RED "err_auto_send Error on udp send : %d\r\n" TXT_RST, err);
		}

	}
	else
	{
		xil_printf("err_auto_send Error on client_pcb == NULL!\r\n");
	}
}
#endif //#if defined (XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES)

#endif //#if defined (UDP_VIDEO)

#endif // UDP_COMMAND_SRV

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
