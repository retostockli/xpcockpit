/* 
 * File:   network_config.c
 * Author: stockli
 *
 * Created on August 20, 2026, 9:18 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include "mcc_generated_files/TCPIPLibrary/udpv4.h"
#include "mcc_generated_files/TCPIPLibrary/tcpip_config.h"
#include "mcc_generated_files/TCPIPLibrary/ip_database.h"
#include "mcc_generated_files/TCPIPLibrary/ethernet_driver.h"
#include "mcc_generated_files/TCPIPLibrary/mac_address.h"
#include "mcc_generated_files/device_config.h"
#include "mcc_generated_files/mcc.h"
#include "network_config.h"
#include "udp.h"
/*
 * 
 */
uint32_t myIpAddress;
uint32_t mySubnetMask;
uint32_t myGateway;
uint8_t myMacAddress[6];
uint32_t yourIpAddress;
uint16_t myPort;
uint16_t yourPort;


void network_config(void)
{
    
    myIpAddress = MAKE_IPV4_ADDRESS(192,168,1,55);
    mySubnetMask = MAKE_IPV4_ADDRESS(255,255,255,0);
    myGateway = MAKE_IPV4_ADDRESS(192,168,1,1);
    myMacAddress[0] = 0x02;
    myMacAddress[1] = 0x02;
    myMacAddress[2] = 0x02;
    myMacAddress[3] = 0x02;
    myMacAddress[4] = 0x11;
    myMacAddress[5] = 0x17;
    
    yourIpAddress = MAKE_IPV4_ADDRESS(192,168,1,105);
    myPort = 1024;
    yourPort = 1026;
     
    // Set application-defined MAC address
    ETH_SetMAC(myMacAddress);

    // Update the MAC address used by ARP
    ETH_GetMAC((uint8_t*)&hostMacAddress);

    
    //Application-defined network configuration
    ipdb_setAddress(myIpAddress);
    ipdb_setSubNetMASK(mySubnetMask);
    ipdb_setRouter(myGateway);
    ipdb_setGateway(myGateway);   
         
    /* UDP Packet Initializations*/
    UDP_Initialize(yourIpAddress,myPort,yourPort);
}
