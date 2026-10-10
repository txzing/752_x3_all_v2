#include "../bsp.h"

/* 应用层以太网命令与应答格式见同目录《LVDS以太网控制协议.md》，与 tcp_cmd.c、udp_cmd.c 保持一致即可互通。 */

#if defined (XPAR_XEMACPS_NUM_INSTANCES) || defined (XPAR_XAXIETHERNET_NUM_INSTANCES)
#if defined (UDP_UPDATE) || defined (TCP_UPDATE) || defined (TCP_COMMAND_SRV) || defined (UDP_COMMAND_SRV)

#if defined (UDP_UPDATE) || defined (TCP_UPDATE)
u8 rxbuffer[MAX_FLASH_LEN];
u32 total_bytes = 0;
#endif // #if defined (UDP_UPDATE) || defined (TCP_UPDATE)

#if defined (UDP_COMMAND_SRV) || defined (TCP_COMMAND_SRV)
extern FlashInfo Flash_Config_Table[28];
extern u32 FlashMake;
extern u32 FCTIndex;	/* Flash configuration table index */

uint8_t receivebuf[1500] = {0};
uint8_t send_buf[1500] = {0};
int receivelen = 0;
int sendlen = 0;

int32_t Status=0;
uint16_t msg_len=0;
uint16_t msg_cmd=0;
uint32_t msg_addr=0;
uint32_t msg_value=0;
uint8_t msg_ack=0;
uint16_t mem_len=0;
#endif

config_Settings_t config;
config_Settings_t default_config;
config_Settings_t global_config;


/* missing declaration in lwIP */
void lwip_init();

#if LWIP_IPV6==0
#if LWIP_DHCP==1
extern volatile int dhcp_timoutcntr;
err_t dhcp_start(struct netif *netif);
#endif
#endif

struct netif server_netif;

#if LWIP_IPV6==1
void print_ip6(char *msg, ip_addr_t *ip)
{
	print(msg);
//	bsp_printf(" %s\n\r", inet6_ntoa(*ip));
	bsp_printf(" %x:%x:%x:%x:%x:%x:%x:%x\n\r",
			IP6_ADDR_BLOCK1(&ip->u_addr.ip6),
			IP6_ADDR_BLOCK2(&ip->u_addr.ip6),
			IP6_ADDR_BLOCK3(&ip->u_addr.ip6),
			IP6_ADDR_BLOCK4(&ip->u_addr.ip6),
			IP6_ADDR_BLOCK5(&ip->u_addr.ip6),
			IP6_ADDR_BLOCK6(&ip->u_addr.ip6),
			IP6_ADDR_BLOCK7(&ip->u_addr.ip6),
			IP6_ADDR_BLOCK8(&ip->u_addr.ip6));

}
#else
void print_ip(char *msg, ip_addr_t *ip)
{
	print(msg);
	bsp_printf("%d.%d.%d.%d\n\r", ip4_addr1(ip), ip4_addr2(ip),
			ip4_addr3(ip), ip4_addr4(ip));
}

void print_ip_settings(ip_addr_t *ip, ip_addr_t *mask, ip_addr_t *gw)
{

	print_ip("Board IP: ", ip);
	print_ip("Netmask : ", mask);
	print_ip("Gateway : ", gw);
}

void assign_default_ip(ip_addr_t *ip, ip_addr_t *mask, ip_addr_t *gw)
{
	int err;

	bsp_printf("Configuring default IP %s \r\n", DEFAULT_IP_ADDRESS);

	err = inet_aton(DEFAULT_IP_ADDRESS, ip);
	if (!err)
		bsp_printf("Invalid default IP address: %d\r\n", err);

	err = inet_aton(DEFAULT_IP_MASK, mask);
	if (!err)
		bsp_printf("Invalid default IP MASK: %d\r\n", err);

	err = inet_aton(DEFAULT_GW_ADDRESS, gw);
	if (!err)
		bsp_printf("Invalid default gateway address: %d\r\n", err);
}
#endif // LWIP_IPV6


void print_headers()
{
    bsp_printf("\r\n");
    bsp_printf("%20s %6s\r\n", "Server", "Port");
    bsp_printf("%20s %6s\r\n", "--------------------", "------");
#if defined (UDP_UPDATE)
	print_udp_update_header();
#endif // #if defined (UDP_UPDATE)
#if defined (TCP_UPDATE)
	print_tcp_update_header();
#endif // #if defined (TCP_UPDATE)
#if defined (TCP_COMMAND_SRV)
	print_tcp_cmd_header();
#endif // #if defined (TCP_COMMAND_SRV)
#if defined (UDP_COMMAND_SRV)
	print_udp_cmd_header();
#endif // #if defined (UDP_COMMAND_SRV)
    bsp_printf("\r\n");
}

void start_applications(void)
{
#if defined (UDP_UPDATE) || defined (TCP_UPDATE)
	int Status;
	Status = qspi_init();
    if (Status != XST_SUCCESS)
    {
        bsp_printf(TXT_RED "In %s: QSPI init failed...\r\n" TXT_RST, __func__);
        // return XST_FAILURE;
    }
    bsp_printf("Successfully init QSPI\r\n");
#endif // #if defined (UDP_UPDATE) || defined (TCP_UPDATE)

#if defined (UDP_UPDATE)
	start_udp_update_application();
#endif // #if defined (UDP_UPDATE)
#if defined (TCP_UPDATE)
	start_tcp_update_application();
#endif // #if defined (TCP_UPDATE)
#if defined (TCP_COMMAND_SRV)
	start_tcp_cmd_application();
#endif // #if defined (TCP_COMMAND_SRV)
#if defined (UDP_COMMAND_SRV)
	start_udp_cmd_application();
#endif // #if defined (UDP_COMMAND_SRV)

}

void transfer_data(struct netif *netif)
{
    if (TcpFastTmrFlag) {
        tcp_fasttmr();
        TcpFastTmrFlag = 0;
    }
    if (TcpSlowTmrFlag) {
        tcp_slowtmr();
        TcpSlowTmrFlag = 0;
    }
	xemacif_input(netif);

#if defined (UDP_UPDATE)
	transfer_udp_update_data();
#endif // #if defined (UDP_UPDATE)
#if defined (TCP_UPDATE)
	transfer_tcp_update_data();
#endif // #if defined (TCP_UPDATE)
#if defined (TCP_COMMAND_SRV)
	transfer_tcp_cmd_data();
#endif // #if defined (TCP_COMMAND_SRV)
#if defined (UDP_COMMAND_SRV)
	transfer_udp_cmd_data();
#endif // #if defined (UDP_COMMAND_SRV)

#if defined (PLATFORM_ZYNQ) || defined (PLATFORM_ZYNQMP)
	if (reset_pl)
	{
		reset_pl = 0;
		PsSoftwareReset();
	}
#endif

}

