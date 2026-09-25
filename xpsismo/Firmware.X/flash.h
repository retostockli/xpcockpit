/* 
 * File:   flash.h
 * Author: stockli
 *
 * Created on September 25, 2026, 4:48 PM
 */

#ifndef FLASH_H
#define	FLASH_H

#include <stdbool.h>


#define CONFIG_FLASH_ADDRESS  0xF800UL
#define CONFIG_MAGIC          0x49504346UL

bool IPAddress_Read(uint32_t *ipAddress);
bool IPAddress_Write(uint32_t ipAddress);


#endif	/* FLASH_H */

