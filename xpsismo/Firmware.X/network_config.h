/* Microchip Technology Inc. and its subsidiaries.  You may use this software 
 * and any derivatives exclusively with Microchip products. 
 * 
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER 
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A 
 * PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION 
 * WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
 * INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
 * WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
 * BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE 
 * FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS 
 * IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF 
 * ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE 
 * TERMS. 
 */

/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef NETWORK_CONFIG_H
#define NETWORK_CONFIG_H
    
#define IPV4_A(ip)  ((uint8_t)((ip >> 24) & 0xFF))
#define IPV4_B(ip)  ((uint8_t)((ip >> 16) & 0xFF))
#define IPV4_C(ip)  ((uint8_t)((ip >>  8) & 0xFF))
#define IPV4_D(ip)  ((uint8_t)( ip        & 0xFF))

// Default Network values if no flash data is available (e.g. after reset button was pressed)
static const uint8_t MYIPADDRESS_DEFAULT[4] = {192,168,1,55};
static const uint8_t MYSUBNETMASK_DEFAULT[4] = {255,255,255,0};
static const uint8_t MYGATEWAY_DEFAULT[4] = {192,168,1,1};
static const uint8_t MYMACADDRESS_DEFAULT[6] = {0x00,0x00,0x00,0x00,0x11,0x17};
static const uint8_t YOURIPADDRESS_DEFAULT[4] = {192,168,1,105};
static const uint16_t MYPORT_DEFAULT = 1024;
static const uint16_t YOURPORT_DEFAULT = 1026;
static const uint8_t DAUGHTERCARDCONFIG_DEFAULT = 0x00;
   
extern uint32_t myIpAddress;
extern uint32_t mySubnetMask;
extern uint32_t myGateway;
extern uint8_t myMacAddress[6];
extern uint32_t yourIpAddress;
extern uint16_t myPort;
extern uint16_t yourPort;
extern uint8_t daughterCardConfig;

void network_config(void);    

#endif	