int lwip_common_init(struct netif *netif)
{
//	 int Status;

#if LWIP_IPV6==0
	 ip_addr_t ipaddr, netmask, gw;
#endif

	/* the mac address of the board. this should be unique per board */
	unsigned char mac_ethernet_address[] = { 0x10, 0x0a, 0x35, 0x00, 0x01, 0x02 };

	netif = &server_netif;

#if LWIP_IPV6==0
#if LWIP_DHCP==1
	ipaddr.addr = 0;
	gw.addr = 0;
	netmask.addr = 0;
#else
	/* initliaze IP addresses to be used */
	IP4_ADDR(&ipaddr,  192, 168,   1, 10);
	IP4_ADDR(&netmask, 255, 255, 255,  0);
	IP4_ADDR(&gw,      192, 168,   1,  1);
#endif
#endif

	ipaddr.addr = global_config.ipaddr_u32;
	netmask.addr = global_config.mask_u32;
	gw.addr = global_config.gw_u32;
	memcpy(mac_ethernet_address, global_config.mac_address, 6);

	lwip_init();

#if (LWIP_IPV6 == 0)
	/* Add network interface to the netif_list, and set it as default */
	if (!xemac_add(netif, &ipaddr, &netmask,
			&gw, mac_ethernet_address,
			PLATFORM_EMAC_BASEADDR)) {
		bsp_printf("Error adding N/W interface\n\r");
		return XST_FAILURE;
	}
#else
	/* Add network interface to the netif_list, and set it as default */
	if (!xemac_add(netif, NULL, NULL, NULL, mac_ethernet_address,
			PLATFORM_EMAC_BASEADDR)) {
		bsp_printf("Error adding N/W interface\n\r");
		return XST_FAILURE;
	}
	netif->ip6_autoconfig_enabled = 1;

	netif_create_ip6_linklocal_address(netif, 1);
	netif_ip6_addr_set_state(netif, 0, IP6_ADDR_VALID);

	print_ip6("\n\rBoard IPv6 address ", &netif->ip6_addr[0].u_addr.ip6);

#endif

	netif_set_default(netif);

	/* now enable interrupts */
//	platform_enable_interrupts();

	/* specify that the network if is up */
	netif_set_up(netif);

#if (LWIP_IPV6 == 0)
#if (LWIP_DHCP==1)
	/* Create a new DHCP client for this interface.
	 * Note: you must call dhcp_fine_tmr() and dhcp_coarse_tmr() at
	 * the predefined regular intervals after starting the client.
	 */
	dhcp_start(netif);
	dhcp_timoutcntr = 24;

	while(((netif->ip_addr.addr) == 0) && (dhcp_timoutcntr > 0)) {
		xemacif_input(netif);
		if (TcpFastTmrFlag) {
			tcp_fasttmr();
			TcpFastTmrFlag = 0;
		}
		if (TcpSlowTmrFlag) {
			tcp_slowtmr();
			TcpSlowTmrFlag = 0;
		}
	}
	if (dhcp_timoutcntr <= 0) {
		if ((netif->ip_addr.addr) == 0) {
			bsp_printf("DHCP request timed out\r\n");
			bsp_printf("Configuring default IP of 192.168.1.10\r\n");
			IP4_ADDR(&(netif->ip_addr),  192, 168,   1, 10);
			IP4_ADDR(&(netif->netmask), 255, 255, 255,  0);
			IP4_ADDR(&(netif->gw),      192, 168,   1,  1);
		}
	}

	ipaddr.addr = netif->ip_addr.addr;
	gw.addr = netif->gw.addr;
	netmask.addr = netif->netmask.addr;
#endif // #if (LWIP_DHCP==1)
	print_ip_settings(&ipaddr, &netmask, &gw);
#endif // #if (LWIP_IPV6 == 0)

	/* start the application (web server, rxtest, txtest, etc..) */
	start_applications();
	print_headers();

#if (defined UDP_VIDEO)
	bsp_printf(TXT_RED "now use UDP_VIDEO\r\n" TXT_RST);
#elif (defined TCP_VIDEO)
	bsp_printf(TXT_RED "now use TCP_VIDEO\r\n" TXT_RST);
#endif

#if defined (UDP_VIDEO) || (defined TCP_VIDEO)
	VC_inst.send_pic_start = 0;
	VC_inst.video_sending = 0;
	VC_inst.send_video_start = 0;
	err_ch = 0;
#endif

	return XST_SUCCESS;
}

void send_msg(const char *msg)
{
#if defined (UDP_UPDATE)
	udp_update_svr_send_msg(msg);
#endif // #if defined (UDP_UPDATE)
#if defined (TCP_UPDATE)
	tcp_update_svr_send_msg(msg);
#endif // #if defined (UDP_UPDATE)
}

void process_print(u8 percent)
{
#if defined (UDP_UPDATE)
	udp_update_process_print(percent);
#endif // #if defined (UDP_UPDATE)
#if defined (TCP_UPDATE)
	tcp_update_process_print(percent);
#endif // #if defined (UDP_UPDATE)
}

uint8_t checksum(uint8_t * ptr, int16_t cnt)
{
	int16_t i = 0;
	uint8_t calc=0;
	for(i=0;i<cnt;i++)
	{
		calc=calc + ptr[i];
	}

	return calc=~calc&0xff;
}

void print_config(const config_Settings_t* cfg)
{
	uint32_t ip = __builtin_bswap32(cfg->ipaddr_u32);
	uint32_t mask = __builtin_bswap32(cfg->mask_u32);
	uint32_t gw = __builtin_bswap32(cfg->gw_u32);

	xil_printf("IP: %d.%d.%d.%d\r\n",
		(ip >> 24) & 0xFF, (ip >> 16) & 0xFF, (ip >> 8) & 0xFF, ip & 0xFF);

	xil_printf("MASK: %d.%d.%d.%d\r\n",
		(mask >> 24) & 0xFF, (mask >> 16) & 0xFF, (mask >> 8) & 0xFF, mask & 0xFF);

	xil_printf("GW: %d.%d.%d.%d\r\n",
		(gw >> 24) & 0xFF, (gw >> 16) & 0xFF, (gw >> 8) & 0xFF, gw & 0xFF);

	xil_printf("MAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n",
		cfg->mac_address[0], cfg->mac_address[1], cfg->mac_address[2],
		cfg->mac_address[3], cfg->mac_address[4], cfg->mac_address[5]);
}

void init_default_config(void)
{
	default_config.reserved_container=(long)0x0000000000000000;
	default_config.ipaddr_u32=0x0a01a8c0; // 192.168.1.10
	default_config.mask_u32=0x00ffffff;	  // 255.255.255.0
	default_config.gw_u32=0x0101a8c0;	  // 192.168.1.1

	default_config.mac_address[0]=0x10;   //6byte
	default_config.mac_address[1]=0x0a;
	default_config.mac_address[2]=0x35;
	default_config.mac_address[3]=0x00;
	default_config.mac_address[4]=0x01;
	default_config.mac_address[5]=0x02;
	default_config.mac_address[6]=0xff;
	default_config.mac_address[7]=0xff;
}

void loadconfig(config_Settings_t * config_p)
{
    int32_t Status;
    uint8_t ReadBuffer[1 + sizeof(config_Settings_t)];
    uint8_t checksum_v, checksum_v1;

    memset(ReadBuffer, 0, sizeof(ReadBuffer));

    Status = EepromReadData(&ps_i2c_0, ReadBuffer, sizeof(ReadBuffer),
			    (u16)EEPROM_ADDR_ETH_CFG);

    if (Status != XST_SUCCESS)
    {
        xil_printf("read config from eeprom failed, now set default config\r\n");
        memcpy(&config, &default_config, sizeof(config));
    }
    else
    {
        xil_printf("read config from eeprom\r\n");

        memcpy(&checksum_v, ReadBuffer, 1);
        memcpy(&config, ReadBuffer + 1, sizeof(config));

        checksum_v1 = checksum((uint8_t *)&config, sizeof(config_Settings_t));

        if (checksum_v1 != checksum_v)
        {
            xil_printf("read config checksum failed, now set default config\r\n");
            memcpy(&config, &default_config, sizeof(config));
        }
        else
        {
        	print_config(&config);
        }
    }

    memcpy(config_p, &config, sizeof(config));
    asm("nop");
}


