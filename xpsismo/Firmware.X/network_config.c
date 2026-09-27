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
#include "flash.h"
#include "common.h"

#define GET_BIT(value, bit) (((value) >> (bit)) & 0x01)

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
uint8_t daughterCardConfig;

void network_config(void)
{
    
    /* Try reading Network Configuration from Flash Memory */
    printf("\n");
    if (Config_Read(&myIpAddress, &mySubnetMask, &myGateway, myMacAddress, 
        &yourIpAddress, &myPort, &yourPort, &daughterCardConfig))
    {
        printf("SUCCESSFULLY READ NETWORK DATA FROM FLASH MEMORY: \n");
    }
    else
    {
        printf("NO VALID FLASH MEMORY NETWORK DATA. USING DEFAULTS. \n");
        
        myIpAddress = MAKE_IPV4_ADDRESS(MYIPADDRESS_DEFAULT[0],MYIPADDRESS_DEFAULT[1],MYIPADDRESS_DEFAULT[2],MYIPADDRESS_DEFAULT[3]);
        mySubnetMask = MAKE_IPV4_ADDRESS(MYSUBNETMASK_DEFAULT[0],MYSUBNETMASK_DEFAULT[1],MYSUBNETMASK_DEFAULT[2],MYSUBNETMASK_DEFAULT[3]);
        myGateway = MAKE_IPV4_ADDRESS(MYGATEWAY_DEFAULT[0],MYGATEWAY_DEFAULT[1],MYGATEWAY_DEFAULT[2],MYGATEWAY_DEFAULT[3]);
        myMacAddress[0] = MYMACADDRESS_DEFAULT[0];
        myMacAddress[1] = MYMACADDRESS_DEFAULT[1];
        myMacAddress[2] = MYMACADDRESS_DEFAULT[2];
        myMacAddress[3] = MYMACADDRESS_DEFAULT[3];
        myMacAddress[4] = MYMACADDRESS_DEFAULT[4];
        myMacAddress[5] = MYMACADDRESS_DEFAULT[5];
        yourIpAddress = MAKE_IPV4_ADDRESS(YOURIPADDRESS_DEFAULT[0],YOURIPADDRESS_DEFAULT[1],YOURIPADDRESS_DEFAULT[2],YOURIPADDRESS_DEFAULT[3]);
        myPort = MYPORT_DEFAULT;
        yourPort = YOURPORT_DEFAULT;
        daughterCardConfig = DAUGHTERCARDCONFIG_DEFAULT;
    }
       
    daughter_output1 = GET_BIT(daughterCardConfig,0);
    daughter_output2 = GET_BIT(daughterCardConfig,1);
    daughter_servo = GET_BIT(daughterCardConfig,2);
    daughter_display1 = GET_BIT(daughterCardConfig,3);
    daughter_input1 = GET_BIT(daughterCardConfig,4);
    daughter_analoginput = GET_BIT(daughterCardConfig,5);
    daughter_display2 = GET_BIT(daughterCardConfig,6);
    daughter_input2 = GET_BIT(daughterCardConfig,7);
        
    printf("My IP Address:      %u.%u.%u.%u\n",IPV4_A(myIpAddress),IPV4_B(myIpAddress),IPV4_C(myIpAddress),IPV4_D(myIpAddress));    
    printf("My Subnet Mask:     %u.%u.%u.%u\n",IPV4_A(mySubnetMask),IPV4_B(mySubnetMask),IPV4_C(mySubnetMask),IPV4_D(mySubnetMask));    
    printf("My Gateway:         %u.%u.%u.%u\n",IPV4_A(myGateway),IPV4_B(myGateway),IPV4_C(myGateway),IPV4_D(myGateway));    
    printf("My MAC Address:     %02X:%02X:%02X:%02X:%02X:%02X\n",myMacAddress[0],myMacAddress[1],myMacAddress[2],myMacAddress[3],myMacAddress[4],myMacAddress[5]);
    printf("Server IP Address:  %u.%u.%u.%u\n",IPV4_A(yourIpAddress),IPV4_B(yourIpAddress),IPV4_C(yourIpAddress),IPV4_D(yourIpAddress));    
    printf("My UDP Port:        %i\n",myPort);    
    printf("Server UDP Port:    %i\n",yourPort);    
    printf("DAUGHTER INPUT1:    %i \n",daughter_input1);
    printf("DAUGHTER INPUT2:    %i \n",daughter_input2);
    printf("DAUGHTER ANA INPUT: %i \n",daughter_analoginput);
    printf("DAUGHTER OUTPUT1:   %i \n",daughter_output1);
    printf("DAUGHTER OUTPUT2:   %i \n",daughter_output2);
    printf("DAUGHTER SERVO:     %i \n",daughter_servo);
    printf("DAUGHTER DISPLAY1:  %i \n",daughter_display1);
    printf("DAUGHTER DISPLAY2:  %i \n",daughter_display2);     
    printf("\n");
     
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
