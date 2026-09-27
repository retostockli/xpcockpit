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
#define CONFIG_MAGIC          0x49534146UL

bool Config_Read(uint32_t *myIpAddress, uint32_t *mySubnetMask, uint32_t *myGateway, uint8_t *myMacAddress, 
        uint32_t *yourIpAddress, uint16_t *myPort, uint16_t *yourPort, uint8_t *daughterCardConfig);

bool Config_Write(uint32_t myIpAddress, uint32_t mySubnetMask, uint32_t myGateway, uint8_t myMacAddress[6], 
        uint32_t yourIpAddress, uint16_t myPort, uint16_t yourPort, uint8_t daughterCardConfig);

bool Config_Erase(void);


#endif	/* FLASH_H */