int32_t saveconfig(config_Settings_t * config_p)
{
	int32_t				Status;
	uint8_t				checksum_v;
	uint8_t				WriteBuffer[1+sizeof(config_Settings_t)];
	u16				n;

	n = (u16)(1U + sizeof(config_Settings_t));
	if (n > (u16)EEPROM_ETH_CFG_MAX_BYTES)
	{
		xil_printf("saveconfig: eth cfg too large (%u)\r\n", (unsigned)n);
		return XST_FAILURE;
	}

	checksum_v = checksum((uint8_t *)config_p, sizeof(config_Settings_t));
	memset(WriteBuffer, 0, sizeof(WriteBuffer));

	memcpy(WriteBuffer, &checksum_v, 1); // checksum 1 byte
	memcpy(WriteBuffer+1, config_p, sizeof(config_Settings_t));

	Status = EepromWriteVerify(&ps_i2c_0, WriteBuffer, n,
				   (u16)EEPROM_ADDR_ETH_CFG);
	if (Status != XST_SUCCESS)
	{
		xil_printf("write config to eeprom failed\r\n");
		return XST_FAILURE;
	}

	return Status;
}

#if defined (UDP_COMMAND_SRV) || defined (TCP_COMMAND_SRV)

static void ack_copy_request(void)
{
	if (receivelen < 8)
	{
		return;
	}
	sendlen = receivelen;
	memcpy(send_buf, receivebuf, (size_t)sendlen);
}

/* 应答失败：回显请求帧，预留字节 2～3 置 FF FF，并重新计算校验 */
void ack_fail_request(void)
{
	if (receivelen < 8)
	{
		return;
	}
	ack_copy_request();
	send_buf[2] = 0xFF;
	send_buf[3] = 0xFF;
	send_buf[sendlen - 1] = checksum(send_buf, sendlen - 1);
}

static uint16_t req_frame_len(void)
{
	return (uint16_t)receivebuf[0] | ((uint16_t)receivebuf[1] << 8);
}

static int req_len_match(uint16_t expected)
{
	return (receivelen == (int)expected) && (req_frame_len() == expected);
}

static int req_len_at_least(uint16_t minimum)
{
	return (receivelen >= (int)minimum) &&
	       (req_frame_len() == (uint16_t)receivelen) &&
	       (req_frame_len() >= minimum);
}

/*
 * ========== Ethernet I2C 0x20(read) / 0x21(write) ==========
 * one parser: read rsp=14+2*len, write rsp=14+len
 * [2:3]=seq echo; [11]xfer [12]done bstat[len] (+read data)
 * BAD_FRAME(6)/PARAM no bus; len==1 single API, len>1 continuous
 * opt: bit0=reg16, bit1=stretch; len 1..64
 */
static void i2c_net_cmd(int is_write)
{
	uint8_t ch, addr7, len, opt, xfer, cmd;
	uint16_t reg0, i, done, sl, expect;
	uint8_t data[64];
	uint8_t bstat[64];
	stretch_mode sm;
	int st, frame_ok;

	cmd = is_write ? 0x21U : 0x20U;
	frame_ok = 1;
	len = 1U;
	if (receivelen >= 11) {
		len = receivebuf[7];
	} else {
		frame_ok = 0;
	}

	if ((len == 0U) || (len > 64) ||
	    ((14 + (is_write ? (int)len : (int)len * 2)) > (int)ARRAY_SIZE(send_buf))) {
		len = 1U;
		xfer = (uint8_t)I2C_ERR_PARAM;
		done = 0;
		memset(bstat, xfer, 1);
		if (!is_write) {
			memset(data, 0xFF, 1);
		}
		goto pack;
	}

	expect = is_write ? (uint16_t)(12U + len) : 12U;
	if (!req_len_match(expect) ||
	    (checksum(receivebuf, (int16_t)(expect - 1U)) != receivebuf[expect - 1U])) {
		frame_ok = 0;
	}

	done = 0;
	memset(bstat, (uint8_t)I2C_ERR_PARAM, len);
	if (!is_write) {
		memset(data, 0xFF, len);
	}

	if (!frame_ok) {
		xfer = (uint8_t)I2C_ERR_BAD_FRAME;
		memset(bstat, xfer, len);
		goto pack;
	}

	ch    = receivebuf[5];
	addr7 = receivebuf[6];
	reg0  = ((uint16_t)receivebuf[8] << 8) | receivebuf[9];
	opt   = receivebuf[10];

	if ((ch >= (uint8_t)I2C_NO_BUTT) || (addr7 > 0x7FU) ||
	    ((opt & (uint8_t)(~0x03U)) != 0U) ||
	    (((opt & 0x01U) == 0U) &&
	     ((uint16_t)(reg0 + (uint16_t)len - 1U) > 0x00FFU))) {
		xfer = (uint8_t)I2C_ERR_PARAM;
		memset(bstat, xfer, len);
		goto pack;
	}

	sm = (opt & 0x02U) ? STRETCH_ON : STRETCH_OFF;
	if (len == 1U) {
		if (is_write) {
			st = (opt & 0x01U)
				? xgpio_i2c_reg16_write((i2c_no)ch, (char)addr7, reg0,
						(char)receivebuf[11], sm)
				: xgpio_i2c_reg8_write((i2c_no)ch, (char)addr7,
						(char)(reg0 & 0xFFU), (char)receivebuf[11], sm);
		} else {
			st = (opt & 0x01U)
				? xgpio_i2c_reg16_read((i2c_no)ch, (char)addr7, reg0,
						&data[0], sm)
				: xgpio_i2c_reg8_read((i2c_no)ch, (char)addr7,
						(char)(reg0 & 0xFFU), &data[0], sm);
		}
		xfer = (uint8_t)st;
		if (st == I2C_OK) {
			done = 1;
			bstat[0] = (uint8_t)I2C_OK;
		} else {
			bstat[0] = (uint8_t)st;
			if (!is_write) {
				data[0] = 0xFF;
			}
		}
	} else {
		if (is_write) {
			st = (opt & 0x01U)
				? xgpio_i2c_reg16_continuous_write((i2c_no)ch, addr7, reg0,
						&receivebuf[11], len, sm, &done)
				: xgpio_i2c_reg8_continuous_write((i2c_no)ch, addr7,
						(u8)(reg0 & 0xFFU), &receivebuf[11], len, sm, &done);
		} else {
			st = (opt & 0x01U)
				? xgpio_i2c_reg16_continuous_read((i2c_no)ch, addr7, reg0,
						data, len, sm, &done)
				: xgpio_i2c_reg8_continuous_read((i2c_no)ch, addr7,
						(u8)(reg0 & 0xFFU), data, len, sm, &done);
		}
		xfer = (uint8_t)st;
		if (st == I2C_OK) {
			memset(bstat, (uint8_t)I2C_OK, len);
		} else {
			for (i = 0; i < len; i++) {
				if (i < done) {
					bstat[i] = (uint8_t)I2C_OK;
				} else if (i == done) {
					bstat[i] = (uint8_t)st;
					if (!is_write) {
						data[i] = 0xFF;
					}
				} else {
					bstat[i] = (uint8_t)I2C_ERR_SKIPPED;
					if (!is_write) {
						data[i] = 0xFF;
					}
				}
			}
		}
	}

pack:
	sendlen = 14 + (is_write ? (int)len : (int)len * 2);
	sl = (uint16_t)sendlen;
	if (receivelen >= 11) {
		memcpy(send_buf, receivebuf, 11);
	} else {
		memset(send_buf, 0, 11);
		if (receivelen >= 4) {
			memcpy(send_buf + 2, receivebuf + 2, 2);
		}
		send_buf[4] = cmd;
	}
	send_buf[7] = len;
	send_buf[11] = xfer;
	send_buf[12] = (uint8_t)done;
	memcpy(send_buf + 13, bstat, len);
	if (!is_write) {
		memcpy(send_buf + 13 + len, data, len);
	}
	memcpy(send_buf, &sl, 2);
	send_buf[sendlen - 1] = checksum(send_buf, sendlen - 1);
}


