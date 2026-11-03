#ifndef __CONFIG_H__
#define __CONFIG_H__

/*
 * 全局视频模式（与 src 下 vdma / vtc / tpg / clk_wiz / csi_tx 等 #if defined 一致）：
 *   R1080 — 1080p 类；R4K — 4K 类；二者都不定义时走各模块 #else 默认分支。
 */
#define R1080		/* 1920 x 1080 */
//#define R4K		/* 3840 x 2160 */

/*
 * for rs485 tcl settings(mb/zynq/znyqmp)
bsp config stdin axi_uartlite_0
bsp config stdout axi_uartlite_0
#bsp config stdin processer_ss_axi_uartlite_0
#bsp config stdout processer_ss_axi_uartlite_0
 */


/*
 * for sd card tcl settings(zynq/znyqmp)
bsp setlib -name xilffs
 */


/*
 * for dp port (znyqmp)
bsp setdriver -ip psu_dp -driver dppsu -ver 1.2
bsp regenerate
 */


/*
 * software version by compile time
 */
#define SW_VER_BY_COMPILE_TIME


/*
ps i2c dosen't support streching, use MIO/XGPIO or XIIC
 */
//#define	SER_CFG
#define	DES_CFG
//#define	SERDES_3G

/*
 * using hdmi-tx by SIL9136
 */
//#define SIL9136


/*
 * using hdmi-rx by IT6801
 */
//#define IT6801


/*
 * using multi-channel adc ADS7128
 */
//#define ADS7128


/*
 * for remote update tcl settings(zynq/znyqmp)
bsp setlib -name lwip211
bsp config mem_size 524288
bsp config memp_n_pbuf 1024
bsp config memp_n_tcp_pcb 32
bsp config memp_n_tcp_seg 1024
bsp config pbuf_pool_size 16384
bsp config tcp_snd_buf 65536
bsp config tcp_wnd 65535
bsp config n_rx_descriptors 512
bsp config n_tx_descriptors 512
 */

// if you want to use follow liwp related module at the same time, keep a single `server_netif` definition
#define UDP_COMMAND_SRV
//#define TCP_COMMAND_SRV
#define UDP_UPDATE
//#define TCP_UPDATE

#define UDP_VIDEO
//#define TCP_VIDEO

/* TCP_UPDATE need bigger tcp_snd_buf than default
domain active
bsp setlib -name lwip211
bsp config mem_size 33554432
bsp config memp_n_pbuf 4096
bsp config memp_n_tcp_pcb 1024
bsp config memp_n_tcp_seg 1024
##bsp config pbuf_pool_size 2048
bsp config pbuf_pool_size 4096
bsp config tcp_snd_buf 65536
bsp config tcp_wnd 8192
*/

#define USING_EEPROM

#define MODBUS_RTU_SLAVE

#endif // __CONFIG_H__
