/*
 * settings.h
 *
 *  Created on: Jan 29, 2019
 *      Author: alex
 */

#ifndef SRC_SETTINGS_H_
#define SRC_SETTINGS_H_

typedef struct
{
	uint64_t reserved_container;
	uint32_t ipaddr_u32;
	uint32_t mask_u32;
	uint32_t gw_u32;
	uint8_t mac_address[8];
} __attribute__ ((__packed__)) config_Settings_t;

extern config_Settings_t global_config;
extern config_Settings_t default_config;

#endif /* SRC_SETTINGS_H_ */