void msg_cmd_0x10(void)//read mem
{
	if (!req_len_match(10))
	{
		ack_fail_request();
		return;
	}

	memcpy(&msg_addr,receivebuf+5,4);
	msg_value = Xil_In32(msg_addr); // don't use signed int, otherwise hardfail
	sendlen=receivelen + 4;
	memcpy(send_buf,receivebuf,receivelen);
	memcpy(send_buf+9,&msg_value,4);
	memcpy(send_buf,&sendlen,2);
	send_buf[sendlen-1] = checksum(send_buf,sendlen-1);
}

void msg_cmd_0x11(void)//write mem
{
	if (!req_len_match(14))
	{
		ack_fail_request();
		return;
	}

	memcpy(&msg_addr,receivebuf+5,4);
	memcpy(&msg_value,receivebuf+9,4);
	Xil_Out32(msg_addr, msg_value);
	msg_value = Xil_In32(msg_addr); // don't use signed int, otherwise hardfail
	sendlen=receivelen;
	memcpy(send_buf,receivebuf,receivelen);
	memcpy(send_buf+9,&msg_value,4);
	memcpy(send_buf,&sendlen,2);
	send_buf[sendlen-1] = checksum(send_buf,sendlen-1);
}

void msg_cmd_0x12(void)//read block men
{
	memcpy(&msg_addr,receivebuf+5,4);
	memcpy(&mem_len,receivebuf+9,4);
	if (!req_len_match(14) || mem_len == 0U)
	{
		ack_fail_request();
		return;
	}
	sendlen=receivelen+mem_len*4;
	if(sendlen > (int)ARRAY_SIZE(send_buf) || sendlen > 1400)
	{
		ack_fail_request();
		return;
	}
	memcpy(send_buf,receivebuf,receivelen);
	memcpy(send_buf+receivelen-1,(void*)msg_addr,mem_len*4);
	memcpy(send_buf,&sendlen,2);
	send_buf[sendlen-1] = checksum(send_buf,sendlen-1);
}

void msg_cmd_0x13(void)//write block men
{
	uint16_t expect_len;

	memcpy(&msg_addr,receivebuf+5,4);
	memcpy(&mem_len,receivebuf+9,4);
	if (mem_len == 0U)
	{
		ack_fail_request();
		return;
	}
	expect_len = (uint16_t)(14U + mem_len * 4U);
	if (!req_len_match(expect_len))
	{
		ack_fail_request();
		return;
	}
	memcpy((void*)msg_addr,receivebuf+13,mem_len*4);
	sendlen = 14;
	memcpy(send_buf,receivebuf,sendlen);
	memcpy(send_buf,&sendlen,2);
	send_buf[sendlen-1] = checksum(send_buf,sendlen-1);
}

void msg_cmd_0x20(void)
{
	i2c_net_cmd(0);
}

void msg_cmd_0x21(void)
{
	i2c_net_cmd(1);
}


#if defined (XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES)
void msg_cmd_0x30(void)
{
	uint8_t cmd_index;
	uint8_t ch;
	uint32_t msg_send;
	uint32_t base_addr = 0U;

	if (!req_len_at_least(8))
	{
		ack_fail_request();
		return;
	}

	memcpy(&cmd_index,receivebuf+5,1);
	memcpy(&ch,receivebuf+6,1);
//	for (int i = 0; i < receivelen; i++)
//	{
//		xil_printf("0x%02X ", receivebuf[i]);  // 打印每个字节的十六进制值
//	}
	if(cmd_index == 0)
	{
		base_addr = pixel_compare_axi_base_eth(ch);
		if(base_addr == 0U)
		{
			ack_fail_request();
			return;
		}
		/* 关比较: 清除 bit0(比较开始) */
		Xil_Out32(base_addr + STATUS, 0U);
		ack_copy_request();

	}
	else if(cmd_index == 1)
	{
		base_addr = pixel_compare_axi_base_eth(ch);
		if(base_addr == 0U)
		{
			ack_fail_request();
			return;
		}
		{
			u32 ip_status;

			ip_status = Xil_In32(base_addr + STATUS);
			if(!CHB(ip_status, BIT(2)))
			{
				ack_fail_request();
			}
			else
			{
				pixel_cp_start[ch - 1] = 1U;
				pixel_cp_start_cnt[ch - 1] = 0U;
				ack_copy_request();
			}
		}
	}
	else if(cmd_index == 2)
	{
		uint8_t threshold;
		if (!req_len_at_least(9))
		{
			ack_fail_request();
			return;
		}
		memcpy(&threshold,receivebuf+7,1);
		base_addr = pixel_compare_axi_base_eth(ch);
		if(base_addr == 0U)
		{
			ack_fail_request();
			return;
		}
		Xil_Out32(base_addr + PIXEL_THRESHOLD, threshold);
		ack_copy_request();

	}
	else if(cmd_index == 3)
	{

		uint32_t col = 0;
		uint32_t line = 0;
		uint32_t fps = 0;

		if (ch < 1U || ch > CHANNEL_NUM)
		{
			ack_fail_request();
			return;
		}
#if defined (XPAR_AXI_PASSTHROUGH_MONITOR_NUM_INSTANCES)
		base_addr = vdma_passthrough_mon_base_lvds((u8)(ch - 1U));
#else
		base_addr = 0U;
#endif
		if(base_addr == 0U)
		{
			ack_fail_request();
			return;
		}

		vdma_passthrough_read_mon(base_addr, &col, &line, &fps);

		memcpy(send_buf,receivebuf,7);
		memcpy(send_buf+7,&col,4);
		memcpy(send_buf+11,&line,4);
		memcpy(send_buf+15,&fps,4);
		sendlen = 20;
		memcpy(send_buf,&sendlen,2);
		send_buf[sendlen-1] = checksum(send_buf,sendlen-1);

	}
	else if(cmd_index == 4)
	{
		if (ch < 1U || ch > (uint8_t)CHANNEL_NUM)
		{
			ack_fail_request();
			return;
		}
		if (pixel_compare_axi_base_eth(ch) == 0U)
		{
			ack_fail_request();
			return;
		}
		vcmp_m_refresh_channel((u8)(ch - 1U));

		sendlen = 8 + (unsigned)sizeof(vcmp_message);
		memcpy(send_buf, receivebuf, 7);
		memcpy(send_buf, &sendlen, 2);
		memcpy(send_buf + 7, &vcmp_m[ch - 1U], (unsigned)sizeof(vcmp_message));
		send_buf[sendlen - 1] = checksum(send_buf, sendlen - 1);
	}
	else if(cmd_index == 5)
	{
		if (receivelen < 8)
		{
			ack_fail_request();
			return;
		}
		if (ch == 0xFFU)
		{
			err_auto_send = 1;
			xil_printf("err_auto_send open\r\n");
			ack_copy_request();
		}
		else
		{
			ack_fail_request();
		}
	}
	else if(cmd_index == 6)
	{
		if (receivelen < 8)
		{
			ack_fail_request();
			return;
		}
		if (ch == 0x00U)
		{
			err_auto_send = 0;
			xil_printf("err_auto_send close\r\n");
			ack_copy_request();
		}
		else
		{
			ack_fail_request();
		}
	}
	else if (cmd_index == 7 || cmd_index == 8)
	{
		uint32_t pixel;
		uint32_t pixel_swap;
		u32 reg_off = (cmd_index == 7) ? RGB_CNT_PIXEL : RGB_NOT_PIXEL;

		if (receivelen < 11)
		{
			ack_fail_request();
			return;
		}
		pixel = ((uint32_t)receivebuf[7] << 16) |
		        ((uint32_t)receivebuf[8] << 8) |
		        (uint32_t)receivebuf[9];
		xil_printf("cmd_index %x pixel %x\r\n",cmd_index, pixel);

		pixel_swap = rbg_swap_rgb(pixel);
		xil_printf("pixel_swap %x\r\n",pixel_swap);
		base_addr = pixel_compare_axi_base_eth(ch);
		if (base_addr == 0U)
		{
			ack_fail_request();
			return;
		}
		Xil_Out32(base_addr + reg_off, pixel_swap);
		ack_copy_request();
	}
	else if(cmd_index == 9)
	{
		/* ROI：0 起始闭区间。+7..+13 为 u16 小端 xs,xe,ys,ye（与上位机 Set Compare Area 一致） */
		uint16_t xs = 0U, xe = 0U, ys = 0U, ye = 0U;
		if (receivelen >= 15U)
		{
			memcpy(&xs, receivebuf + 7, 2);
			memcpy(&xe, receivebuf + 9, 2);
			memcpy(&ys, receivebuf + 11, 2);
			memcpy(&ye, receivebuf + 13, 2);
		}
		else
		{
			ack_fail_request();
			return;
		}

		if (xs > xe || ys > ye)
		{
			ack_fail_request();
			return;
		}

		base_addr = pixel_compare_axi_base_eth(ch);
		if (base_addr == 0U)
		{
			ack_fail_request();
			return;
		}
		xil_printf("xs %d xe %d  ys %d ye %d\r\n",xs, xe,  ys, ye);
		Xil_Out32(base_addr + ROI_X_START, xs & 0xFFFFU);
		Xil_Out32(base_addr + ROI_X_END, xe & 0xFFFFU);
		Xil_Out32(base_addr + ROI_Y_START, ys & 0xFFFFU);
		Xil_Out32(base_addr + ROI_Y_END, ye & 0xFFFFU);
		ack_copy_request();
	}
	else if(cmd_index == 0xa)
	{
		uint16_t p_x = 0U, p_y = 0U;
		if (receivelen >= 16U)
		{
			memcpy(&p_x, receivebuf + 7, 2);
			memcpy(&p_y, receivebuf + 11, 2);
		}
		else
		{
			ack_fail_request();
			return;
		}
		xil_printf("p_x %d p_y %d\r\n",p_x,p_y);
		base_addr = pixel_compare_axi_base_eth(ch);
		if (base_addr == 0U)
		{
			ack_fail_request();
			return;
		}
		Xil_Out32(base_addr + POINT_X, p_x & 0xFFFFU);
		Xil_Out32(base_addr + POINT_Y, p_y & 0xFFFFU);
		ack_copy_request();
	}
	else if (cmd_index == 0xb)
	{
		/* 设置错误像素个数阈值 ERR_PIXEL_CNT（达到该个数才产生中断，默认 1） */
		u32 err_cnt_thr = 0U;

		if (!req_len_at_least(12))
		{
			ack_fail_request();
			return;
		}
		memcpy(&err_cnt_thr, receivebuf + 7, 4);
		base_addr = pixel_compare_axi_base_eth(ch);
		if (base_addr == 0U)
		{
			ack_fail_request();
			return;
		}
		xil_printf("ERR_PIXEL_CNT ch%d = %u\r\n", (int)ch, (unsigned)err_cnt_thr);
		Xil_Out32(base_addr + ERR_PIXEL_CNT, err_cnt_thr);
		ack_copy_request();
	}
	else if (cmd_index == 0x0c)
	{
		/* Touch INT: clear 0x23, pulse GPIO2 then GPIO4, return 0x23. ch=1..3. */
		u8 reg23 = 0xFF;
		int st;

		st = tp_touch_intr_read(ch, &reg23);
		if (st != I2C_OK)
		{
			ack_fail_request();
			return;
		}
		memcpy(send_buf, receivebuf, 7);
		send_buf[7] = reg23;
		sendlen = 9;
		memcpy(send_buf, &sendlen, 2);
		send_buf[sendlen - 1] = checksum(send_buf, sendlen - 1);
	}
	else
	{
		ack_fail_request();
	}
}
#endif//#if defined (XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES)

void msg_cmd_0x40(void)
{
	uint16_t cmd_index;
	uint32_t msg_send;

	if (receivelen < 7)
	{
		ack_fail_request();
		return;
	}

	memcpy(&cmd_index,receivebuf+5,2);
	sendlen = 0;

	if(cmd_index == 0)
	{
#if defined (XPAR_AXI_LITE_REG_NUM_INSTANCES) && (XPAR_AXI_LITE_REG_0_DEVICE_ID == 0)
		uint32_t ver = __HW_VER__;
#else
		uint32_t ver = 0;
#endif
		msg_send = ver;

		memcpy(send_buf,receivebuf,7);
		memcpy(send_buf+7,&msg_send,4);
		sendlen = 12;
	}
	if(cmd_index == 1)
	{
		uint32_t ver = __SW_VER__;
		msg_send = ver;

		memcpy(send_buf,receivebuf,7);
		memcpy(send_buf+7,&msg_send,4);
		sendlen = 12;
	}
	if(cmd_index == 2)
	{
		uint32_t ver = 0x00800000; // keep same as type==0 in qspi_update()
		msg_send = ver;

		memcpy(send_buf,receivebuf,7);
		memcpy(send_buf+7,&msg_send,4);
		sendlen = 12;
	}
	if(cmd_index == 3)
	{
		uint32_t ver = FlashMake;
		char *send_arr = NULL;
		if(FlashMake == MICRON_ID_BYTE0)
		{
			send_arr = "MICRON";
		}
		else if(FlashMake == SPANSION_ID_BYTE0)
		{
			send_arr = "SPANSION";
		}
		else if(FlashMake == WINBOND_ID_BYTE0)
		{
			send_arr = "WINBOND";
		}
		else if(FlashMake == MACRONIX_ID_BYTE0)
		{
			send_arr = "MACRONIX";
		}
		else if(FlashMake == ISSI_ID_BYTE0)
		{
			send_arr = "ISSI";
		}
		if (send_arr == NULL)
		{
			ack_fail_request();
			return;
		}
		bsp_printf("string = %s,len = %d\r\n",send_arr,strlen(send_arr));
		memcpy(send_buf,receivebuf,7);
		memcpy(send_buf+7,send_arr,strlen(send_arr));
		sendlen = 7+strlen(send_arr)+1;
	}
	if(cmd_index == 4)
	{
		FlashInfo*flash_ptr=&Flash_Config_Table[FCTIndex];
		uint32_t ver = (flash_ptr->FlashDeviceSize)/1024;
		msg_send = ver;

		memcpy(send_buf,receivebuf,7);
		memcpy(send_buf+7,&msg_send,4);
		sendlen = 12;
	}
	if(cmd_index == 5)// cur_ch_get
	{
		memcpy(send_buf,receivebuf,7);
		memcpy(send_buf+7,&current_ch,1);
		sendlen = 9;
	}
	if(cmd_index == 6) //cur_ch_set
	{
		uint8_t var1;
		if (!req_len_at_least(9))
		{
			ack_fail_request();
			return;
		}
		memcpy(&var1,receivebuf+7,1);
		switch_ch = var1;

		if (switch_ch < 1U || switch_ch > CHANNEL_NUM)
		{
			ack_fail_request();
			return;
		}

		ack_copy_request();
	}
	if(cmd_index == 7)
	{
#if defined (PLATFORM_ZYNQ) || defined (PLATFORM_ZYNQMP)
		uint8_t var1;
		if (!req_len_at_least(9))
		{
			ack_fail_request();
			return;
		}
		memcpy(&var1,receivebuf+7,1);
		reset_pl = var1;
		ack_copy_request();
#endif
	}
	if(cmd_index == 8)// ch_num
	{
		memcpy(send_buf,receivebuf,7);
		send_buf[7] = CHANNEL_NUM;
		sendlen = 9;
	}
	if(cmd_index == 10) // read global_config
	{
		if (!req_len_at_least(8))
		{
			ack_fail_request();
			return;
		}
		memcpy(send_buf,receivebuf,7);
		memcpy(send_buf+7, &global_config, sizeof(config_Settings_t));
		sendlen = 7 + sizeof(config_Settings_t) + 1;
	}
	if(cmd_index == 11) // save global_config
	{
		int32_t Status;
		uint8_t sendmsg;

		if (!req_len_at_least((uint16_t)(8U + sizeof(config_Settings_t))))
		{
			ack_fail_request();
			return;
		}
		memcpy(&global_config, receivebuf + 7, sizeof(config_Settings_t));

		//
//		print_config(&global_config);

		Status = saveconfig(&global_config);

		if (Status != XST_SUCCESS)
		{
			sendmsg = 1;
			xil_printf("--------saveconfig fail!---------\r\n");
		}
		else
		{
			sendmsg = 0;
			xil_printf("--------saveconfig ok!---------\r\n");
		}


		memcpy(send_buf, receivebuf, 7);
		memcpy(send_buf + 7, &sendmsg, 1);
		sendlen = 7 + 1 + 1;
		if (Status != XST_SUCCESS)
		{
			send_buf[2] = 0xFF;
			send_buf[3] = 0xFF;
		}
	}
	if(cmd_index == 12) // read default_config
	{
		memcpy(send_buf,receivebuf,7);
		memcpy(send_buf+7, &default_config, sizeof(config_Settings_t));
		sendlen = 7 + sizeof(config_Settings_t) + 1;

	}
	if(cmd_index == 13) //
	{
		xil_printf("------------test RELAY_CUTOFF------------\r\n");
		usleep(100*1000);
		XGpioPs_WritePin(&Gpio, RELAY_CUTOFF, 1) ; // RELAY_CUTOFF: 0-keep; 1-cutoff

		return;
	}

	if (sendlen == 0)
	{
		ack_fail_request();
		return;
	}

	memcpy(send_buf,&sendlen,2);
	send_buf[sendlen-1] = checksum(send_buf,sendlen-1);
}

#if defined (XPAR_XV_TPG_NUM_INSTANCES)

/* Host ch is 1-based (1..VIDEO_SRC_COUNT) -> 0-based TPG index. */
static int video_host_ch_to_idx(u8 host_ch, u8 *idx)
{
	if ((host_ch < 1U) || (host_ch > (u8)VIDEO_SRC_COUNT) || (idx == NULL))
	{
		return XST_FAILURE;
	}
	*idx = (u8)(host_ch - 1U);
	return XST_SUCCESS;
}

void msg_cmd_0x50(void)
{
	uint8_t cmd_index;
	uint8_t host_ch;
	uint8_t ch;
	XV_tpg *inst = NULL;

	if (!req_len_at_least(8))
	{
		ack_fail_request();
		return;
	}

	memcpy(&cmd_index, receivebuf + 5, 1);
	memcpy(&host_ch, receivebuf + 6, 1);

	/* cmd 7 (POC voltage) does not need TPG instance */
	if (cmd_index != 7U)
	{
		if (video_host_ch_to_idx(host_ch, &ch) != XST_SUCCESS)
		{
			ack_fail_request();
			return;
		}
		inst = tpg_get_instance(ch);
		if (inst == NULL)
		{
			ack_fail_request();
			return;
		}
	}

	if (cmd_index == 0U)
	{
		uint8_t bckgnd;

		if (receivelen < 9)
		{
			ack_fail_request();
			return;
		}
		memcpy(&bckgnd, receivebuf + 7, 1);
		if (tpg_set_bckgnd(ch, (u32)bckgnd) != XST_SUCCESS)
		{
			ack_fail_request();
			return;
		}
		ack_copy_request();
	}
	else if (cmd_index == 1U)
	{
		uint8_t mode;
		uint8_t speed = 1U;

		if (receivelen < 9)
		{
			ack_fail_request();
			return;
		}
		memcpy(&mode, receivebuf + 7, 1);
		if (receivelen >= 10)
		{
			memcpy(&speed, receivebuf + 8, 1);
		}
		if (tpg_set_box_motion(ch, mode, speed) != XST_SUCCESS)
		{
			ack_fail_request();
			return;
		}
		ack_copy_request();
	}
	else if (cmd_index == 2U)
	{
		u8 r;
		u8 g;
		u8 b;

		if (receivelen < 11)
		{
			ack_fail_request();
			return;
		}
		r = receivebuf[7];
		g = receivebuf[8];
		b = receivebuf[9];
		if (tpg_set_box_color(ch, r, g, b) != XST_SUCCESS)
		{
			ack_fail_request();
			return;
		}
		ack_copy_request();
	}
	else if (cmd_index == 3U)
	{
		uint32_t bckgnd;
		uint32_t ovrlay;
		uint32_t speed;
		u8 r;
		u8 g;
		u8 b;

		bckgnd = XV_tpg_Get_bckgndId(inst);
		ovrlay = XV_tpg_Get_ovrlayId(inst);
		speed = XV_tpg_Get_motionSpeed(inst);
		r = (u8)XV_tpg_Get_boxColorR(inst);
		g = (u8)XV_tpg_Get_boxColorG(inst);
		b = (u8)XV_tpg_Get_boxColorB(inst);

		memcpy(send_buf, receivebuf, 7);
		memcpy(send_buf + 7, &bckgnd, 4);
		memcpy(send_buf + 11, &ovrlay, 4);
		memcpy(send_buf + 15, &speed, 4);
		send_buf[19] = r;
		send_buf[20] = g;
		send_buf[21] = b;
		sendlen = 23;
		memcpy(send_buf, &sendlen, 2);
		send_buf[sendlen - 1] = checksum(send_buf, sendlen - 1);
	}
	/* cmd 7: read 4-port POC voltage (mV) and reply over network */
	else if (cmd_index == 7U)
	{
		u16 get_volt_port1 = 0;
		u16 get_volt_port2 = 0;
		u16 get_volt_port3 = 0;
		u16 get_volt_port4 = 0;

		get_adc_voltage(0, &get_volt_port1);
		get_adc_voltage(1, &get_volt_port2);
		get_adc_voltage(2, &get_volt_port3);
		get_adc_voltage(3, &get_volt_port4);

		xil_printf("\r\n------------------------\r\n");
		xil_printf("Detect port poc Voltage\r\n");
		xil_printf("Port1 poc Voltage = %d\r\n", get_volt_port1);
		xil_printf("Port2 poc Voltage = %d\r\n", get_volt_port2);
		xil_printf("Port3 poc Voltage = %d\r\n", get_volt_port3);
		xil_printf("Port4 poc Voltage = %d\r\n", get_volt_port4);
		xil_printf("------------------------\r\n");

		memcpy(send_buf, receivebuf, 7);
		memcpy(send_buf + 7, &get_volt_port1, 2);
		memcpy(send_buf + 9, &get_volt_port2, 2);
		memcpy(send_buf + 11, &get_volt_port3, 2);
		memcpy(send_buf + 13, &get_volt_port4, 2);
		sendlen = 16;
		memcpy(send_buf, &sendlen, 2);
		send_buf[sendlen - 1] = checksum(send_buf, sendlen - 1);
	}
	else
	{
		ack_fail_request();
	}
}

void msg_cmd_0x51(void)
{
	uint8_t cmd_index;
	uint8_t host_ch;
	uint8_t ch;
	u16 width;
	u16 height;
	u8 fps;
	u16 h_fp, h_sw, h_bp, v_fp, v_sw, v_bp;
	u8 h_pol, v_pol;
	const video_src_cfg_t *cfg;

	/*
	 * Host channel is 1-based (1..VIDEO_SRC_COUNT = TPG count).
	 * get ch source sub=0 len=10 reply: ch + source type
	 * get ch count  sub=1 len=9  reply: count at offset 7
	 * set           sub=2 len=27:
	 *   ch wLE hLE fps | h_fp h_sw h_bp h_pol | v_fp v_sw v_bp v_pol | csum
	 * get soft      sub=3: reply same layout (applied timing)
	 * save          sub=4 len=8: persist runtime geom to EEPROM (explicit)
	 * get meas      sub=5 len=13: ch wLE hLE fps (TPG mon measured)
	 * success: ack_copy / filled reply; fail: rsvd=FFFF
	 */
	sendlen = 0;

	if (!req_len_at_least(8))
	{
		ack_fail_request();
		return;
	}

	cmd_index = receivebuf[5];

	if (cmd_index == 0U)
	{
		u8 src;

		/* Query channel source type for host ch (1-based). */
		host_ch = receivebuf[6];
		if (video_host_ch_to_idx(host_ch, &ch) != XST_SUCCESS)
		{
			ack_fail_request();
			return;
		}
		src = video_src_source_type(ch);
		sendlen = 10;
		memset(send_buf, 0, (size_t)sendlen);
		memcpy(send_buf, &sendlen, 2);
		send_buf[2] = receivebuf[2];
		send_buf[3] = receivebuf[3];
		send_buf[4] = 0x51;
		send_buf[5] = 0;
		send_buf[6] = host_ch;
		send_buf[7] = src;
		send_buf[sendlen - 1] = checksum(send_buf, sendlen - 1);
	}
	else if (cmd_index == 1U)
	{
		/* Query channel count (== TPG instance count). */
		sendlen = 9;
		memset(send_buf, 0, (size_t)sendlen);
		memcpy(send_buf, &sendlen, 2);
		send_buf[2] = receivebuf[2];
		send_buf[3] = receivebuf[3];
		send_buf[4] = 0x51;
		send_buf[5] = 1;
		send_buf[6] = receivebuf[6];
		send_buf[7] = video_src_host_count();
		send_buf[sendlen - 1] = checksum(send_buf, sendlen - 1);
	}
	else if (cmd_index == 2U)
	{
		if (!req_len_at_least(27))
		{
			ack_fail_request();
			return;
		}
		host_ch = receivebuf[6];
		if (video_host_ch_to_idx(host_ch, &ch) != XST_SUCCESS)
		{
			ack_fail_request();
			return;
		}
		memcpy(&width, receivebuf + 7, 2);
		memcpy(&height, receivebuf + 9, 2);
		fps = receivebuf[11];
		memcpy(&h_fp, receivebuf + 12, 2);
		memcpy(&h_sw, receivebuf + 14, 2);
		memcpy(&h_bp, receivebuf + 16, 2);
		h_pol = receivebuf[18];
		memcpy(&v_fp, receivebuf + 19, 2);
		memcpy(&v_sw, receivebuf + 21, 2);
		memcpy(&v_bp, receivebuf + 23, 2);
		v_pol = receivebuf[25];
		if (video_src_set_timing(ch, width, height, fps,
					 h_fp, h_sw, h_bp, h_pol,
					 v_fp, v_sw, v_bp, v_pol) != XST_SUCCESS)
		{
			ack_fail_request();
			return;
		}
		ack_copy_request();
	}
	else if (cmd_index == 3U)
	{
		u16 t16;

		host_ch = receivebuf[6];
		if (video_host_ch_to_idx(host_ch, &ch) != XST_SUCCESS)
		{
			ack_fail_request();
			return;
		}
		cfg = video_src_cfg_get(ch);
		if (cfg == NULL)
		{
			ack_fail_request();
			return;
		}
		sendlen = 27;
		memset(send_buf, 0, (size_t)sendlen);
		memcpy(send_buf, &sendlen, 2);
		send_buf[2] = receivebuf[2];
		send_buf[3] = receivebuf[3];
		send_buf[4] = 0x51;
		send_buf[5] = 3;
		send_buf[6] = host_ch;
		memcpy(send_buf + 7, &cfg->width, 2);
		memcpy(send_buf + 9, &cfg->height, 2);
		send_buf[11] = cfg->fps;
		t16 = (u16)cfg->h_fp;
		memcpy(send_buf + 12, &t16, 2);
		t16 = (u16)cfg->h_sw;
		memcpy(send_buf + 14, &t16, 2);
		t16 = (u16)cfg->h_bp;
		memcpy(send_buf + 16, &t16, 2);
		send_buf[18] = (u8)cfg->h_pol;
		t16 = (u16)cfg->v_fp;
		memcpy(send_buf + 19, &t16, 2);
		t16 = (u16)cfg->v_sw;
		memcpy(send_buf + 21, &t16, 2);
		t16 = (u16)cfg->v_bp;
		memcpy(send_buf + 23, &t16, 2);
		send_buf[25] = (u8)cfg->v_pol;
		send_buf[sendlen - 1] = checksum(send_buf, sendlen - 1);
	}
	else if (cmd_index == 4U)
	{
		/* Explicit save - same intent as 0x40/11 eth saveconfig. */
		if (!req_len_match(8))
		{
			ack_fail_request();
			return;
		}
		if (video_inject_save_nv() != XST_SUCCESS)
		{
			ack_fail_request();
			return;
		}
		ack_copy_request();
	}
	else if (cmd_index == 5U)
	{
		u16 mw;
		u16 mh;
		u8 mfps;

		host_ch = receivebuf[6];
		if (video_host_ch_to_idx(host_ch, &ch) != XST_SUCCESS)
		{
			ack_fail_request();
			return;
		}
		if (video_src_get_measured(ch, &mw, &mh, &mfps) != XST_SUCCESS)
		{
			ack_fail_request();
			return;
		}
		sendlen = 13;
		memset(send_buf, 0, (size_t)sendlen);
		memcpy(send_buf, &sendlen, 2);
		send_buf[2] = receivebuf[2];
		send_buf[3] = receivebuf[3];
		send_buf[4] = 0x51;
		send_buf[5] = 5;
		send_buf[6] = host_ch;
		memcpy(send_buf + 7, &mw, 2);
		memcpy(send_buf + 9, &mh, 2);
		send_buf[11] = mfps;
		send_buf[sendlen - 1] = checksum(send_buf, sendlen - 1);
	}
	else
	{
		ack_fail_request();
	}
}
#endif /* XPAR_XV_TPG_NUM_INSTANCES */

void msg_cmd_0x80(void)
{
#if defined (UDP_VIDEO) || defined (TCP_VIDEO)
	if(receivebuf[6] == 0x00)  //Turn off the image display of the host computer
	{
		vdma_lwip_stop_media();
		xil_printf("close all video source\r\n");
	}
	else if( receivebuf[6] == 0x01)  //send picture
	{
		if(receivebuf[5] == 0x00)
		{
			vdma_lwip_stop_media();
		}
		else if(receivebuf[5] == 0xff)
		{
			vdma_lwip_arm_pic_capture();
		}
		else
		{
			vdma_lwip_arm_pic_capture();
		}
	}
	else if(receivebuf[6] == 0x02)  //send video
	{
		if(receivebuf[5] == 0x00)
		{
			vdma_lwip_stop_media();
		}
		else if(receivebuf[5] == 0xff)
		{
			vdma_lwip_arm_video_stream();
		    xil_printf("sending video\r\n");
		}
		else
		{
			xil_printf("sending video cmd err\r\n");
		}

	}
	else if( receivebuf[6] == 0x03)  //send err_picture
	{
#if defined (XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES)
		if(receivebuf[5] == 0x00)
		{
			for(u8 ch = 0; ch < XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES; ch++)
			{
				VC_inst.send_err_start[ch] = 0;
			}
		}
		else if(receivebuf[5] == 0xff)
		{
			for(u8 ch = 0; ch < XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES; ch++)
			{
				VC_inst.send_err_start[ch] = 1;
			}
			xil_printf("sending all ch err_pic\r\n");
		}
		else
		{
			if(receivebuf[5] > XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES)
			{
				xil_printf("pic source cmd error\r\n");
			}
			else
			{
				VC_inst.send_err_start[receivebuf[5] - 1] = 1;
				xil_printf("sending ch %d err_pic\r\n",receivebuf[5]);
			}
		}
#endif
	}
	else if(receivebuf[6] == 0x04)  //switch video
	{
#if defined (XPAR_XAXIS_SWITCH_NUM_INSTANCES) && (XPAR_XAXIS_SWITCH_NUM_INSTANCES >= 1U)
		if(receivebuf[5] <= CHANNEL_NUM)
		{

			xil_printf("------------swictch video source %d------------\r\n",receivebuf[5]);
			if (receivebuf[5] >= 1U)
			{
				switch_ch = receivebuf[5];
			}
		}
		else
		{
			xil_printf("video switch video cmd error\r\n");
		}
#endif
	}
	else
	{
		xil_printf("video source cmd error\r\n");

	}
#endif
}

#if defined (XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES)
void err_auto_send_func(u8 ch)
{
	if (ch >= XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES)
	{
		return;
	}

//	xil_printf("err_auto_send %u\r\n", (unsigned)ch + 1U);

	sendlen = 8 + (int)sizeof(vcmp_message);
	memcpy(send_buf, &sendlen, 2);
	send_buf[2] = 0x00;
	send_buf[3] = 0x00;
	send_buf[4] = 0x30;
	send_buf[5] = 0x05;
	send_buf[6] = 0xff;
	memcpy(send_buf + 7, &vcmp_m[ch], sizeof(vcmp_message));
	send_buf[sendlen - 1] = checksum(send_buf, sendlen - 1);

#if defined (UDP_COMMAND_SRV) && defined (UDP_VIDEO)
	{
		err_t err;
		struct pbuf *pq;
		struct udp_pcb *upcb = udp_cmd_get_client_pcb();

		if (upcb == NULL)
		{
			xil_printf("err_auto_send: UDP client null\r\n");
			return;
		}
		pq = pbuf_alloc(PBUF_TRANSPORT, (u16_t)sendlen, PBUF_POOL);
		if (pq == NULL)
		{
			xil_printf("err_auto_send pbuf_alloc %d fail\r\n", sendlen);
			return;
		}
		pbuf_take(pq, (char *)send_buf, (u16_t)sendlen);
		err = udp_send(upcb, pq);
		pbuf_free(pq);
		if (err != ERR_OK)
		{
			xil_printf("err_auto_send UDP send err %d\r\n", (int)err);
		}
	}
#elif defined (TCP_COMMAND_SRV) && defined (TCP_VIDEO)
	{
		err_t err;
		struct tcp_pcb *tpcb = tcp_cmd_get_client_pcb();

		if (tpcb == NULL)
		{
			xil_printf("err_auto_send: TCP cmd client null\r\n");
			return;
		}
		if ((u32)tcp_sndbuf(tpcb) < (u32)sendlen)
		{
			xil_printf("err_auto_send: TCP sndbuf low need %d have %u\r\n", sendlen,
			           (unsigned)tcp_sndbuf(tpcb));
			return;
		}
		err = tcp_write(tpcb, send_buf, (u16_t)sendlen, TCP_WRITE_FLAG_COPY);
		if (err != ERR_OK)
		{
			xil_printf("err_auto_send tcp_write %d\r\n", (int)err);
			return;
		}
		err = tcp_output(tpcb);
		if (err != ERR_OK)
		{
			xil_printf("err_auto_send tcp_output %d\r\n", (int)err);
		}
	}
#else
	(void)ch;
#endif
}
#endif /* XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES */

#endif //#if defined (UDP_COMMAND_SRV) || defined (TCP_COMMAND_SRV)



#endif // #if defined (UDP_UPDATE) || defined (TCP_UPDATE) || defined (TCP_COMMAND_SRV) || defined (UDP_COMMAND_SRV)
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
